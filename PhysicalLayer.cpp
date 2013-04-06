
#include "PhysicalLayer.h"

/*
bool sendFrame(Frame *outFrame) {
    return true;
};
 */


PhysicalLayer::PhysicalLayer() {
    externalFD = 0;
    internalFD = 0;
    externalPort = 0;
}

PhysicalLayer::~PhysicalLayer() {
    if (externalFD != 0)
        close(externalFD);
    if (internalFD != 0)
        close(internalFD);
}


bool PhysicalLayer::initialize(int portExternal, int portInternal) {
    printf("Initializing Physical Layer\n");
    internalFD = 0;
    externalPort = portExternal;

    internalFD = listenForInternalService(portInternal, "Physical Layer");
    if (internalFD == 0){
        printf("Error, closing down\n");
        return false;
    }

/*
    internalDLLFD = connectToInternalService(portInternalDLL, "Datalink Layer");
    if (internalDLLFD == 0) {
        printf("Error, closing down\n");
        return false;
    }  */

    return true;
}


bool PhysicalLayer::run() {
    //begin main execution loop
    char upBuffer[1024];
    memset(upBuffer, '\0', 1024);
    char generalBuffer[255];
    int n;
    int upBufferUsed = 0;
    Frame* receivedFrame[100];
    int framesReceived = 0;
    int serializedLength;

    struct timeval tv;
    fd_set readfds;

    int socketsReady;

    int maxFDId;

    while (internalFD != 0) {
        tv.tv_sec = 2;
        tv.tv_usec = 500000;
        FD_ZERO(&readfds);
        FD_SET(internalFD, &readfds);

        maxFDId = internalFD;

        socketsReady = select(maxFDId+1, &readfds, NULL, NULL, &tv);

        if (socketsReady > 0) {
            if (FD_ISSET(internalFD, &readfds)) {
                memset(generalBuffer, '\0', 255);
                n = read(internalFD, generalBuffer, 255);
                if (n == 0) {
                    printf("Connection to Datalink Layer lost, exiting...\n");
                    internalFD = 0;
                }
                else {
                    printf("==========================================\n");
                    printf("Received message from Datalink Layer:");
                    for (int i = 0; i < 255; i++) {
                        printf("%c", generalBuffer[i]);
                    }
                    printf("\n");

                    memcpy(upBuffer+upBufferUsed, generalBuffer, n);
                    upBufferUsed += n;
                    memset(generalBuffer, '\0', MAX_FRAME_SIZE);

                    //process only complete messages
                    while (upBufferUsed >= MAX_FRAME_SIZE) {
                        receivedFrame[framesReceived++] = new Frame(upBuffer);

                        //shift buffer
                        memmove(upBuffer, (upBuffer + MAX_FRAME_SIZE), (1024-MAX_FRAME_SIZE));
                        //null out end bytes
                        memset(upBuffer+1024-MAX_FRAME_SIZE, '\0', MAX_FRAME_SIZE);
                        upBufferUsed -= MAX_FRAME_SIZE;
                    }

                    //TODO this really doesn't belong here, but for testing purposes...
                    if (framesReceived > 0)
                        printf("Message received, bouncing back up to Datalink Layer!\n");
                    for (int i = 0; i < framesReceived; i++) {
                        char* serializedFrame = receivedFrame[i]->serialize(serializedLength);

                        printf("Forwarding to Datalink Layer\n", serializedFrame);

                        n = write(internalFD, serializedFrame, MAX_FRAME_SIZE);
                        if (n < 0)
                             printf("ERROR writing to socket");
                        delete serializedFrame;
                    }

                    for (int i = 0; i < framesReceived; i++) {
                        delete receivedFrame[i];
                        receivedFrame[i] = NULL;
                    }
                    framesReceived = 0;
                }
            }
            //TODO write the outgoing packet info
        }
    }
    return true;
}

int main (int argc, char *argv[]) {
    printf("Starting Physical Layer\n");

    PhysicalLayer *myPH = new PhysicalLayer();
    if (!myPH->initialize(EXTERNAL_PORT, PH_PORT)) {
        delete myPH;
        return -1;
    }

    myPH->run();

    delete myPH;
    return 1;
}

/*
bool PhysicalLayer::sendFrame(Frame *outFrame) {
    return false;
}

bool PhysicalLayer::receiveData() {
    return false;
}

Packet* PhysicalLayer::convertFrameToPacket(Frame *inFrame) {
    //stuff bits

    //cut into XXX length segments, -3?  provided they don't break the EOL identifier
    return NULL;
}


// chars read?  or remaining?
int PhysicalLayer::readPacket(char* inString, Packet *outPacket) {
    return 0;
}



char* PhysicalLayer::stuffBits(char *inStream) {
    //find sequence

    //replace

    //advance

    //find sequence (repeat)

    return NULL;
}


*/
