#include "ChatClient.h"

int ChatClient::run(int argc, char *argv[]) {
        messagesSent = 0;
        char* my_string;
        char* p;
        bool safe_to_connect = false;
        char *sendline = new char[MAXLINE];
        char recvline[MAXLINE];
        size_t num_bytes = 100;
        int bytes_read;
        int sockfd;
        struct hostent* lh;
        struct in_addr** addr_list;
        struct sockaddr_in servaddr;

        //display a nice interface
        printf("%s\n\n", "");
        printf("%s\n", "-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-");
        printf("%s\n", "\tWelcome to the Chat Room: Client Side!");
        printf("%s\n", "\t A Simple Chat Application by Team 6");
        printf("%s\n", "-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-");
        printf("%s\n", ""); 

        //make sure the correct number of arguments are specified
        if (argc < 3) {
            fprintf(stderr, "\tusage: %s <host> <port> [optional: <error percentage>]\n", argv[0]);
            fprintf(stderr, "\t<host>\t\t\t- IP Address or hostname of server\n");
            fprintf(stderr, "\t<port>\t\t\t- port to connect on\n");
            exit(1);
        }

        // sets the hostname and the port to communicate with
        hostname = argv[1];
        port = atoi(argv[2]);

        //create an object for accessing the data link layer
        NetworkLayer* networkLayer = new NetworkLayer();
        if(!networkLayer->initialize(DLL_PORT)) {
            perror("Error, could not connect to internal services layers\n");
            delete networkLayer;
            exit(1);
        }

        printf("To enter the chat room, use the 'join' command.\n");

        while(1) {
            //read from stdin
		    printf("Enter a command: ");
		    my_string = (char*) malloc(num_bytes + 1);
		    bytes_read = getline(&my_string, &num_bytes, stdin);
		    strcpy(sendline, my_string);
		    p = strchr(sendline, '\n');
		    if (p != NULL) {
			    *p = '\0';  // get rid of the '\n'
            }

            	//If the client receives a join command, start a connection to the server
            	if(strncasecmp(sendline, "join ", 5) == 0) {
                if(!safe_to_connect) {
                    printf("Connecting to server...\n");
                    safe_to_connect = true;

                    Message *m = new Message(Message_Join, sendline, strlen(sendline), messagesSent++, (char*)"source", (char*)"target");
                    networkLayer->sendMessage(m);
                }
                else {
                    printf("You already connected!!!\n");
                }
            	}

            //make sure we are connected to the server before allowing the user to execute other commands
            if(safe_to_connect) {
                if(strncasecmp(sendline, "speak ", 6) == 0) {
                    printf("speak\n");
                    Message *m = new Message(Message_Speak, sendline, strlen(sendline), messagesSent++, (char*)"source", (char*)"target");
                    networkLayer->sendMessage(m);
                }
                else if(strncasecmp(sendline, "whisper ", 8) == 0) {
                    printf("whisper\n");
                    Message *m = new Message(Message_Whisper, sendline, strlen(sendline), messagesSent++, (char*)"source", (char*)"target");
                    networkLayer->sendMessage(m);
                }
                else if(strncasecmp(sendline, "kick ", 5) == 0) {
                    printf("kick\n");
                    Message *m = new Message(Message_Kick, sendline, strlen(sendline), messagesSent++, (char*)"source", (char*)"target");
                    networkLayer->sendMessage(m);
                }
                else if(strncasecmp(sendline, "list", 4) == 0) {
                    printf("list\n");
                    Message *m = new Message(Message_List, sendline, strlen(sendline), messagesSent++, (char*)"source", (char*)"target");
                    networkLayer->sendMessage(m);
                }
                else if(strncasecmp(sendline, "quit", 4) == 0) {
                    printf("Remove this message later...Exiting...\n");
                    Message *m = new Message(Message_Quit, sendline, strlen(sendline), messagesSent++, (char*)"source", (char*)"target");
                    networkLayer->sendMessage(m);
                    exit(0);       
                }
                else if(strncasecmp(sendline, "help", 4) == 0) {
                    show_help();
                }
                else {
                    //catch all invalid commands, and make sure the user doesn't type 'join' again
                    if(strncasecmp(sendline, "join ", 5) != 0) {
                        printf("Invalid command. Type 'help' to see a list of valid commands.\n");
                    }
                }
            }
            else {
                //the user cannot use the commands until they are connected
                printf("You have to join the chat room first!\n");
            }
            printf("\n");
        }
    delete networkLayer;
    printf("Done.\n");
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
        printf("Client is being removed from the chat room!\n");
    }
    else if(m->type == Message_Whisper) {
        printf("Private Message from Client: %s\n", m->data);
    }
    else if(m->type == Message_List) {
        printf("Listing users currently connected to chat room:\n");
    }
    else if(m->type == Message_Quit) {
        printf("Exiting...\n");
    }
}

/**
 *  This function displays a list of command usages for the supported
 *  chat room commands.
 */
void ChatClient::show_help() {
        printf("List of valid commands for the chat room:\n\n");
        printf("\t- Join - Allow a user to join the chat room. Must specify a user name.\n");
        printf("\t\tExample: \n");
        printf("\t\t\tjoin Trevor\n\n");

        printf("\t- Kick - Forcibly remove a user from the chat room by specifying their username.\n");
        printf("\t\tExample: \n");
        printf("\t\t\tkick James\n\n");

        printf("\t- Speak - Send a message to all users in the chat room.\n");
        printf("\t\tExample: \n");
        printf("\t\t\tspeak Hello there!\n\n");

        printf("\t- Whisper - Send a private message to a single user in the chat room by specifying their name.\n");
        printf("\t\tExample: \n");
        printf("\t\t\twhisper Peiwei Hello there!\n\n");

        printf("\t- List - Display a list of all users in the chat room.\n");
        printf("\t\tExample: \n");
        printf("\t\t\tlist\n\n");

        printf("\t- Help - Display these instructions about how to use the chat room commands.\n");
        printf("\t\tExample: \n");
        printf("\t\t\thelp\n\n");

        printf("\t- Quit - Allow a user to exit the chat room on their own free will.\n");
        printf("\t\tExample: \n");
        printf("\t\t\tquit\n\n");
}

/**
 *  This is the main function which creates a chat client object
 *  and runs the chat.
 */
int main(int argc, char* argv[]) {
	ChatClient cc = ChatClient();
	cc.run(argc, argv);
}
