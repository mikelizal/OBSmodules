//
// Copyright (C) 2012 Naiara Garcia Royo
// Copyright (C) 2012 Universidad Publica de Navarra
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

package obsmodules.src.tests.sources;

// BCP generator: generates iniBCP and endBCP packets.
// Input for OBS_OEConverter.
// Line Format:
//
// <pre>
// <iniBCPsendTime> <endBCPsendDelta> <gateIndex> <burstArrivalDelta> <burstColour> ... 
// ... <label> <burstifierId> <numSeq> <senderId> <burstSize> <BCPLength>
// </pre>
//
simple testSource4
{
    parameters:
        string inputFile; //File with the information about the BCPs.
        @display("i=block/source");
    gates:
        output out[];
}
