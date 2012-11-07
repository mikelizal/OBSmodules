//
// Copyright (C) 2012 Naiara Garcia Royo
// Copyright (C) 2012 Universidad Publica de Navarra
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

package obsmodules.test.BurstSenderTest;

import obsmodules.src.tests.sources.testSource2;
import obsmodules.src.tests.sinks.testSink3;
import obsmodules.src.EdgeNode.OBS_BurstSender;


// Network used to check that the OBS_BurstSender module works properly.
network BurstSenderTest
{
    parameters:
        int numBurstifiers; //Number of burstifiers
        int numLambdas; //Number of data channels
    submodules:
        source: testSource2 {
            gates:
                out[numBurstifiers];
        }
        burstSender: OBS_BurstSender {
            parameters:
                numLambdas = numLambdas;
                testing = true;
                @display("i=block/process;q=ScheduledBursts");
            gates:
                in[numBurstifiers];
                out[numLambdas+1];
        }
        sink: testSink3 {
            gates:
                in[numLambdas+1];
        }
    connections:
        for i=0..numBurstifiers-1 {
            source.out[i] --> burstSender.in[i];
        }
        for i=0..numLambdas {
            burstSender.out[i] --> sink.in[i];
        }
}
