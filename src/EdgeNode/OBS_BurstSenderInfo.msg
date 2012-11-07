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

// Additional burst control info included in OBS_BurstSender's automessages.
class OBS_BurstSenderInfo
{
      int burstId = -1; // Internal burst Id used only by sender module. This is independent for any other Id.
      int burstifierId = -1; // Burstifier Id. Part of burst identifier.
      int numSeq = -1; // Sequence number. The other part of burst identifier.
      int assignedLambda = -1; // Assigned lambda.
      int label = -1; // Destination label.
      //simtime_t time; // Message creation timestamp. Used to insert into BCP's sender queue.
}
