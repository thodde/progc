#include "ChatServer.h"

ChatServer::ChatServer() {
    //create the list for storing users in the chat room
    user_list = NULL;
    head_ptr = user_list;
}

int ChatServer::run(int argc, char *argv[]) {
    bool sentJoinMessage = false;
    Message* response = NULL;
    bool hasError = false;

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
    networkLayer = new NetworkLayer();
    if(!networkLayer->initialize(port)) {
        perror("Error, could not connect to internal services layers\n");
        delete networkLayer;
        exit(1);
    }

    while(1) {
        if(!sentJoinMessage) {
            networkLayer->sendMessage(createListenForClientsMessage(2666));
            sentJoinMessage = true;
        }
        response = networkLayer->checkForMessages(hasError);
    }

    return 0;
}

//TODO: Implement
bool send_file() {
    return true;
}   

/**
 * Display the list of currently connected users. Sends each
 * username in a separate message.
 */
void ChatServer::list_users(Message* m) {
    int messagesSent = 0;
    MemberNode* cursor = user_list;

    while(cursor->next != NULL) {
        printf("%s\n", cursor->username);
        Message* message = new Message(Message_List, cursor->username, strlen(cursor->username), messagesSent++, (char*)"server", m->sourceName);
        networkLayer->sendMessage(message);
        cursor = cursor->next;
    }
}

/**
 * Add a user to the chat room
 */
bool ChatServer::add_user(char* user_name) {
    if(user_name == NULL) 
        return false;

    //create the list
    if(user_list == NULL) {
        user_list = new MemberNode();
        user_list->next = NULL;
        user_list->username = user_name;
        head_ptr = user_list;
    }

    printf("Adding %s to user list.\n", user_name);

    //iterate to the end of the list
    MemberNode* tmp = head_ptr;
    while(tmp->next != NULL) {
        tmp = tmp->next;
    }

    //add the new user to the end of the list
    tmp->next = new MemberNode();
    tmp = tmp->next;
    tmp->username = user_name;
    tmp->next = NULL;

    return true;
}

/**
 * Remove a user from the chat room. There are two scenarios:
 *   1) The user has been kicked out
 *   2) The user has requested to exit
 */
bool ChatServer::remove_user(char* user_name) {
    int messagesSent = 0;

    if(user_name == NULL) 
        return false;

    //in case the list is empty
    if(user_list == NULL)
        return false;

    //in case the name is the first item in the list
    if(strcasecmp(user_name, head_ptr->username) == 0) {
        head_ptr->username = NULL;
        head_ptr->next = NULL;        
        return true;
    }

    MemberNode* tmp = head_ptr;
    MemberNode* previous = head_ptr;

    //in case the name is somewhere in the middle of the list
    while(tmp->next != NULL) {
        if(strcasecmp(user_name, tmp->next->username) == 0) {
            tmp->next = tmp->next->next;
            delete previous->next;
            return true;
        }

        tmp = tmp->next;

        //in case the name is the last item in the list
        if((tmp->next == NULL) && (strcasecmp(user_name, tmp->next->username) == 0)) {
            delete tmp;
            previous->next = NULL;
            return true;
        }

        previous = previous->next;
    }

    char* msg;
    strcpy(msg, "You have been removed from the chat room");
    Message* message = new Message(Message_Kick, msg, strlen(msg), messagesSent++, (char*)"server", user_name);
    networkLayer->sendMessage(message);
    
    return true;
}

/**
 * Sends a message to every user in the chat room.
 */
void ChatServer::speak(Message* m) {
    int messagesSent = 0;
    MemberNode* cursor = head_ptr;

    //iterate through the user list and send the message to everyone
    while(cursor->next != NULL) {
        //do not send the message back to the sender
        if(strcasecmp(cursor->username, m->sourceName) != 0) {
            Message* message = new Message(Message_Speak, m->data, strlen(m->data), messagesSent++, (char*)"server", cursor->username);
            networkLayer->sendMessage(message);
        }
        cursor = cursor->next;
    }
}

/**
 * Sends a message to a single user in the chat room.
 */
void ChatServer::whisper(Message* m) {
    int messagesSent = 0;
    MemberNode* cursor = head_ptr;

    //send the message directly to the user
    Message* message = new Message(Message_Speak, m->data, strlen(m->data), messagesSent++, (char*)"server", m->targetName);
    networkLayer->sendMessage(message);
}

/**
 * Tells every user in the chat room that a user has left.
 */
void ChatServer::quit(Message* m) {
    int messagesSent = 0;
    char* exitMessage;
    MemberNode* cursor = head_ptr;

    //build the message to be sent
    strcpy(exitMessage, m->sourceName);
    strcat(exitMessage, " has left the chat room.");

    //iterate through the user list and send the message to everyone
    while(cursor->next != NULL) {
        //do not send the message back to the sender
        if(strcasecmp(cursor->username, m->sourceName) != 0) {
            Message* message = new Message(Message_Quit, exitMessage, strlen(exitMessage), messagesSent++, (char*)"server", cursor->username);
            networkLayer->sendMessage(message);
        }
        cursor = cursor->next;
    }
}

/**
 * This function determines what type of message has been received
 * and it acts accordingly.
 */
void ChatServer::receive_message(Message* m) {
    if(m->type == Message_Join) {
        printf("%s joined the chat!\n", m->sourceName);
	    add_user(m->sourceName);
    }
    else if(m->type == Message_Speak) {
        printf("%s:\n %s\n", m->sourceName, m->data);
        speak(m);
    }
    else if(m->type == Message_Kick) {
        printf("%s is being removed from the chat room by %s!\n", m->targetName, m->sourceName);
        remove_user(m->sourceName);
    }
    else if(m->type == Message_Whisper) {
        printf("Private Message from %s:\n %s\n", m->sourceName, m->data);
        whisper(m);
    }
    else if(m->type == Message_List) {
        printf("Listing users currently connected to chat room...\n");
	    list_users(m);
    }
    else if(m->type == Message_SendFile) {
        //TODO: Implement
        printf("IM SENDING A GIANT FILE!!!!");
    }
    else if(m->type == Message_Quit) {
        printf("%s has left the room.\n", m->sourceName);
        remove_user(m->sourceName);
        quit(m);
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
