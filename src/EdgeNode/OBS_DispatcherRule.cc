//
// Copyright (C) 2010-2012 Javier Armendariz Silva, Naiara Garcia Royo, Felix Espina Antolin
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

#include "OBS_DispatcherRule.h"

OBS_DispatcherRule::OBS_DispatcherRule(string rule){
   OBS_DispatcherRule(rule.c_str());
}

OBS_DispatcherRule::OBS_DispatcherRule(char* rule){
   char *token;
   //Initialize all values to false
   int i;
   for(i=0;i<5;i++) isSet[i] = false;

   token = strtok(rule," \n");
   while(token != NULL){
	   if(strcmp(token,"srcAddr") == 0){ //Rule has a source IP address
         //the next token should be an IP Address. If not, error!
         token = strtok(NULL," \n"); //Take the next token, which is the value
         if(token != NULL){ //If not null...
//            srcAddr = IPAddress(token); //Copy value with the correct format
            srcAddr = IPv4Address(token); //Copy value with the correct format
            isSet[0] = true;
         }
         else{
        	 opp_error("Cannot parse the value of the source IP address in the rule");
         }
      }
      else if(strcmp(token,"destAddr") == 0){ //Rule has a destination IP Address
         //the next token should be an IP Address. if not, error!
         token = strtok(NULL," \n");
         if(token != NULL){
//            destAddr = IPAddress(token);
            destAddr = IPv4Address(token);
            isSet[1] = true;
         }
         else{
        	 opp_error("Cannot parse the value of the destination IP address in the rule");
         }
      }
      else if(strcmp(token,"protocol") == 0){ // Rule has a IP Protocol value
         token = strtok(NULL," \n");
         if(token != NULL){
               protocol = atoi(token);
               isSet[2] = true;
         }
         else{
        	 opp_error("Cannot parse the value of the IP protocol in the rule");
         }
      }
      else if(strcmp(token,"srcPort") == 0){ //Rule has a TCP/UDP source port
         //the next token should be a short integer (1-65xxx). if not, error!
         token = strtok(NULL," ");
         if(token != NULL){
            srcPort = atoi(token);
            if(srcPort == 0){
            	opp_error("Wrong value for the source port");
            }
            isSet[3] = true;
         }
         else{
        	 opp_error("Cannot parse the value of the source port in the rule");
         }
      }
      else if(strcmp(token,"destPort") == 0){ //Rule has a TCP/UDP destination port
         //the next token should be a short integer (1-65xxx). if not, error!
         token = strtok(NULL," ");
         if(token != NULL){

            destPort = atoi(token);
            if(destPort == 0){
               opp_error("Wrong value for the destination port");
            }
            isSet[4] = true;
         }
         else{
        	 opp_error("Cannot parse the value of the destination port in the rule");
         } //Any other char will return error.
      }
      token = strtok(NULL," ");
   }
}

OBS_DispatcherRule::~OBS_DispatcherRule(){}

bool OBS_DispatcherRule::match(cMessage *msg){
   // Make sure the received message is an IPDatagram
//   IPDatagram *recvIP = check_and_cast< IPDatagram* > (msg);
   IPv4Datagram *recvIP = check_and_cast< IPv4Datagram* > (msg);
   // Later we'll extract TCP/UDP header if necessary

   // Check which fields are set in this rule
   if(isSet[0]){
      if(!(recvIP->getSrcAddress() == srcAddr)) return false;
   }
   if(isSet[1]){
      if(!(recvIP->getDestAddress() == destAddr)) return false;
   }
   if(isSet[2]){
      if(!(recvIP->getTransportProtocol() == protocol)) return false;
   }
   if(isSet[3]){
      // Lookup IP header in order to identify TCP/UDP header and then extract it.
      int recvSrcPort = 0;
      if(recvIP->getTransportProtocol() == IP_PROT_TCP){
    	  TCPSegment *recvTCP = check_and_cast < TCPSegment* > (recvIP->getEncapsulatedPacket());
         recvSrcPort = recvTCP->getSrcPort();
      }else if(recvIP->getTransportProtocol() == IP_PROT_UDP){
    	  UDPPacket *recvUDP = check_and_cast < UDPPacket* > (recvIP->getEncapsulatedPacket());
         recvSrcPort = recvUDP->getSourcePort();         
      }

      if(!(recvSrcPort == srcPort)) return false;
   }
   if(isSet[4]){
      //The same thing than the isSet[3] case, but now we extract the destination port.
      int recvDestPort = 0;
      if(recvIP->getTransportProtocol() == IP_PROT_TCP){
    	 TCPSegment *recvTCP = check_and_cast < TCPSegment* > (recvIP->getEncapsulatedPacket());
         recvDestPort = recvTCP->getDestPort();
      }else if(recvIP->getTransportProtocol() == IP_PROT_UDP){
    	 UDPPacket *recvUDP = check_and_cast < UDPPacket* > (recvIP->getEncapsulatedPacket());
         recvDestPort = recvUDP->getDestinationPort();         
      }

      if(!(recvDestPort == destPort)) return false;
   }
   //If everything is OK, return true and finish :)
   return true;
}
