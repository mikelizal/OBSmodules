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

package obsmodules.test.BurstDisassemblerTest;

import obsmodules.src.tests.sources.testSource3;
import obsmodules.src.tests.sinks.testSink1;
import obsmodules.src.EdgeNode.OBS_BurstDisassembler;


// Network used to check that the OBS_BurstDisassembler module works properly.
network BurstDisassemblerTest
{
    parameters:
        int numLambdas; //Number of data channels
    submodules:
        source: testSource3 {
            gates:
                out[numLambdas+1];
        }
        burstDisassembler: OBS_BurstDisassembler {
            parameters:
                @display("i=block/join");
            gates:
                in[numLambdas+1];
        }
        sink: testSink1 {
            gates:
                in[1];
        }
    connections:
        burstDisassembler.out --> sink.in[0];

        for i=0..numLambdas {
            source.out[i] --> burstDisassembler.in[i];
        }
}
