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

package obsmodules.test.CoreNodeTest;

import obsmodules.src.tests.sources.testSource3;
import obsmodules.src.tests.sinks.testSink3;
import obsmodules.src.CoreNode.OBS_CoreNode;


// Network used to check that the OBS_CoreNode module works properly.
network CoreNodeTest
{
    parameters:
        int numPorts; //Number of input (output) fibers
        int totalInLambdas; //Sum of the values on the string "lambdasPerInPort"
        int totalOutLambdas; //Sum of the values on the string "lambdasPerOutPort"
    submodules:
        source: testSource3 {
            gates:
                out[totalInLambdas+numPorts];
        }
        coreNode: OBS_CoreNode {
            parameters:
                numPorts = numPorts; //Number of fibers
                @display("i=block/switch");
            gates:
                in[totalInLambdas+numPorts];
                out[totalOutLambdas+numPorts];
        }
        sink: testSink3 {
            gates:
                in[totalOutLambdas+numPorts];
        }

    connections:
        for i=0..totalInLambdas+numPorts-1 {
            source.out[i] --> coreNode.in[i];
        }

        for i=0..totalOutLambdas+numPorts-1 {
            coreNode.out[i] --> sink.in[i];
        }
}
