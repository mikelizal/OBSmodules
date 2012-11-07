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

#include "OBS_Burst.h"

Register_Class(OBS_Burst);

void OBS_Burst::insertMessage(cMessage *msg){
   messages_var.insert(msg);
}

cMessage* OBS_Burst::retrieveMessage(){
   //Take the first element of the queue and retrieve it
   if(!messages_var.empty()){
        cMessage *msg = (cMessage*)messages_var.pop();
	return msg;
   }else return NULL; //If empty, return NULL
}

bool OBS_Burst::hasMessages(){
   if(messages_var.empty()) return false;
   else return true;
}

OBS_Burst::~OBS_Burst(){
   messages_var.clear();
}
