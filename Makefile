all: ChatServer ChatClient dl_layer phy_layer

ChatServer: ChatServer.o dl_layer.o phy_layer.o 
	g++ -o ChatServer ChatServer.o dl_layer.o phy_layer.o 

ChatClient: ChatClient.o dl_layer.o phy_layer.o
	g++ -o ChatClient ChatClient.o dl_layer.o phy_layer.o

common.o: common.cpp
	g++ -c common.cpp

ChatServer.o: ChatServer.cpp common.o
	g++ -c ChatServer.cpp common.o

ChatClient.o: ChatClient.cpp common.o
	g++ -c ChatClient.cpp common.o

dl_layer.o: dl_layer.cpp
	g++ -c dl_layer.cpp

phy_layer.o: phy_layer.cpp
	g++ -c phy_layer.cpp

clean:
	rm -rf *o ChatClient ChatServer common dl_layer phy_layer
