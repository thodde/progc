#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "common.h"
#include "phy_layer.h"

/*
bool sendFrame(Frame *outFrame) {
    return true;
};
 */


int run(int port_dll, int port_listening) {
    printf("my dll is at port: %i and I'm listening on port: %i\n", port_dll, port_listening);

    int listen_sd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in cliaddr, servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    listen(listenfd, LISTENQ);

    printf("%s\n","Server running...waiting for connections.");

    clilen = sizeof(cliaddr);
    connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);

    while (1) {
        break;
    }

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("please initialize the physical layer using the following:\n");
        printf("\tphy_layer listening_port dll_port\n");
        return(-1);
    }

    int port_server = atoi(argv[2]);
    int port_dll = atoi(argv[3]);

    return run(port_dll, port_server);
};
