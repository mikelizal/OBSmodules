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

package obsmodules.Examples.jitel09;

import obsmodules.src.EdgeNode.OBS_EdgeNode;
import obsmodules.src.CoreNode.OBS_CoreNode;
import inet.nodes.inet.StandardHost;


network jitel09Simulation
{
    parameters:
        int lambdasEdge1; //Number of data channels in the fiber connecting edge1 and core1
        int lambdasEdge2; //Number of data channels in the fiber connecting edge2 and core1
        int lambdasEdge3; //Number of data channels in the fiber connecting edge3 and core2
        int lambdasEdge4; //Number of data channels in the fiber connecting edge4 and core2
        int lambdasCore; //Number of data channels in the fiber connecting core1 and core2
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
        host3: StandardHost {
            parameters:
                routingFile = "H3.irt";
                @display("p=639,120");
        }
        host4: StandardHost {
            parameters:
                routingFile = "H4.irt";
                @display("p=636,264");
        }
        edge1: OBS_EdgeNode {
            parameters:
                numInLambdas = lambdasEdge1;
                numOutLambdas = lambdasEdge1;
                obs.assembler.dispatcher.dispatcherRules = "DispatcherRules.dat";
                routingTable.routingFile = "Edge1.irt";
                @display("p=159,112");
            gates:
                obsIn[lambdasEdge1+1];
                obsOut[lambdasEdge1+1];
        }
        edge2: OBS_EdgeNode {
            parameters:
                numInLambdas = lambdasEdge2;
                numOutLambdas = lambdasEdge2;
                obs.assembler.dispatcher.dispatcherRules = "DispatcherRules.dat";
                routingTable.routingFile = "Edge2.irt";
                @display("p=158,272");
            gates:
                obsIn[lambdasEdge2+1];
                obsOut[lambdasEdge2+1];
        }
        edge3: OBS_EdgeNode {
            parameters:
                numInLambdas = lambdasEdge3;
                numOutLambdas = lambdasEdge3;
                obs.assembler.dispatcher.dispatcherRules = "DispatcherRules.dat";
                routingTable.routingFile = "Edge3.irt";
                @display("p=505,120");
            gates:
                obsIn[lambdasEdge3+1];
                obsOut[lambdasEdge3+1];
        }
        edge4: OBS_EdgeNode {
            parameters:
                numInLambdas = lambdasEdge4;
                numOutLambdas = lambdasEdge4;
                obs.assembler.dispatcher.dispatcherRules = "DispatcherRules.dat";
                routingTable.routingFile = "Edge4.irt";
                @display("p=503,264");
            gates:
                obsIn[lambdasEdge4+1];
                obsOut[lambdasEdge4+1];
        }
        core1: OBS_CoreNode {
            parameters:
                ControlUnit.RoutingTable.routeTableFile = "core1Route.dat";
                numPorts = 3;
                @display("p=267,192");
        }
        core2: OBS_CoreNode {
            parameters:
                ControlUnit.RoutingTable.routeTableFile = "core2Route.dat";
                numPorts = 3;
                @display("p=411,192");
        }
    connections:
        //Connections between hosts and Edge Nodes
        host1.ethg++ <--> { datarate=100Gbps; } <--> edge1.ethIO++;

        host2.ethg++ <--> { datarate=100Gbps; } <--> edge2.ethIO++;

        host3.ethg++ <--> { datarate=100Gbps; } <--> edge3.ethIO++;

        host4.ethg++ <--> { datarate=100Gbps; } <--> edge4.ethIO++;

        //Core 1 connections
        for i=0..lambdasEdge1 {
            edge1.obsOut[i] --> core1.in++;
            core1.out++ --> edge1.obsIn[i];
        }
        for i=0..lambdasEdge2 {
            edge2.obsOut[i] --> core1.in++;
            core1.out++ --> edge2.obsIn[i];
        }

        //Core 2 connections
        for i=0..lambdasEdge3 {
            edge3.obsOut[i] --> core2.in++;
            core2.out++ --> edge3.obsIn[i];
        }
        for i=0..lambdasEdge4 {
            edge4.obsOut[i] --> core2.in++;
            core2.out++ --> edge4.obsIn[i];
        }

        //Connections between core nodes
        for i=0..lambdasCore {
            core1.out++ --> core2.in++;
            core2.out++ --> core1.in++;
        }
}
