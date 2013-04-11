#include "ChatServer.h"

int ChatServer::run(int argc, char *argv[]) {
	printf("%s\n\n", "");
        printf("-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-\n");
        printf("\tWelcome to the Chat Room: Server Side!\n");
        printf("\t A Simple Chat Application by Team 6\n");
        printf("-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-\n");
        printf("%s\n\n", "");

        //make sure the server is given all the info it needs when it is executed
        if (argc != 2) {
            fprintf(stderr, "\tusage: %s <port>\n", argv[0]);
            fprintf(stderr, "\t<port>\t- port to listen on\n");
            exit(1);
        } 
       
        //assign a value to the port
        port = atoi(argv[1]);

	//create an object for accessing the data link layer
        NetworkLayer* networkLayer = new NetworkLayer();
        if(!networkLayer->initialize(DLL_PORT)) {
            perror("Error, could not connect to internal services layers\n");
            delete networkLayer;
            exit(1);
        }

        return 0;
}

void receive_message(Message* m) {
    if(m->type == Message_Join) {
        printf("Client joined the chat!\n");
    }
    else if(m->type == Message_Speak) {
        printf("%s\n", m->data);
    }
    else if(m->type == Message_Kick) {
        printf("Client is being removed from the chat room by the administrator!\n");
    }
    else if(m->type == Message_Whisper) {
        printf("Private Message from Client: %s\n", m->data);
    }
    else if(m->type == Message_List) {
        printf("Listing users currently connected to chat room:\n");
    }
    else if(m->type == Message_Quit) {
        printf("Exiting...\n");
        exit(0);
    }
}

/**
 * The main function creates a chat server object
 * and starts running it.
 */
int main(int argc, char *argv[]) {
	ChatServer cs = ChatServer();
	cs.run(argc, argv);
}
