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
// Assigns an output gate for each received message using a rules file. If a message doesn't match any rule, it will be discarded.
// If a message matches more than one rule, it will be sent following the first one.
//
// Rules file is defined on dispatcherRules parameter. This file must be a plain text file and each line represents a rule. Rules use the following format:
// 
// <pre>
// <label> <value> [<label> <value> ...]
// </pre>
//
// Where <label> can take any of these values:
// - <b>srcAddr</b> : source IP address
// - <b>destAddr</b> : destination IP address
// - <b>protocol</b> : IP protocol field
// - <b>srcPort</b> : source TCP/UDP port
// - <b>destPort</b> : destination TCP/UDP port
//
// Also, you can put comments on this file typing # at the beginning of the commented line.
//
// An example rules file:
//
// <pre>
// <b># TCP (protocol 6) packet with http traffic from 10.0.3.3</b>
// destPort 80 srcAddr 10.0.3.3 protocol 6
// <b># XMPP TCP traffic (port 5222) from 10.0.1.1 to 10.0.2.1, and the same with destAddr 10.0.2.3</b>
// destAddr 10.0.2.1 srcAddr 10.0.1.1 destPort 5222
// srcAddr 10.0.1.1 destAddr 10.0.2.3 destPort 5222  
// </pre>
//
// Note that labels can be placed in any order and rules don't need to use the same labels.
//
simple OBS_PacketDispatcher
{
    parameters:
        int numQueues; // Number of burstifier queues (should be the same as the dispatcher rules and the output gates).
        string dispatcherRules; // Plain text file with the rules (see description).
    gates:
        input in;
        output out[];
}

