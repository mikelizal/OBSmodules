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

//
// Optical interface for the Edge Node. It has an assembler that receives packets from the network layer and groups them into bursts. Also, it receives assembled bursts and disassembles them.
//
module OBS_EdgeInterface
{
    parameters:
        //string inColours; // Colour array for input data channels.
        string outColours; // Colour array for output data channels.
        int numInLambdas; // Number of input data channels.
        int numOutLambdas; // Number of output data channels.
        @display("bgb=184,234");
    gates:
        input physIn[];
        output physOut[];
//        input netwIn; // Input from the network layer
//        output netwOut; // Output from the network layer
		input upperLayerIn;
		output upperLayerOut;
    submodules:
        assembler: OBS_BurstAssembler {
            parameters:
                colours = outColours;
                numLambdas = numOutLambdas;
                @display("p=62,80;i=block/fork");
//            gates:
//                out[numOutLambdas+1];
        }
        disassembler: OBS_BurstDisassembler {
            parameters:
                //colours = inColours;
                @display("p=66,160;i=block/join");
//            gates:
//                in[numInLambdas+1];
        }
    connections:
//        netwIn --> assembler.in;
		upperLayerIn --> assembler.in;

        for i=0..numOutLambdas {
//            assembler.out[i] --> physOut++;
            assembler.out++ --> physOut++;
        }

        for i=0..numInLambdas {
//            physIn++ --> disassembler.in[i];
            physIn++ --> disassembler.in++;
        }

//        disassembler.out --> netwOut;
		disassembler.out --> upperLayerOut;
}
