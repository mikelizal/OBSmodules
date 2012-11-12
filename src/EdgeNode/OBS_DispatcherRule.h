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

//#include<IPAddress.h>
#include<IPv4Address.h>
#include<string>
#include<string.h>
//#include<IPDatagram_m.h>
#include<IPv4Datagram.h>
#include<TCPSegment.h>
#include<UDPPacket.h>

using namespace std;

//! Represents a single dispatcher rule which provides a match checking method.
class OBS_DispatcherRule{
   protected:
      bool isSet[5]; //!< Define whether fields are taken into account or not. Each field's indexes are commented below.
//      IPAddress srcAddr; //!< Source address (isSet[0]).
//      IPAddress destAddr; //!< Destination address (isSet[1]).
      IPv4Address srcAddr; //!< Source address (isSet[0]).
      IPv4Address destAddr; //!< Destination address (isSet[1]).
      int protocol;//!< IP protocol (isSet[2]).
      short srcPort; //!< Source port (isSet[3]).
      short destPort; //!< Destination port (isSet[4]).

   public:
      //! @param rule A string which follows Dispatcher file format (see documentation).
      OBS_DispatcherRule(string rule);
      OBS_DispatcherRule(char* rule);
      virtual ~OBS_DispatcherRule();

      //! Matching method: tells whether or not msg matches the object's rule.
      bool match(cMessage *msg);
};
