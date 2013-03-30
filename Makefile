all: ChatServer ChatClient

ChatServer: ChatServer.o DatalinkLayer.o PhysicalLayer.o Message.o
	g++ -o ChatServer ChatServer.o DatalinkLayer.o PhysicalLayer.o Message.o

ChatClient: ChatClient.o DatalinkLayer.o PhysicalLayer.o Message.o
	g++ -o ChatClient ChatClient.o DatalinkLayer.o PhysicalLayer.o Message.o

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

Message.o: Message.cpp
	g++ -c Message.cpp

clean:
	rm -rf *o ChatClient ChatServer common DatalinkLayer PhysicalLayer
