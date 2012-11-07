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

package obsmodules.test.OpticalMonitorTest;

import obsmodules.src.tests.sources.testSource3;
import obsmodules.src.tests.sinks.testSink3;
import obsmodules.src.misc.OBS_OpticalMonitor;


// Network used to check that the OBS_OpticalMonitor module works properly.
network OpticalMonitorTest
{
    parameters:
        int numLambdas; //Number of data channels
    submodules:
        source: testSource3 {
            gates:
                out[numLambdas+1];
        }
        opticalMonitor: OBS_OpticalMonitor {
            gates:
                in[numLambdas+1];
                out[numLambdas+1];
        }
        sink: testSink3 {
            gates:
                in[numLambdas+1];
        }
    connections:
        for i=0..numLambdas {
            source.out[i] --> opticalMonitor.in[i];
            opticalMonitor.out[i] --> sink.in[i];
        }
}
