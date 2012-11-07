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

#include "OBS_OpticalMonitor.h"

Define_Module(OBS_OpticalMonitor);

void OBS_OpticalMonitor::initialize(){
   //create the output file
   //TODO Include a module Id in the name string in order to use more than one single monitor in the network
	//TODO Use append mode to handle multiple simulation runs in one single file
	data_f = fopen(par("reportFile"),"w");

	//Write output file header (OMNeT style)
   fprintf(data_f,"vector 1 \"OBS_OpticalMonitor id:%d\" \"Received Burst info\" 1\n",getId());
   fprintf(data_f,"vector 2 \"OBS_OpticalMonitor id:%d\" \"Received BCP info\" 1\n",getId());
}

void OBS_OpticalMonitor::handleMessage(cMessage *msg){
   //store arrival gate
   cGate *gate = msg->getArrivalGate();
   //don't forget to forward incoming messages

   //check if msg is a BCP or a Burst
   if(dynamic_cast< OBS_Burst *> (msg) != NULL){
      OBS_Burst *b = check_and_cast < OBS_Burst *> (msg);
      //check if kind=1 (Burst beginning)
      if(b->getKind() == 1){
         //register it!
    	 fprintf(data_f,"1 %s %d %d %lld\n",simTime().str().c_str(),b->getBurstifierId(),b->getNumSeq(),b->getByteLength());
      }
   }else if(dynamic_cast< OBS_BurstControlPacket *> (msg) != NULL){
      OBS_BurstControlPacket *c = check_and_cast < OBS_BurstControlPacket *> (msg);
      if(c->getKind() == 1)
         //register BCP ini info
    	 fprintf(data_f,"2 %s %d %d %d\n",simTime().str().c_str(),c->getBurstifierId(),c->getNumSeq(),c->getBurstColour());
   }
   //forward everything
   send(msg,"out",gate->getIndex());
}

void OBS_OpticalMonitor::finish(){
   fclose(data_f);
}
