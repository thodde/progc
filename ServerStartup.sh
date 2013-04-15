# Author: Trevor Hodde
# This script starts up the two bottom layers before starting the application layer
#!/bin/bash -ex 

declare PORT=$1

if [ -z "$PORT" ] ; then
	PORT=2666
fi

declare result=$(pgrep "PhysicalLayer")
if [ -n "$result" ] ; then
	echo "Physical Layer process already running. Killing..."
	kill -9 $result
fi

./PhysicalLayer.exe 0 0 3001 & > Physical_Layer.log 

echo "Please Wait. Starting Physical Layer..."
sleep 5 

declare dataLinkResult=$(pgrep "DatalinkLayer")
if [ -n "$dataLinkResult" ] ; then
	echo "Data Link Layer process already running. Killing..."
	kill -9 $dataLinkResult
fi

./DatalinkLayer.exe 3002 3001 & > Datalink_Layer.log

echo "Please Wait. Starting Datalink Layer..."
sleep 5

echo "Starting Server..."
./ChatServer.exe 3002 

echo "Done."
