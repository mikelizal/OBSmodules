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
#include "testSink3.h"

Define_Module(testSink3);

void testSink3::initialize(){
	//Open the output file in write mode
	outputFile.open(par("outputFile"));
	if (!outputFile.is_open())
		opp_error("Cannot create the output file for the test");
}

void testSink3::handleMessage(cMessage *msg){
	if (dynamic_cast <OBS_BurstControlPacket *> (msg) != NULL){
		OBS_BurstControlPacket *bcp = (OBS_BurstControlPacket *) msg;
		if (bcp->getKind() == 1){
			outputFile << "1\t" << simTime() << "\t" << bcp->getArrivalGate()->getIndex() << "\t" << bcp->getBurstifierId() << "\t" << bcp->getNumSeq() << "\t" << bcp->getSenderId() << "\t" << bcp->getLabel() << "\t" << bcp->getBurstSize() << "\t" << bcp->getBurstArrivalDelta() << "\t" << bcp->getBurstColour() << "\t" << bcp->getByteLength() << endl;
		}
		else if (bcp->getKind() == 2){
			outputFile << "2\t" << simTime() << "\t" << bcp->getArrivalGate()->getIndex() << "\t" << bcp->getBurstifierId() << "\t" << bcp->getNumSeq() << endl;
		}
		else{
			cout << "<testSink3><" << simTime() << "> WARNING: Received an OBS_BurstControlPacket with an unknown kind." << endl;
		}
	}
	else if (dynamic_cast <OBS_Burst *> (msg) != NULL){
		OBS_Burst *burst = (OBS_Burst *) msg;
		if (burst->getKind() == 1){
			//Save burst's information in the file
			int bId = burst->getBurstifierId();
			int seq = burst->getNumSeq();
			outputFile << "3\t" << simTime() << "\t" << burst->getArrivalGate()->getIndex() << "\t" << bId << "\t" << seq << "\t" << burst->getSenderId() << "\t" << burst->getByteLength() << "\t" << burst->getNumPackets() << "\t" << burst->getMinOffset() << "\t" << burst->getMaxOffset() << endl;
			
			//Save packet's information in the file
			int protocol;
			while (burst->hasMessages()){
//				IPDatagram *datagram = check_and_cast <IPDatagram *> (burst->retrieveMessage());
                IPv4Datagram *datagram = check_and_cast <IPv4Datagram *> (burst->retrieveMessage());
				protocol = datagram->getTransportProtocol();
				if (protocol == IP_PROT_TCP){
					TCPSegment *segment = check_and_cast <TCPSegment *> (datagram->getEncapsulatedPacket());
					outputFile << "P\t" << datagram->getSrcAddress() << "\t" << datagram->getDestAddress() << "\t" << bId << "\t" << seq << "\t" << protocol << "\t" << segment->getSrcPort() << "\t" << segment->getDestPort() << "\t" << datagram->getByteLength() << endl;
					//delete segment;
				}
				else if (protocol == IP_PROT_UDP){
					UDPPacket *packet = check_and_cast <UDPPacket *> (datagram->getEncapsulatedPacket());
					outputFile << "P\t" << datagram->getSrcAddress() << "\t" << datagram->getDestAddress() << "\t" << bId << "\t" << seq << "\t" << protocol << "\t" << packet->getSourcePort() << "\t" << packet->getDestinationPort() << "\t" << datagram->getByteLength() << endl;
					//delete packet;
				}
				else{
					cout << "<testSink3><" << simTime() << "> WARNING: Received a packet with a protocol different from TCP or UDP." << endl;
					outputFile << "P\t" << datagram->getSrcAddress() << "\t" << datagram->getDestAddress() << "\t" << bId << "\t" << seq << "\t" << protocol << "\t" << "-1" << "\t" << "-1" << "\t" << datagram->getByteLength() << endl;
				}
				delete datagram;
			}
		}
		else if (burst->getKind() == 2){
			//Save burst's information in the file
			outputFile << "4\t" << simTime() << "\t" << burst->getArrivalGate()->getIndex() << "\t" << burst->getBurstifierId() << "\t" << burst->getNumSeq() << endl;
		}
		else{
			cout << "<testSink3><" << simTime() << "> WARNING: Received an OBS_Burst with an unknown kind." << endl;
		}
	}
	else{
		cout << "<testSink3><" << simTime() << "> WARNING: Received a message which is neither an OBS_BurstControlPacket nor an OBS_Burst." << endl;
	}
	delete msg;
}

void testSink3::finish(){
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

		//Compare the report files
		bool reportOK = true;
		int reportLine = 0;
		if (par("compareReport")){
			//Open both files in read mode
			ifstream currentReport, patternReport;
			string patternRLine, currentRLine;
			//Find the OpticalMonitor or CoreControlLogic module
			//to get its reportFile parameter
			cGate *in = gate("in",0);
			cModule *modulo = in->getPathStartGate()->getOwnerModule();
			while ((dynamic_cast<OBS_CoreControlLogic *> (modulo) == NULL) && (dynamic_cast<OBS_OpticalMonitor *> (modulo) == NULL)){
				if(modulo->isGateVector("in")){
					in = modulo->gate("in",0);
				}
				else{
					in = modulo->gate("in");
				}
				modulo = in->getPathStartGate()->getOwnerModule();
			}
			if(modulo->hasPar("reportFile")){
				currentReport.open(modulo->par("reportFile"));
			}
			else{
				opp_error("The tested module has not a parameter named 'reportFile'");
			}

			if (!currentReport.is_open())
				opp_error("Cannot open the report");
			patternReport.open(par("reportPattern"));
			if (!patternReport.is_open())
				opp_error("Cannot open the pattern of the report");

			while(patternReport.good() && currentReport.good()){
				getline(patternReport,patternRLine);
				getline(currentReport,currentRLine);
				reportLine++;
				if (patternRLine.compare(currentRLine) != 0){//Different lines
					reportOK = false;
					break;
				}
			}

			//Skip the lines at the end of the file
			while (reportOK && patternReport.good()){
				getline(patternReport, patternRLine);
				reportLine++;
				if (!patternRLine.empty()){//Not a white line
					reportOK = false;
					break;
				}
			}

			//Skip the lines at the end of the file
			while (reportOK && currentReport.good()){
				getline(currentReport, currentRLine);
				reportLine++;
				if (!currentRLine.empty()){//Not a white line
					reportOK = false;
					break;
				}
			}

			//Close files
			currentReport.close();
			patternReport.close();

			if(!reportOK){
				cout << "WARNING: The report files don't match on line " << reportLine << endl;
				//Return 4 as status if it is simulating in Cmdenv
				cEnvir *env = simulation.getActiveSimulation()->getActiveEnvir();
				if (!env->isGUI()){
					exit(4);
				}
			}

		}
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

void testSink3::openFiles(){
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
		string llamada1 = "sort -k 4,5 -k 1 -o " + ordOutputName + " " + outputName;
		string llamada2 = "sort -k 4,5 -k 1 -o " + ordPatternName + " " + patternName;
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

bool testSink3::compareFiles(){
	bool OK = true;
	string lineStr, currentLine, patternLine;
	int line = 0;
	stringstream int2str;

	errorPhrase = "";

	int compType = par("comparisonType");

	if(compType == 1){
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
		char receivedTimeChr[20], burstArrivalTimeChr[20], receivedTimePChr[20], burstArrivalTimePChr[20], colourPChr[20], labelPChr[20], burstifierIdPChr[20], numSeqPChr[20], senderIdPChr[20], sizePChr[20], lengthPChr[20], gateIndexPChr[20], minOffsetPChr[20], minOffsetChr[20], maxOffsetPChr[20], maxOffsetChr[20], numPacketsPChr[20],srcAddrPChr[20], destAddrPChr[20], protocolPChr[20], srcPortPChr[20], destPortPChr[20], srcAddrChr[20], destAddrChr[20];
		int val, colour, label, burstifierId, numSeq, senderId, size, gateIndex, numPackets, protocol, srcPort, destPort;
		long long int length;
		stringstream char2int, int2str;
		string params = "";

		//Take the time tolerance value
		simtime_t timeTol = par("timeTolerance");

		while(OK && patternFile.good() && currentFile.good()){
			getline(patternFile,patternLine);
			getline(currentFile,currentLine);
			line++;

			if (patternLine.empty() || currentLine.empty()){
				break;
			}

			string firstPosP = patternLine.substr(0,1);
			string firstPos = currentLine.substr(0,1);

			//1. Compare iniBCP output
			if (firstPosP.compare("1") == 0){
				if (firstPos.compare("1") != 0){
					OK = false;
					errorPhrase = "Line ";
					int2str << line;
					int2str >> lineStr;
					errorPhrase += lineStr;
					errorPhrase += " type doesn't match with the pattern";
					break;
				}

				val = sscanf(patternLine.data(), "1 %s %s %s %s %s %s %s %s %s %s", receivedTimePChr, gateIndexPChr, burstifierIdPChr, numSeqPChr, senderIdPChr, labelPChr, sizePChr, burstArrivalTimePChr, colourPChr, lengthPChr);
				if (val != 10){
					opp_error("Pattern file: wrong format on line %d", line);
				}

				val = sscanf(currentLine.data(), "1 %s %d %d %d %d %d %d %s %d %lld", receivedTimeChr, &gateIndex, &burstifierId, &numSeq, &senderId, &label, &size, burstArrivalTimeChr, &colour, &length);
				if (val != 10){
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

				///\internal It should be better to compare it relatively
				///\internal because its value is assigned automatically
				///\internal in OBS_BurstSender
				if(!strcmp(senderIdPChr,"*") == 0){ //Compare this parameter
					//Convert from char * to int
					char2int.clear();
					char2int << senderIdPChr;
					int senderIdP;
					char2int >> senderIdP;

					//Compare values
					if (senderId != senderIdP){
						OK = false;
						params += "\n\t\tSenderId";
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

				if(!strcmp(sizePChr,"*") == 0){ //Compare this parameter
					//Convert from char * to int
					char2int.clear();
					char2int << sizePChr;
					int sizeP;
					char2int >> sizeP;

					//Compare values
					if (size != sizeP){
						OK = false;
						params += "\n\t\tBurstSize";
					}
				}

				if(!strcmp(burstArrivalTimePChr,"*") == 0){ //Compare this parameter
					//Convert from char * to SimTime
					simtime_t burstArrivalTimeP = SimTime::parse(burstArrivalTimePChr);
					simtime_t burstArrivalTime = SimTime::parse(burstArrivalTimeChr);

					//Compare times
					//if (fabs(burstArrivalTime - burstArrivalTimeP) <= timeTol){ //OK
					if (fabs(burstArrivalTime - burstArrivalTimeP) > timeTol){ //ERROR
						OK = false;
						params += "\n\t\tBurstArrivalDelta";
					}
				}

				if(!strcmp(colourPChr,"*") == 0){ //Compare this parameter
					//Convert from char * to int
					char2int.clear();
					char2int << colourPChr;
					int colourP;
					char2int >> colourP;

					//Compare values
					if (colour != colourP){
						OK = false;
						params += "\n\t\tBurstColour";
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
			//2. Compare endBCP output
			else if (firstPosP.compare("2") == 0){
				if (firstPos.compare("2") != 0){
					OK = false;
					errorPhrase = "Line ";
					int2str << line;
					int2str >> lineStr;
					errorPhrase += lineStr;
					errorPhrase += " type doesn't match with the pattern";
					break;
				}

				val = sscanf(patternLine.data(), "2 %s %s %s %s", receivedTimePChr, gateIndexPChr, burstifierIdPChr, numSeqPChr);
				if (val != 4){
					opp_error("Pattern file: wrong format on line %d", line);
				}

				val = sscanf(currentLine.data(), "2 %s %d %d %d", receivedTimeChr, &gateIndex, &burstifierId, &numSeq);
				if (val != 4){
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
			}
			//3. Compare iniBurst output
			else if (firstPosP.compare("3") == 0){
				if (firstPos.compare("3") != 0){
					OK = false;
					errorPhrase = "Line ";
					int2str << line;
					int2str >> lineStr;
					errorPhrase += lineStr;
					errorPhrase += " type doesn't match with the pattern";
					break;
				}

				val = sscanf(patternLine.data(), "3 %s %s %s %s %s %s %s %s %s", receivedTimePChr, gateIndexPChr, burstifierIdPChr, numSeqPChr, senderIdPChr, lengthPChr, numPacketsPChr, minOffsetPChr, maxOffsetPChr);
				if (val != 9){
					opp_error("Pattern file: wrong format on line %d", line);
				}

				val = sscanf(currentLine.data(), "3 %s %d %d %d %d %lld %d %s %s", receivedTimeChr, &gateIndex, &burstifierId, &numSeq, &senderId, &length, &numPackets, minOffsetChr, maxOffsetChr);
				if (val != 9){
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

				///\internal It should be better to compare it relatively
				///\internal because its value is assigned automatically
				///\internal in OBS_BurstSender
				if(!strcmp(senderIdPChr,"*") == 0){ //Compare this parameter
					//Convert from char * to int
					char2int.clear();
					char2int << senderIdPChr;
					int senderIdP;
					char2int >> senderIdP;

					//Compare values
					if (senderId != senderIdP){
						OK = false;
						params += "\n\t\tSenderId";
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
			}
			//4. Compare endBurst output
			else if (firstPosP.compare("4") == 0){
				if (firstPos.compare("4") != 0){
					OK = false;
					errorPhrase = "Line ";
					int2str << line;
					int2str >> lineStr;
					errorPhrase += lineStr;
					errorPhrase += " type doesn't match with the pattern";
					break;
				}

				val = sscanf(patternLine.data(), "4 %s %s %s %s", receivedTimePChr, gateIndexPChr, burstifierIdPChr, numSeqPChr);
				if (val != 4){
					opp_error("Pattern file: wrong format on line %d", line);
				}

				val = sscanf(currentLine.data(), "4 %s %d %d %d", receivedTimeChr, &gateIndex, &burstifierId, &numSeq);
				if (val != 4){
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
			}
			//5. Compare packet output
			else if(firstPosP.compare("P") == 0){
				if (firstPos.compare("P") != 0){
					OK = false;
					errorPhrase = "Line ";
					int2str << line;
					int2str >> lineStr;
					errorPhrase += lineStr;
					errorPhrase += " type doesn't match with the pattern";
					break;
				}

				val = sscanf(patternLine.data(), "P %s %s %*s %*s %s %s %s %s", srcAddrPChr, destAddrPChr, protocolPChr, srcPortPChr, destPortPChr, lengthPChr);
				if (val != 6){
					opp_error("Pattern file: wrong format on line %d", line);
				}

				val = sscanf(currentLine.data(), "P %s %s %*d %*d %d %d %d %lld", srcAddrChr, destAddrChr, &protocol, &srcPort, &destPort, &length);
				if (val != 6){
					opp_error("Output file: wrong format on line %d", line);
				}

				if(!strcmp(srcAddrPChr,"*") == 0){ //Compare this parameter
					//Convert from char * to IPAddress
//                    IPAddress srcAddrP;
//                    IPAddress srcAddr;
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
//					IPAddress destAddrP;
//					IPAddress destAddr;
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
			else{
				opp_error("Pattern file: Unknown beginning character on line %d", line);
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

void testSink3::deleteFiles(){
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
