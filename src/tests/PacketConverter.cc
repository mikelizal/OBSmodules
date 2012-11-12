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

#include<omnetpp.h>
//#include "IPDatagram.h"
//#include <IPControlInfo.h>
#include "IPv4Datagram.h"
#include <IPv4ControlInfo.h>

using namespace std;

//
// Dummy class created for the use in simulations where packets are created using IPTrafGen (Dispatcher Test, for example)
//
class PacketConverter : public cSimpleModule
{
   protected:
      virtual void handleMessage(cMessage *msg);
   public:
      virtual ~PacketConverter();
};

Define_Module(PacketConverter);

PacketConverter::~PacketConverter(){}

void PacketConverter::handleMessage(cMessage *msg){
   //IPTrafGen generates a cMessage packet which contains a IPControlInfo object encapsulated
//   IPControlInfo *IPCtl = check_and_cast < IPControlInfo *> (msg->getControlInfo());
   IPv4ControlInfo *IPCtl = check_and_cast < IPv4ControlInfo *> (msg->getControlInfo());

   // Create a IPDatagram object
//   IPDatagram *IPSal = new IPDatagram();
   IPv4Datagram *IPSal = new IPv4Datagram();
   // Fill IP datagram with control info fields
   // I dont know if exists another way to copy IP Addresses. This works.
//   int srcCtlAddr = (IPCtl->getSrcAddr()).getInt(); //Convert to integer and insert this value into IP Address constructors
//   int destCtlAddr = (IPCtl->getDestAddr()).getInt();
//   IPSal->setSrcAddress(srcCtlAddr);
//   IPSal->setDestAddress(destCtlAddr);
   IPSal->setSrcAddress(IPCtl->getSrcAddr());
   IPSal->setDestAddress(IPCtl->getDestAddr());
   // Also, add datagram length
   cPacket *pkt = check_and_cast<cPacket *> (msg);
   IPSal->setBitLength(pkt->getBitLength());
   //It'd be cool to insert TCP/UDP packets...
   //Send the IPDatagram object
   send(IPSal,"out");
   //...and that's all!
   delete msg;      
}
