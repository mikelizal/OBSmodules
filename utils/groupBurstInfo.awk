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
#===================
# groupBurstInfo.awk
#===================
#
# Description:
#    This script groups both BCP and Burst information into one single line in order to
#    make post-processing easier.
#
# Input:
#    OBS_OpticalMonitor output file
# Output:
#    The generated vector will be printed to stdout. The output vector have the following format:
#   
#     ____________________________________________________________________ 
#    |   (Burst Identifier)    | BCP Info |           Burst Info          |
#    |--------------------------------------------------------------------|
#    | id_burstifier | num_seq |   t_bcp  | t_burst | burst_size | colour |
#    |____________________________________________________________________|
#
#
#Example of use:
#===============
#
# shell:> awk -F " " -f groupBurstInfo.awk monitorOut.dat > burstInfo.dat
#

BEGIN{
   printf("#id_burstifer num_seq t_bcp t_burst burst_size colour\n\n");
}
{
   if($1 == 2){
      t_bcp[$3,$4] = $2;
      colour[$3,$4] = $5;
   }
   if($1 == 1){
      printf("%d %d %f %f %d %d\n",$3,$4,t_bcp[$3,$4],$2,$5,colour[$3,$4]);
      delete t_bcp[$3,$4];
      delete colour[$3,$4];
   } 
}
