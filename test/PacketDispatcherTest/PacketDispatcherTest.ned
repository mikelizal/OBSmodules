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

package obsmodules.test.PacketDispatcherTest;

import obsmodules.src.tests.sources.testSource1;
import obsmodules.src.tests.sinks.testSink1;
import obsmodules.src.EdgeNode.OBS_PacketDispatcher;


// Network used to check that the OBS_PacketDispatcher module works properly.
network PacketDispatcherTest
{
    parameters:
        int numPacketBurstifiers; //Number of packet burstifiers
    submodules:
        source: testSource1;
        packetDispatcher: OBS_PacketDispatcher {
            parameters:
                numQueues = numPacketBurstifiers;
                @display("i=block/classifier");
            gates:
                out[numPacketBurstifiers];
        }
        sink: testSink1 {
            gates:
                in[numPacketBurstifiers];
        }
    connections:
        source.out --> packetDispatcher.in;

        for i=0..numPacketBurstifiers-1 {
            packetDispatcher.out[i] --> sink.in[i];
        }
}
