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
#include "OBS_ScheduledBurstItem_m.h"
#include "OBS_Burst.h"
#include <vector>
using namespace std;

//! Just a list that stores OBS_ScheduledBurstItem objects and allows random access to all elements.
class BurstList : public cObject{ //NOTE: Inherits from cObject because it will be used as message Control field
   private:

      long int counter; //!< Counter used to assign a unique Id to each element.

      long int numElems; //!< Number of elements inside the list.
   protected:
      long int listSize; //!< List size.
      int maxSize; //!< Maximum size.
      int maxElems; //!< Maximum number of elements.
      cOutVector numElemsVector; //!< Output vector.
      cQueue burstList; //!< The list itself.
   public:
      BurstList();
      ~BurstList();
      //! Inserts a burst into the list.
      //! @param burst OBS_Burst message.
      //! @return The index value of the burst. Or -1 if something bad happened.
      int insertBurst(OBS_Burst *burst,simtime_t sendTime);
      void removeBurst(int bId);
      OBS_Burst* retrieveBurst(int index);
      int retrieveBurstSize(int index);
      simtime_t retrieveSendTime(int index);
      simtime_t retrieveMinOffset(int index);  
      simtime_t retrieveMaxOffset(int index);
      //! Set max size and/or max length of the queue.
      void setMaxSize(int size);
      void setMaxElems(int numElems);  
};
