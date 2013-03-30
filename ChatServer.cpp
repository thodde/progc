#include "ChatServer.h"


ChatServer::ChatServer() {
    printf("%s\n\n", "");
    printf("-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-\n");
    printf("\tWelcome to the Chat Room: Server Side!\n");
    printf("\t A Simple Chat Application by Team 6\n");
    printf("-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-\n");
    printf("%s\n\n", "");
}

int ChatServer::run(int argc, char *argv[]) {
        //make sure the server is given all the info it needs when it is executed
        if (argc != 2) {
            fprintf(stderr, "\tusage: %s <port>\n", argv[0]);
            fprintf(stderr, "\t<port>\t- port to listen on\n");
            exit(1);
        } 
       
        //assign a value to the port
        port = atoi(argv[1]);
        return 0;
}

int main(int argc, char *argv[]) {
	
}
