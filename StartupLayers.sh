#!/bin/bash 

declare result=$(pgrep "PhysicalLayer")
if [ -n "$result" ] ; then
	echo "Physical Layer process already running. Killing..."
	kill -9 $result
fi
./PhysicalLayer & > /dev/null 

declare dataLinkResult=$(pgrep "DatalinkLayer")
if [ -n "$dataLinkResult" ] ; then
	echo "Data Link Layer process already running. Killing..."
	kill -9 $dataLinkResult
fi
./DatalinkLayer & > /dev/null

echo "Done."
