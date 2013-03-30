all: ChatServer ChatClient DatalinkLayer PhysicalLayer

ChatServer: ChatServer.o DatalinkLayer.o PhysicalLayer.o
	g++ -o ChatServer ChatServer.o DatalinkLayer.o PhysicalLayer.o

ChatClient: ChatClient.o DatalinkLayer.o PhysicalLayer.o
	g++ -o ChatClient ChatClient.o DatalinkLayer.o PhysicalLayer.o

common.o: common.cpp
	g++ -c common.cpp

ChatServer.o: ChatServer.cpp common.o
	g++ -c ChatServer.cpp common.o

ChatClient.o: ChatClient.cpp common.o
	g++ -c ChatClient.cpp common.o

DatalinkLayer.o: DatalinkLayer.cpp
	g++ -c DatalinkLayer.cpp

PhysicalLayer.o: PhysicalLayer.cpp
	g++ -c PhysicalLayer.cpp

clean:
	rm -rf *o ChatClient ChatServer common DatalinkLayer PhysicalLayer
