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
#include "testSink2.h"

Define_Module(testSink2);

void testSink2::initialize(){
	//Open the output file in write mode
	outputFile.open(par("outputFile"));
	if (!outputFile.is_open())
		opp_error("Cannot create the output file for the test");
}

void testSink2::handleMessage(cMessage *msg){
	if ((dynamic_cast <OBS_Burst *> (msg) != NULL)&&(dynamic_cast <OBS_BurstifierInfo *> (msg->getControlInfo()) != NULL)){
		OBS_Burst *burst = (OBS_Burst *) msg;
		OBS_BurstifierInfo *info = (OBS_BurstifierInfo *) (burst->removeControlInfo());

		//Save burst's information in the file
		int bId = burst->getBurstifierId();
		int seq = burst->getNumSeq();
		outputFile << simTime() << "\t" << bId << "\t" << seq << "\t" << burst->getNumPackets() << "\t" << burst->getMinOffset() << "\t" << burst->getMaxOffset() << "\t" << info->getLabel() << "\t" << burst->getByteLength() << endl;

		int protocol;
		while (burst->hasMessages()){
//			IPDatagram *datagram = check_and_cast <IPDatagram *> (burst->retrieveMessage());
            IPv4Datagram *datagram = check_and_cast <IPv4Datagram *> (burst->retrieveMessage());
			protocol = datagram->getTransportProtocol();
			if (protocol == IP_PROT_TCP){
				TCPSegment *segment = check_and_cast <TCPSegment *> (datagram->getEncapsulatedPacket());
				//Save datagram's information in the file
				outputFile << "P\t" << bId << "\t" << seq << "\t" << datagram->getSrcAddress() << "\t" << datagram->getDestAddress() << "\t" << protocol << "\t" << segment->getSrcPort() << "\t" << segment->getDestPort() << "\t" << datagram->getByteLength() << endl;
				//delete segment;
			}
			else if (protocol == IP_PROT_UDP){
				UDPPacket *packet = check_and_cast <UDPPacket *> (datagram->getEncapsulatedPacket());
				//Save datagram's information in the file
				outputFile << "P\t" << bId << "\t" << seq << "\t" << datagram->getSrcAddress() << "\t" << datagram->getDestAddress() << "\t" << protocol << "\t" << packet->getSourcePort() << "\t" << packet->getDestinationPort() << "\t" << datagram->getByteLength() << endl;
				//delete packet;
			}
			else{
				cout << "<testSink2><" << simTime() << "> WARNING: Received a packet with a protocol different from TCP or UDP." << endl;
				outputFile << "P\t" << bId << "\t" << seq << "\t" << datagram->getSrcAddress() << "\t" << datagram->getDestAddress() << "\t" << protocol << "\t" << "-1" << "\t" << "-1" << "\t" << datagram->getByteLength() << endl;
			}
			delete datagram;
		}
	}
	else{
		cout << "<testSink2><" << simTime() << "> WARNING: Received a message which is not an OBS_Burst." << endl;
	}
	delete msg;
}

void testSink2::finish(){
	bool OK = true;

	//Close the output file
	outputFile.close();

	//Open both files in read mode
	openFiles();

	//Compare files
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

void testSink2::openFiles(){
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
	else if (compType == 3){
		//Get the file names without apostrophes
		string outputName = par("outputFile").str();
		string patternName = par("patternFile").str();
		size_t pos = outputName.find_first_of('"');
		outputName = outputName.erase(pos,1);
		pos = outputName.find_last_of('"');
		outputName = outputName.erase(pos,1);
		pos = patternName.find_first_of('"');
		patternName = patternName.erase(pos,1);
		pos = patternName.find_last_of('"');
		patternName = patternName.erase(pos,1);
		string ordOutputName = outputName + "_Ord";
		string ordPatternName= patternName + "_Ord";

		//Order the files
		string llamada1 = "sort -k 2,3 -o " + ordOutputName + " " + outputName;
		string llamada2 = "sort -k 2,3 -o " + ordPatternName + " " + patternName;
		system(llamada1.c_str());
		system(llamada2.c_str());

		//Open both files in read mode
		currentFile.open(ordOutputName.c_str());
		if (!currentFile.is_open())
			opp_error("Cannot open the ordered output file for the test");
		//patternFile.open("pOrd");
		patternFile.open(ordPatternName.c_str());
		if (!patternFile.is_open())
			opp_error("Cannot open the ordered pattern file for the test");
	}
	else{
		opp_error("Unknown comparison type");
	}
}

bool testSink2::compareFiles(){
	bool OK = true;
	string lineStr, currentLine, patternLine;
	int line = 0;
	stringstream int2str;

	errorPhrase = "";

	int compType = par("comparisonType");

	if (compType == 1){
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
	else if (compType == 2 || compType == 3){
		char receivedTimeChr[20], receivedTimePChr[20], labelPChr[20], burstifierIdPChr[20], numSeqPChr[20], lengthPChr[20], minOffsetPChr[20], minOffsetChr[20], maxOffsetPChr[20], maxOffsetChr[20], numPacketsPChr[20],srcAddrPChr[20], destAddrPChr[20], protocolPChr[20], srcPortPChr[20], destPortPChr[20], srcAddrChr[20], destAddrChr[20];
		int val, label, burstifierId, numSeq, numPackets, protocol, srcPort, destPort;
		long long int length;
		stringstream char2int, int2str;
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

			string firstPosP = patternLine.substr(0,1);
			string firstPos = currentLine.substr(0,1);
			//1. Compare burst output
			if (firstPosP.compare("P") != 0){
				if (firstPos.compare("P") == 0){
					OK = false;
					errorPhrase = "Line ";
					int2str << line;
					int2str >> lineStr;
					errorPhrase += lineStr;
					errorPhrase += " type doesn't match with the pattern";
					break;
				}

				val = sscanf(patternLine.data(), "%s %s %s %s %s %s %s %s", receivedTimePChr, burstifierIdPChr, numSeqPChr, numPacketsPChr, minOffsetPChr, maxOffsetPChr, labelPChr, lengthPChr);
				if (val != 8){
					opp_error("Pattern file: wrong format on line %d", line);
				}

				val = sscanf(currentLine.data(), "%s %d %d %d %s %s %d %lld", receivedTimeChr, &burstifierId, &numSeq, &numPackets, minOffsetChr, maxOffsetChr, &label, &length);
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

				if(!strcmp(numPacketsPChr,"*") == 0){ //Compare this parameter
					//Convert from char * to int
					char2int.clear();
					char2int << numPacketsPChr;
					int numPacketsP;
					char2int >> numPacketsP;

					//Compare values
					if (numPackets != numPacketsP){
						OK = false;
						params += "\n\t\tNumPackets";
					}
				}

				if(!strcmp(minOffsetPChr,"*") == 0){ //Compare this parameter
					//Convert from char * to SimTime
					simtime_t minOffsetP = SimTime::parse(minOffsetPChr);
					simtime_t minOffset = SimTime::parse(minOffsetChr);

					//Compare times
					//if (fabs(minOffset - minOffsetP) <= timeTol){ //OK
					if (fabs(minOffset - minOffsetP) > timeTol){ //ERROR
						OK = false;
						params += "\n\t\tMinOffset";
					}
				}

				if(!strcmp(maxOffsetPChr,"*") == 0){ //Compare this parameter
					//Convert from char * to SimTime
					simtime_t maxOffsetP = SimTime::parse(maxOffsetPChr);
					simtime_t maxOffset = SimTime::parse(maxOffsetChr);

					//Compare times
					//if (fabs(maxOffset - maxOffsetP) <= timeTol){ //OK
					if (fabs(maxOffset - maxOffsetP) > timeTol){ //ERROR
						OK = false;
						params += "\n\t\tMaxOffset";
					}
				}

				if(!strcmp(labelPChr,"*") == 0){ //Compare this parameter
					//Convert from char * to int
					char2int.clear();
					char2int << labelPChr;
					int labelP;
					char2int >> labelP;

					//Compare values
					if (label != labelP){
						OK = false;
						params += "\n\t\tLabel";
					}
				}

				///\internal It should be better to compare it relatively
				///\internal because its value is assigned automatically
				///\internal in OBS_PacketBurstifier and OBS_FileBurstifier
				if(!strcmp(burstifierIdPChr,"*") == 0){ //Compare this parameter
					//Convert from char * to int
					char2int.clear();
					char2int << burstifierIdPChr;
					int burstifierIdP;
					char2int >> burstifierIdP;

					//Compare values
					if (burstifierId != burstifierIdP){
						OK = false;
						params += "\n\t\tBurstifierId";
					}
				}

				if(!strcmp(numSeqPChr,"*") == 0){ //Compare this parameter
					//Convert from char * to int
					char2int.clear();
					char2int << numSeqPChr;
					int numSeqP;
					char2int >> numSeqP;

					//Compare values
					if (numSeq != numSeqP){
						OK = false;
						params += "\n\t\tNumSeq";
					}
				}

				if(!strcmp(lengthPChr,"*") == 0){ //Compare this parameter
					//Convert from char * to int
					char2int.clear();
					char2int << lengthPChr;
					int lengthP;
					char2int >> lengthP;

					//Compare values
					if (length != lengthP){
						OK = false;
						params += "\n\t\tLength";
					}
				}
			}

			//2. Compare packet output
			else{
				if (firstPos.compare("P") != 0){
					OK = false;
					errorPhrase = "Line ";
					int2str << line;
					int2str >> lineStr;
					errorPhrase += lineStr;
					errorPhrase += " type doesn't match with the pattern";
					break;
				}

				val = sscanf(patternLine.data(), "P %*s %*s %s %s %s %s %s %s", srcAddrPChr, destAddrPChr, protocolPChr, srcPortPChr, destPortPChr, lengthPChr);
				if (val != 6){
					opp_error("Pattern file: wrong format on line %d", line);
				}

				val = sscanf(currentLine.data(), "P %*d %*d %s %s %d %d %d %lld", srcAddrChr, destAddrChr, &protocol, &srcPort, &destPort, &length);
				if (val != 6){
					opp_error("Output file: wrong format on line %d", line);
				}

				if(!strcmp(srcAddrPChr,"*") == 0){ //Compare this parameter
					//Convert from char * to IPAddress
//					IPAddress srcAddrP;
//					IPAddress srcAddr;
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
//                    IPAddress destAddrP;
//                    IPAddress destAddr;
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
					//Convert from char * to int
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

void testSink2::deleteFiles(){
	int compType = par("comparisonType");

	remove(par("outputFile"));
	if (compType == 3){
		//Get the file names without apostrophes
		string outputName = par("outputFile").str();
		string patternName = par("patternFile").str();
		size_t pos = outputName.find_first_of('"');
		outputName = outputName.erase(pos,1);
		pos = outputName.find_last_of('"');
		outputName = outputName.erase(pos,1);
		pos = patternName.find_first_of('"');
		patternName = patternName.erase(pos,1);
		pos = patternName.find_last_of('"');
		patternName = patternName.erase(pos,1);
		string ordOutputName = outputName + "_Ord";
		string ordPatternName= patternName + "_Ord";

		//Delete the files
		remove(ordOutputName.c_str());
		remove(ordPatternName.c_str());
	}
}
