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

#include <omnetpp.h>
#include "OBS_Burst.h"
#include "OBS_BurstifierInfo_m.h"
#include <stdio.h>
using namespace std;

//! Generates 'fake' bursts based on a text file. See ned documentation for details.
class OBS_FileBurstifier : public cSimpleModule
{
   protected:
      FILE *msgFile; //!< Burst generator file.
      int burstCounter; //!< Generated bursts counter.
      int destLabel; //!< Destination label.

      virtual void initialize();
      virtual void handleMessage(cMessage *msg);
};

Define_Module(OBS_FileBurstifier);

void OBS_FileBurstifier::initialize(){
   destLabel = par("destLabel");
   //Open the file specified in "burstFile" ned parameter
   msgFile = fopen(par("burstFile"),"r");
   if(msgFile == NULL){
	   opp_error("Cannot open burst generation file");
   }
   
   int sal,burstsize; //sal contains the return value of sscanf
   burstCounter = 0;
   simtime_t tsend,minoffset,maxoffset;
   char tsendStr[50],minoffsetStr[50],maxoffsetStr[50];
   //simtime_t is read as a double. But it may change in later versions of OMNeT++ 
   //file format:
   //t-send offsetMin offsetMax burstSize

   //first, scan a line and check if it has the comment tag #
   char line[500];
   char* status;
   
   line[0]='\0';

   status = fgets(line,500,msgFile);
   if(status == NULL) return; 

   while(line[0] == '#'){ //A comment
      status = fgets(line,500,msgFile);
      if(status == NULL) return;
   }

   sal = sscanf(line,"%s %s %s %d\n",tsendStr,minoffsetStr,maxoffsetStr,&burstsize);

   if(sal != 0 && sal != EOF){

	  tsend = SimTime::parse(tsendStr);

	  if(simTime()<=tsend){
		  minoffset = SimTime::parse(minoffsetStr);
		  maxoffset = SimTime::parse(maxoffsetStr);

		  //create first burst
		  OBS_Burst *fBurst = new OBS_Burst("assembledBurst");
	      //Fill burst info and then schedule it to its send time
	      fBurst->setMinOffset(minoffset);
	      fBurst->setMaxOffset(maxoffset);
	      fBurst->setByteLength(burstsize);
	      fBurst->setBurstifierId(getId());
	      fBurst->setNumSeq(burstCounter);
	      OBS_BurstifierInfo *fInfo = new OBS_BurstifierInfo();
	      fInfo->setLabel(destLabel);
	      fBurst->setControlInfo(fInfo);
	      fBurst->setNumPackets(0);
	      //SenderID will be set once the burst arrives at the sender
	      burstCounter++;

	      scheduleAt(tsend,fBurst);
	  }
	  else
		  opp_error("Cannot send a burst scheduled to the past");
   }
   
   recordScalar("my ID",getId());    
}

void OBS_FileBurstifier::handleMessage(cMessage *msg){
  //First of all, send received burst
  send(msg,"out");
      
   int sal,burstsize;
   simtime_t tsend,minoffset,maxoffset;
   char tsendStr[50],minoffsetStr[50],maxoffsetStr[50];
   //Scan a line and check if it has the comment tag #
   char line[500];
   char* status;

   status = fgets(line,500,msgFile);
   if(status == NULL) return;

   while(line[0] == '#'){
      status = fgets(line,500,msgFile);
      if(status == NULL) return;
   }
   //got a line that's not a comment

   //simtime_t is read as a double. But it may change in later versions of OMNeT++ 
   sal = sscanf(line,"%s %s %s %d\n",tsendStr,minoffsetStr,maxoffsetStr,&burstsize);

   if(sal != 0 && sal != EOF){ //if read was successful...

	  tsend = SimTime::parse(tsendStr);

	  if(simTime()<=tsend){
		  minoffset = SimTime::parse(minoffsetStr);
		  maxoffset = SimTime::parse(maxoffsetStr);

		  //Create next burst. Just like the initialize() method.
		  OBS_Burst *burst = new OBS_Burst("assembledBurst");

	      //Fill burst & schedule it
	      burst->setMinOffset(minoffset);
	      burst->setMaxOffset(maxoffset);
	      burst->setByteLength(burstsize);
	      burst->setBurstifierId(getId());
	      burst->setNumSeq(burstCounter);
	      OBS_BurstifierInfo *info = new OBS_BurstifierInfo();
	      info->setLabel(destLabel);
	      burst->setControlInfo(info);
	      burst->setNumPackets(0);
	      //SenderID will be set once the burst arrives at the sender
	      burstCounter++;

	      scheduleAt(tsend,burst);
	  }
	  else
		  opp_error("Cannot send a burst scheduled to the past");
   }      
}
