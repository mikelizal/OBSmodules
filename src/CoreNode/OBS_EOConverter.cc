//
// Copyright (C) 2010-2012 Javier Armendariz Silva, Naiara Garcia Royo
// Copyright (C) 2010-2012 Universidad Publica de Navarra
//
// This file is part of OBSModules.
//
// OBSModules is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// OBSModules is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with OBSModules.  If not, see <http://www.gnu.org/licenses/>.
//

#include<omnetpp.h>
#include "OBS_BurstControlPacket_m.h"
#include "OBS_BCPControlInfo_m.h"

using namespace std;

//! Electrical-Optical converter. Converts electrical BCP to the optical way (BCP beginning and BCP end)
class OBS_EOConverter : public cSimpleModule
{
   private:
      double dataRate; //!< Data rate of the optical fiber.
      simtime_t EOConversionDelay; //!< Module conversion delay. Set as ned parameter.
   protected:
      cQueue **BCPqueues; //!< BCPs pending to be sent (because they found BCP channel busy). One for each fiber.
      int numPorts; //!< Number of fibers.
      bool *control_is_busy; //!< True if control channel is currently busy.
      virtual void initialize();
      virtual void handleMessage(cMessage *msg);
   public:
      virtual ~OBS_EOConverter();
};

Define_Module(OBS_EOConverter);

OBS_EOConverter::~OBS_EOConverter(){
	for (int i=0; i<numPorts; i++)
	    delete BCPqueues[i];
	delete [] BCPqueues;
	free(control_is_busy);
}

void OBS_EOConverter::initialize(){
   dataRate = par("dataRate");
   EOConversionDelay = par("EOConversionDelay");

   numPorts = par("numPorts");
   control_is_busy = (bool *)calloc(numPorts,sizeof(bool));
   BCPqueues = new cQueue *[numPorts];
   for (int i=0; i<numPorts; i++)
   {
	   control_is_busy[i] = false;
       char buf[32];
       sprintf(buf, "waitingBCPs{%d}", i);
       BCPqueues[i] = new cQueue(buf);
   }
}

void OBS_EOConverter::handleMessage(cMessage *msg){
	//If the message is not a BCP, it must be a control message
	//to take a BCP from the queue
	if(dynamic_cast <OBS_BurstControlPacket *> (msg) == NULL){
		if (msg->isSelfMessage()){
			//The port number will be saved in the kind parameter of cMessage
			//not to create an special class
			int waitingPort = msg->getKind();
			control_is_busy[waitingPort] = false;
			if (!BCPqueues[waitingPort]->isEmpty()){
				OBS_BurstControlPacket *waitingBCP = (OBS_BurstControlPacket*) BCPqueues[waitingPort]->pop();
				scheduleAt(simTime(),waitingBCP);
			}
			delete msg;
		}
		else{
			opp_error("Received an unknown message");
		}
	}
	else{
		if(!msg->isSelfMessage()){//If the BCP has arrived from the outside
		    //Restart the BCPArrival
			OBS_BurstControlPacket *bcp = check_and_cast < OBS_BurstControlPacket *> (msg);
			OBS_BCPControlInfo *info = (OBS_BCPControlInfo*)bcp->getControlInfo();
		    info->setBCPArrival(simTime()+EOConversionDelay);
			scheduleAt(simTime()+EOConversionDelay,bcp);
		}
		else{//If the BCP is a self-message (It has waited the EOConversionDelay)
			OBS_BurstControlPacket *bcp = check_and_cast < OBS_BurstControlPacket *> (msg); // Convert (if possible) to BCP message data type
			//The control info should not be removed until the BCP is sent
			OBS_BCPControlInfo *infoAux = (OBS_BCPControlInfo*)bcp->getControlInfo();

			int port = infoAux->getPort();

		   if (control_is_busy[port]){//If the control channel is busy transmitting another BCP, enqueue it
			   BCPqueues[port]->insert(bcp);
		   }
		   else{
			   //Take the control channel
			   control_is_busy[port] = true;

			   OBS_BCPControlInfo *info = (OBS_BCPControlInfo*)bcp->removeControlInfo(); // Extract control info
			   OBS_BurstControlPacket *endBCP = new OBS_BurstControlPacket("endBCP");
			   endBCP->setBurstifierId(bcp->getBurstifierId());
			   endBCP->setNumSeq(bcp->getNumSeq());

			   int bcpSize = bcp->getBitLength(); // Take necessary attributes from BCP

			   //Finally, just send optical BCP beginning/end with the correct timing between them
			   bcp->setName("iniBCP");
			   bcp->setKind(1);
			   endBCP->setKind(2);
			   //The time the BCP has been on queue and the EOConversionDelay have to be subtracted from the burstArrivalDelta
			   bcp->setBurstArrivalDelta(bcp->getBurstArrivalDelta() - EOConversionDelay - (simTime() - info->getBCPArrival()));
			   send(bcp,"out",info->getPort());
			   //Now send BCP ending message after Tx time
			   sendDelayed(endBCP,(bcpSize/dataRate),"out",info->getPort());
			   //Control message which informs that the control channel of the port i is free
			   cMessage *ctlMsg=new cMessage("control_is_free");
			   ctlMsg->setKind(port);
			   scheduleAt(simTime()+(bcpSize/dataRate),ctlMsg);
			   //Delete control info
			   delete info;
		   }
		}
	}
}
