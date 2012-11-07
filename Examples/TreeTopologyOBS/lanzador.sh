#!/bin/bash

execToLaunch="./TreeTopologyOBSr$1"
tmpFile="/tmp/pmaptmp"

#./pmap.sh $$ "${execToLaunch}" > "${tmpFile}"  &
#./pmap.sh $$ "./TreeTopologyOBS" > "${tmpFile}"  &
./pmap.sh $$ "obsmodules" > "${tmpFile}"  &


if [ ! -d trace$2 ]; then
 mkdir trace$2
fi

initTime=`date "+%s"`
${execToLaunch} > trace$2/trace$3G-load$4
endTime=`date "+%s"`

echo "Simulation duration: " $(( $endTime-$initTime )) "(sec)"
echo "Simulation memory file: "
cat "${tmpFile}"
#echo "Simulation memory mean: " `awk 'BEGIN{i=0; sum=0} {i=i+1; split($4, a, "K"); sum=sum+a[1]} END{print sum/i}' "${tmpFile}"`
echo "Simulation memory mean: " `awk 'BEGIN{i=0; sum=0} {i=i+1; sum=sum+$4} END{print sum/i}' "${tmpFile}"`
rm -rf "${tmpFile}"

