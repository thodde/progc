#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "common.h"

/*
bool sendFrame(Frame *outFrame) {
    return true;
};
 */


int run(int port_dll, int port_listening) {
    printf("my dll is at port: %i and I'm listening on port: %i\n", port_dll, port_listening);

    int listen_sd = socket(AF_INET, SOCK_STREAM, 0);

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
