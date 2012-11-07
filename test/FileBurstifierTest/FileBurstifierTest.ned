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

package obsmodules.test.FileBurstifierTest;

import obsmodules.src.tests.sinks.testSink2;
import obsmodules.src.EdgeNode.OBS_FileBurstifier;


// Network used to check that the OBS_FileBurstifier module works properly.
network FileBurstifierTest
{
    submodules:
        fileBurstifier: OBS_FileBurstifier {
            parameters:
                @display("i=block/queue");
        }
        sink: testSink2;
    connections:
        fileBurstifier.out --> sink.in;
}
