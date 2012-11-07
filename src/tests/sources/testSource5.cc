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
#include "testSource5.h"

Define_Module(testSource5);

void testSource5::initialize(){

	//Open the input file in read mode
	inputFile.open(par("inputFile"));
	if (!inputFile.is_open())
		opp_error("Cannot open the input file for the test");

	//Read the first line and schedule the electrical BCP
	processLine();

}

void testSource5::handleMessage(cMessage *msg){
	if (dynamic_cast <OBS_BurstControlPacket *> (msg) != NULL){
		//Send the electrical BCP from the output gate (this module has only one)
		send(msg,"out");
		//Read the following line and schedule the electrical BCP
		processLine();
	}
	else
		opp_error("Received an unknown packet");
}

void testSource5::finish(){
	//Close the file
	inputFile.close();
}

void testSource5::processLine(){
	string line;
	int val, burstifierId, numSeq, senderId, label, colour, size, port;
	simtime_t sendTime, burstArrival, BCPArrival;
	char sendTimeChr[20], burstArrivalChr[20], BCPArrivalChr[20], burstifierIdChr[20], numSeqChr[20], senderIdChr[20], labelChr[20], colourChr[20], sizeChr[20];
	long long length;
	stringstream char2int;

	if (inputFile.good()){//If there is no problem and the end of file is not reached, get BCP's information
			//Read the line and get the values
			getline(inputFile,line);
			lineNum++;
			val = sscanf(line.data(), "%s %s %s %s %s %s %s %s %lld %d %s", sendTimeChr, burstifierIdChr, numSeqChr, senderIdChr, labelChr, burstArrivalChr, colourChr, sizeChr, &length, &port, BCPArrivalChr);
			if (val != 11){
				opp_error("Wrong format on line %d", lineNum);
			}
			sendTime = SimTime::parse(sendTimeChr);
			if(simTime()<=sendTime){//If the bcp is not scheduled to the past, schedule it

				//Create the electrical BCP and fill it
				OBS_BurstControlPacket *bcp = new OBS_BurstControlPacket();
				bcp->setKind(3);
				bcp->setByteLength(length);
				burstArrival = SimTime::parse(burstArrivalChr);
				bcp->setBurstArrivalDelta(burstArrival);
				if(!strcmp(colourChr,"*") == 0){ //Optional parameter
					char2int.clear();
					char2int << colourChr;
					char2int >> colour;
					bcp->setBurstColour(colour);
				}
				if(!strcmp(labelChr,"*") == 0){ //Optional parameter
					char2int.clear();
					char2int << labelChr;
					char2int >> label;
					bcp->setLabel(label);
				}
				if(!strcmp(burstifierIdChr,"*") == 0){ //Optional parameter
					char2int.clear();
					char2int << burstifierIdChr;
					char2int >> burstifierId;
					bcp->setBurstifierId(burstifierId);
				}
				if(!strcmp(numSeqChr,"*") == 0){ //Optional parameter
					char2int.clear();
					char2int << numSeqChr;
					char2int >> numSeq;
					bcp->setNumSeq(numSeq);
				}
				if(!strcmp(senderIdChr,"*") == 0){ //Optional parameter
					char2int.clear();
					char2int << senderIdChr;
					char2int >> senderId;
					bcp->setSenderId(senderId);
				}
				if(!strcmp(sizeChr,"*") == 0){ //Optional parameter
					char2int.clear();
					char2int << sizeChr;
					char2int >> size;
					bcp->setBurstSize(size);
				}

				OBS_BCPControlInfo *info = new OBS_BCPControlInfo();
				info->setPort(port);
				if(!strcmp(sizeChr,"*") == 0){ //Optional parameter
					BCPArrival = SimTime::parse(BCPArrivalChr);
					info->setBCPArrival(BCPArrival);
				}

				bcp->setControlInfo(info);

				//Schedule the BCP
				scheduleAt(sendTime,bcp);
			}
			else{
				opp_error("Cannot send a BCP scheduled to the past");
			}
		}
}
