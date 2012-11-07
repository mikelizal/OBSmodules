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

#include "BurstList.h"

BurstList::BurstList(){
   counter = 0;
   listSize = 0;
   numElems = 0;
   maxSize = 0; //
   maxElems = 0; // Default: disabled
   numElemsVector.setName("burstListSize");
   burstList.setName("scheduledBursts");
}

BurstList::~BurstList(){
   burstList.clear();
}

//Inserts the burst at the end of the vector. Returns the position where the burst is stored.
int BurstList::insertBurst(OBS_Burst *burst,simtime_t sendTime){
   if(maxElems != 0){
      if(numElems  == maxElems)
         return -1;
   }
   if(maxSize != 0){
	   if(listSize + burst->getBitLength() > maxSize)
          return -1;
   }

   OBS_ScheduledBurstItem *item = new OBS_ScheduledBurstItem();
   item->setBurstId(counter);
   counter++;
   listSize += burst->getBitLength();
   numElems++;
   numElemsVector.record(numElems);

   item->setMinOffset(burst->getMinOffset());
   item->setMaxOffset(burst->getMaxOffset());
   item->setSendTime(sendTime); 
   item->encapsulate(burst);
   item->setListIndex(counter-1);
   
   burstList.insert(item);

   return counter-1;
}

void BurstList::removeBurst(int bId){

   cQueue::Iterator iter_list(burstList,0);//front-back
   OBS_ScheduledBurstItem *item;
   OBS_Burst *burst;
   while(!iter_list.end()){
      item = (OBS_ScheduledBurstItem*)iter_list++;
      if(item->getBurstId() == bId){
         burst = check_and_cast <OBS_Burst *> (item->decapsulate());
         listSize -= burst->getBitLength();
         delete burstList.remove(item);
         delete burst;
         numElems--;
         numElemsVector.record(numElems);
         return;
      }
   }
}

OBS_Burst* BurstList::retrieveBurst(int index){

   cQueue::Iterator iter_list(burstList,0);//front-back
   OBS_ScheduledBurstItem *item;

   while(!iter_list.end()){
      item = (OBS_ScheduledBurstItem*)iter_list++;
      if(item->getListIndex() == index) break;
   }
   
   //Create a 'clone' message and replace the original with it.
   //If this method preserve the original message, we'll eventually get an message ownership error because another module wants to edit a message which is stored here
   OBS_Burst *element = check_and_cast<OBS_Burst *>(item->decapsulate());
   OBS_Burst *clone = element->dup();
   clone->setKind(99);
   item->encapsulate(clone);
  

   //return clone;
   return element;
}

//From now on, every method works in the same way: They use an iterator to find the requested message and return the requested value.
//Probably, this will be more efficient merging all of them in a single function

int BurstList::retrieveBurstSize(int index){
   cQueue::Iterator iter_list(burstList,0);//front-back
   OBS_ScheduledBurstItem *item;

   while(!iter_list.end()){
      item = (OBS_ScheduledBurstItem*)iter_list++;
      if(item->getListIndex() == index) break;
   }

   OBS_Burst *element = check_and_cast<OBS_Burst *>(item->getEncapsulatedPacket());
   return element->getByteLength();
}

simtime_t BurstList::retrieveSendTime(int index){
   cQueue::Iterator iter_list(burstList,0);//front-back
   OBS_ScheduledBurstItem *item;
   while(!iter_list.end()){
      item = (OBS_ScheduledBurstItem*)iter_list++;
      if(item->getListIndex() == index) break;
   }

   return item->getSendTime();
}

simtime_t BurstList::retrieveMinOffset(int index){
   cQueue::Iterator iter_list(burstList,0);//front-back
   OBS_ScheduledBurstItem *item;

   while(!iter_list.end()){
      item = (OBS_ScheduledBurstItem*)iter_list++;
      if(item->getListIndex() == index) break;
   }

   return item->getMinOffset();
} 
simtime_t BurstList::retrieveMaxOffset(int index){
   cQueue::Iterator iter_list(burstList,0);
   OBS_ScheduledBurstItem *item;

   while(!iter_list.end()){
      item = (OBS_ScheduledBurstItem*)iter_list++;
      if(item->getListIndex() == index) break;
   }

   return item->getMaxOffset();
}

void BurstList::setMaxSize(int size){
   maxSize = size;
}

void BurstList::setMaxElems(int numElems){
  maxElems = numElems;
}
