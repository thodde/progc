#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

//TODO: Make sure all these are accurate
#define     MAX_PACKET_SIZE         192
#define     MAX_PACKET_PAYLOAD      160 //this will need to be lowered for the frame header
#define     MAX_FRAME_SIZE          100

#define     MAX_FRAME_PAYLOAD       100 //this will need to be lowered for the frame header
#define     FRAME_START_CHAR        1
#define     FRAME_END_CHAR          23

#define     EXTERNAL_PORT           2001
#define     PH_PORT                 2002
#define     DLL_PORT                2003

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
int connectToInternalService(int port, const char *serviceName, bool blocking);

#endif
