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
#include<map>

using namespace std;

//! This module acts as an interface to the core node's modules (control unit and OXC). Separates data and control channels.
//! Also, this implements methods to translate "colours" to physical gate IDs.
class OBS_CoreInput : public cSimpleModule{
   private:
      vector< map<int,int> > colours; //!< Input colour map.
      int numPorts; //!< Number of connected fibers.
      int *inPortBegin; //!< This array stores the input gate index to which the beginning of each optical fiber is connected. For example: If lambdasPerPort is "3 2 3", it will be "0 4 7".
      int *outDataBegin; //!< This array stores the output gate index to which the beginning of the data channels of each optical fiber is connected. For example: If lambdasPerPort is "3 2 3", it will be "3 6 8".
      int *portLen; //!< Number of channels (data+control) for each optical fiber.
   public:
      virtual ~OBS_CoreInput();
      //! Return the OXC gate where burst will pass through.
      int getOXCGate(int port, int lambda);
      //! The name explains itself :)
      int getLambdaByColour(int port,int colour);
   protected:
      virtual void initialize();
      virtual void handleMessage(cMessage *msg);
      int getInPort(int gateIndex);
      int getInLambda(int gateIndex);
};
