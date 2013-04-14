

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "common.h"

//TODO validate this
#define MESSAGE_HEADER_SIZE         40

typedef enum { Message_Join, Message_Kick, Message_Whisper, Message_Speak, Message_List, Message_Quit, Message_SendFile, Message_Stack_Control } Message_Type;

struct Message {
public:
    Message(Message_Type newType, char* newData, int size, unsigned int newId, char *newSourceName, char *newTargetName);
    Message(char *stream);

    Message_Type type;
    int dataLength; //length of the data field
    char *data;
    unsigned int messageId;
    char sourceName[10];
    char targetName[10];

    char* serialize(int &serializedLength);
    bool deSerialize(char *stream);
};

Message* createConnectToServerMessage(char *serverAddress, char *clientName, int serverPort);
Message* createListenForClientsMessage(int serverPort);

//arbitrary max message length 50 packets

#endif
