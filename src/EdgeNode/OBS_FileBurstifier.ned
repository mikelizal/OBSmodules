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
// Generates 'fake' bursts using the burstFile info.
//
// burstFile has the following format:
//
// <pre>
// <t-send> <minOffsetMin> <maxOffset> <burstSize>
// </pre>
// Where:
// - <b>t-send</b>: Simulation time when the burst will be sent
// - <b>minOffset</b>: Minimum offset
// - <b>maxOffset</b>: Maximum offset
// - <b>burstSize</b>: Burst size
//
// Also, you can comment lines adding a # at the beginning.
//
// Example:
//
// <pre>
// <b>#Send a 'fake' burst (with no packets) every 0.5 seconds with a length of 375 bytes</b>
// <b>#With a minimum offset of 0.1s and a maximum offset of 0.3s</b>
// 0 0.1 0.3 375
// 0.5 0.1 0.3 375
// 1 0.1 0.3 375
// 1.5 0.1 0.3 375
// 2 0.1 0.3 375
// 2.5 0.1 0.3 375
// 3 0.1 0.3 375
// <b>#And so on...</b>
// </pre>
//
simple OBS_FileBurstifier
{
    parameters:
        int destLabel; //Destination label.
        string burstFile; //Burst generation file.
    gates:
        output out;
}
