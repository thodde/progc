
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
    curFrameId = 1;
 //   numberOfFrames = 0;

    statsFramesSent = 0;
    statsFramesReceived = 0;
    statsPacketsReceived = 0;
    statsPacketsSent = 0;
    statsBytesSent = 0;
    statsBytesReceived = 0;
    statsFrameOverheadSent = 0;
    statsFrameOverheadReceived = 0;
    statsErrorFramesDetected = 0;

    for (int i = 0; i < MAX_ENDPOINT_CONNECTIONS; i++) {
        myWindows[i] = NULL;
    }
};


DatalinkLayer::~DatalinkLayer() {
    printf("Runtime Stats\nSent\tPackets: %li Frames: %li Bytes: %li Overhead Bytes: %li\n", statsPacketsSent,
        statsFramesSent, statsBytesSent, statsFrameOverheadSent);
    printf("Received\tPackets: %li Frames: %li Bytes: %li Overhead Bytes: %li\n", statsPacketsReceived,
        statsFramesReceived, statsBytesReceived, statsFrameOverheadReceived);
    printf("Errors\tFrames Detected: %li\n", statsErrorFramesDetected);
    if (internalUpFD != 0)
        close(internalUpFD);
    if (internalDownFD != 0)
        close(internalDownFD);

    for (int i = 0; i < MAX_ENDPOINT_CONNECTIONS; i++) {
        if(myWindows[i] != NULL) {
            delete myWindows[i];
            myWindows[i] = NULL;
        }
    }
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

SlidingWindow* DatalinkLayer::getSlidingWindow(char* clientName) {
    for (int i = 0; i < MAX_ENDPOINT_CONNECTIONS; i++) {
        if (myWindows[i] != NULL) {
            if (strcmp(myWindows[i]->clientName, clientName) == 0)
                return myWindows[i];
        }
    }

    //Not found, create a new sliding window for this guy
    for (int i = 0; i < MAX_ENDPOINT_CONNECTIONS; i++) {
        if (myWindows[i] == NULL) {
            myWindows[i] = new SlidingWindow(clientName);
            return myWindows[i];
        }
    }

    return NULL; //client doesn't have a sliding window entry...error!
}


bool DatalinkLayer::receiveDataFromNetworkLayer() {
    char generalBuffer[255];
    memset(generalBuffer, '\0', 255);
    int n = read(internalUpFD, generalBuffer, 255); 
    FrameNode* queuedFrames[10]; //start with 10 and increase later 
    int queueLength = 0; 

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
            Packet *newPacket = new Packet(upBuffer);
            statsPacketsSent++;

            //used because of issues with memmove
            char *tmpBuffer = new char[DEFAULT_BUFFER_SIZE];
            memset(tmpBuffer, '\0', DEFAULT_BUFFER_SIZE);
            memcpy(tmpBuffer, upBuffer + MAX_PACKET_SIZE, (DEFAULT_BUFFER_SIZE-MAX_PACKET_SIZE));
            delete upBuffer;
            upBuffer = tmpBuffer;
            upBufferUsed -= MAX_PACKET_SIZE;

            printf("Received Packet (%i) of payload (%i) final ?(%s) From Network Layer\n", newPacket->packetId, newPacket->payloadUsed, (newPacket->finalPacket ? "yes" : "no"));

            FrameNode *currentFrame = convertPacketsToFrames(newPacket);

            if (currentFrame->data->type == Frame_Stack_Control) {
                //bypass the sliding window, this is just going to control the physical layer
                while (currentFrame != NULL) {
                    char* serializeFrame = currentFrame->data->serialize();
                    printf("Forwarding Physical Layer Control Frame (%i) of with payload of size: %i\n", currentFrame->data->frameId, currentFrame->data->payloadUsed);
                    statsFramesSent++;
                    statsBytesSent += MAX_FRAME_SIZE;
                    statsFrameOverheadSent += MAX_FRAME_SIZE;

                    if (!guaranteedSocketWrite(internalDownFD, serializeFrame, MAX_FRAME_SIZE))
                         printf("ERROR writing to socket");
                    FrameNode *tmp = currentFrame;
                    delete tmp->data;
                    delete tmp;
                    currentFrame = currentFrame->next;
                }
            }
            else {
                SlidingWindow *windowCursor = getSlidingWindow(newPacket->targetName);

                if (windowCursor == NULL) {
                    printf("Error!!!  Sliding window buffer exceeded!\n");
                }
                else {
                    while (currentFrame != NULL) {

                        windowCursor->addFrameToSend(currentFrame->data);
                        currentFrame = currentFrame->next;
                    }
                }
            }
        }
    }
    return sendAllAvailableFrames();
}

bool DatalinkLayer::sendAllAvailableFrames() {
    for (int i = 0; i < MAX_ENDPOINT_CONNECTIONS; i++) {
        if (myWindows[i] != NULL) {
            Frame *outFrame = myWindows[i]->getFrameToSend();
            while (outFrame != NULL) {
                char* serializeFrame = outFrame->serialize();
                printf("Forwarding Physical Layer Frame (%i) of with payload of size: %i\n", outFrame->frameId, outFrame->payloadUsed);
                statsFramesSent++;
                statsBytesSent += MAX_FRAME_SIZE;
                if (outFrame->type == Frame_Data)
                    statsFrameOverheadSent += (MAX_FRAME_SIZE - MAX_FRAME_PAYLOAD);
                else
                    statsFrameOverheadSent += MAX_FRAME_SIZE;

                if (!guaranteedSocketWrite(internalDownFD, serializeFrame, MAX_FRAME_SIZE))
                     printf("ERROR writing to socket");

                outFrame = myWindows[i]->getFrameToSend();
            }
        }
    }
    return true;
}

bool DatalinkLayer::receiveAllAvailablePackets() {
    for (int i = 0; i < MAX_ENDPOINT_CONNECTIONS; i++) {
        if (myWindows[i] != NULL) {
            FrameNode *frameList = myWindows[i]->getFullPacketToReceive();
            while (frameList != NULL) {
                Packet *outPacket = convertFramesToPacket(frameList);

                if (outPacket != NULL) {
                    char* serializedPacket = outPacket->serialize();
                    printf("Forwarding to Network Layer Packet (%i) with payload (%i)\n", outPacket->packetId, outPacket->payloadUsed);
                    statsPacketsReceived++;

                    if (!guaranteedSocketWrite(internalUpFD, serializedPacket, MAX_PACKET_SIZE))
                         printf("ERROR writing to socket");
                }
                frameList = myWindows[i]->getFullPacketToReceive();
            }
        }
    }
    return true;
}

bool DatalinkLayer::receiveDataFromPhysicalLayer() {
    char generalBuffer[255];
    memset(generalBuffer, '\0', 255);
    int n = read(internalDownFD, generalBuffer, 255);
    int previousFrameId = 0;

    if (n == 0) {
        //error, socket was closed
        printf("Connection to Physical Layer lost, exiting...\n");
        internalDownFD = 0;
    }
    else {
        memcpy(downBuffer+downBufferUsed, generalBuffer, n);
        downBufferUsed += n;
        memset(generalBuffer, '\0', MAX_FRAME_SIZE);
        statsBytesReceived += n;

        //process only complete messages
        while (downBufferUsed >= MAX_FRAME_SIZE) {
            Frame *newFrame = new Frame(downBuffer);
            statsFramesReceived++;

            if (newFrame->type == Frame_Data)
                statsFrameOverheadReceived += (MAX_FRAME_SIZE - MAX_FRAME_PAYLOAD);
             else
                statsFrameOverheadReceived += MAX_FRAME_SIZE;

            //TODO track statsErrorFramesDetected;
            printf("Received Frame from Physical Layer Frame (%i) with payload (%i) final? (%s)\n", newFrame->frameId, newFrame->payloadUsed, (newFrame->finalFrame ? "yes" : "no"));

            //if an ACK is received, display its frame number
            if(newFrame->type == Frame_Ack) {
                printf("Received acknowledgement for frame (%i)\n", newFrame->frameId);
            }


            SlidingWindow *localWindow = getSlidingWindow(newFrame->sourceName);
            localWindow->addFrameToReceive(newFrame);

            //shift buffer
            char *tmpBuffer = new char[DEFAULT_BUFFER_SIZE];
            memset(tmpBuffer, '\0', DEFAULT_BUFFER_SIZE);
            memcpy(tmpBuffer, downBuffer + MAX_FRAME_SIZE, (DEFAULT_BUFFER_SIZE-MAX_FRAME_SIZE));
            delete downBuffer;
            downBuffer = tmpBuffer;
            downBufferUsed -= MAX_FRAME_SIZE;

        }

        receiveAllAvailablePackets();
    }

    sendAllAvailableFrames();
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
    retval->data = new Frame(curFrameId++, true, Frame_Stack_Control, inPacket->sourceName, inPacket->targetName, true);
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
        headPtr->data = new Frame(curFrameId++, false, Frame_Join, inPacket->sourceName, inPacket->targetName, true);
    else
        headPtr->data = new Frame(curFrameId++, false, Frame_Data, inPacket->sourceName, inPacket->targetName, true);

    int bytesAdded = headPtr->data->setPayload(byteStream, serializedLength);
    byteStream += bytesAdded;
    serializedLength -= bytesAdded;

    FrameNode* cursor = headPtr;

    while (serializedLength > 0) {
        cursor->next = new FrameNode();
        cursor = cursor->next;
        cursor->next = NULL;
        cursor->data = new Frame(curFrameId++, false, Frame_Data, inPacket->sourceName, inPacket->targetName, false);
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
