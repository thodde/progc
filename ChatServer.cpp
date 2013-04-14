#include "ChatServer.h"

ChatServer::ChatServer() {
    //create the list for storing users in the chat room
    user_list = NULL;
    head_ptr = user_list;
}

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

void ChatServer::list_users() {

}

bool ChatServer::add_user(char* user_name) {
    if(user_name == NULL) 
        return false;

    if(user_list == NULL) {
        user_list = new MemberNode();
        user_list->next = NULL;
        user_list->username = user_name;
        head_ptr = user_list;
    }

    printf("Adding %s to user list.\n", user_name);

    MemberNode* tmp = head_ptr;
    while(tmp->next != NULL) {
        tmp = tmp->next;
    }

    tmp->next = new MemberNode();
    tmp = tmp->next;
    tmp->username = user_name;
    tmp->next = NULL;

    return true;
}

bool ChatServer::remove_user(char* user_name) {

}

void ChatServer::receive_message(Message* m) {
    if(m->type == Message_Join) {
        printf("%s joined the chat!\n", m->sourceName);
	    add_user(m->sourceName);
    }
    else if(m->type == Message_Speak) {
        printf("%s: %s\n", m->sourceName, m->data);
    }
    else if(m->type == Message_Kick) {
        printf("%s is being removed from the chat room by %s!\n", m->targetName, m->sourceName);
        remove_user(m->sourceName);
    }
    else if(m->type == Message_Whisper) {
        printf("Private Message from %s: %s\n", m->sourceName, m->data);
    }
    else if(m->type == Message_List) {
        printf("Listing users currently connected to chat room...\n");
	    list_users();
    }
    else if(m->type == Message_Quit) {
        printf("%s has left the room.\n", m->sourceName);
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
