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
#include<vector>

using namespace std;

//! This module does the reverse task of CoreInput: Gather both control and data channels and reorder it in order to connect the core node with another OBS node.
//! It also supports "colours" for data channels.
class OBS_CoreOutput : public cSimpleModule{
   private:
      vector< map<int,int> > colours; //!< Output colour map array (one map for each fiber).
      int **gate2Colour; //! Mapping between port/lambda and colours.
      int numPorts; //! Number of connected fibers.
      int *portLen; //! Number of channels (data+control) for each optical fiber.
      int *inDataBegin; //! This array stores the input gate index to which the beginning of the data channels of each optical fiber is connected. For example: If lambdasPerPort is "3 2 3", it will be "3 6 8".
      int *outPortBegin; //! This array stores the output gate index to which the beginning of each optical fiber is connected. For example: If lambdasPerPort is "3 2 3", it will be "0 4 7".
   protected:
      virtual void initialize();
      virtual void handleMessage(cMessage *msg);
      int getOutPort(int gateIndex);
      int getOutLambda(int gateIndex);
   public:
      virtual ~OBS_CoreOutput();
      //! Convert port and lambda to OXC output gate.
      int getOXCGate(int port,int lambda);
      //! The name explains itself :).
      int getLambdaByColour(int port,int colour);
      //! Get output colour by port and lambda.
      int getColourByLambda(int port,int lambda);
};
