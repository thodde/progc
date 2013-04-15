#ifndef __CHAT_CLIENT_H___
#define __CHAT_CLIENT_H___

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "common.h"
#include "NetworkLayer.h"
#include "Message.h"

#define MAXLINE 4096   /*max text line length*/

class ChatClient {
    //command line parameters
    public:
        int port;
        char* hostname;

    //funtion prototypes
    public:
        int run(int argc, char* argv[]);
        void show_help();
	void receive_message(Message* m);

    private:
        unsigned int messagesSent;
};
#endif
