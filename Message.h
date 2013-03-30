

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "common.h"

typedef enum { Message_Join, Message_Kick, Message_Whisper } Message_Type;

class Message {
public:
    Message();

    Message_Type type;
    char *data;
};


#endif