#!/bin/bash
root=OBSmodules
twoRuns="PacketDispatcherTest PacketBurstifierTest BurstSenderTest CoreNodeTest \
CoreInputTest CoreOutputTest OEConverterTest EOConverterTest DropBurstTest"
OK=0
for i in $twoRuns; do
	cd $i
	../../$root -c r1 -n ../.. -u Cmdenv > /dev/null
	status=$?
	echo -n "$i (r1): "
	if [ $status = "0" ]; then
		echo "OK"
	elif [ $status = "3" ]; then
		echo "ERROR"
		OK=1
	elif [ $status = "4" ]; then
		echo "WARNING"
		OK=1
	else
		echo "NOT SIMULATED"
		OK=1
	fi
	../../$root -c r2 -n ../.. -u Cmdenv > /dev/null
	status=$?
	echo -n "$i (r2): "
	if [ $status = "0" ]; then
		echo "OK"
	elif [ $status = "3" ]; then
		echo "ERROR"
		OK=1
	elif [ $status = "4" ]; then
		echo "WARNING"
		OK=1
	else
		echo "NOT SIMULATED"
		OK=1
	fi
	cd ..
done

oneRun="FileBurstifierTest BurstDisassemblerTest OpticalMonitorTest"
for i in $oneRun; do
	cd $i
	../../$root -c r1 -n ../.. -u Cmdenv > /dev/null
	status=$?
	echo -n "$i: "
	if [ $status = "0" ]; then
		echo "OK"
	elif [ $status = "3" ]; then
		echo "ERROR"
		OK=1
	elif [ $status = "4" ]; then
		echo "WARNING"
		OK=1
	else
		echo "NOT SIMULATED"
		OK=1
	fi
	cd ..
done

echo -n "OBSMODULES TESTS "
if [ $OK = "0" ]; then
	echo "WITHOUT PROBLEMS."
else
	echo "WITH PROBLEMS."
	echo "Try to simulate the tests with an output different from OK individually to look for errors."
fi




