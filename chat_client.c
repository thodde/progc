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

int port;
char* hostname;
int error_rate;

int main(int argc, char *argv[]) {
    char* my_string;
    char* p;
    bool safe_to_connect = false;
    char sendline[MAXLINE];
    char recvline[MAXLINE];
    size_t num_bytes = 100;
    int bytes_read;

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

        	//If the client receives a login command, start a connection to the server
        	if(strstr(sendline, "join") != NULL) {
        	    safe_to_connect = true;
        	}
        else if(strstr(sendline, "quit") != NULL) {
            printf("Exiting...\n");
            exit(0);       
        }
        printf("\n");
    }
}
