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
#include<stdlib.h>
#include<stdio.h>
#include "OBS_CoreRoutingTableEntry_m.h"
#include<list>
using namespace std;


//! Core node routing table. Converts the routing info stored in a file to a file structure and provides direct methods to request information.
class OBS_CoreRoutingTable : public cSimpleModule{
   private:
      cQueue routingTable;  //!< Table data structure.
   protected:
      virtual void initialize();
   public:
      virtual ~OBS_CoreRoutingTable();
      //! Given input fiber/colour/label, return the corresponding output from routing table.
      OBS_CoreRoutingTableEntry *getEntry(int inPort,int inColour,int inLabel);
};

