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
#include<stdio.h>
#include<string.h>
#include "OBS_DispatcherRule.h"

using namespace std;

//! Uses a rule file to assign an output gate for each message received.
class OBS_PacketDispatcher : public cSimpleModule{
   protected:
      OBS_DispatcherRule *rules; //!< Dispatcher rules.
      int numQueues; //!< Number of output gates (must be the same as the rules set on rules file).

      //Statistics
      cStdDev recvPackSize; //!< Received packets' length statistics.
      int droppedPacket; //!< Number of dropped packets.
      cOutVector recvPackSizeVec; //!< Length of the packets received (in bytes).

      virtual void initialize();
      virtual void finish();

      virtual void handleMessage(cMessage *msg);
   public:
      virtual ~OBS_PacketDispatcher();
};
