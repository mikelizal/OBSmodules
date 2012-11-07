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

//Also known as BCP. Control Message for OBS transmissions.
//
//The kind field identifies the class of this message:
//
// - <b>kind 1</b>: Optical BCP beginning
// - <b>kind 2</b>: Optical BCP end
// - <b>kind 3</b>: Electrical BCP (used on OBS_CoreControlUnit)
packet OBS_BurstControlPacket
{
    simtime_t burstArrivalDelta = -1; //Difference between current time and estimated burst arrival time
    int burstColour = -1; //Burst's colour
    int label = -1; //Destination label

    int burstifierId = -1; //Burstifier id
    int numSeq = -1; //Sequence number into packet burstifier

    int senderId = -1; //Burst sender identifier. Not used as a Id

    int burstSize = -1; // Associated burst size
}
