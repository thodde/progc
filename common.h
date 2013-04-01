
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
#define     PACKET_START_CHAR       1
#define     PACKET_END_CHAR         23

#define     EXTERNAL_PORT           2001
#define     PH_PORT                 2002
#define     DLL_PORT                2003

typedef enum { Frame_Init, Frame_Data, Frame_Ack, Frame_Final } Frame_Type;
typedef enum { Packet_Init, Packet_Data, Packet_Ack, Packet_Final } Packet_Type;

struct Frame {
    char *nextHop; // name of the machine at the next endpoint
    int port; // target port at the next machine

    unsigned int frameId; // identifier used by sliding window protocol
    Frame_Type type;
    int payload[MAX_FRAME_SIZE];    // minus overhead
    unsigned int checkByte; // byte used for error detection.  MOD 255 of the Frame object to 0
};

struct Packet {
    Packet_Type type;

    int payload[MAX_PACKET_SIZE];

//    char* serialize();
//    void deSerialize(char *inString);
};

/**
 *  Blocks a process until a connect request has been accepted on the specified port.  This is used to bring up
 *  the interal service connections.
 *  @param port - the port of the current service.
 *  @param serviceName - a descriptive name used to log errors and successes during the setup process
 *  @returns the file descriptor of the socket for the connection or 0 if failed
 */
int listenForInternalService(int port, const char *serviceName);

/**
 *  Establishes a connection to an existing internal service listed at the indicated port.
 *  @param port - the port number where the internal service resides
 *  @param serviceName - a descriptive name used to log errors and successes during the setup process
 *  @returns the file descriptor of the socket for the connection or 0 if failed
 */
int connectToInternalService(int port, const char *serviceName);

#endif
