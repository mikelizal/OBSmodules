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

#include "OBS_CoreControlLogic.h"

Define_Module(OBS_CoreControlLogic);

OBS_CoreControlLogic::~OBS_CoreControlLogic(){
	   free(recvBurstCounter);
	   free(schedBurstCounter);
}

void OBS_CoreControlLogic::initialize(){
   dropCounter = 0;
   WATCH(dropCounter);

   guardTime = par("guardTime");
   processingTime = par("BCPProcessingDelay");
   cModule *parent = getParentModule();
   dataRate = par("dataRate");

   if (strcmp(par("reportFile"), "") == 0) {
      data_f = NULL;
   }
   else {
      data_f = fopen(par("reportFile"),"w");
   }

   //IMPORTANT: USE check_and_cast WHEN LINKING WITH ANOTHER SUBMODULE. OTHERWISE WE WILL GET A NULL POINTER ERROR

   //TODO: Module names should not be hard-coded. Modify this when possible.
   // One possible way to do this consist in a loop that searches for all neighbor modules and assigns the object reference depending on the module type (check if the module belongs to OBS_RoutingTable,
   // if not check if it belongs to Horizon Table type, etc...)
   // As long as there are no module arrays, it shouldn't give any problem.
   routingTable = check_and_cast<OBS_CoreRoutingTable*>(parent->getSubmodule("RoutingTable"));
   gatesHorizon = check_and_cast<OBS_CoreOutputHorizon*>(parent->getSubmodule("GatesHorizon"));
   coreInput = check_and_cast<OBS_CoreInput*>((parent->getParentModule())->getSubmodule("Input"));
   coreOutput = check_and_cast<OBS_CoreOutput*>((parent->getParentModule())->getSubmodule("Output"));
   oxc = check_and_cast<OBS_OpticalCrossConnect*>((parent->getParentModule())->getSubmodule("OXC"));

   //Initialize all statistics
   int numInPorts = coreInput->par("numPorts");
   int numOutPorts = coreOutput->par("numPorts");
   recvBurstCounter= (int*)calloc(numInPorts,sizeof(int));
   schedBurstCounter = (int*)calloc(numOutPorts,sizeof(int));

   int i=0;
   for(i=0;i<numInPorts;i++){
      recvBurstCounter[i] = 0;
      WATCH(recvBurstCounter[i]);
   }
   for(i=0;i<numOutPorts;i++){
      schedBurstCounter[i] = 0;
      WATCH(schedBurstCounter[i]);
   }
}

// Assume this module input is connected to the OE Converter, so we only receive electrical BCPs
void OBS_CoreControlLogic::handleMessage(cMessage *msg){
   //OXC Programming automessage. If msg has kind = OBS_SCHEDULE_OXC
   if(msg->getKind() == OBS_SCHEDULE_OXC){
      OBS_ControlUnitInfo *info = check_and_cast <OBS_ControlUnitInfo*>(msg);
      oxc->setGate(info->getInGate(),info->getOutGate());
      delete msg; 
      return;
   }
   else if(msg->getKind() == OBS_UNSCHEDULE_OXC){ //OXC Unscheduling auto message
      OBS_ControlUnitInfo *info = check_and_cast <OBS_ControlUnitInfo*>(msg);
      oxc->unsetGate(info->getInGate());
      delete msg;
      return;
   }
   //Step 1 - BCP received
   OBS_BurstControlPacket *bcp = check_and_cast <OBS_BurstControlPacket*>(msg);

   //extract BCP control info
   OBS_BCPControlInfo *info = (OBS_BCPControlInfo*)bcp->getControlInfo();

   //Step 2 - Extract all necessary BCP info
   int burstColour = bcp->getBurstColour(); //Burst colour
   int arrivalPort = info->getPort(); //Arrival port
   simtime_t arrivalDelta = bcp->getBurstArrivalDelta(); //Time offset between BCP and Burst
   int burstLength = bcp->getBurstSize(); //Burst length
   int destLabel = bcp->getLabel(); //Burst destination label

   int burstifierId = bcp->getBurstifierId();
   int numSeq = bcp->getNumSeq();

   int inGate = coreInput->getOXCGate(arrivalPort, coreInput->getLambdaByColour(arrivalPort,burstColour)); //Request OXC input gate for incoming burst
   //Count this message as a received burst
   recvBurstCounter[arrivalPort]++;

   simtime_t burstArrival = simTime() + arrivalDelta;

   // Check if burst is scheduled to arrive in the future. If everything works OK burst should behave this way. But, it's possible that BCP and burst offset was very small and in some point of the path, burst enters the Core Node before it's BCP. In this case, discard this message (Burst was discarded already)
   if(burstArrival < simTime()){
	//BCP discarded. Burst was here and discarded too.
	delete msg;
	dropCounter++;

    //Burst dropped (case 1)
    if (data_f != NULL){
    	fprintf(data_f,"%d %d %s %s 1 1\n", burstifierId, numSeq, simTime().str().c_str(), burstArrival.str().c_str());
    }

	return;
   }

   //Time calculations. See documentation about guardTimes.
   simtime_t OXCConnectTime = burstArrival - guardTime/2;
   int burstLengthInBits = burstLength*8; //Burst length in bits
   simtime_t OXCChannelIsFree = burstArrival + (burstLengthInBits/dataRate);
   simtime_t OXCDisconnectTime = OXCChannelIsFree + (guardTime)/4;
   simtime_t newHorizon = OXCChannelIsFree + (3*guardTime)/4;

   //Query Core Node routing table asking for output port,colour and label
   OBS_CoreRoutingTableEntry *result = routingTable->getEntry(arrivalPort,burstColour,destLabel);
   if(result == NULL) opp_error("Error in routing table query (Control Unit id: %d)",getId());

   int outPort = result->getOutPort();
   int outColour = result->getOutColour();
   int outLabel = result->getOutLabel();

   delete result; //Once the query data is stored, I must clean it.

   int lambda = 0;
   if(outColour == -9){ // * option. Choose the lambda with closest horizon
      	// Choose the best channel
        lambda = gatesHorizon->findNearestLambda(outPort,burstArrival);        

	if(lambda == -1){ // if none of the lambdas is available
        	delete msg;
        	dropCounter++;

        	//Burst dropped (case 2)
            if (data_f != NULL){
            	fprintf(data_f,"%d %d %s %s 1 2\n", burstifierId, numSeq, simTime().str().c_str(), burstArrival.str().c_str());
            }

         	return;
      	}
   }else{//Not *. Use the assigned channel
	    //Check if channel is free at the burst arrival moment. (outColour is actually a colour, that's why I convert it using getLambdaByColour method)
      	lambda = coreOutput->getLambdaByColour(outPort,outColour);

      	if(gatesHorizon->getHorizon(outPort,lambda) > burstArrival ){
         	//Drop this BCP if the channel is not available at the time of Burst arrival
         	delete msg;
         	dropCounter++;

         	//Burst dropped (case 3)
            if (data_f != NULL){
            	fprintf(data_f,"%d %d %s %s 1 3\n", burstifierId, numSeq, simTime().str().c_str(), burstArrival.str().c_str());
            }
            
         	return;
	}
   }

   OBS_ControlUnitInfo *controlInfo = new OBS_ControlUnitInfo();
   OBS_ControlUnitInfo *controlInfo1 = new OBS_ControlUnitInfo();

   //Count the incoming Burst
   schedBurstCounter[outPort]++;

   //Update horizon array
   gatesHorizon->updateHorizon(outPort,lambda, newHorizon);

   // Schedule OXC Programming (set an automessage)
   controlInfo->setInGate(inGate);
   controlInfo->setOutGate(coreOutput->getOXCGate(outPort,lambda));
   controlInfo->setKind(OBS_SCHEDULE_OXC);
   controlInfo->setSchedulingPriority(2);

   scheduleAt(OXCConnectTime,controlInfo);

   // Schedule OXC Unprogramming (set an automessage)
   controlInfo1->setInGate(inGate);
   controlInfo1->setKind(OBS_UNSCHEDULE_OXC);
   controlInfo1->setSchedulingPriority(1);

   scheduleAt(OXCDisconnectTime,controlInfo1);

   // Update BCP
   info->setPort(outPort);

   bcp->setBurstColour(coreOutput->getColourByLambda(outPort,lambda));
   bcp->setBurstArrivalDelta(arrivalDelta - processingTime);

   if(outLabel == -9) bcp->setLabel(destLabel);
   else bcp->setLabel(outLabel);
   
   // Wait the Control Logic processing time and then forward this message
   sendDelayed(bcp,processingTime,"out");

   //Burst sent
   if (data_f != NULL){
	   fprintf(data_f,"%d %d %s %s 0 0\n", burstifierId, numSeq, simTime().str().c_str(), burstArrival.str().c_str());
   }
}

void OBS_CoreControlLogic::finish(){
   // Register dropped Bursts
   recordScalar("Burst dropped in Core Node",dropCounter);
   if (data_f != NULL){
      fclose(data_f);
   }
}
