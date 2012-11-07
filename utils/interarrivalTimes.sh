#
# Copyright (C) 2010-2012 Javier Armendariz Silva, Naiara Garcia Royo
# Copyright (C) 2010-2012 Universidad Publica de Navarra
#
# This file is part of OBSModules.
#
# OBSModules is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# OBSModules is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with OBSModules.  If not, see <http:#www.gnu.org/licenses/>.
#

#/bin/bash

# Extract and store in a file the interarrival time between bursts
awk -F " " 'Begin{ant=0;}{printf("%f\n",$4-ant); ant=$4}' burstInfo.dat > burstInterArrival.dat

# In this file, we calculate the distance between BCP and Burst
awk -F " " '{printf("%f\n", $4-$3 )}' burstInfo.dat > burstOffsets.dat
