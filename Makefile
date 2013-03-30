all: chat_server chat_client dl_layer phy_layer

chat_server: chat_server.o dl_layer.o phy_layer.o 
	g++ -o chat_server chat_server.o dl_layer.o phy_layer.o 

ChatClient: ChatClient.o dl_layer.o phy_layer.o
	g++ -o ChatClient ChatClient.o dl_layer.o phy_layer.o

common.o: common.cpp
	g++ -c common.cpp

chat_server.o: chat_server.cpp common.o
	g++ -c chat_server.cpp common.o

ChatClient.o: ChatClient.cpp common.o
	g++ -c ChatClient.cpp common.o

dl_layer.o: dl_layer.cpp
	g++ -c dl_layer.cpp

phy_layer.o: phy_layer.cpp
	g++ -c phy_layer.cpp

clean:
	rm -rf *o ChatClient chat_server common dl_layer phy_layer
