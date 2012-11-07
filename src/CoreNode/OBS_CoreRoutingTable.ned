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
// Lookup table used to route received bursts.
//
// Routes are defined using the routingFile file. This file contains a routing table entry in each line . Entries have the following format:
//
// <pre>
// <inPort> <inColour> <inLabel> <outPort> <outColour> <outLabel>
// </pre>
//
// All fields accept integer values representing the input/output port, the lambda's colour and the destination label. Also, in/out colours and labels can accept the wildcard *, representing all colours or input labels of the given port. In the output label field, '*' means that the burst will preserve its label value.
// Comments can be included typing # at the beggining of the line.
//
// Example:
//
// <pre>
// <b>#All bursts from port 0 will be redirected to port 2 renaming the label to 3</b>
// 0 * * 2 * 3
// <b>#Bursts from port 1 and label 3 will go out to port 3 and colour 1 preserving the same label </b>
// 1 * 3 3 1 *
// </pre>
//
simple OBS_CoreRoutingTable
{
    parameters:
        string routeTableFile; // Routing file name
}
