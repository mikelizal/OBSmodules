//
// Copyright (C) 2010-2013 Javier Armendariz Silva, Naiara Garcia Royo, Felix Espina Antolin
// Copyright (C) 2010-2013 Universidad Publica de Navarra
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

package obsmodules.Examples.EdgeNodeTest;

import obsmodules.src.misc.OBS_OpticalMonitor;
import obsmodules.src.EdgeNode.OBS_EdgeNode;
import inet.nodes.inet.StandardHost;

// Edge Node testing network.
// It consists of two hosts interconnected by an OBS network. Each host has its own edge node.
// Between each edge, an optical monitor module is connected in order to collect information about bursts that pass through the network.
//
network EdgeNodeTest
{
    parameters:
        int numLambdas; // Number of data channels (same for all fibers)
    submodules:
        host1: StandardHost {
            parameters:
                routingFile = "H1.irt";
                @display("p=46,72;i=device/pc2");
        }
        edge1: OBS_EdgeNode {
            parameters:
                routingTable.routingFile = "Edge1.irt";
                @display("p=191,168;i=block/classifier2");
            gates:
                obsIn[numLambdas+1];
                obsOut[numLambdas+1];
        }
        monitor: OBS_OpticalMonitor {
            parameters:
                @display("p=246,240;i=block/process");
            gates:
                in[numLambdas+1];
                out[numLambdas+1];
        }
        edge2: OBS_EdgeNode {
            parameters:
                routingTable.routingFile = "Edge2.irt";
                @display("p=302,168;i=block/classifier2");
            gates:
                obsIn[numLambdas+1];
                obsOut[numLambdas+1];
        }
        host2: StandardHost {
            parameters:
                routingFile = "H2.irt";
                @display("p=440,112;i=device/pc2");
        }
    connections:
        host1.ethg++ <--> { datarate=100Gbps; } <--> edge1.ethIO++;

        host2.ethg++ <--> { datarate=100Gbps; } <--> edge2.ethIO++;

        for i=0..numLambdas {
            edge1.obsOut[i] --> monitor.in[i];
            monitor.out[i] --> edge2.obsIn[i];
            edge2.obsOut[i] --> edge1.obsIn[i];
        }
}
