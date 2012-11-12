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
#include "testSource1.h"

Define_Module(testSource1);

void testSource1::initialize(){

	//Open the input file in read mode
	inputFile.open(par("inputFile"));
	if (!inputFile.is_open())
		opp_error("Cannot open the input file for the test");

	//Read the first line and schedule the packet
	processLine();

}

void testSource1::handleMessage(cMessage *msg){
//	if (dynamic_cast <IPDatagram *> (msg) != NULL){
    if (dynamic_cast <IPv4Datagram *> (msg) != NULL){
		//Send the packet from the output gate (this module has only one)
		send(msg,"out");
		//Read the following line and schedule the packet
		processLine();
	}
	else
		opp_error("Received an unknown packet");
}

void testSource1::finish(){
	//Close the file
	inputFile.close();
}

void testSource1::processLine(){
	string line;
	int val, protocol;
	simtime_t sendTime;
	char sendTimeChr[20], srcAddr[20], destAddr[20], protocolChr[20], srcPortChr[20], destPortChr[20];
	unsigned short srcPort, destPort;
	long long length;
	stringstream char2int;

	if (inputFile.good()){//If there is no problem and the end of file is not reached, get packet's information
			//Read the line and get the values
			getline(inputFile,line);
			lineNum++;
			val = sscanf(line.data(),"%s %s %s %s %s %s %lld", sendTimeChr, srcAddr, destAddr, protocolChr, srcPortChr, destPortChr, &length);
			if (val != 7){
				opp_error("Wrong format on line %d", lineNum);
			}
			sendTime = SimTime::parse(sendTimeChr);
			if(simTime()<=sendTime){//If the datagram is not scheduled to the past, schedule it
				//Create the datagram and fill it
//				IPDatagram *datagram = new IPDatagram();
                IPv4Datagram *datagram = new IPv4Datagram();
				datagram->setByteLength(length);
				if(!strcmp(srcAddr,"*") == 0){ //Optional parameter
//					datagram->setSrcAddress(srcAddr);
                    IPv4Address srcAddrP;
                    srcAddrP.set(srcAddr);
                    datagram->setSrcAddress(srcAddrP);
				}
				if(!strcmp(destAddr,"*") == 0){ //Optional parameter
//					datagram->setDestAddress(destAddr);
                    IPv4Address destAddrP;
                    destAddrP.set(destAddr);
                    datagram->setDestAddress(destAddrP);
				}
				//Create the packet and encapsulate it
				if(!strcmp(protocolChr,"*") == 0){ //Optional parameter
					char2int.clear();
					char2int << protocolChr;
					char2int >> protocol;
					datagram->setTransportProtocol(protocol);
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
						datagram->encapsulate(packet);
					}
					else{
						cout << "<testSource1><" << simTime() << "> WARNING: Attempted to attach a packet that is neither TCP nor UDP." << endl;
					}
				}

				//Schedule the first datagram
				scheduleAt(sendTime,datagram);
			}
			else{
				opp_error("Cannot send a packet scheduled to the past");
			}
		}
}
