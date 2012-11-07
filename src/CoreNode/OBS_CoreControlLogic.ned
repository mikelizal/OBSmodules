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
// Core node's control logic. Sets/unsets OXC connections based on the info carried by the BCP.
//
// Once this module receives the electrical BCP, sends a query to the routing table about the output fiber, colour and label for the incoming burst. If the returned output colour is *, the lambda with
// the horizon closest to the burst arrival time will be selected.
// Finally, the module updates the BCP info (label, output colour, arrival time) and forwards it to the next module. Also, it generates auto-messages for the OXC channel setting and unsetting.
//
// It gives the possibility to save in an output file the information about the
// bursts that have crossed it. The format of each line of the file will be the following one:
//
// <pre>
// <burstifierId> <seqNum> <tBCP> <tBurst> <dropped> <reason>
// </pre>
//
// where
//
// <ul>
//  <li>burstifierId – identifier of the burstifier which has created it.</li>
//  <li>seqNum – sequence number.</li>
//  <li>tBCP – time when the BCP has been received.</li>
//  <li>tBurst – time when the BCP has been received.</li>
//  <li>dropped – tells if the burst has been dropped (1) or not (0).</li>
//  <li>reason – tells why the burst has been dropped (in case that dropped = 1):
//	 <ol>
//    <li>the burst has passed the BCP.</li>
//    <li>there is not any available output data channel.</li>
//    <li>the output data channel configured is not available.</li>
//	 </ol>
//  </li>
// </ul>
//
simple OBS_CoreControlLogic
{
    parameters:
        double guardTime @unit(s); //Guard time put between OXC line reservation and burst arrival time.
        double BCPProcessingDelay @unit(s); //Processing time of BCP
        double dataRate @unit(bps); //Data rate of optical channel. Used to calculate OXC's connection unset time.
        string reportFile; // Name of the file to save information about the bursts that cross the core node. "" not to save anything.
    gates:
        input in;
        output out;
}
