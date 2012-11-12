//
// Copyright (C) 2010-2012 Javier Armendariz Silva, Naiara Garcia Royo, Felix Espina Antolin
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

#include <list>
#include "OBS_Burst_m.h"
using namespace std;

//! This class extends OBS_Burst_Base (generated from OBS_Burst.msg) adding methods to access the message queue.
class OBS_Burst : public OBS_Burst_Base
 {
     public:
     ///\internal Declarations copied as-is the template proposed in OBS_Burst_Base
     OBS_Burst(const char *name=NULL, int kind=0) : OBS_Burst_Base(name,kind) {}
//     OBS_Burst(const OBS_Burst& other) : OBS_Burst_Base(other.getName()) {operator=(other);}
     OBS_Burst(const OBS_Burst& other) : OBS_Burst_Base(other) {operator=(other);}
     ~OBS_Burst();
     OBS_Burst& operator=(const OBS_Burst& other){OBS_Burst_Base::operator=(other);return *this;}

     virtual OBS_Burst *dup() const {return new OBS_Burst(*this);}

     virtual void insertMessage(cMessage *msg); //!< Inserts a message in the queue.
     virtual cMessage* retrieveMessage(); //!< Retrieves and deletes the first message of the queue. NULL if queue is empty.
     virtual bool hasMessages(); //!< Returns true if queue is not empty. Otherwise, returns false.
 };
