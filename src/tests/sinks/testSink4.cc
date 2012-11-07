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
#include "testSink4.h"

Define_Module(testSink4);

void testSink4::initialize(){
	//Open the output file in write mode
	outputFile.open(par("outputFile"));
	if (!outputFile.is_open())
		opp_error("Cannot create the output file for the test");
}

void testSink4::handleMessage(cMessage *msg){
	if (dynamic_cast <OBS_BurstControlPacket *> (msg) != NULL){
		OBS_BurstControlPacket *bcp = (OBS_BurstControlPacket *) msg;
		if (bcp->getKind() == 1){
			outputFile << "1\t" << simTime() << "\t" << bcp->getArrivalGate()->getIndex() << "\t" << bcp->getBurstifierId() << "\t" << bcp->getNumSeq() << "\t" << bcp->getSenderId() << "\t" << bcp->getBurstArrivalDelta() << "\t" << bcp->getBurstColour() << "\t" << bcp->getLabel() << "\t" << bcp->getBurstSize() << "\t" << bcp->getByteLength() << endl;
		}
		else if (bcp->getKind() == 2){
			outputFile << "2\t" << simTime() << "\t" << bcp->getArrivalGate()->getIndex() << "\t" << bcp->getBurstifierId() << "\t" << bcp->getNumSeq() << endl;
		}
		else{
			cout << "<testSink4><" << simTime() << "> WARNING: Received an OBS_BurstControlPacket with an unknown kind." << endl;
		}
	}
	else{
		cout << "<testSink4><" << simTime() << "> WARNING: Received a message which is not an OBS_BurstControlPacket." << endl;
	}
	delete msg;
}

void testSink4::finish(){
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

void testSink4::openFiles(){
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

bool testSink4::compareFiles(){
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
		char receivedTimeChr[20], burstArrivalTimeChr[20], receivedTimePChr[20], burstArrivalTimePChr[20], colourPChr[20], labelPChr[20], burstifierIdPChr[20], numSeqPChr[20], senderIdPChr[20], sizePChr[20], lengthPChr[20], gateIndexPChr[20];
		int val, colour, label, burstifierId, numSeq, senderId, size, gateIndex;
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

			string firstPos = patternLine.substr(0,1);
			//Compare iniBCP output
			if (firstPos.compare("1") == 0){
				firstPos = currentLine.substr(0,1);
				if (!firstPos.compare("1") == 0){
					OK = false;
					errorPhrase = "Line ";
					int2str << line;
					int2str >> lineStr;
					errorPhrase += lineStr;
					errorPhrase += " type doesn't match with the pattern";
					break;
				}

				val = sscanf(patternLine.data(), "1 %s %s %s %s %s %s %s %s %s %s", receivedTimePChr, gateIndexPChr, burstifierIdPChr, numSeqPChr, senderIdPChr, burstArrivalTimePChr, colourPChr, labelPChr, sizePChr, lengthPChr);
				if (val != 10){
					opp_error("Pattern file: wrong format on line %d", line);
				}

				val = sscanf(currentLine.data(), "1 %s %d %d %d %d %s %d %d %d %lld", receivedTimeChr, &gateIndex, &burstifierId, &numSeq, &senderId, burstArrivalTimeChr, &colour, &label, &size, &length);
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
			//Compare endBCP output
			else if (firstPos.compare("2") == 0){
				firstPos = currentLine.substr(0,1);
				if (!firstPos.compare("2") == 0){
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

void testSink4::deleteFiles(){
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
