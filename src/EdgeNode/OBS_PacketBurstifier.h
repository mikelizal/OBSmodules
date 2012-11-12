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

#include <omnetpp.h>
//#include<IPDatagram_m.h>
#include<IPv4Datagram.h>
#include "OBS_Burst.h"
#include "OBS_BurstifierInfo_m.h"


//! This class stores incoming packets and assembles an optical burst using a given criteria (time, number of packets, total size).
class OBS_PacketBurstifier : public cSimpleModule{
   protected:
	  //Main parameters:
      cQueue burstContent; //!< Packet queue.
   
      simtime_t timeout; //!< Burstifier's timeout. If reached, burst will be assembled with the current content of the queue.
      int maxSize; //!< Burst maximum size.
      int numPackets; //!< Maximum number of packets.
      simtime_t minOffset; //!< BCP's minimum offset.
      simtime_t maxOffset; //!< BCP's maximum offset.
      int minSizeWithPadding; //!<  Padding value. Fills burst size to this value if not reached during packet collecting. If 0 doesn't do padding.
      bool overflowLastPacket; //!< If true, allows the last packet to enter the burst even if size is exceeded by that. If false, burst will be assembled and the current message will be inserted as the first message of the next burst.
      int cteHeader; //!< Burst header size (in bytes).
      int varHeader; //!< 'Variable' header size (in bytes). Attached to total burst size when a packet is inserted into the burst.

      //Current burst information
      int burstBits; //!< Current burst size (in bits).
      int numBurstPackets; //!< Number of packets of the current burst.

      int destLabel; //!< Burst destination label.
 
      cMessage *timeout_msg; //!< Timeout message.

      //Statistics
      int burstCounter; //!< Created burst counter.
      cStdDev recvPackSize; //!< Received packets' length statistics.
      cOutVector recvPackSizeVec; //!< Length of the packets received (in bytes).
      cStdDev queuedTime; //!< Time between the first packet is queued and each of the packets are queued.
      cOutVector avgPacketTime; //!< Average packet delay due the burst assembly.

      simtime_t firstPacket_t; //!< Arrival time of the first packet for the current burst.

      virtual void initialize();
      virtual void finish();
      virtual void handleMessage(cMessage *msg);
   public:
      virtual ~OBS_PacketBurstifier();
   private:
      //! Builds an OBS_Burst object with all elements inside burstContent.
      void assembleBurst();
};
