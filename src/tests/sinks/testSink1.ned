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

package obsmodules.src.tests.sinks;

// Packet receiver: receives IP datagrams which encapsulate TCP segments or UDP packets. 
// Output for OBS_PacketDispatcher and OBS_BurstDisassembler.
// Line Format:
//
// <pre>
// <receivedTime> <inputGateIndex> <srcAddr> <destAddr> <protocol> <srcPort> ...
// ... <destPort> <datagramLength>
// </pre>
//
simple testSink1
{
    parameters:
        int comparisonType = default(2); //Type of comparison.
        double timeTolerance @unit(s) = default(0s); //Tolerance to compare time values.
        string patternFile; //File with the pattern to be compared with the output.
        string outputFile; //File where the output will be saved.
        @display("i=block/sink");
    gates:
        input in[];
}
