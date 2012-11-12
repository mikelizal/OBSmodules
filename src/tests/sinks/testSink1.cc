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
#include "testSink1.h"

Define_Module(testSink1);

void testSink1::initialize(){
	//Open the output file in write mode
	outputFile.open(par("outputFile"));
	if (!outputFile.is_open())
		opp_error("Cannot create the output file for the test");
}

void testSink1::handleMessage(cMessage *msg){
//	if (dynamic_cast <IPDatagram *> (msg) != NULL){
//    IPDatagram *datagram = (IPDatagram *) msg;
    if (dynamic_cast <IPv4Datagram *> (msg) != NULL){
		IPv4Datagram *datagram = (IPv4Datagram *) msg;

		int protocol = datagram->getTransportProtocol();
		if(protocol == IP_PROT_TCP){
			TCPSegment *segment = check_and_cast <TCPSegment *> (datagram->getEncapsulatedPacket());
			//Save datagram's information in the file
			outputFile << simTime() << "\t" << datagram->getArrivalGate()->getIndex() << "\t" << datagram->getSrcAddress() << "\t" << datagram->getDestAddress() << "\t" << protocol << "\t" << segment->getSrcPort() << "\t" << segment->getDestPort() << "\t" << datagram->getByteLength() << endl;
		}
		else if(protocol == IP_PROT_UDP){
			UDPPacket *packet = check_and_cast <UDPPacket *> (datagram->getEncapsulatedPacket());
			//Save datagram's information in the file
			outputFile << simTime() << "\t" << datagram->getArrivalGate()->getIndex() << "\t" << datagram->getSrcAddress() << "\t" << datagram->getDestAddress() << "\t" << protocol << "\t" << packet->getSourcePort() << "\t" << packet->getDestinationPort() << "\t" << datagram->getByteLength() << endl;
		}
		else{
			cout << "<testSink1><" << simTime() << "> WARNING: Received a packet with a protocol different from TCP or UDP." << endl;
			outputFile << simTime() << "\t" << datagram->getArrivalGate()->getIndex() << "\t" << datagram->getSrcAddress() << "\t" << datagram->getDestAddress() << "\t" << protocol << "\t" << "-1" << "\t" << "-1" << "\t" << datagram->getByteLength() << endl;
		}
	}
	else{
		cout << "<testSink1><" << simTime() << "> WARNING: Received a message which is not an IPDatagram." << endl;
	}
	delete msg;
}

void testSink1::finish(){
	bool OK = true;

	//Close the output file
	outputFile.close();

	//Open both files in read mode
	openFiles();

	//CompareFiles
	OK = compareFiles();

	//Close files
	patternFile.close();
	currentFile.close();

	//Print the result
	cModule *parent = getParentModule();
	cout << parent->getName() << ": ";
	if (OK){
		cout << " OK" << endl;
		//Delete the output files
		deleteFiles();
	}
	else {
		cout << " ERROR" << endl;
		cout << "\t" << errorPhrase << endl;
		//Return 3 as status if it is simulating in Cmdenv
		cEnvir *env = simulation.getActiveSimulation()->getActiveEnvir();
		if (!env->isGUI()){
			exit(3);
		}
	}
}

void testSink1::openFiles(){
	int compType = par("comparisonType");

	if (compType == 1 || compType == 2){
		//Open both files in read mode
		currentFile.open(par("outputFile"));
		if (!currentFile.is_open())
			opp_error("Cannot open the output file for the test");
		patternFile.open(par("patternFile"));
		if (!patternFile.is_open())
			opp_error("Cannot open the pattern file for the test");
	}
	else{
		opp_error("Unknown comparison type");
	}
}

bool testSink1::compareFiles(){
	bool OK = true;
	string lineStr, currentLine, patternLine;
	int line = 0;
	stringstream int2str;

	errorPhrase = "";

	int compType = par("comparisonType");

	if(compType == 1){
		//Compare files
		while(patternFile.good() && currentFile.good()){
			getline(patternFile,patternLine);
			getline(currentFile,currentLine);
			line++;
			if (patternLine.compare(currentLine) != 0){//Different lines
				OK = false;
				errorPhrase = "Line ";
				int2str << line;
				int2str >> lineStr;
				errorPhrase += lineStr;
				errorPhrase += " doesn't match the pattern.";
				break;
			}
		}

		//Skip the lines at the end of the file
		while (OK && patternFile.good()){
			getline(patternFile, patternLine);
			line++;
			if (!patternLine.empty()){//Not a white line
				OK = false;
				errorPhrase = "Pattern file: Unexpected values on line ";
				int2str << line;
				int2str >> lineStr;
				errorPhrase += lineStr;
				errorPhrase += ".";
				break;
			}
		}

		//Skip the lines at the end of the file
		while (OK && currentFile.good()){
			getline(currentFile, currentLine);
			if (!currentLine.empty()){//Not a white line
				OK = false;
				errorPhrase = "Output file: Unexpected values on line ";
				int2str << line;
				int2str >> lineStr;
				errorPhrase += lineStr;
				errorPhrase += ".";
				break;
			}
		}
	}
	else if (compType == 2){
		char receivedTimePChr[20], gateIndexPChr[20], srcAddrPChr[20], destAddrPChr[20], protocolPChr[20], srcPortPChr[20], destPortPChr[20], lengthPChr[20], receivedTimeChr[20], srcAddrChr[20], destAddrChr[20];
		int val, gateIndex, protocol, srcPort, destPort;
		long long int length;
		stringstream char2int;
		string params = "";

		//Take the time tolerance value
		simtime_t timeTol = par("timeTolerance");

		//Compare files
		while(OK && patternFile.good() && currentFile.good()){
			getline(patternFile,patternLine);
			getline(currentFile,currentLine);
			line++;

			if (patternLine.empty() || currentLine.empty()){
				break;
			}

			val = sscanf(patternLine.data(), "%s %s %s %s %s %s %s %s", receivedTimePChr, gateIndexPChr, srcAddrPChr, destAddrPChr, protocolPChr, srcPortPChr, destPortPChr, lengthPChr);
			if (val != 8){
				opp_error("Pattern file: wrong format on line %d", line);
			}

			val = sscanf(currentLine.data(), "%s %d %s %s %d %d %d %lld", receivedTimeChr, &gateIndex, srcAddrChr, destAddrChr, &protocol, &srcPort, &destPort, &length);
			if (val != 8){
				opp_error("Output file: wrong format on line %d", line);
			}

			if(!strcmp(receivedTimePChr,"*") == 0){ //Compare this parameter
				//Convert from char * to SimTime
				simtime_t receivedTimeP = SimTime::parse(receivedTimePChr);
				simtime_t receivedTime = SimTime::parse(receivedTimeChr);

				//Compare times
				//if (fabs(receivedTime - receivedTimeP) <= timeTol){ //OK
				if (fabs(receivedTime - receivedTimeP) > timeTol){ //ERROR
					OK = false;
					params += "\n\t\tReceivedTime";
				}
			}

			if(!strcmp(gateIndexPChr,"*") == 0){ //Compare this parameter
				//Convert from char * to int
				char2int.clear();
				char2int << gateIndexPChr;
				int gateIndexP;
				char2int >> gateIndexP;

				//Compare values
				if (gateIndex != gateIndexP){
					OK = false;
					params += "\n\t\tInputGateIndex";
				}
			}

			if(!strcmp(srcAddrPChr,"*") == 0){ //Compare this parameter
				//Convert from char * to IPAddress
//				IPAddress srcAddrP;
//				IPAddress srcAddr;
                IPv4Address srcAddrP;
                IPv4Address srcAddr;

				srcAddrP.set(srcAddrPChr);
				srcAddr.set(srcAddrChr);

				//Compare addresses
				if (!srcAddrP.equals(srcAddr)){ //ERROR
					OK = false;
					params += "\n\t\tSrcAddr";
				}
			}

			if(!strcmp(destAddrPChr,"*") == 0){ //Compare this parameter
				//Convert from char * to IPAddress
//				IPAddress destAddrP;
//				IPAddress destAddr;
                IPv4Address destAddrP;
                IPv4Address destAddr;

				destAddrP.set(destAddrPChr);
				destAddr.set(destAddrChr);

				//Compare addresses
				if (!destAddrP.equals(destAddr)){ //ERROR
					OK = false;
					params += "\n\t\tDestAddr";
				}
			}

			if(!strcmp(protocolPChr,"*") == 0){ //Compare this parameter
				//Convert from char * to int
				char2int.clear();
				char2int << protocolPChr;
				int protocolP;
				char2int >> protocolP;

				//Compare values
				if (protocol != protocolP){
					OK = false;
					params += "\n\t\tProtocol";
				}
			}

			if(!strcmp(srcPortPChr,"*") == 0){ //Compare this parameter
				//Convert from char * to int
				char2int.clear();
				char2int << srcPortPChr;
				int srcPortP;
				char2int >> srcPortP;

				//Compare values
				if (srcPort != srcPortP){
					OK = false;
					params += "\n\t\tSrcPort";
				}
			}

			if(!strcmp(destPortPChr,"*") == 0){ //Compare this parameter
				//Convert from char * to int
				char2int.clear();
				char2int << destPortPChr;
				int destPortP;
				char2int >> destPortP;

				//Compare values
				if (destPort != destPortP){
					OK = false;
					params += "\n\t\tDestPort";
				}
			}

			if(!strcmp(lengthPChr,"*") == 0){ //Compare this parameter
				//Convert from char * to long long int
				char2int.clear();
				char2int << lengthPChr;
				long long int lengthP;
				char2int >> lengthP;

				//Compare values
				if (length != lengthP){
					OK = false;
					params += "\n\t\tLength";
				}
			}
		}

		if (!params.empty()){
			errorPhrase = "The following parameter(s) doesn't match on line ";
			int2str << line;
			int2str >> lineStr;
			errorPhrase += lineStr;
			errorPhrase += ".";
			errorPhrase += params;
		}

		//Skip the lines at the end of the file
		while (OK && patternFile.good()){
			getline(patternFile, patternLine);
			line++;
			if (!patternLine.empty()){//Not a white line
				OK = false;
				errorPhrase = "Pattern file: Unexpected values on line ";
				int2str << line;
				int2str >> lineStr;
				errorPhrase += lineStr;
				errorPhrase += ".";
				break;
			}
		}

		//Skip the lines at the end of the file
		while (OK && currentFile.good()){
			getline(currentFile, currentLine);
			line++;
			if (!currentLine.empty()){//Not a white line
				OK = false;
				errorPhrase = "Output file: Unexpected values on line ";
				int2str << line;
				int2str >> lineStr;
				errorPhrase += lineStr;
				errorPhrase += ".";
				break;
			}
		}
	}
	else{
		opp_error("Unknown comparison type");
	}

	return OK;
}

void testSink1::deleteFiles(){
	//int compType = par("comparisonType");

	remove(par("outputFile"));
}
