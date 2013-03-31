

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "common.h"

typedef enum { Message_Join, Message_Kick, Message_Whisper, Message_Speak, Message_List, Message_Quit, Message_KeepAlive } Message_Type;

class Message {
public:
    Message(Message_Type type, char* data);

    Message_Type type;
    char *data;
};


#endif
