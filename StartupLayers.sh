# Author: Trevor Hodde
# This script starts up the two bottom layers before starting the application layer
#!/bin/bash -ex 

declare host=${1:-127.0.0.1}

declare result=$(pgrep "PhysicalLayer")
if [ -n "$result" ] ; then
	echo "Physical Layer process already running. Killing..."
	kill -9 $result
fi
./PhysicalLayer.exe 0 0 2001 & > /dev/null 

declare dataLinkResult=$(pgrep "DatalinkLayer")
if [ -n "$dataLinkResult" ] ; then
	echo "Data Link Layer process already running. Killing..."
	kill -9 $dataLinkResult
fi
./DatalinkLayer.exe 2002 2001 & > /dev/null

echo "Done."
