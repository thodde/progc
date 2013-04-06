
#include "NetworkLayer.h"

NetworkLayer::NetworkLayer() {
    internalFD = 0;
    curPacketId = 0;
    memset(partialMessageBuffer, '\0', PARTIAL_MESSAGE_BUFFER_LENGTH);
    partialBufferUsed = 0;
    //packetsReceived = 0;
    globalSentPackets = 0;
    globalReceivedPackets = 0;
}

NetworkLayer::~NetworkLayer() {
    if (internalFD != 0)
        close(internalFD);
}

bool NetworkLayer::initialize(int portInternal) {
    printf("Initializing Network Layer\n");
    internalFD = 0;

    internalFD = connectToInternalService(portInternal, "Datalink Layer", true);
    if (internalFD == 0) {
        printf("Error, closing down\n");
        return false;
    }

    return true;
}


bool NetworkLayer::sendMessage(Message *newMessage) {
    if (internalFD == 0)
        return false;

/*
    printf("Message previously unedited:");
    for(int i = 0; i < newMessage->dataLength; i++)
        printf("%c", newMessage->data[i]);
    printf("\n");
*/

    //validate message parameters (like length > 0
    // break into packets
    PacketNode *sendList = convertMessageToPackets(newMessage);

    if (sendList == NULL) //errors converting message
        return false;

//    int n;
    int bytesWritten = 0;
    while (sendList != NULL) {

        char *sendStream = sendList->data->serialize();

/*
        printf("Network Layer sending:");
        for(int i = 0; i < MAX_PACKET_SIZE; i++)
            printf("%c", sendStream[i]);
        printf("\n");
*/

        if (!guaranteedSocketWrite(internalFD, sendStream, MAX_PACKET_SIZE))
            printf("ERROR writing to socket, entire message not sent or error encountered\n");

        bytesWritten += MAX_PACKET_SIZE;
        globalSentPackets++;

        delete sendList->data;
        sendList = sendList->next;
    }
    printf("Bytes written this pass: %i, Total Packets Sent: %i\n", bytesWritten, globalSentPackets);

    return true;
}

PacketNode* NetworkLayer::convertMessageToPackets(Message *inMessage) {
    if (inMessage == NULL)
        return NULL;

    int serializedLength;
    char *byteStream = inMessage->serialize(serializedLength);

    printf("Message of size: %i\n", serializedLength);
/*
    printf("Initial Message:");
    for (int i = 0; i < serializedLength; i++)
        printf("%c", byteStream[i]);
    printf("\n");
*/

    PacketNode *headPtr = new PacketNode();
    headPtr->next = NULL;

    headPtr->data = new Packet(curPacketId++, false);

    int bytesAdded = headPtr->data->setPayload(byteStream, serializedLength);
    byteStream += bytesAdded;
    serializedLength -= bytesAdded;

    //printf("Packet %i of size %i\n", curPacketId-1, bytesAdded);

    PacketNode *cursor = headPtr;

/*
    printf("Packet'ed message (%i): ", cursor->data->packetId);
    for (int i = 0; i < MAX_PACKET_PAYLOAD; i++)
        printf("%c", cursor->data->payload[i]);
    printf("\n");
*/

/*
    printf("Remaining Message(%i): ", cursor->data->packetId);
    for (int i = 0; i < (serializedLength); i++)
        printf("%c", byteStream[i]);
    printf("\n");
*/

    while (serializedLength > 0) {
        cursor->next = new PacketNode();
        cursor = cursor->next;
        cursor->next = NULL;
        cursor->data = new Packet(curPacketId++, false);
        bytesAdded = cursor->data->setPayload(byteStream, serializedLength);
        byteStream += bytesAdded;
        serializedLength -= bytesAdded;
        //printf("Packet %i of size %i\n", curPacketId-1, bytesAdded);

/*
        printf("Remaining 1 Message(%i): ", cursor->data->packetId);
        for (int i = 0; i < (serializedLength); i++)
            printf("%c", byteStream[i]);
        printf("\n");
*/
/*
        printf("Packet'ed message (%i): ", cursor->data->packetId);
        for (int i = 0; i < MAX_PACKET_PAYLOAD; i++)
            printf("%c", cursor->data->payload[i]);
        printf("\n");
*/
    }

    //printf("Final packet data: %s\n", cursor->data->payload);
    cursor->data->finalPacket = true;

    return headPtr;
}

Message* NetworkLayer::checkForMessages() {
    if (internalFD == 0)
        return NULL;

    int n;
    struct timeval tv;
    fd_set readfds;
    int socketsReady;
    //timeout of 1/2 a second
    tv.tv_usec = 500000;
    FD_ZERO(&readfds);
    FD_SET(internalFD, &readfds);

    if (socketsReady > 0) {
        if (FD_ISSET(internalFD, &readfds)) {
            char generalBuffer[255];
            memset(generalBuffer, '\0', 255);
            int n = read(internalFD, generalBuffer, 255);

            if (n == 0) {
                printf("Error, lost connection with Datalink Layer\n");
                return NULL;
            }
            else {
                printf("Received message from Datalink Layer\n");
        /*
                printf("Received message from Datalink Layer:");
                for (int i = 0; i < n; i++) {
                    printf("%c", generalBuffer[i]);
                }
                printf("\n");
        */

                memcpy(partialMessageBuffer+partialBufferUsed, generalBuffer, n);
                partialBufferUsed += n;
                memset(generalBuffer, '\0', MAX_PACKET_SIZE);

                //process only complete messages
                while (partialBufferUsed >= MAX_PACKET_SIZE) {
                    globalReceivedPackets++;
                    if (!addPacket(new Packet(partialMessageBuffer))) {
                        printf("Error while recreating packet!\n");
                    }


                    //shift buffer
                    memmove(partialMessageBuffer, (partialMessageBuffer + MAX_PACKET_SIZE), (PARTIAL_MESSAGE_BUFFER_LENGTH-MAX_PACKET_SIZE));
                    //null out end bytes
                    memset(partialMessageBuffer+PARTIAL_MESSAGE_BUFFER_LENGTH-MAX_PACKET_SIZE, '\0', MAX_PACKET_SIZE);
                    partialBufferUsed -= MAX_PACKET_SIZE;

                    /*
                    printf("Packet Rebuilt.  Payload:");
                    for (int i = 0; i < MAX_PACKET_PAYLOAD; i++) {
                        printf("%c", receivedPackets[packetsReceived - 1]->payload[i]);
                    }
                    printf("\n");
                    */
                }


                //printf("Total received packets: %i\n", globalReceivedPackets);
        /*
                printf("packets received: %i\n", packetsReceived);
        */

                while (hasFinalPacket()) {
                    printf("Rebuilding Message\n");

                    PacketNode *packetList = extractPacketList();
                    if (packetList == NULL) {
                        printf("Error rebuilding packet list\n");
                        return NULL;
                    }
                    return convertPacketsToMessage(packetList);
                }
            }
        }
    }

    return NULL;
}

bool NetworkLayer::addPacket(Packet* newPacket) {
    if (receivedPackets == NULL) {
        receivedPackets = new PacketNode();
        receivedPackets->next = NULL;
        receivedPackets->data = newPacket;
        return true;
    }

    PacketNode *cursor = receivedPackets;
    while (cursor->next != NULL)
        cursor = cursor->next;

    cursor->next = new PacketNode();
    cursor = cursor->next;
    cursor->next = NULL;
    cursor->data = newPacket;

    return true;
}

bool NetworkLayer::hasFinalPacket() {
    PacketNode *cursor = receivedPackets;
    while (cursor != NULL) {
        if (cursor->data->finalPacket)
            return true;

        cursor = cursor->next;
    }

    return false;
}


Message* NetworkLayer::convertPacketsToMessage(PacketNode *headptr) {
    PacketNode *cursor = headptr;
    int bytesNeeded = 0;

    //iterate through and figure out Payload * Packet size for the array of chars needed
    while (cursor != NULL) {
        bytesNeeded += cursor->data->payloadUsed;
        cursor = cursor->next;
    }

    if (bytesNeeded <= 0)
        return NULL;

    char messagePayload[bytesNeeded];
    memset(messagePayload, '\0', bytesNeeded);
    char *payloadCursor = messagePayload;

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

    return new Message(messagePayload);
}


PacketNode* NetworkLayer::extractPacketList() {
    if (receivedPackets == NULL)
        return NULL;

    PacketNode *packetList = receivedPackets;
    PacketNode *cursor = receivedPackets;

    while (cursor != NULL) {
        if (cursor->data->finalPacket) {
            //break the list here
            receivedPackets = cursor->next;
            cursor->next = NULL;
            return packetList;
        }
        cursor = cursor->next;
    }

    //no final packet found
    return NULL;
}

