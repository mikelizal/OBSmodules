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

//
// Self-message used in OBS_CoreControlUnit to schedule OXC connections/disconnections.
//
// This message kind can take two values:
//
// - OBS_SCHEDULE_OXC (1)
// - OBS_UNSCHEDULE_OXC (2): In this case, only inGate field will be read
//
message OBS_ControlUnitInfo
{
    int inGate = -1; // OXC input gate
    int outGate = -1; //OXC output gate
}
