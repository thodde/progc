

#define     MAX_FRAME_SIZE          192
#define     MAX_PACKET_SIZE         100
#define     MESSAGE_INITIATOR       1
#define     MESSAGE_TERMINATOR      23

typedef enum { Message_Join, Message_Kick, Message_Whisper } Message_Type;
typedef enum { Frame_Init, Frame_Data, Frame_Ack, Frame_Final } Frame_Type;
typedef enum { Packet_Init, Packet_Data, Packet_Ack, Packet_Final } Packet_Kind;


typedef struct {
    Message_Type type;
    char *data;
} Message;

typedef struct {
    Frame_Type type;
    int payload[MAX_FRAME_SIZE];
} Frame;

typedef struct {
    Packet_Kind type;
    int payload[MAX_PACKET_SIZE];
} Packet;

struct sockaddr_in cliaddr, servaddr;



