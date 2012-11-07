//
// Copyright (C) 2012 Naiara Garcia Royo
// Copyright (C) 2012 Universidad Publica de Navarra
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
#include <fstream>
#include <omnetpp.h>
#include "OBS_BurstControlPacket_m.h"

using namespace std;

//! BCP receiver: receives iniBCP and endBCP packets.
//! Output for OBS_EOConverter.
class testSink4 : public cSimpleModule{
   protected:
      virtual void initialize();
      virtual void finish();
      virtual void handleMessage(cMessage *msg);
   private:
      void openFiles();
      bool compareFiles();
      void deleteFiles();
      ifstream currentFile, patternFile;
      ofstream outputFile;
      string errorPhrase;
};
