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

#include "OBS_CoreOutputHorizon.h"

Define_Module(OBS_CoreOutputHorizon);

OBS_CoreOutputHorizon::~OBS_CoreOutputHorizon(){
	int i;
	int numPorts = par("numPorts");
	for(i=0;i<numPorts;i++)
	   free(horizon[i]);
	free(horizon);
	free(portLambdas);
}

void OBS_CoreOutputHorizon::initialize(){
   int numPorts = par("numPorts");
   int i=0;
   int j = 0;
   portLambdas= (int*)calloc(numPorts,sizeof(int));
   
   cStringTokenizer tokenizer(par("lambdasPerPort").stringValue());
   while(tokenizer.hasMoreTokens()){
      portLambdas[i] = atoi(tokenizer.nextToken());
      i++;
   }

   horizon = (simtime_t**)calloc(numPorts,sizeof(simtime_t*));

   for(i=0;i<numPorts;i++){
      horizon[i] = (simtime_t*)calloc(portLambdas[i],sizeof(simtime_t)); 
      // Fill horizon matrix with zeros
      for(j=0;j<portLambdas[i];j++){
         horizon[i][j] = 0;
	 WATCH(horizon[i][j]);
      }
   }

}

//Find a lambda which horizon value is nearest (or even equal) to given arrival time. 
int OBS_CoreOutputHorizon::findNearestLambda(int port,simtime_t arrivalTime){
   Enter_Method("find me the nearest lambda for port %d and time %s",port,arrivalTime.str().c_str());

   int i;
   int min = 0;

   simtime_t minDiff = -1; //Initial value -1 because 0 is a valid time difference

   for(i=0;i<portLambdas[port];i++){
      // A simple minimum algorithm
      if(arrivalTime > horizon[port][i]){ 
         if(minDiff == -1){ //No minimum encountered already. For now this value is fine.
	    min = i;
            minDiff = arrivalTime - horizon[port][i];
         }else if((arrivalTime - horizon[port][i]) < minDiff){ //Return first minimum value encountered
            min = i;
            minDiff = arrivalTime - horizon[port][i];
         }
      }
   }
   //If minDiff is -1 at this point, no minimum found
   if(minDiff == -1) return -1;

   return min;
}

void OBS_CoreOutputHorizon::updateHorizon(int port, int lambda, simtime_t newTime){
   Enter_Method("update %d,%d horizon",port,lambda);
   horizon[port][lambda] = newTime;
}

simtime_t OBS_CoreOutputHorizon::getHorizon(int port,int lambda){
   Enter_Method("request %d,%d horizon",port,lambda);
   if(lambda == -1) return -1; //Just in case lambda = -1
   return horizon[port][lambda];
}
