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
#include <stdio.h>
#include "OBS_Burst.h"
#include "OBS_BurstControlPacket_m.h"

//! An optical sniffer that is connected to an optical fiber and writes information regarding received traffic in an output file.
class OBS_OpticalMonitor : public cSimpleModule{
   protected:
        FILE *data_f; //!< output file descriptor.

        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void finish();
};
