#ifndef __CHAT_SERVER_H___
#define __CHAT_SERVER_H___

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "common.h"
#include "Message.h"
#include "NetworkLayer.h"

class ChatServer {
	public:
		int run(int argc, char *argv[]);

	private:
   		int port;
    		bool isServer;
};

#endif
