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

package obsmodules.test.CoreOutputTest;

import obsmodules.src.tests.sources.testSource3;
import obsmodules.src.tests.sinks.testSink3;
import obsmodules.src.CoreNode.OBS_CoreOutput;


// Network used to check that the OBS_CoreOutput module works properly.
network CoreOutputTest
{
    parameters:
        int numPorts; //Number of fibers
        int totalLambdas; //Sum of the values on the string "lambdasPerPort"
    submodules:
        source: testSource3 {
            gates:
                out[totalLambdas+numPorts];
        }
        coreOutput: OBS_CoreOutput {
            parameters:
                numPorts = numPorts;
                @display("i=block/departure");
            gates:
                in[totalLambdas+numPorts];
                out[totalLambdas+numPorts];
        }
        sink: testSink3 {
            gates:
                in[totalLambdas+numPorts];
        }
    connections:
        for i=0..totalLambdas+numPorts-1 {
            source.out[i] --> coreOutput.in[i];
            coreOutput.out[i] --> sink.in[i];
        }
}
