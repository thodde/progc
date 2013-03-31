
#ifndef __COMMON_H__
#define __COMMON_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


#define     MAX_FRAME_SIZE          192
#define     MAX_PACKET_SIZE         100
#define     PACKET_START            1
#define     PACKET_END              23

#define     EXTERNAL_PORT           2001
#define     PH_PORT                 2002
#define     DLL_PORT                2003
#define     NL_PORT                 2004

//#define

typedef enum { Frame_Init, Frame_Data, Frame_Ack, Frame_Final } Frame_Type;
typedef enum { Packet_Init, Packet_Data, Packet_Ack, Packet_Final } Packet_Type;




struct Frame {
    Frame_Type type;
    int payload[MAX_FRAME_SIZE];    // minus overhead
};

struct Packet {
    Packet_Type type;

    int payload[MAX_PACKET_SIZE];

//    char* serialize();
//    void deSerialize(char *inString);
};

int listenForInternalService(int port, const char *serviceName);
int connectToInternalService(int port, const char *serviceName);

#endif
