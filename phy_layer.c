#include <stdio.h>
#include <string.h>
#include "common.h"

/*
bool sendFrame(Frame *outFrame) {
    return true;
};
 */

int runClient(int port_dll) {
    printf("I'm a client, my dll is at port: %i\n", port_dll);

    while (1) {
        break;
    }

    return 0;
};

int runServer(int port_dll, int port_external) {
    printf("I'm a server, my dll is at port: %i and my server is listening on port: %i\n", port_dll, port_external);

    while (1) {
        break;

    }
    return 0;
};

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("please initialize the physical layer using the following\n");
        printf("\tphy_layer server listening_port dll_port\n");
        printf("\tphy_layer client dll_port\n");
        return(-1);
    }
    
    if (strcmp(argv[1], "server") == 0) {
        //server input validation
        if (argc < 4) {
            printf("\tphy_layer server listening_port dll_port\n");
            return(-1);
        }

        int port_server = atoi(argv[2]);
        int port_dll = atoi(argv[3]);

        return runServer(port_dll, port_server);
    }
    else if (strcmp(argv[1], "client") == 0) {
        //client input validation
        if (argc < 3) {
            printf("\tphy_layer client dll_port\n");
            return(-1);
        }

        int port_dll = atoi(argv[2]);

        return runClient(port_dll);
    }
    else {
        printf("Unrecognized physical layer type.  Please use server or client");
        return(-1);;
    }
    
};
