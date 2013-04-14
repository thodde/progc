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

struct MemberNode {
    char* username;
    MemberNode* next;
};

class ChatServer {
	public:
        ChatServer();
		int run(int argc, char *argv[]);
        void list_users();
        bool add_user(char* user_name);
        bool remove_user(char* user_name);
        bool send_file();
        void speak(Message* m);
        void receive_message(Message* m);

        NetworkLayer* networkLayer;
        MemberNode* head_ptr;
        MemberNode* user_list;

	private:
   		int port;
    		bool isServer;
};

#endif
