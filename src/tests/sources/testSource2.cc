//
// Copyright (C) 2012 Naiara Garcia Royo, Felix Espina Antolin
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
#include "testSource2.h"

Define_Module(testSource2);

void testSource2::initialize(){

	//Open the input file in read mode
	inputFile.open(par("inputFile"));
	if (!inputFile.is_open())
		opp_error("Cannot open the input file for the test");

	//Read the first line and schedule the burst
	processLine();

}

void testSource2::handleMessage(cMessage *msg){
	if (dynamic_cast <OBS_Burst *> (msg) != NULL){
		//Send the burst from the defined output gate
		//assigning the label as control info
		SourceControlInfo *info = check_and_cast <SourceControlInfo *> (msg->removeControlInfo());
		OBS_BurstifierInfo *lInfo = new OBS_BurstifierInfo();
		lInfo->setLabel(info->getLabel());
		msg->setControlInfo(lInfo);
		send(msg,"out",info->getSendingIndex());
		delete info;
		//Read the following line and schedule the burst
		processLine();
	}
	else
		opp_error("Received an unknown packet");
}

void testSource2::finish(){
	//Close the file
	inputFile.close();
}

void testSource2::processLine(){
	string line;
	int index, val, burstifierId, numSeq, label, numPackets;
	simtime_t sendTime, minOffset, maxOffset;
	char sendTimeChr[20], minOffsetChr[20], maxOffsetChr[20];
	long long length;

	if (inputFile.good()){//If there is no problem and the end of file is not reached, get bursts's information
			//Read the line and get the values
			getline(inputFile,line);
			lineNum++;
			val = sscanf(line.data(),"%s %d %d %d %d %d %s %s %lld", sendTimeChr, &index, &burstifierId, &numSeq, &numPackets, &label, minOffsetChr, maxOffsetChr, &length);
			if (val != 9){
				string firstPos = line.substr(0,1);
				if (firstPos.compare("P") == 0){
					opp_error("Unexpected packet definition on line %d", lineNum);
				}
				else{
					opp_error("Wrong format on line %d", lineNum);
				}
			}
			sendTime = SimTime::parse(sendTimeChr);
			if(simTime()<=sendTime){//If the burst is not scheduled to the past, schedule it
				//Create the burst and fill it
				OBS_Burst *burst = new OBS_Burst();
				burst->setByteLength(length);
				burst->setNumPackets(numPackets);
				minOffset = SimTime::parse(minOffsetChr);
				burst->setMinOffset(minOffset);
				maxOffset = SimTime::parse(maxOffsetChr);
				burst->setMaxOffset(maxOffset);
				burst->setBurstifierId(burstifierId);
				burst->setNumSeq(numSeq);

				//Create the datagrams, fill them and insert them in the burst
				int i=1;
				while ((i<=numPackets)&&(inputFile.good())){
					int protocol;
					char srcAddr[20], destAddr[20], datagramLengthChr[20], protocolChr[20], srcPortChr[20], destPortChr[20];
					unsigned short srcPort, destPort;
					long long datagramLength;
					stringstream char2int;

					getline(inputFile,line);
					lineNum++;
					val = sscanf(line.data(),"P %s %s %s %s %s %s", srcAddr, destAddr, protocolChr, srcPortChr, destPortChr, datagramLengthChr);
					if (val != 6){
						string firstPos = line.substr(0,1);
						if (firstPos.compare("P") != 0){
							opp_error("Packet definition missing on line %d", lineNum);
						}
						else{
							opp_error("Wrong format on line %d", lineNum);
						}
					}
					//Create the IP datagram and fill it
//					IPDatagram *datagram = new IPDatagram();
                    IPv4Datagram *datagram = new IPv4Datagram();
					if(!strcmp(datagramLengthChr,"*") == 0){ //Optional parameter
						char2int.clear();
						char2int << datagramLengthChr;
						char2int >> datagramLength;
						datagram->setByteLength(datagramLength);
					}
					if(!strcmp(srcAddr,"*") == 0){ //Optional parameter
//						datagram->setSrcAddress(srcAddr);
					    IPv4Address srcAddrP;
					    srcAddrP.set(srcAddr);
                        datagram->setSrcAddress(srcAddrP);
					}
					if(!strcmp(srcAddr,"*") == 0){ //Optional parameter
//						datagram->setDestAddress(destAddr);
                        IPv4Address destAddrP;
                        destAddrP.set(destAddr);
                        datagram->setDestAddress(destAddrP);
					}
					if(!strcmp(protocolChr,"*") == 0){ //Optional parameter
						char2int.clear();
						char2int << protocolChr;
						char2int >> protocol;
						datagram->setTransportProtocol(protocol);
						//Create the encapsulate packet and encapsulate it
						if (protocol == IP_PROT_TCP){
							TCPSegment *packet = new TCPSegment();
							if(!strcmp(srcPortChr,"*") == 0){ //Optional parameter
								char2int.clear();
								char2int << srcPortChr;
								char2int >> srcPort;
								packet->setSrcPort(srcPort);
							}
							if(!strcmp(destPortChr,"*") == 0){ //Optional parameter
								char2int.clear();
								char2int << destPortChr;
								char2int >> destPort;
								packet->setDestPort(destPort);
							}
							datagram->setTransportProtocol(IP_PROT_TCP);
							datagram->encapsulate(packet);
						}
						else if (protocol == IP_PROT_UDP){
							UDPPacket *packet = new UDPPacket();
							if(!strcmp(srcPortChr,"*") == 0){ //Optional parameter
								char2int.clear();
								char2int << srcPortChr;
								char2int >> srcPort;
								packet->setSourcePort(srcPort);
							}
							if(!strcmp(destPortChr,"*") == 0){ //Optional parameter
								char2int.clear();
								char2int << destPortChr;
								char2int >> destPort;
								packet->setDestinationPort(destPort);
							}
							datagram->setTransportProtocol(IP_PROT_UDP);
							datagram->encapsulate(packet);
						}
						else{
							cout << "<testSource2><" << simTime() << "> WARNING: Attempted to attach a packet that is neither TCP nor UDP." << endl;
						}
					}
					burst->insertMessage(datagram);
					i++;
				}

				//Add control info
				SourceControlInfo *info = new SourceControlInfo();
				info->setSendingIndex(index);
				info->setLabel(label);
				burst->setControlInfo(info);

				//Schedule the burst
				scheduleAt(sendTime,burst);
			}
			else{
				opp_error("Cannot send a burst scheduled to the past");
			}
		}
}
