//
// Copyright (C) 2010-2013 Felix Espina
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

package obsmodules.Examples.EdgeNodeExample;

import obsmodules.src.EdgeNode.OBS_EdgeNode;
import inet.nodes.inet.StandardHost;


network EdgeNodeExample
{
    parameters:
        int lambdasEdge; //Number of data channels in the fiber connecting edges
    submodules:
        host1: StandardHost {
            parameters:
                routingFile = "H1.irt";
                @display("p=46,112");
        }
        host2: StandardHost {
            parameters:
                routingFile = "H2.irt";
                @display("p=48,272");
        }
        edge1: OBS_EdgeNode {
            parameters:
                numInLambdas = lambdasEdge;
                numOutLambdas = lambdasEdge;
                obs.assembler.dispatcher.dispatcherRules = "DispatcherRules.dat";
                routingTable.routingFile = "Edge1.irt";
                @display("p=159,112");
            gates:
                obsIn[lambdasEdge+1];
                obsOut[lambdasEdge+1];
        }
        edge2: OBS_EdgeNode {
            parameters:
                numInLambdas = lambdasEdge;
                numOutLambdas = lambdasEdge;
                obs.assembler.dispatcher.dispatcherRules = "DispatcherRules.dat";
                routingTable.routingFile = "Edge2.irt";
                @display("p=158,272");
            gates:
                obsIn[lambdasEdge+1];
                obsOut[lambdasEdge+1];
        }
    connections:
        //Connections between hosts and Edge Nodes
        host1.ethg++ <--> { datarate=1Gbps; } <--> edge1.ethIO++;
        host2.ethg++ <--> { datarate=1Gbps; } <--> edge2.ethIO++;

        //Core 1 connections
        for i=0..lambdasEdge {
            edge1.obsOut[i] --> edge2.obsIn[i];
            edge2.obsOut[i] --> edge1.obsIn[i];
        }
}