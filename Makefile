all: chat_server chat_client

chat_server: chat_server.o 
	gcc -o chat_server chat_server.o 

chat_client: chat_client.o
	gcc -o chat_client chat_client.o

chat_server.o: chat_server.c
	gcc -c chat_server.c

chat_client.o: chat_client.c
	gcc -c chat_client.c

clean:
	rm -rf *o chat_client chat_server
