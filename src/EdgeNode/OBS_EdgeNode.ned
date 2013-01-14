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

package obsmodules.src.EdgeNode;

import inet.nodes.inet.NetworkLayer;
import inet.networklayer.ipv4.RoutingTable;
import inet.networklayer.common.InterfaceTable;
import inet.linklayer.ppp.PPPInterface;
import inet.linklayer.ethernet.EthernetInterface;
import inet.base.NotificationBoard;


//
// Edge node for a OBS network. It's just a Router (from the INET Framework) including an OBS Interface.
//
module OBS_EdgeNode
{
    parameters:
        int numInLambdas; // Number of input data channels for the OBS interface.
        int numOutLambdas; // Number of output data channels for the OBS interface.
        //string inColours; // Colour array for input data channels.
        string outColours; // Colour array for output data channels.
        @display("i=block/classifier2");
    gates:
        inout pppIO[];	// PPP port
        inout ethIO[];	// Ethernet port
        input obsIn[];  // OBS input
        output obsOut[]; //OBS output
    submodules:
        notificationBoard: NotificationBoard {
            parameters:
                @display("p=60,60;i=block/control");
        }
        interfaceTable: InterfaceTable {
            parameters:
                @display("p=150,60;i=block/table");
        }
        routingTable: RoutingTable {
            parameters:
                IPForward = true;
                routerId = "auto";
                @display("p=240,60;i=block/table");
        }
        networkLayer: NetworkLayer {
            parameters:
                @display("p=192,149;i=block/fork;q=queue");
//            gates:
//                // From the network layer, only one line goes to OBS Interface (the +1 in the following lines)
//                ifIn[(sizeof(pppIO)+sizeof(ethIO))+1];
//                ifOut[(sizeof(pppIO)+sizeof(ethIO))+1];
        }
        ppp[sizeof(pppIO)]: PPPInterface {
            parameters:
                @display("p=82,257,row,110;q=l2queue;i=block/ifcard");
        }
        eth[sizeof(ethIO)]: EthernetInterface {
            parameters:
                @display("p=185,257,row,110;q=l2queue;i=block/ifcard");
        }
        obs: OBS_EdgeInterface {
            parameters:
                numInLambdas = numInLambdas;
                numOutLambdas = numOutLambdas;
                //inColours = inColours; 
                outColours = outColours;
                @display("p=283,257;q=l2queue;i=block/ifcard");
//            gates:
//                physIn[sizeof(obsIn)];
//                physOut[sizeof(obsOut)];
        }
    connections allowunconnected:
        // connections to the outer world
        for i=0..sizeof(pppIO)-1 {
            pppIO[i] <--> ppp[i].phys;
//            ppp[i].upperLayerOut --> networkLayer.ifIn[i];
//            ppp[i].upperLayerIn <-- networkLayer.ifOut[i];
            ppp[i].upperLayerOut --> networkLayer.ifIn++;
            ppp[i].upperLayerIn <-- networkLayer.ifOut++;
        }

        for i=0..sizeof(ethIO)-1 {
            ethIO[i] <--> eth[i].phys;
//            eth[i].upperLayerOut --> networkLayer.ifIn[sizeof(pppIO)+i];
//            eth[i].upperLayerIn <-- networkLayer.ifOut[sizeof(pppIO)+i];
            eth[i].upperLayerOut --> networkLayer.ifIn++;
            eth[i].upperLayerIn <-- networkLayer.ifOut++;
        }

        for i=0..sizeof(obsIn)-1 {
//            obsIn[i] --> obs.physIn[i];
            obsIn[i] --> obs.physIn++;
        }

        for i=0..sizeof(obsOut)-1 {
//            obs.physOut[i] --> obsOut[i];
            obs.physOut++ --> obsOut[i];
        }

        //Connection between OBS Interface and Network layer. Only one line, as mentioned before
//        obs.netwOut --> networkLayer.ifIn[sizeof(pppIO)+sizeof(ethIO)];
//        obs.netwIn <-- networkLayer.ifOut[sizeof(pppIO)+sizeof(ethIO)];
		obs.upperLayerOut --> networkLayer.ifIn++;
		obs.upperLayerIn <-- networkLayer.ifOut++;
}
