

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "common.h"

#define MESSAGE_HEADER_SIZE         10

typedef enum { Message_Join, Message_Kick, Message_Whisper, Message_Speak, Message_List, Message_Quit } Message_Type;

struct Message {
public:
    Message(Message_Type newType, char* newData);

    char *targetMachine;  //perhaps names like SERVER or CLIENT_A?  Or possibly an IP address

    Message_Type type;
    int dataLength; //length of the data field
    char *data;

    char* serialize();
    //deSerialize
};



#endif
