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

package obsmodules.src.EdgeNode;

//
// Receives TCP/UDP packets and groups them in optical bursts.
//
// This module will behave as shown in this diagram:
//
// <center><img src=../img/PacketBurstifier.jpg></center>
//
simple OBS_PacketBurstifier
{
    parameters:
        double timeout @unit(s); // Burstifier's timeout. If reached, burst will be assembled with the current content of the queue.
        int maxSize @unit(B); // Burst maximum size.
        int numPackets; // Maximum number of packets.
        double minOffset @unit(s); // BCP's minimum offset.
        double maxOffset @unit(s); // BCP's maximum offset.
        int minSizeWithPadding @unit(B); // Minimum size a burst should have. If not reached, will be filled until this value. If 0, no padding will be done.
        bool overflowLastPacket; // If true, allows the last packet to enter the burst even if size is exceeded by that. If false, burst will be assembled and the current message will be inserted as the first message of the next burst.
        int cteHeader @unit(B); // Burst header size.
        int varHeader @unit(B); // Header added for each message inside the burst.
        int destLabel; // Burst destination label.
    gates:
        input in;
        output out;
}
