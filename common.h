

#define     MAX_FRAME_SIZE          192
#define     MAX_PACKET_SIZE         100

struct Message {
    int type;
    char *data;
};

struct Frame {
    int type;
    int data[MAX_FRAME_SIZE];
};

struct Packet {
    int type;
    int data[MAX_PACKET_SIZE];
};