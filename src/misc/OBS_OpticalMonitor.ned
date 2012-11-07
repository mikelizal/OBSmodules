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

package obsmodules.src.misc;

//
// This module acts as an optical sniffer. It can be connected to an optical fiber (data channels + control channel) and all data will pass through it transparently.
// Meanwhile, the module registers information about received traffic and prints it to an output file as vectors.
// 
// The output file has the following format:
//
// <pre>
// vector 1 "OBS_OpticalMonitor id:00" "Received Burst info" 1
// vector 2 "OBS_OpticalMonitor id:00" "Received BCP info" 1
// <data lines> ...
// </pre>
//
// Before data lines, there are two lines of vector declaration. Both of them begin with the keyword 'vector' followed by the vector Id, then a string with the name
// of this module and their identifier (according to the id() function) during the simulation, followed by the vector description. The line ends with a 1 representing the multiplicity 
// on cOutVector objects (in this case, always 1).
//
// Data lines can have a different format depending on the vector they belong to:
// - Vector 1 (Burst info)
//
// <pre> 1 <simTime> <burstifierId> <numSeq> <length> </pre>
//
// - Vector 2 (BCP info)
//
// <pre> 2 <simTime> <burstifierId> <numSeq> <colour> </pre>
//
//
// As you can see, the format of this file is pretty similar to OMNeT's vector files, so you can post-process the output file just like any .vec file.
//
simple OBS_OpticalMonitor
{
    parameters:
        string reportFile; // Output filename
        @display("i=block/blackboard");
    gates:
        input in[];
        output out[];
}
