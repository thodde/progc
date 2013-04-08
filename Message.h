

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "common.h"

//TODO validate this
#define MESSAGE_HEADER_SIZE         20

typedef enum { Message_Join, Message_Kick, Message_Whisper, Message_Speak, Message_List, Message_Quit, Message_Stack_Control } Message_Type;

struct Message {
public:
    Message(Message_Type newType, char* newData, int size, unsigned int newId);
    Message(char *stream);

    char *targetMachine;  //perhaps names like SERVER or CLIENT_A?  Or possibly an IP address

    Message_Type type;
    int dataLength; //length of the data field
    char *data;
    unsigned int messageId;

    char* serialize(int &serializedLength);
    bool deSerialize(char *stream);
};


Message* createConnectToServerMessage(char *serverName, int serverPort);
Message* createListenForClientsMessage(int serverPort);

//arbitrary max message length 50 packets

#endif
