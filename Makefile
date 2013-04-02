all: ChatServer ChatClient PhysicalLayer DatalinkLayer testAZ

ChatServer: ChatServer.o NetworkLayer.o common.o Message.o
	g++ -o ChatServer ChatServer.o NetworkLayer.o common.o Message.o

ChatClient: ChatClient.o NetworkLayer.o common.o Message.o
	g++ -o ChatClient ChatClient.o NetworkLayer.o common.o Message.o

testAZ: testAZ.o Message.o NetworkLayer.o common.o
	g++ -o testAZ testAZ.o Message.o NetworkLayer.o common.o 

testAZ.o: testAZ.cpp
	g++ -c testAZ.cpp

common.o: common.cpp
	g++ -c common.cpp

ChatServer.o: ChatServer.cpp common.o Message.o
	g++ -c ChatServer.cpp common.o Message.o

ChatClient.o: ChatClient.cpp common.o Message.o
	g++ -c ChatClient.cpp common.o Message.o

DatalinkLayer: DatalinkLayer.o common.o
	g++ -o DatalinkLayer DatalinkLayer.o common.o

DatalinkLayer.o: DatalinkLayer.cpp
	g++ -c DatalinkLayer.cpp

PhysicalLayer: PhysicalLayer.o common.o
	g++ -o PhysicalLayer PhysicalLayer.o common.o

PhysicalLayer.o: PhysicalLayer.cpp
	g++ -c PhysicalLayer.cpp

NetworkLayer.o: NetworkLayer.cpp
	g++ -c NetworkLayer.cpp

Message.o: Message.cpp
	g++ -c Message.cpp

clean:
	rm -rf *o ChatClient ChatServer common DatalinkLayer PhysicalLayer
