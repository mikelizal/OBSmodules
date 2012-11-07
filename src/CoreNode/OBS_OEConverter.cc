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


//! Optical-Electrical converter. Gathers beginning/end BCP messages and creates an 'electrical BCP' including info concerning input fiber and iniBCP arrival time.
class OBS_OEConverter : public cSimpleModule
{
   private:
      list<OBS_BurstControlPacket*> pendingMsgs; //!< BCP's pending for conversion.
      simtime_t OEConversionDelay; //!< Module conversion delay. Set as ned parameter.
   protected:
      virtual void initialize();
      virtual void handleMessage(cMessage *msg);
	  virtual void finish();
   public:
      virtual ~OBS_OEConverter();
};

Define_Module(OBS_OEConverter);

OBS_OEConverter::~OBS_OEConverter(){
   //Cleanup pendingMsgs queue
   list<OBS_BurstControlPacket*>::iterator iter_list;
   OBS_BurstControlPacket *item;
   for(iter_list = pendingMsgs.begin();iter_list != pendingMsgs.end(); iter_list++){
      item = *iter_list;
      delete item;
   }
   pendingMsgs.clear();
}

void OBS_OEConverter::initialize(){
   OEConversionDelay = par("OEConversionDelay");
}

void OBS_OEConverter::handleMessage(cMessage *msg){
   // BCP received
   cGate *gate = msg->getArrivalGate();
   OBS_BurstControlPacket *bcp = check_and_cast < OBS_BurstControlPacket *> (msg);
   int bId = bcp->getBurstifierId();
   int nSeq = bcp->getNumSeq();

   if(bcp->getKind() == 1){ //If BCP beginning is received, enqueue it
      OBS_BCPControlInfo *ctlMsg = new OBS_BCPControlInfo(); // Add BCP arrival information
      ctlMsg->setBCPArrival(simTime());
      bcp->setControlInfo(ctlMsg);
      pendingMsgs.push_back(bcp);
   }else{ //BCP ending. Look for this BCP into the queue and assemble an "Electrical BCP"
      //Look up using BCP Id fields
      list<OBS_BurstControlPacket*>::iterator iter_list;
      OBS_BurstControlPacket *item;
      for(iter_list = pendingMsgs.begin();iter_list != pendingMsgs.end(); iter_list++){
         item = *iter_list;
 
         if((item->getBurstifierId() == bId) && (item->getNumSeq() == nSeq)){ // BCP beginning found!

            OBS_BurstControlPacket *bcpDup = item->dup(); //Duplicate this message. It will be the electrical BCP
    	    OBS_BCPControlInfo *ctlMsg = (OBS_BCPControlInfo*)item->removeControlInfo(); //Remove the control info object
            pendingMsgs.remove(item);

            bcpDup->setKind(3); // Kind=3 Electrical BCP
	        bcpDup->setName("elecBCP"); //Modified in order to make graphical simulation clearer

            ctlMsg->setPort(gate->getIndex());
            bcpDup->setBurstArrivalDelta(bcpDup->getBurstArrivalDelta() - (simTime() - ctlMsg->getBCPArrival()) - OEConversionDelay); // Update arrival time considering conversion delay, original BCP arrival time, and current time.
            bcpDup->setControlInfo(ctlMsg);
            sendDelayed(bcpDup,OEConversionDelay,"out"); // Send electrical BCP to out gate
            // Drop received msg. The end
            delete msg;
	        delete item;
            return;
         }
      }
   }
}

void OBS_OEConverter::finish(){
   recordScalar("BCPs pending to be converted in electrical", pendingMsgs.size());
}
