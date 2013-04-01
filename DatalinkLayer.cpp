
#include "DatalinkLayer.h"

/*
bool sendMessage(Message *outMessage) {
    return true;
};*/

DatalinkLayer::DatalinkLayer() {
    internalUpFD = 0;
    internalDownFD = 0;
};


DatalinkLayer::~DatalinkLayer() {
    if (internalUpFD != 0)
        close(internalUpFD);
    if (internalDownFD != 0)
        close(internalDownFD);
}

bool DatalinkLayer::initialize(int portInternalUp, int portInternalDown) {
    printf("Initializing Datalink Layer\n");
    internalUpFD = 0;
    internalDownFD = 0;


    internalDownFD = connectToInternalService(portInternalDown, "Physical Layer");
    if (internalDownFD == 0) {
        printf("Error, closing down\n");
        return false;
    }

    internalUpFD = listenForInternalService(portInternalUp, "Datalink Layer");
    if (internalUpFD == 0){
        printf("Error, closing down\n");
        return false;
    }

    return true;
}

bool DatalinkLayer::run() {
    //begin main execution loop
    char buffer[255];
    memset(buffer, '\0', 255);
    int n;
    while (true) {
        n = read(internalUpFD,buffer,255);
        if (n > 0) {
            printf("Received message from Network Layer\n");
            printf("Message content: %s\n", buffer);

            //process message, probably a different function
            //divide frames to packets?

            printf("Forwarding to Physical Layer\n");
            n = write(internalDownFD, buffer, 255);
            if (n < 0)
                 printf("ERROR writing to socket");

            memset(buffer, '\0', 255);
        }

        n = read(internalDownFD,buffer,255);
        if (n > 0) {
            printf("Received message from Physical Layer\n");
            printf("Message content: %s\n", buffer);
            printf("To be written\n");
            memset(buffer, '\0', 255);
        }
    }
    return true;
}

int main (int argc, char *argv[]) {
    printf("Starting Datalink Layer\n");

    DatalinkLayer *myDL = new DatalinkLayer();
    if (!myDL->initialize(DLL_PORT, PH_PORT)) {
        delete myDL;
        return -1;
    }

    myDL->run();

    delete myDL;
    return 1;
}


  /*
bool DatalinkLayer::dl_send(Packet *outPacket) {
    return true;
}

bool DatalinkLayer::dl_recv(Packet *inPacket) {
    return true;
}
*/


