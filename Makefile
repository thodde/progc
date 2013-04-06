all: ChatServer ChatClient PhysicalLayer DatalinkLayer testAZ

ChatServer: ChatServer.o NetworkLayer.o Packet.o common.o Message.o
	g++ -o ChatServer.exe ChatServer.o NetworkLayer.o Packet.o common.o Message.o

ChatClient: ChatClient.o NetworkLayer.o Packet.o common.o Message.o
	g++ -o ChatClient.exe ChatClient.o NetworkLayer.o Packet.o common.o Message.o

testAZ: testAZ.o Message.o NetworkLayer.o Packet.o common.o
	g++ -o testAZ.exe testAZ.o Message.o NetworkLayer.o Packet.o common.o 

testAZ.o: testAZ.cpp
	g++ -c testAZ.cpp

common.o: common.cpp
	g++ -c common.cpp

ChatServer.o: ChatServer.cpp
	g++ -c ChatServer.cpp

ChatClient.o: ChatClient.cpp
	g++ -c ChatClient.cpp

DatalinkLayer: DatalinkLayer.o Frame.o Packet.o common.o
	g++ -o DatalinkLayer.exe DatalinkLayer.o Frame.o Packet.o common.o

DatalinkLayer.o: DatalinkLayer.cpp
	g++ -c DatalinkLayer.cpp

PhysicalLayer: PhysicalLayer.o Frame.o common.o
	g++ -o PhysicalLayer.exe PhysicalLayer.o Frame.o common.o

PhysicalLayer.o: PhysicalLayer.cpp
	g++ -c PhysicalLayer.cpp

NetworkLayer.o: NetworkLayer.cpp Packet.cpp
	g++ -c NetworkLayer.cpp Packet.cpp

Message.o: Message.cpp
	g++ -c Message.cpp

clean:
	rm -rf *.o *.exe
