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

#include "OBS_OpticalCrossConnect.h"

Define_Module(OBS_OpticalCrossConnect);

OBS_OpticalCrossConnect::~OBS_OpticalCrossConnect(){
	free(schedulingTable);
}

void OBS_OpticalCrossConnect::initialize(){
   // All input gates initialized to -1
   schedulingTable = (int*)calloc(gateSize("in"),sizeof(int));
   int i;
   for(i=0;i<gateSize("in");i++){
      schedulingTable[i] = -1;
      WATCH(schedulingTable[i]);
   }
}

void OBS_OpticalCrossConnect::handleMessage(cMessage *msg){
   //Pretty easy algorithm: check if input gate has a scheduled connection and send the message to assigned output gate.
   cGate *gate = msg->getArrivalGate();

   if(schedulingTable[gate->getIndex()] == -1) delete msg; // Output gate not assigned. Drop burst
   else
      send(msg,"out",schedulingTable[gate->getIndex()]);
}

void OBS_OpticalCrossConnect::setGate(int inGate,int outGate){
   Enter_Method("programming gate connection %d -> %d",inGate,outGate);

   if(schedulingTable[inGate] != -1) opp_error("Attempting to schedule an already scheduled input channel. Channel id: %d",inGate);
   schedulingTable[inGate] = outGate;
}

void OBS_OpticalCrossConnect::unsetGate(int inGate){
   Enter_Method("unprogramming gate %d",inGate);
   schedulingTable[inGate] = -1;
}
