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

#include "OBS_BurstControlPacket_m.h"
#include "OBS_Burst.h"
#include <omnetpp.h>
#include <stdio.h>
using namespace std;

//! This module discards bursts (including BCP's) using the given loss probability.
class OBS_DropBurst : public cSimpleModule
{
   protected:
      double dropProb; //!< Burst drop probability.
      int dropCounter; //!< Dropped burst counter.
      list<OBS_BurstControlPacket*> doomedBursts; //!< List of burst to be dropped. Stores the ini BCP of all selected bursts.

      virtual void initialize();
      virtual void handleMessage(cMessage *msg);
};

Define_Module(OBS_DropBurst);

void OBS_DropBurst::initialize(){
   dropProb = par("dropProb");
   dropCounter = 0;
   WATCH(dropCounter);
}

void OBS_DropBurst::handleMessage(cMessage *msg){
   cGate *gate = msg->getArrivalGate(); //Get arrival gate

   if(dynamic_cast<OBS_BurstControlPacket*>(msg) != NULL && msg->getKind() == 1){ //If it's a BCP beginning
      OBS_BurstControlPacket *bcpIni = check_and_cast<OBS_BurstControlPacket*>(msg);
      
      double drand = dblrand();

      //Decide whether drop the burst or not
      if(drand < dropProb){ //this burst is DOOMED!!!! I'll drop it! mwuahahaha!!!
         dropCounter++;
         doomedBursts.push_back(bcpIni);
      }
      else
         send(msg,"out",gate->getIndex());
   }
   else{ //Else... could be either a Burst (ini/end) or a BCP end
      
      list<OBS_BurstControlPacket*>::iterator iter_list; // Iterator used to iterate across doomed burst list
      OBS_BurstControlPacket *item;

      if(dynamic_cast<OBS_BurstControlPacket*>(msg) != NULL){ //BCP end received

         OBS_BurstControlPacket *bcpEnd = check_and_cast<OBS_BurstControlPacket*>(msg);
         for(iter_list = doomedBursts.begin();iter_list != doomedBursts.end(); iter_list++){ //Search for this burst in doomed list
            
            item = *iter_list;

            if((item->getBurstifierId() == bcpEnd->getBurstifierId()) && (item->getNumSeq() == bcpEnd->getNumSeq())){ //I found it!!
               delete msg;
               break;
            }
         }
         if(iter_list == doomedBursts.end())
            send(msg,"out",gate->getIndex());
      }
      else if(dynamic_cast<OBS_Burst*>(msg) != NULL){ //Burst received
         OBS_Burst *burst = check_and_cast<OBS_Burst*>(msg);

         for(iter_list = doomedBursts.begin();iter_list != doomedBursts.end(); iter_list++){ //Search for this burst in doomed list
            item = *iter_list;
            if((item->getBurstifierId() == burst->getBurstifierId()) && (item->getNumSeq() == burst->getNumSeq())){ //I found it!!
               delete msg;
               if(burst->getKind() == 2){ //If the current message is a Burst end, delete this burst from the queue
                  delete *iter_list;
                  doomedBursts.erase(iter_list);    
               }
               break;
            }
         }

         if(iter_list == doomedBursts.end()) //If not found, forward this message
            send(msg,"out",gate->getIndex());
      }
   }
}
