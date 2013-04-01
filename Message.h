

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "common.h"

typedef enum { Message_Join, Message_Kick, Message_Whisper, Message_Speak, Message_List, Message_Quit } Message_Type;

struct Message {
public:
    Message(Message_Type newType, const char* newData);

    char *targetMachine;  //perhaps names like SERVER or CLIENT_A?  Or possibly an IP address

    Message_Type type;
    int dataLength; //length of the data field
    const char *data;

    //serialize
    //deSerialize
};



#endif
