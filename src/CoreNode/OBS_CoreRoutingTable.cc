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

#include "OBS_CoreRoutingTable.h"

Define_Module(OBS_CoreRoutingTable);

OBS_CoreRoutingTable::~OBS_CoreRoutingTable(){
   while(routingTable.length() > 0){
      delete routingTable.pop();
   }
   routingTable.clear();
}

void OBS_CoreRoutingTable::initialize(){
	FILE* routingFile = fopen(par("routeTableFile"),"r");
   if(routingFile == NULL)
	   opp_error("Cannot open forwarding file");

   routingTable.setName("routingTable");

   char* routeString = (char*)calloc(1500,sizeof(char));
   while(fgets(routeString,1500,routingFile) != NULL){
         //Ignore lines which begin with '#'. They're comments
         if(routeString[0] == '#') continue;
         //For some reason fgets does an 'extra' reading after the last element. Try to avoid this 'ghost' element reading
         if(strcmp(routeString,"\n") != 0){
            OBS_CoreRoutingTableEntry* entry = new OBS_CoreRoutingTableEntry();
            //Use a tokenizer object to analyze string
            cStringTokenizer tokenizer(routeString);
            //Routing table format:
            //inPort inColour inLabel outPort outColour outLabel
            //inPort and outPort don't support the wildcard *
            const char *token = tokenizer.nextToken();
            
            //inPort
            entry->setInPort(atoi(token));
            //inColour
            token = tokenizer.nextToken();
            if(strcmp(token,"*") == 0) entry->setInColour(-9);
            else entry->setInColour(atoi(token));
            //inLabel
            token = tokenizer.nextToken();
            if(strcmp(token,"*") == 0) entry->setInLabel(-9);
            else entry->setInLabel(atoi(token));

            //outPort
            token = tokenizer.nextToken();
            entry->setOutPort(atoi(token));
            //outColour
            token = tokenizer.nextToken();
            if(strcmp(token,"*") == 0) entry->setOutColour(-9);
            else entry->setOutColour(atoi(token));
            //outLabel
            token = tokenizer.nextToken();
            if(strcmp(token,"*") == 0) entry->setOutLabel(-9);
            else entry->setOutLabel(atoi(token));

            //Insert into 'routing' table                
            routingTable.insert(entry);
         }
      }
   free(routeString);
   fclose(routingFile);
}

//Iterates through the routing table using an iterator and returns when a match is found
OBS_CoreRoutingTableEntry *OBS_CoreRoutingTable::getEntry(int inPort,int inColour,int inLabel){
	Enter_Method_Silent();
   OBS_CoreRoutingTableEntry *item;

   for(cQueue::Iterator iter_list(routingTable,1) ; !iter_list.end() ; iter_list--){//back-front
      item = (OBS_CoreRoutingTableEntry*) iter_list();

      if((item->getInPort() == inPort) && ((item->getInColour() == inColour) || (item->getInColour() == -9)) && ((item->getInLabel() == inLabel) || (item->getInLabel() == -9))) return item->dup();
   }
   return NULL;
}
