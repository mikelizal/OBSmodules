//
// Copyright (C) 2010-2012 Javier Armendariz Silva, Naiara Garcia Royo
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

package obsmodules.src.EdgeNode;

//
// This module groups packets into optical bursts and generates signalling messages (BCP).
//
// Check submodule pages for more details.
//
module OBS_BurstAssembler
{
    parameters:
        string colours; // Array of colour identificators for each output channel. Should be a list of int numbers separated by spaces. Use a empty string if you don't want to use colours.
        int numLambdas; // Number of output data channels (control channel not included).
        int numPacketBurstifiers; // Number of OBS_PacketBurstifier modules present in the module.
        int numFileBurstifiers; // Number of OBS_FileBurstifier modules present in the module.
        @display("bgb=522,413");
    gates:
        input in; // Electronic input
        output out[]; //Optical output
    submodules:
        dispatcher: OBS_PacketDispatcher {
            parameters:
                numQueues = numPacketBurstifiers;
                @display("p=72,208;i=block/classifier");
            gates:
                out[numPacketBurstifiers];
        }
        packetBurstifier[numPacketBurstifiers]: OBS_PacketBurstifier {
            parameters:
                @display("i=block/queue;q=burstifierQueue");
        }
        fileBurstifier[numFileBurstifiers]: OBS_FileBurstifier {
            parameters:
                @display("p=243,275;i=block/queue");
        }
        sender: OBS_BurstSender {
            parameters:
                colours = colours;
                numLambdas = numLambdas;
                @display("p=405,188;i=block/process;q=ScheduledBursts");
            gates:
                in[numPacketBurstifiers+numFileBurstifiers];
                out[numLambdas+1];
        }
    connections:
        // Connect the input with the dispatcher
        in --> dispatcher.in;

        // Connect OBS_PacketBurstifiers first
        for i=0..numPacketBurstifiers-1 {
            dispatcher.out[i] --> packetBurstifier[i].in;
            packetBurstifier[i].out --> sender.in[i];
        }

        // Then connect OBS_FileBurstifiers (if any available)
        for i=numPacketBurstifiers..(numPacketBurstifiers+numFileBurstifiers)-1 {
            fileBurstifier[i-numPacketBurstifiers].out --> sender.in[i];
        }

        // Connect OBS_BurstSender to module's output
        for i=0..numLambdas {
            sender.out[i] --> out++;
        }
}
