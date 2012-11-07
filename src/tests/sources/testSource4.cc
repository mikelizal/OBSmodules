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
#include "testSource4.h"

Define_Module(testSource4);

void testSource4::initialize(){

	//Open the input file in read mode
	inputFile.open(par("inputFile"));
	if (!inputFile.is_open())
		opp_error("Cannot open the input file for the test");

	//Read the first line and schedule the iniBCP, endBCP
	processLine();
}

void testSource4::handleMessage(cMessage *msg){
	if (dynamic_cast <OBS_BurstControlPacket *> (msg) != NULL){
		//Send the BCPs from the defined output gate (control channel)
		SourceControlInfo *info = check_and_cast <SourceControlInfo *> (msg->removeControlInfo());
		send(msg,"out",info->getSendingIndex()); 
		delete info;
		//If it is a iniBCP read the following line and schedule the iniBCP, endBCP
		if (msg->getKind() == 1){
			processLine();
		}
	}
	else{
		opp_error("Received an unknown packet");
	}
}

void testSource4::finish(){
	//Close the file
	inputFile.close();
}

void testSource4::processLine(){
	string line;
	int index, val, burstifierId, numSeq, label, colour, senderId, burstSize; 
	simtime_t sendTime, BCPDelta, arrivalTime;
	char sendTimeChr[20], BCPDeltaChr[20], arrivalTimeChr[20] ,colourChr[20], labelChr[20], senderIdChr[20], burstSizeChr[20], lengthChr[20];
	long long length;
	stringstream char2int;

	if (inputFile.good()){//If there is no problem and the end of file is not reached, get object's information
			getline(inputFile,line);
			lineNum++;
			val = sscanf(line.data(),"%s %s %d %s %s %s %d %d %s %s %s", sendTimeChr, BCPDeltaChr, &index, arrivalTimeChr, colourChr, labelChr, &burstifierId, &numSeq,  senderIdChr, burstSizeChr, lengthChr);
			if (val != 11){
				opp_error("Wrong format on line %d", lineNum);
			}
			sendTime = SimTime::parse(sendTimeChr);
			if(simTime()<=sendTime){//If the BCP is not scheduled to the past, schedule it

				//Create the BCP beginning
				OBS_BurstControlPacket *iniBCP = new OBS_BurstControlPacket("iniBCP");
				iniBCP->setKind(1);
				SourceControlInfo *info = new SourceControlInfo();
				info->setSendingIndex(index);
				iniBCP->setControlInfo(info);

				//Create the BCP end
				OBS_BurstControlPacket *endBCP = new OBS_BurstControlPacket("endBCP");
				endBCP->setKind(2);
				SourceControlInfo *info2 = info->dup();
				endBCP->setControlInfo(info2);

				arrivalTime = SimTime::parse(arrivalTimeChr);
				iniBCP->setBurstArrivalDelta(arrivalTime);

				if(!strcmp(colourChr,"*") == 0){ //Optional parameter
					char2int.clear();
					char2int << colourChr;
					char2int >> colour;
					iniBCP->setBurstColour(colour);
				}

				if(!strcmp(labelChr,"*") == 0){ //Optional parameter
					char2int.clear();
					char2int << labelChr;
					char2int >> label;
					iniBCP->setLabel(label);
				}

				iniBCP->setBurstifierId(burstifierId);
				iniBCP->setNumSeq(numSeq);

				endBCP->setBurstifierId(burstifierId);
				endBCP->setNumSeq(numSeq);

				if(!strcmp(burstSizeChr,"*") == 0){ //Optional parameter
					char2int.clear();
					char2int << burstSizeChr;
					char2int >> burstSize;
					iniBCP->setBurstSize(burstSize);
				}

				if(!strcmp(lengthChr,"*") == 0){ //Optional parameter
					char2int.clear();
					char2int << lengthChr;
					char2int >> length;
					iniBCP->setByteLength(length);
				}
			
				if(!strcmp(senderIdChr,"*") == 0){ //Optional parameter
					char2int.clear();
					char2int << senderIdChr;
					char2int >> senderId;
					iniBCP->setSenderId(senderId);
				}

				//Schedule the BCP beginning
				scheduleAt(sendTime, iniBCP);
				//Schedule the BCP end
				BCPDelta = SimTime::parse(BCPDeltaChr);
				scheduleAt(sendTime+BCPDelta, endBCP);
			}
			else{
				opp_error("Cannot send a BCP scheduled to the past");
			}
		}
}
