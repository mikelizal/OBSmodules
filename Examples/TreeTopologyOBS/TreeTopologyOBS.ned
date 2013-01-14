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

package obsmodules.Examples.TreeTopologyOBS;

import obsmodules.src.EdgeNode.OBS_EdgeNode;
import obsmodules.src.CoreNode.OBS_CoreNode;
import inet.nodes.inet.StandardHost;


network TreeTopologyOBS
{
    parameters:
        int lambdasEdge1; //Number of data channels in the fiber connecting edge1 and core1
        int lambdasEdge2; //Number of data channels in the fiber connecting edge2 and core2
        int lambdasEdge3; //Number of data channels in the fiber connecting edge3 and core3
        int lambdasEdge4; //Number of data channels in the fiber connecting edge4 and core4
        int lambdasEdge5; //Number of data channels in the fiber connecting edge5 and core5
        int lambdasCore1to2; //Number of data channels in the fiber connecting core1 and core2
        int lambdasCore1to3; //Number of data channels in the fiber connecting core1 and core3
        int lambdasCore1to4; //Number of data channels in the fiber connecting core1 and core4
        int lambdasCore4to5; //Number of data channels in the fiber connecting core4 and core5
    submodules:
        // 5 hosts which send and receive traffic
        host1: StandardHost {
            parameters:
                routingFile = "H1.irt";
                @display("p=76,157");
        }
        host2: StandardHost {
            parameters:
                routingFile = "H2.irt";
                @display("p=615,81");
        }
        host3: StandardHost {
            parameters:
                routingFile = "H3.irt";
                @display("p=107,456");
        }
        host4: StandardHost {
            parameters:
                routingFile = "H4.irt";
                @display("p=1013,149");
        }
        host5: StandardHost {
            parameters:
                routingFile = "H5.irt";
                @display("p=993,490");
        }
        // Edge Nodes
        edge1: OBS_EdgeNode {
            parameters:
                numInLambdas = lambdasEdge1;
                numOutLambdas = lambdasEdge1;
                routingTable.routingFile = "Edge1.irt";
                @display("p=207,239");
            gates:
                obsIn[lambdasEdge1+1];
                obsOut[lambdasEdge1+1];
        }
        edge2: OBS_EdgeNode {
            parameters:
                numInLambdas = lambdasEdge2;
                numOutLambdas = lambdasEdge2;
                routingTable.routingFile = "Edge2.irt";
                @display("p=471,81");
            gates:
                obsIn[lambdasEdge2+1];
                obsOut[lambdasEdge2+1];
        }
        edge3: OBS_EdgeNode {
            parameters:
                numInLambdas = lambdasEdge3;
                numOutLambdas = lambdasEdge3;
                routingTable.routingFile = "Edge3.irt";
                @display("p=250,456");
            gates:
                obsIn[lambdasEdge3+1];
                obsOut[lambdasEdge3+1];
        }
        edge4: OBS_EdgeNode {
            parameters:
                numInLambdas = lambdasEdge4;
                numOutLambdas = lambdasEdge4;
                routingTable.routingFile = "Edge4.irt";
                @display("p=801,149");
            gates:
                obsIn[lambdasEdge4+1];
                obsOut[lambdasEdge4+1];
        }
        edge5: OBS_EdgeNode {
            parameters:
                numInLambdas = lambdasEdge5;
                numOutLambdas = lambdasEdge5;
                routingTable.routingFile = "Edge5.irt";
                @display("p=829,490");
            gates:
                obsIn[lambdasEdge5+1];
                obsOut[lambdasEdge5+1];
        }
        //5 Core Nodes
        core1: OBS_CoreNode {
            parameters:
                ControlUnit.RoutingTable.routeTableFile = "core1Route.dat";
                numPorts = 4;
                @display("p=317,247");
        }
        core2: OBS_CoreNode {
            parameters:
                ControlUnit.RoutingTable.routeTableFile = "core2Route.dat";
                numPorts = 2;
                @display("p=386,104");
        }
        core3: OBS_CoreNode {
            parameters:
                ControlUnit.RoutingTable.routeTableFile = "core3Route.dat";
                numPorts = 2;
                @display("p=414,413");
        }
        core4: OBS_CoreNode {
            parameters:
                ControlUnit.RoutingTable.routeTableFile = "core4Route.dat";
                numPorts = 3;
                @display("p=664,247");
        }
        core5: OBS_CoreNode {
            parameters:
                ControlUnit.RoutingTable.routeTableFile = "core5Route.dat";
                numPorts = 2;
                @display("p=722,388");
        }
    connections:
        //First of all, connect hosts with Edge Nodes
        host1.ethg++ <--> { datarate=100Gbps; } <--> edge1.ethIO++;

        host2.ethg++ <--> { datarate=100Gbps; } <--> edge2.ethIO++;

        host3.ethg++ <--> { datarate=100Gbps; } <--> edge3.ethIO++;

        host4.ethg++ <--> { datarate=100Gbps; } <--> edge4.ethIO++;

        host5.ethg++ <--> { datarate=100Gbps; } <--> edge5.ethIO++;

        //Core Node connections. It will determine fiber ID in Core's routing table

		//First of all, make connections between Edge -> Core. So, those connections will be identified as 0

        //Fiber 0 of core 1 from Core1 to Edge1:
        for i=0..lambdasEdge1 {
            edge1.obsOut[i] --> core1.in++;
            core1.out++ --> edge1.obsIn[i];
        }
        //Fiber 0 of core 2 from Core2 to Edge2:
        for i=0..lambdasEdge2 {
            edge2.obsOut[i] --> core2.in++;
            core2.out++ --> edge2.obsIn[i];
        }
        //Fiber 0 of core 3 from Core3 to Edge3:
        for i=0..lambdasEdge3 {
            edge3.obsOut[i] --> core3.in++;
            core3.out++ --> edge3.obsIn[i];
        }
        //Fiber 0 of core 4 from Core4 to Edge4:
        for i=0..lambdasEdge4 {
            edge4.obsOut[i] --> core4.in++;
            core4.out++ --> edge4.obsIn[i];
        }
        //Fiber 0 of core 5 from Core5 to Edge5:
        for i=0..lambdasEdge5 {
            edge5.obsOut[i] --> core5.in++;
            core5.out++ --> edge5.obsIn[i];
        }

        //Finally, interconnect Core Nodes
        //Core 1
        //Fiber 1 to core 2:
        for i=0..lambdasCore1to2 {
            core1.out++ --> core2.in++;
            core2.out++ --> core1.in++;
        }
        //Fiber 2 to core 3:
        for i=0..lambdasCore1to3 {
            core1.out++ --> core3.in++;
            core3.out++ --> core1.in++;
        }
        //Fiber 3 to core 4:
        for i=0..lambdasCore1to4 {
            core1.out++ --> core4.in++;
            core4.out++ --> core1.in++;
        }

		//Core 2
		// Fiber 0 to Core1 is already done making Fiber 1 in Core 1


		//Core 3
		//Fiber 0 is done. It is Core1 <-> Core3

        //Core 4 and 5
        //Fiber 2 of Core 4 and Fiber 4 of Core 4
        for i=0..lambdasCore4to5 {
            core4.out++ --> core5.in++;
            core5.out++ --> core4.in++;
        }
}

