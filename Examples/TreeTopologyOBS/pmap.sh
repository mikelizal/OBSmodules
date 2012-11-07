#!/bin/bash

while [ `ps -p $1 | tail -n 1 | awk '{print $1}'` != "PID" ] ; do
	pid=`ps x | grep $2 | grep -v "pmap.sh" | awk 'NR==1{print $1}'`
	#if [ ${pid} != "" ] ; then
	if [ ! ${pid} = "" ] ; then
		pmap -d ${pid} | tail -n 1
	fi

	sleep 1

done

