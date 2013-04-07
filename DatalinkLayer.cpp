
#include "DatalinkLayer.h"

/*
bool sendMessage(Message *outMessage) {
    return true;
};*/

DatalinkLayer::DatalinkLayer() {
    internalUpFD = 0;
    internalDownFD = 0;
    upBuffer = new char[DEFAULT_BUFFER_SIZE];
    downBuffer = new char[DEFAULT_BUFFER_SIZE];
    memset(upBuffer, '\0', DEFAULT_BUFFER_SIZE);
    memset(downBuffer, '\0', DEFAULT_BUFFER_SIZE);
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
    char generalBuffer[255];
    int n;
    upBufferUsed = 0;
    downBufferUsed = 0;
    packetsReceived = 0;
    int serializedLength;
    framesReceived = NULL;

    while (internalUpFD != 0 && internalDownFD != 0) {
        if  (checkSocketHasData(internalUpFD)) {
            memset(generalBuffer, '\0', 255);
            n = read(internalUpFD, generalBuffer, 255);
            if (n == 0) {
                //error, socket was closed
                printf("Connection to Network Layer lost, exiting...\n");
                internalUpFD = 0;
            }
            else {
            /*
                printf("Received message from Network Layer:");
                for (int i = 0; i < 255; i++) {
                    printf("%c", generalBuffer[i]);
                }
                printf("\n");
                */

                memcpy(upBuffer+upBufferUsed, generalBuffer, n);
                upBufferUsed += n;
                memset(generalBuffer, '\0', MAX_PACKET_SIZE);


                //printf("Buffer available size(%i): ", upBufferUsed);
                /*
                for (int i = 0; i < upBufferUsed; i++)
                    printf("%c", upBuffer[i]);
                printf("\n");
                */

                //process only complete messages
                while (upBufferUsed >= MAX_PACKET_SIZE) {
                    receivedPackets[packetsReceived++] = new Packet(upBuffer);

/*
                    printf("Complete packet from Network Layer (%i) payload(%i): ", receivedPackets[packetsReceived-1]->packetId, receivedPackets[packetsReceived-1]->payloadUsed);
                    for (int i = 0; i < receivedPackets[packetsReceived-1]->payloadUsed; i++)
                        printf("%c", receivedPackets[packetsReceived-1]->payload[i]);
                    printf("\n");
                    */


                    char *tmpBuffer = new char[DEFAULT_BUFFER_SIZE];
                    memset(tmpBuffer, '\0', DEFAULT_BUFFER_SIZE);
                    memcpy(tmpBuffer, upBuffer + MAX_PACKET_SIZE, (DEFAULT_BUFFER_SIZE-MAX_PACKET_SIZE));
                    delete upBuffer;
                    upBuffer = tmpBuffer;
                    upBufferUsed -= MAX_PACKET_SIZE;

                    printf("Received Packet (%i) of payload (%i) From Network Layer\n", receivedPackets[packetsReceived - 1]->packetId, receivedPackets[packetsReceived - 1]->payloadUsed);
                }

                for (int i = 0; i < packetsReceived; i++) {
                    //char* serializedPacket = receivedPackets[i]->serialize();

                    //printf("Forwarding to Physical Layer\n");

                    /*
                    printf("Serialized Packet:");
                    for (int y = 0; y < MAX_PACKET_SIZE; y++)
                        printf("%c", serializedPacket[y]);
                    printf("\n");
                    */

                        //process message by dividing frames to packets
                    FrameNode* currentFrame = convertPacketsToFrames(receivedPackets[i]);

                    while (currentFrame != NULL) {
                        char* serializeFrame = currentFrame->data->serialize();
                        //printf("=========================================\n");

                        printf("Forwarding Physical Layer Frame (%i) of with payload of size: %i\n", currentFrame->data->frameId, currentFrame->data->payloadUsed);

/*
                        for(int x = 0; x < MAX_FRAME_SIZE; x++) {
                            printf("%c", serializeFrame[x]);
                        }
                        printf("\n");     */



                        //n = write(internalDownFD, );
                        if (!guaranteedSocketWrite(internalDownFD, serializeFrame, MAX_FRAME_SIZE))
                             printf("ERROR writing to socket");
                        currentFrame = currentFrame->next;
                    }

//TODO MEMORY LEAKS!!!
                    //delete serializedPacket;
                }

                for (int i = 0; i < packetsReceived; i++) {
                    //delete receivedPackets[i];
                    receivedPackets[i] = NULL;
                }
                packetsReceived = 0;
            }
        }

    /*
                printf("Forwarding to Physical Layer\n");
                n = write(internalDownFD, buffer, MAX_PACKET_SIZE);
                if (n < 0)
                     printf("ERROR writing to socket");

                memset(buffer, '\0', MAX_PACKET_SIZE);
                */


        if (checkSocketHasData(internalDownFD)) {
            receiveDataFromPhysicalLayer();
        }

    }
    return true;
}


bool DatalinkLayer::receiveDataFromPhysicalLayer() {
    char generalBuffer[255];
    memset(generalBuffer, '\0', 255);
    int n = read(internalDownFD, generalBuffer, 255);
    if (n == 0) {
        //error, socket was closed
        printf("Connection to Physical Layer lost, exiting...\n");
        internalDownFD = 0;
    }
    else {
        memcpy(downBuffer+downBufferUsed, generalBuffer, n);
        downBufferUsed += n;
        memset(generalBuffer, '\0', MAX_FRAME_SIZE);

        //process only complete messages
        while (downBufferUsed >= MAX_FRAME_SIZE) {
            Frame *newFrame = new Frame(downBuffer);
            addFrameReceived(newFrame);

            printf("Received Frame from Physical Layer Frame (%i) with payload (%i)\n", newFrame->frameId, newFrame->payloadUsed);

            //shift buffer
            char *tmpBuffer = new char[DEFAULT_BUFFER_SIZE];
            memset(tmpBuffer, '\0', DEFAULT_BUFFER_SIZE);
            memcpy(tmpBuffer, downBuffer + MAX_FRAME_SIZE, (DEFAULT_BUFFER_SIZE-MAX_FRAME_SIZE));
            delete downBuffer;
            downBuffer = tmpBuffer;
            downBufferUsed -= MAX_FRAME_SIZE;
        }

    //TODO this should change to whatever receiving intelligence we use
        while (hasFinalFrame()) {
            FrameNode *frameList = extractFrameList();
            Packet *outPacket = convertFramesToPacket(frameList);
            char* serializedPacket = outPacket->serialize();

            printf("Forwarding to Network Layer Packet (%i) with payload (%i)\n", outPacket->packetId, outPacket->payloadUsed);
            if (!guaranteedSocketWrite(internalUpFD, serializedPacket, MAX_PACKET_SIZE))
                 printf("ERROR writing to socket");
        }
    }
    return true;
}

FrameNode* DatalinkLayer::convertPacketsToFrames(Packet* inPacket) {
    if (inPacket == NULL)
        return NULL;

    int serializedLength = MAX_PACKET_SIZE;
    char *byteStream = inPacket->serialize();

    //printf("Packet of size: %i\n", inPacket->payloadUsed);

    FrameNode *headPtr = new FrameNode();
    headPtr->next = NULL;

    headPtr->data = new Frame(curFrameId++, false);

    int bytesAdded = headPtr->data->setPayload(byteStream, serializedLength);
    byteStream += bytesAdded;
    serializedLength -= bytesAdded;

    //printf("Frame %i of size %i\n", curFrameId-1, bytesAdded);

    FrameNode* cursor = headPtr;
    /*
    printf("Creating Frame from Packet, current Frame payload: ");
    for (int i = 0; i < cursor->data->payloadUsed; i++)
        printf("%c", cursor->data->payload[i]);
    printf("\n");
    */

    while (serializedLength > 0) {
        cursor->next = new FrameNode();
        cursor = cursor->next;
        cursor->next = NULL;
        cursor->data = new Frame(curFrameId++, false);
        bytesAdded = cursor->data->setPayload(byteStream, serializedLength);
        byteStream += bytesAdded;
        serializedLength -= bytesAdded;
        /*
        printf("Frame %i of size %i: ", curFrameId-1, bytesAdded);
        for (int i = 0; i < cursor->data->payloadUsed; i++)
            printf("%c", cursor->data->payload[i]);
        printf("\n");*/
    }
    cursor->data->finalFrame = true;

    //printf("Final frame data: %s\n", cursor->data->payload);
    //cursor->data->finalFrame = true;

    return headPtr;
}

Packet* DatalinkLayer::convertFramesToPacket(FrameNode *headptr) {
    FrameNode *cursor = headptr;

    char packetStream[MAX_PACKET_SIZE];
    memset(packetStream, '\0', MAX_PACKET_SIZE);
    char *payloadCursor = packetStream;

    cursor = headptr;
    while (cursor != NULL) {
        // strip out payload
        //printf("Cursor at %i, limit of %i, next message size: %i\n", (int)(payloadCursor - packetStream), MAX_PACKET_SIZE, cursor->data->payloadUsed);
        memcpy(payloadCursor, cursor->data->payload, cursor->data->payloadUsed);
        //printf("Copied frame data from frame %i with payload %i\n", cursor->data->frameId, cursor->data->payloadUsed);
        payloadCursor += cursor->data->payloadUsed;
        cursor = cursor->next;
    }

    Packet *retval = new Packet(packetStream);

    //printf("Here's the packet I deseralized in the DLL %i\n", retval->packetId);

    while (headptr != NULL) {
        cursor = headptr;
        headptr = headptr->next;
        delete cursor->data;
        delete cursor;
    }

    return retval;
//    return new Packet(packetPayload);
}

bool DatalinkLayer::hasFinalFrame() {
    if (framesReceived == NULL)
        return false;

    FrameNode *cursor = framesReceived;

    while (cursor != NULL) {
        if (cursor->data->finalFrame)
            return true;

        cursor = cursor->next;
    }

    return false;
}

bool DatalinkLayer::addFrameReceived(Frame *inFrame) {
    if (framesReceived == NULL) {
        framesReceived = new FrameNode();
        framesReceived->next = NULL;
        framesReceived->data = inFrame;
        return true;
    }

    FrameNode *cursor = framesReceived;

    while(cursor->next != NULL)
        cursor = cursor->next;

    cursor->next = new FrameNode();
    cursor = cursor->next;
    cursor->next = NULL;
    cursor->data = inFrame;
    return true;
}

FrameNode* DatalinkLayer::extractFrameList() {
    if (framesReceived == NULL)
        return NULL;

    FrameNode *frameList = framesReceived;
    FrameNode *cursor = framesReceived;

    while (cursor != NULL) {
        if (cursor->data->finalFrame) {
            //break the list here
            framesReceived = cursor->next;
            cursor->next = NULL;
            return frameList;
        }
        cursor = cursor->next;
    }

    //no final packet found
    return NULL;
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