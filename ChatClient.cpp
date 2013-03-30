#include "ChatClient.h"

int ChatClient::run(int argc, char *argv[]) {
        char* my_string;
        char* p;
        bool safe_to_connect = false;
        char sendline[MAXLINE];
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
        if (argc < 3 || argc > 4) {
            fprintf(stderr, "\tusage: %s <host> <port> [optional: <error percentage>]\n", argv[0]);
            fprintf(stderr, "\t<host>\t\t\t- IP Address or hostname of server\n");
            fprintf(stderr, "\t<port>\t\t\t- port to connect on\n");
       	    fprintf(stderr, "\t<error percentage>\t- optional packet drop rate [1-100 default: 0]\n");
            exit(1);
        }

        // sets the hostname and the port to communicate with
        hostname = argv[1];
        port = atoi(argv[2]);

        //set the percentage of bits to drop [default: 0]
        if(argv[3] != NULL) {
            error_rate = atoi(argv[3]);
        }
        else {
            error_rate = 0;
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

                    //Create a socket for the client
				    if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
					    perror("Problem in creating the socket");
					    exit(2);
				    }

				    //get the hostname from the command line and grab the associated ip addresses
				    lh = gethostbyname(hostname);
				    addr_list = (struct in_addr **)lh->h_addr_list;

				    memset(&servaddr, 0, sizeof(servaddr));
				    servaddr.sin_family = AF_INET;
				    servaddr.sin_addr.s_addr = "12.34.56.7"; //inet_addr(inet_ntoa(*addr_list[0]));
				    servaddr.sin_port =  htons(port);
	
				    //Connection of the client to the socket
				    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
					    perror("Problem in connecting to the server");
					    exit(3);
				    }

				    //send the command
		        		send(sockfd, sendline, strlen(sendline), 0);
                }
                else {
                    printf("You already connected!!!\n");
                }
            	}

            //make sure we are connected to the server before allowing the user to execute other commands
            if(safe_to_connect) {
                if(strncasecmp(sendline, "speak ", 6) == 0) {
                    printf("speak\n");
                }
                else if(strncasecmp(sendline, "whisper ", 8) == 0) {
                    printf("whisper\n");
                }
                else if(strncasecmp(sendline, "kick ", 5) == 0) {
                    printf("kick\n");
                }
                else if(strncasecmp(sendline, "list", 4) == 0) {
                    printf("list\n");
                }
                else if(strncasecmp(sendline, "quit", 4) == 0) {
                    printf("Exiting...\n");
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
        printf("\tNOTE: Users may also join the chat room as an administrator if one does not already\n");
        printf("\texist to gain administrative privileges. To join as an administrator, a user must specify\n");
        printf("\tthe -op flag with the join command.\n");
        printf("\t\tExample: \n");
        printf("\t\t\tjoin -op Trevor\n\n");

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

/*
int main(int argc, char* argv[]) {
	ChatClient::run(argc, argv);
}*/
