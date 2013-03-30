#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define MAX_CLIENTS 10
#define MAXLINE 4096   /*max text line length*/

typedef int bool;
#define true 1
#define false 0

//command line parameters
int port;
char* hostname;
int error_rate;

//funtion prototypes
void show_help();

int main(int argc, char *argv[]) {
    char* my_string;
    char* p;
    bool safe_to_connect = false;
    char sendline[MAXLINE];
    char recvline[MAXLINE];
    size_t num_bytes = 100;
    int bytes_read;

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

    //set the dropped packets percentage [default: 0]
    if(argv[3] != NULL) {
       error_rate = atoi(argv[3]);
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
        	if(strstr(sendline, "join") != NULL) {
            if(!safe_to_connect) {
                printf("Connecting to server...\n");
                safe_to_connect = true;
            }
            else {
                printf("You already connected!!!\n");
            }
        	}

        //make sure we are connected to the server before allowing the user to execute other commands
        if(safe_to_connect) {
            if(strstr(sendline, "speak") != NULL) {
                printf("speak\n");
            }
            else if(strstr(sendline, "whisper") != NULL) {
                printf("whisper\n");
            }
            else if(strstr(sendline, "kick") != NULL) {
                printf("kick\n");
            }
            else if(strstr(sendline, "list") != NULL) {
                printf("list\n");
            }
            else if(strstr(sendline, "quit") != NULL) {
                printf("Exiting...\n");
                exit(0);       
            }
            else if(strstr(sendline, "help") != NULL) {
                show_help();
            }
            else {
                //catch all invalid commands, and make sure the user doesn't type 'join' again
                if(strstr(sendline, "join") == NULL) {
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
void show_help() {
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
