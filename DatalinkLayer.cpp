
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


    internalDownFD = connectToService(portInternalDown, "Physical Layer", "localhost");
    if (internalDownFD == 0) {
        printf("Error, closing down\n");
        return false;
    }

    internalUpFD = listenForService(portInternalUp, "Datalink Layer", true);
    if (internalUpFD == 0){
        printf("Error, closing down\n");
        return false;
    }

    return true;
}

bool DatalinkLayer::run() {
    //begin main execution loop
    upBufferUsed = 0;
    downBufferUsed = 0;
    packetsReceived = 0;
    framesReceived = NULL;

    while (internalUpFD != 0 && internalDownFD != 0) {
        if  (checkSocketHasData(internalUpFD)) {
            receiveDataFromNetworkLayer();
        }

        if (checkSocketHasData(internalDownFD)) {
            receiveDataFromPhysicalLayer();
        }

    }
    return true;
}


bool DatalinkLayer::receiveDataFromNetworkLayer() {
    char generalBuffer[255];
    memset(generalBuffer, '\0', 255);
    int n = read(internalUpFD, generalBuffer, 255);
    if (n == 0) {
        //error, socket was closed
        printf("Connection to Network Layer lost, exiting...\n");
        internalUpFD = 0;
    }
    else {
        memcpy(upBuffer+upBufferUsed, generalBuffer, n);
        upBufferUsed += n;
        memset(generalBuffer, '\0', MAX_PACKET_SIZE);

        //process only complete messages
        while (upBufferUsed >= MAX_PACKET_SIZE) {
            receivedPackets[packetsReceived++] = new Packet(upBuffer);

            char *tmpBuffer = new char[DEFAULT_BUFFER_SIZE];
            memset(tmpBuffer, '\0', DEFAULT_BUFFER_SIZE);
            memcpy(tmpBuffer, upBuffer + MAX_PACKET_SIZE, (DEFAULT_BUFFER_SIZE-MAX_PACKET_SIZE));
            delete upBuffer;
            upBuffer = tmpBuffer;
            upBufferUsed -= MAX_PACKET_SIZE;

            printf("Received Packet (%i) of payload (%i) final ?(%s) From Network Layer\n", receivedPackets[packetsReceived - 1]->packetId, receivedPackets[packetsReceived - 1]->payloadUsed, (receivedPackets[packetsReceived - 1]->finalPacket ? "yes" : "no"));
        }

        for (int i = 0; i < packetsReceived; i++) {
            //process message by dividing frames to packets
            FrameNode* currentFrame = convertPacketsToFrames(receivedPackets[i]);

            while (currentFrame != NULL) {
                char* serializeFrame = currentFrame->data->serialize();
                //printf("=========================================\n");

                printf("Forwarding Physical Layer Frame (%i) of with payload of size: %i\n", currentFrame->data->frameId, currentFrame->data->payloadUsed);

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

            printf("Received Frame from Physical Layer Frame (%i) with payload (%i) final? (%s)\n", newFrame->frameId, newFrame->payloadUsed, (newFrame->finalFrame ? "yes" : "no"));

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

FrameNode* DatalinkLayer::convertControlPacketToFrame(Packet *inPacket) {
    if (inPacket == NULL)
        return NULL;

    printf("Received Control Packet from Network\n");

    if (inPacket->type != Packet_Stack_Control)
        return NULL;

    FrameNode *retval = new FrameNode();
    retval->next = NULL;
    retval->data = new Frame(curFrameId++, true, Frame_Stack_Control, inPacket->sourceName, inPacket->targetName);
    retval->data->setPayload(inPacket->payload, inPacket->payloadUsed);
    retval->data->type = Frame_Stack_Control;
    return retval;
}

FrameNode* DatalinkLayer::convertPacketsToFrames(Packet* inPacket) {
    if (inPacket == NULL)
        return NULL;

    if (inPacket->type == Packet_Stack_Control)
        return convertControlPacketToFrame(inPacket);

    int serializedLength = MAX_PACKET_SIZE;
    char *byteStream = inPacket->serialize();

    FrameNode *headPtr = new FrameNode();
    headPtr->next = NULL;

    if (inPacket->type == Packet_Join)
        headPtr->data = new Frame(curFrameId++, false, Frame_Join, inPacket->sourceName, inPacket->targetName);
    else
        headPtr->data = new Frame(curFrameId++, false, Frame_Data, inPacket->sourceName, inPacket->targetName);

    int bytesAdded = headPtr->data->setPayload(byteStream, serializedLength);
    byteStream += bytesAdded;
    serializedLength -= bytesAdded;

    FrameNode* cursor = headPtr;

    while (serializedLength > 0) {
        cursor->next = new FrameNode();
        cursor = cursor->next;
        cursor->next = NULL;
        cursor->data = new Frame(curFrameId++, false, Frame_Data, inPacket->sourceName, inPacket->targetName);
        bytesAdded = cursor->data->setPayload(byteStream, serializedLength);
        byteStream += bytesAdded;
        serializedLength -= bytesAdded;
    }
    cursor->data->finalFrame = true;

    return headPtr;
}

Packet* DatalinkLayer::convertFrameJoinToPacket(FrameNode *headptr) {
    if (headptr == NULL)
        return NULL;
    if (headptr->data->type != Frame_Join)
        return NULL;

    Packet *retval = new Packet(0, true, Packet_Join, headptr->data->sourceName, headptr->data->targetName);
    retval->setPayload(headptr->data->payload, headptr->data->payloadUsed);
    return retval;
}

Packet* DatalinkLayer::convertFramesToPacket(FrameNode *headptr) {
    if (headptr == NULL)
        return NULL;
    if (headptr->data->type == Frame_Join)
        return convertFrameJoinToPacket(headptr);

    FrameNode *cursor = headptr;

    char packetStream[MAX_PACKET_SIZE];
    memset(packetStream, '\0', MAX_PACKET_SIZE);
    char *payloadCursor = packetStream;

    cursor = headptr;
    while (cursor != NULL) {
        // strip out payload
        memcpy(payloadCursor, cursor->data->payload, cursor->data->payloadUsed);
        payloadCursor += cursor->data->payloadUsed;
        cursor = cursor->next;
    }

    while (headptr != NULL) {
        cursor = headptr;
        headptr = headptr->next;
        delete cursor->data;
        delete cursor;
    }

    return new Packet(packetStream);
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
    int dllPort = DLL_PORT;
    int phPort = PH_PORT;

    if (argc > 1) {
        dllPort = atoi(argv[1]);
    }
    if (argc > 2) {
        phPort = atoi(argv[2]);
    }

    printf("Starting Datalink Layer\n");

    DatalinkLayer *myDL = new DatalinkLayer();
    if (!myDL->initialize(dllPort, phPort)) {
        delete myDL;
        return -1;
    }

    myDL->run();

    delete myDL;
    return 1;
}