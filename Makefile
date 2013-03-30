all: chat_server chat_client dl_layer phy_layer

chat_server: chat_server.o 
	gcc -o chat_server chat_server.o 

chat_client: chat_client.o
	gcc -o chat_client chat_client.o

dl_layer: dl_layer.o common.o
	gcc -o dl_layer dl_layer.o common.o

phy_layer: phy_layer.o common.o
	gcc -o phy_layer phy_layer.o common.o

common.o: common.c
	gcc -c common.c

chat_server.o: chat_server.c common.o
	gcc -c chat_server.c common.o

chat_client.o: chat_client.c common.o
	gcc -c chat_client.c common.o

dl_layer.o: dl_layer.c
	gcc -c dl_layer.c

phy_layer.o: phy_layer.c
	gcc -c phy_layer.c

clean:
	rm -rf *o chat_client chat_server common dl_layer phy_layer
