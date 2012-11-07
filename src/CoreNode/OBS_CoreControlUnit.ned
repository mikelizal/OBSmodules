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

package obsmodules.src.CoreNode;

//
// Control unit compound module. It enables to separate OE/EO conversion and the control logic into separate modules.
//
// See submodules documentation for more info.
//
module OBS_CoreControlUnit
{
    parameters:
        double dataRate @unit(bps); //Optical channel data rate
        int numPorts; // Number of fibers
        string lambdasPerOutPort; //Number of output data channels for each fiber
        @display("bgb=339,249");
    gates:
        input in[]; //Optical input
        output out[]; //Optical output
    submodules:
        OE: OBS_OEConverter {
            parameters:
                @display("p=64,192;i=block/arrival");
            gates:
                in[sizeof(in)];
        }
        ControlLogic: OBS_CoreControlLogic {
            parameters:
                dataRate = dataRate;
                @display("p=180,160;i=block/cogwheel");
        }
        EO: OBS_EOConverter {
            parameters:
                dataRate = dataRate;
                numPorts = numPorts;
                @display("p=293,192;i=block/departure");
            gates:
                out[sizeof(out)];
        }
        RoutingTable: OBS_CoreRoutingTable {
            parameters:
                @display("p=80,72;i=block/table2");
        }
        GatesHorizon: OBS_CoreOutputHorizon {
            parameters:
                numPorts = numPorts;
                lambdasPerPort = lambdasPerOutPort;
                @display("p=260,72;i=block/table2");
        }
    connections:
        for i=0..sizeof(in)-1 {
            in++ --> OE.in[i];
        }

        OE.out --> ControlLogic.in;
        ControlLogic.out --> EO.in;

        for i=0..sizeof(out)-1 {
            EO.out[i] --> out++;
        }
}
