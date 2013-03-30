#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "ChatServer.h"

class ChatServer {
    public:
        int port;

    int main(int argc, char *argv[]) {
        printf("%s\n\n", "");
        printf("%s\n", "-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-");
        printf("%s\n", "\tWelcome to the Chat Room: Server Side!");
        printf("%s\n", "\t A Simple Chat Application by Team 6");
        printf("%s\n", "-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-");
        printf("%s\n\n", "");

        //make sure the server is given all the info it needs when it is executed
        if (argc != 2) {
            fprintf(stderr, "\tusage: %s <port>\n", argv[0]);
            fprintf(stderr, "\t<port>\t- port to listen on\n");
            exit(1);
        } 
       
        //assign a value to the port
        port = atoi(argv[1]);
    }
};
