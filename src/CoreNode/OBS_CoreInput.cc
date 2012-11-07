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

#include "OBS_CoreInput.h"

Define_Module(OBS_CoreInput);

OBS_CoreInput::~OBS_CoreInput(){
   free(portLen);
   free(inPortBegin);
   free(outDataBegin);
}

void OBS_CoreInput::initialize(){
   numPorts = par("numPorts");
   
   const char* portLenStr = par("lambdasPerPort").stringValue();
   if(strcmp(portLenStr,"") == 0) opp_error("OBS_CoreInput: lambdasPerPort not initialized");

   portLen = (int*)calloc(numPorts,sizeof(int));
   // inPortBegin contains the OMNeT++ input gate index where each logical fiber begins
   inPortBegin = (int*)calloc(numPorts,sizeof(int));
   // outDataBegin contains the OMNeT++ output gate index where each logical fiber begins for data channels
   outDataBegin = (int*)calloc(numPorts,sizeof(int));
 
   cStringTokenizer tokenizer(portLenStr);
   int i = 0;

   while(tokenizer.hasMoreTokens()){
      portLen[i] = atoi(tokenizer.nextToken()) + 1; // lambdasPerPort considers only data channels. In this array we include a control channel for each fiber.
      i++; 
   }
   
   inPortBegin[0] = 0;
   for(i=1;i<numPorts;i++)
      // i-th fiber begins just after (i-1)-th last channel. 
      // So, if you add the number of i-1 fiber channels to the number where i-1 fiber begins, you'll have the beginning gate index for i-th fiber
      inPortBegin[i] = inPortBegin[i-1] + (portLen[i-1]);

   outDataBegin[0] = numPorts; //First data channel will be just after control channels.
   for(i=1;i<numPorts;i++)
      outDataBegin[i] = outDataBegin[i-1] + (portLen[i-1] - 1);


   //Create colour structures
   const char* colourString = par("colours");
   // colour is a vector structure containing map objects. Each vector entry represents an optical fiber and each map represents all colours available. 
   // Maps are indexed by colour number, and the associated value is the logical channel id (lambda)
   
   // Create a temporary map to initialize colour vector
   map<int,int> tempMap;
   colours.reserve(numPorts); // Pre-allocate memory to improve efficiency

   //Add the empty string case:
   if(strcmp(colourString,"") == 0){
      int j=0;
      for(i=0;i<numPorts;i++){
		 for(j=0;j<(portLen[i]-1);j++){
            tempMap[j] = j; // colour = lambda
         }
         colours.push_back(map<int,int>(tempMap)); // Insert tempMap as a vector entry
         tempMap.clear(); // Clear temporary map. Now it's ready to refill
      }
   }else{ // colourString is set. Parse the string and fill the structure
      //Begin with parsing process
      cStringTokenizer colourTokenizer(colourString);

      int colourLambda = 0;
      int colourValue = 0;
      while(colourTokenizer.hasMoreTokens()){
         const char* token = colourTokenizer.nextToken();
         if(strcmp(token,"|") == 0){ //vertical bar read. Change to the next fiber and reinitialize gate counter
            colours.push_back(map<int,int>(tempMap)); //Insert created map into vector
            colourLambda = 0;
            tempMap.clear(); // Clear temporary map
         }else{ //Assume it is an int value (it should). Register this colour
            colourValue = atoi(token); // Convert string value into int
            tempMap[colourValue] = colourLambda; // Insert colour as map index and fiber's lambda as value
            colourLambda++;
         }
      }
      // When parsed string is over, insert resulting map in the last vector position
      colours.push_back(map<int,int>(tempMap));
      tempMap.clear(); // All done and clean :)
   }
}

void OBS_CoreInput::handleMessage(cMessage *msg){
    cGate *gate = msg->getArrivalGate();
    // Convert OMNeT input port value to (fiber,lambda) identifier
    int port = getInPort(gate->getIndex());
    int lambda = getInLambda(gate->getIndex());

    if((portLen[port] -1) == lambda){ // If lambda value is the last one for this fiber, output it as a BCP
       send(msg,"out",port); // First numPorts output gates are assigned to BCP channels. Send this message to the corresponding output
    }
    else{ //Not a BCP, then it's a data burst

       // Change burst priorities in order to try to force event processing order. Burst beginning will have priority 3 and burst end priority 0
       if(msg->getKind() == 1){
    	   msg->setSchedulingPriority(3);
       }
       else
    	   msg->setSchedulingPriority(0);
       send(msg,"out",outDataBegin[port] + lambda); // Use outDataBegin to calculate output OMNeT++ gate index.
    }
}

// Given OMNeT++ input gate index, calculate its corresponding fiber number
int OBS_CoreInput::getInPort(int gateIndex){
   int i;
   // Using inPortBegin and portLen in the last case, look for the fiber where this index belongs to.
   for(i=0;i<numPorts-1;i++)
      // Do gateIndex parameter belong to fiber i channels?
      if((gateIndex >= inPortBegin[i]) && (gateIndex < inPortBegin[i+1]))
         return i;

   // If gateIndex parameter is placed in the last portLen[numPorts-1] channels, then it belongs to the last fiber
   if((gateIndex >= inPortBegin[numPorts-1]) && (gateIndex < inPortBegin[numPorts-1] + portLen[numPorts-1])) return numPorts - 1;

   return -1;
}

// Using OMNeT++ input gate index, calculate lambda number inside the corresponding fiber
int OBS_CoreInput::getInLambda(int gateIndex){
  // First calculate fiber number
  int port = getInPort(gateIndex);
  // Now, subtract the index where this port belongs to. This will make the first lambda in the fiber counts as zero, second fiber as one, etc. instead of OMNeT port value.
  return gateIndex - inPortBegin[port];
}

// Convert data links identified by it's fiber Id and lambda (no colour) to OMNeT's output gate to OXC
int OBS_CoreInput::getOXCGate(int port, int lambda){
   Enter_Method("requesting OXC input gate for port %d and lambda %d\n",port,lambda);
   if((portLen[port] -1) == lambda) // If it's the last lambda of the fiber, it's a BCP
       return -1; // Return -1 as error. Modules that call this method shouldn't do this to control links
    else // Else, data channel...
       // Using outDataBegin for the desired fiber, calculate output channel for this link
       return outDataBegin[port] + lambda - numPorts;
}

// This method receive a link identified by fiber id + colour.
// Convert colour to lambda
int OBS_CoreInput::getLambdaByColour(int port, int colour){
	Enter_Method_Silent();
   if (colours[port].count(colour) != 0){//If the colour exists
	   return colours[port][colour];
   }
   else{//If the colour does not exist
	   opp_error("The colour \"%d\" does not exist", colour);
	   return -1;
   }
}
