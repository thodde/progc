
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
    //TODO create link to server...

    return true;
}


bool PhysicalLayer::run() {
    //begin main execution loop
    char *upBuffer = new char[DEFAULT_BUFFER_SIZE];
    memset(upBuffer, '\0', DEFAULT_BUFFER_SIZE);
    char generalBuffer[255];
    int n;
    int upBufferUsed = 0;
    Frame* receivedFrame[100];
    int framesReceived = 0;
    int serializedLength;

    while (internalFD != 0) {
        if  (checkSocketHasData(internalFD)) {
            memset(generalBuffer, '\0', 255);
            n = read(internalFD, generalBuffer, 255);
            if (n == 0) {
                printf("Connection to Datalink Layer lost, exiting...\n");
                internalFD = 0;
            }
            else {
                memcpy(upBuffer+upBufferUsed, generalBuffer, n);
                upBufferUsed += n;
                memset(generalBuffer, '\0', MAX_FRAME_SIZE);

                //process only complete messages
                while (upBufferUsed >= MAX_FRAME_SIZE) {
                    receivedFrame[framesReceived++] = new Frame(upBuffer);
                    printf("Received From Datalink Layer Frame (%i) with payload (%i)\n", receivedFrame[framesReceived - 1]->frameId, receivedFrame[framesReceived - 1]->payloadUsed);

                    //shift buffer
                    char *tmpBuffer = new char[DEFAULT_BUFFER_SIZE];
                    memset(tmpBuffer, '\0', DEFAULT_BUFFER_SIZE);
                    memcpy(tmpBuffer, upBuffer + MAX_FRAME_SIZE, (DEFAULT_BUFFER_SIZE-MAX_FRAME_SIZE));
                    delete upBuffer;
                    upBuffer = tmpBuffer;
                    upBufferUsed -= MAX_FRAME_SIZE;
                }

                for (int i = 0; i < framesReceived; i++) {
                    char* serializedFrame = receivedFrame[i]->serialize();

                    printf("Forwarding to Datalink Layer Frame(%i) with payload (%i)\n", receivedFrame[i]->frameId, receivedFrame[i]->payloadUsed);

                    if (!guaranteedSocketWrite(internalFD, serializedFrame, MAX_FRAME_SIZE))
                         printf("ERROR writing to socket");
                    //delete serializedFrame;
                }

                for (int i = 0; i < framesReceived; i++) {
                    //delete receivedFrame[i];
                    receivedFrame[i] = NULL;
                }
                framesReceived = 0;
            }
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


