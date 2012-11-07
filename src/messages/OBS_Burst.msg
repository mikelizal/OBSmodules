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

class cQueue;

// Burst message. Contains a message queue and stores info such as the burst Id (using OMNeT's id() function), number of packets, offsets and destination label.
// This message is extended by the OBS_Burst class (OBS_Burst.cc and .h) which implements methods to access the message queue.
packet OBS_Burst
{
    @customize(true); //Use this to extend this message with a subclass
    cQueue messages; //Message queue
    int numPackets = -1; //Number of packets inside the burst
    simtime_t minOffset = -1; //Minimum offset between BCP and Burst
    simtime_t maxOffset = -1; //Maximum offset between BCP and burst  
    int burstifierId = -1; //Burstifier id
    int numSeq = -1; //Sequence number into burst sender
    int senderId = -1; //Burst sender identifier. Should not be used as Id
}
