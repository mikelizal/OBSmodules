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

#include<omnetpp.h>

//! The horizon table store the time where each optical channel is available (this time is often called the horizon).
//! This table is structured as an bidimensional array where the first component represents the port number and the second component is the data channel:
//!
//! horizon[fiber][channel]
//!
//! This table doesn't include fiber's control channels because they don't need the horizon value.
class OBS_CoreOutputHorizon : public cSimpleModule{
   protected:
      simtime_t **horizon; //!< Horizon array. It has two dimensions: horizon[port][lambda].
      int *portLambdas; //!< Number of data channels for each port.

      virtual void initialize();
   public:
      virtual ~OBS_CoreOutputHorizon();
      //! Find the lambda which horizon (time when the channel is free) is lesser than and closer to the arrivalTime value given.
      //! @param port Optical fiber to look for.
      //! @param arrivalTime Time value used for the lookup.
      int findNearestLambda(int port,simtime_t arrivalTime);
      //! Update horizon Value to the newTime value.
      //! @param port Optical fiber.
      //! @param lambda Channel to update.
      //! @param newTime Updated horizon value.
      void updateHorizon(int port, int lambda, simtime_t newTime);

      //! Return the current horizon of the selected channel.
      //! @param port Optical fiber.
      //! @param lambda Optical channel.
      simtime_t getHorizon(int port,int lambda);
};
