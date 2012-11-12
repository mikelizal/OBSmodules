//
// Copyright (C) 2010-2012 Javier Armendariz Silva, Naiara Garcia Royo, Felix Espina Antolin
// Copyright (C) 2010-2012 Universidad Publica de Navarra
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

package obsmodules.Examples.BurstifierTest;

import obsmodules.src.tests.PacketConverter;
import obsmodules.src.EdgeNode.OBS_PacketBurstifier;
import inet.base.Sink;
//import inet.applications.generic.IPTrafGen;
import inet.applications.generic.IPvXTrafGen;


// Network used to check that the packet burstifier works properly.
// We use INET's IPTrafGen as a traffic generator. But this module creates messages that the burstifier can't read. For that reason, a converter module is needed.
network BurstifierTest
{
    submodules:
//        generator: IPTrafGen {
        generator: IPvXTrafGen {
                parameters:
                @display("p=51,72;i=block/source");
        }
        converter: PacketConverter {
            parameters:
                @display("p=98,168;i=block/cogwheel");
        }
        burstifier: OBS_PacketBurstifier {
            parameters:
                @display("p=97,240;i=block/queue");
        }
        sink: Sink {
            parameters:
                @display("p=143,360;i=block/sink");
            gates:
                in[1];
        }
    connections allowunconnected:
        generator.ipOut --> converter.in;
        converter.out --> burstifier.in;
        burstifier.out --> sink.in[0];
}

