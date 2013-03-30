
#ifndef __COMMON_H__
#define __COMMON_H__

#define     MAX_FRAME_SIZE          192
#define     MAX_PACKET_SIZE         100
#define     PACKET_START            1
#define     PACKET_END              23
//#define

typedef enum { Message_Join, Message_Kick, Message_Whisper } Message_Type;
typedef enum { Frame_Init, Frame_Data, Frame_Ack, Frame_Final } Frame_Type;
typedef enum { Packet_Init, Packet_Data, Packet_Ack, Packet_Final } Packet_Type;


class Message {
public:
    Message_Type type;
    char *data;
};

class Frame {
public:
    Frame_Type type;
    int payload[MAX_FRAME_SIZE];
};

class Packet {
public:
    Packet_Type type;
    int payload[MAX_PACKET_SIZE];

    char* serialize();
    void deSerialize(char *inString);
};

#endif
