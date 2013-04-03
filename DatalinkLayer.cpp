
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


    internalDownFD = connectToInternalService(portInternalDown, "Physical Layer", false);
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
    memset(upBuffer, '\0', 1024);
    memset(downBuffer, '\0', 1024);
    char generalBuffer[255];
    int n;
    int upBufferUsed = 0;
    int downBufferUsed = 0;
    Packet* receivedPackets[100];
    int packetsReceived = 0;

    struct timeval tv;
    fd_set readfds;

    int socketsReady;

    int maxFDId;
    if (internalDownFD > internalUpFD)
        maxFDId = internalDownFD;
    else
        maxFDId = internalUpFD;

    while (internalUpFD != 0 && internalDownFD != 0) {
        tv.tv_sec = 2;
        tv.tv_usec = 500000;
        FD_ZERO(&readfds);
        FD_SET(internalDownFD, &readfds);
        FD_SET(internalUpFD, &readfds);

        socketsReady = select(maxFDId+1, &readfds, NULL, NULL, &tv);

        if (socketsReady > 0) {
            if (FD_ISSET(internalUpFD, &readfds)) {
                memset(generalBuffer, '\0', 255);
                n = read(internalUpFD, generalBuffer, 255);
        //        n = recv(internalUpFD, generalBuffer, MAX_PACKET_SIZE, 0);
                if (n == 0) {
                    //error, socket was closed
                    printf("Connection to Network Layer lost, exiting...\n");
                    internalUpFD = 0;
                }
                else {
                    printf("Received message from Network Layer:");
                    for (int i = 0; i < 255; i++) {
                        printf("%c", generalBuffer[i]);
                    }
                    printf("\n");

                    memcpy(upBuffer+upBufferUsed, generalBuffer, n);
                    upBufferUsed += n;
                    memset(generalBuffer, '\0', MAX_PACKET_SIZE);


                    //printf("Buffer available size(%i): ", upBufferUsed);
                    for (int i = 0; i < upBufferUsed; i++)
                        printf("%c", upBuffer[i]);
                    printf("\n");

                    //process only complete messages
                    while (upBufferUsed >= MAX_PACKET_SIZE) {
                        receivedPackets[packetsReceived++] = new Packet(upBuffer);

                        printf("Complete packet from Network Layer (%i) payload(%i): ", receivedPackets[packetsReceived-1]->packetId, receivedPackets[packetsReceived-1]->payloadUsed);
                        for (int i = 0; i < receivedPackets[packetsReceived-1]->payloadUsed; i++)
                            printf("%c", receivedPackets[packetsReceived-1]->payload[i]);
                        printf("\n");

                        //shift buffer
                        memmove(upBuffer, (upBuffer + MAX_PACKET_SIZE), (1024-MAX_PACKET_SIZE));
                        //null out end bytes
                        memset(upBuffer+1024-MAX_PACKET_SIZE, '\0', MAX_PACKET_SIZE);
                        upBufferUsed -= MAX_PACKET_SIZE;
                    }

                    for (int i = 0; i < packetsReceived; i++) {
                        char* serializedPacket = receivedPackets[i]->serialize();

                        printf("Forwarding to Physical Layer\n", serializedPacket);

                        n = write(internalDownFD, serializedPacket, MAX_PACKET_SIZE);
                        if (n < 0)
                             printf("ERROR writing to socket");
 //TODO MEMORY LEAKS!!!
                        //delete serializedPacket;
                    }

                    for (int i = 0; i < packetsReceived; i++) {
                        //delete receivedPackets[i];
                        receivedPackets[i] = NULL;
                    }
                    packetsReceived = 0;
                //process message, probably a different function
                //divide frames to packets?

    /*
                printf("Forwarding to Physical Layer\n");
                n = write(internalDownFD, buffer, MAX_PACKET_SIZE);
                if (n < 0)
                     printf("ERROR writing to socket");

                memset(buffer, '\0', MAX_PACKET_SIZE);
                */
                    }
                }
            }

            //TODO this is only for testing purposes, needs to be rewritten
            if (FD_ISSET(internalDownFD, &readfds)) {
                memset(generalBuffer, '\0', 255);
                n = read(internalDownFD, generalBuffer, 255);
                if (n == 0) {
                    //error, socket was closed
                    printf("Connection to Physical Layer lost, exiting...\n");
                    internalDownFD = 0;
                }
                else {
/*
                    printf("Received message from Physical Layer:");
                    for (int i = 0; i < 255; i++) {
                        printf("%c", generalBuffer[i]);
                    }
                    printf("\n");
*/

                    memcpy(downBuffer+downBufferUsed, generalBuffer, n);
                    downBufferUsed += n;
                    memset(generalBuffer, '\0', MAX_PACKET_SIZE);

                    //process only complete messages
                    while (downBufferUsed >= MAX_PACKET_SIZE) {
                        receivedPackets[packetsReceived++] = new Packet(downBuffer);

                        //shift buffer
                        memmove(downBuffer, (downBuffer + MAX_PACKET_SIZE), (1024-MAX_PACKET_SIZE));
                        //null out end bytes
                        memset(downBuffer+1024-MAX_PACKET_SIZE, '\0', MAX_PACKET_SIZE);
                        downBufferUsed -= MAX_PACKET_SIZE;
                    }

                    for (int i = 0; i < packetsReceived; i++) {
                        char* serializedPacket = receivedPackets[i]->serialize();

                        printf("Forwarding to Network Layer\n", serializedPacket);

                        n = write(internalUpFD, serializedPacket, MAX_PACKET_SIZE);
                        //also check if I couldn't send out whole packet...
                        if (n < 0)
                             printf("ERROR writing to socket");
 //TODO MEMORY LEAKS!
                        //delete serializedPacket;
                    }
                    for (int i = 0; i < packetsReceived; i++) {
                        //delete receivedPackets[i];
                        receivedPackets[i] = NULL;
                    }
                    packetsReceived = 0;
                }
                //process message, probably a different function
                //divide frames to packets?

    /*
                printf("Forwarding to Physical Layer\n");
                n = write(internalDownFD, buffer, MAX_PACKET_SIZE);
                if (n < 0)
                     printf("ERROR writing to socket");

                memset(buffer, '\0', MAX_PACKET_SIZE);
                */
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


