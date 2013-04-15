# Author: Trevor Hodde
# This script starts up the two bottom layers before starting the application layer
#!/bin/bash -ex 

declare IP=$(nslookup $(hostname) | grep "Address:" | awk -F : '{print $2}' | tail -1)

declare HOST=$1
declare PORT=$2

if [ -z "$HOST" ] ; then
	HOST=$IP
fi

if [ -z "$PORT" ] ; then
	PORT=2666
fi

declare result=$(pgrep "PhysicalLayer")
if [ -n "$result" ] ; then
	echo "Physical Layer process already running. Killing..."
	kill -9 $result
fi

./PhysicalLayer.exe 0 0 2001 > Physical_Layer.log 

echo "Please Wait. Starting Physical Layer..."
sleep 5

declare dataLinkResult=$(pgrep "DatalinkLayer")
if [ -n "$dataLinkResult" ] ; then
	echo "Data Link Layer process already running. Killing..."
	kill -9 $dataLinkResult
fi

./DatalinkLayer.exe 2002 2001 & > Datalink_Layer.log

echo "Please Wait. Starting Datalink Layer..."
sleep 5

echo "Starting Client..."
./ChatClient.exe $HOST $PORT 

echo "Done."
