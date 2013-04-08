#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>

#define     FRAME_START_CHAR        1
#define     FRAME_END_CHAR          23
#define     DEFAULT_BUFFER_SIZE     1024

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
int listenForService(int port, const char *serviceName, bool waitForConnection);

/**
 *  Establishes a connection to an existing internal service listed at the indicated port.
 *  @param port - the port number where the internal service resides
 *  @param serviceName - a descriptive name used to log errors and successes during the setup process
 *  @returns the file descriptor of the socket for the connection or 0 if failed
 */
int connectToService(int port, const char *serviceName, const char *serverName);

bool guaranteedSocketWrite(int sockfd, char *stream, int length);

bool checkSocketHasData(int sockfd);

#endif
