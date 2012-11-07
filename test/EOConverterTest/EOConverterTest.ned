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

package obsmodules.test.EOConverterTest;

import obsmodules.src.tests.sources.testSource5;
import obsmodules.src.tests.sinks.testSink4;
import obsmodules.src.CoreNode.OBS_EOConverter;


// Network used to check that the OBS_EOConverter module works properly.
network EOConverterTest
{
    parameters:
        int numPorts; //Number of fibers (control channels)
    submodules:
        source: testSource5;
        EOConverter: OBS_EOConverter {
            parameters:
                numPorts = numPorts;
                @display("i=block/departure");
            gates:
                out[numPorts];
        }
        sink: testSink4 {
            gates:
                in[numPorts];
        }
    connections:
        source.out --> EOConverter.in;
        for i=0..numPorts-1 {
            EOConverter.out[i] --> sink.in[i];
        }
}
