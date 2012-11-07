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
#include "OBS_CoreRoutingTable.h"
#include "OBS_CoreOutputHorizon.h"
#include "OBS_ControlUnitInfo_m.h"
#include "OBS_BurstControlPacket_m.h"
#include "OBS_BCPControlInfo_m.h"
#include "OBS_OpticalCrossConnect.h"
#include "OBS_CoreInput.h"
#include "OBS_CoreOutput.h"

#define OBS_SCHEDULE_OXC 1
#define OBS_UNSCHEDULE_OXC 2

//! Creates a lightpath based on the incoming BCP info.
class OBS_CoreControlLogic : public cSimpleModule{
   private:
     OBS_CoreRoutingTable *routingTable; //!< Pointer to routing table.
     OBS_CoreOutputHorizon *gatesHorizon; //!< Pointer to output horizon.
     OBS_CoreInput *coreInput; //!< Input module pointer.
     OBS_CoreOutput *coreOutput; //!< Output module pointer.
     OBS_OpticalCrossConnect *oxc; //!< OXC pointer.

     simtime_t processingTime; //!< Control unit processing time for each BCP.
     simtime_t guardTime; //!< Offset between burst arrival and channel setting order.
     double dataRate; //!< Optical channel data rate.

     int dropCounter; //!< Dropped bursts counter.
     int *recvBurstCounter; //!< Received burst counter.
     int *schedBurstCounter; //!< Scheduled burst counter.

   protected:

     FILE *data_f; //!< Output file descriptor.
     
	 virtual void initialize();
     virtual void finish();
     virtual void handleMessage(cMessage *msg);
   public:
     virtual ~OBS_CoreControlLogic();
};
