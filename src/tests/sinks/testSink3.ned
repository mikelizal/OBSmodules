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

// BCP and burst receiver: receives iniBCP, endBCP, iniBurst (with or without packets inside of it) and endBurst packets.
// Output for OBS_BurstSender, OBS_CoreInput, OBS_CoreOutput, OBS_OpticalMonitor and OBS_DropBurst.
// Line Format:
//
// <pre>
// 1 <receivedTime> <inputGateIndex> <burstifierId> <numSeq> <senderId> <label> <burstSize> ... 
// ... <burstArrivalDelta> <burstColour> <BCPLength>
// 2 <receivedTime> <inputGateIndex> <burstifierId> <numSeq>
// 3 <receivedTime> <inputGateIndex> <burstifierId> <numSeq> <senderId> <burstLength> <numPackets> ... 
// ... <minOffset> <maxOffset>
// P <srcAddr> <destAddr> <burstifierId> <numSeq> <protocol> <srcPort> <destPort> <datagramLength>
// ...
// 4 <receivedTime> <inputGateIndex> <burstifierId> <numSeq>
// </pre>
//
simple testSink3
{
    parameters:
        int comparisonType = default(3);//Type of comparison.
        double timeTolerance @unit(s) = default(0s);//Tolerance to compare time values.
        string patternFile;//File with the pattern to be compared with the output.
        string outputFile;//File where the output will be saved.
        bool compareReport = default(false);//True to compare the report of the module
        string reportPattern = default("");//File with the pattern to be compared with the report.
        @display("i=block/sink");
    gates:
        input in[];
}
