#include "NetworkLayer.h"

NetworkLayer::NetworkLayer() {
    internalFD = 0;
    curPacketId = 0;
    partialMessageBuffer = new char[DEFAULT_BUFFER_SIZE];
    memset(partialMessageBuffer, '\0', DEFAULT_BUFFER_SIZE);
    partialBufferUsed = 0;

    //TODO set these values in the code below
    statsMessagesSent = 0;
    statsBytesSent = 0;
    statsMessagesReceived = 0;
    statsBytesReceived = 0;
    statsPacketsSent = 0;
    statsPacketsReceived = 0;
    statsBytesOverheadSent = 0;
    statsBytesOverheadReceived = 0;
}

NetworkLayer::~NetworkLayer() {
    printf("Runtime Stats\nSent\tMessages: %li Packets: %li Bytes: %li Overhead Bytes: %li\n", statsMessagesSent,
        statsPacketsSent, statsBytesSent, statsBytesOverheadSent);
    printf("Received\tMessage: %li Packets: %li Bytes: %li Overhead Bytes: %li\n", statsMessagesReceived,
        statsPacketsReceived, statsBytesReceived, statsBytesOverheadReceived);
    if (internalFD != 0)
        close(internalFD);
}

bool NetworkLayer::initialize(int portInternal) {
    printf("Initializing Network Layer\n");
    internalFD = 0;

    internalFD = connectToService(portInternal, "Datalink Layer", "localhost");
    if (internalFD == 0) {
        printf("Error, closing down\n");
        return false;
    }

    return true;
}

bool NetworkLayer::sendMessage(Message *newMessage) {
    int serializedLength;
    if (internalFD == 0)
        return false;

    // break into packets
    statsMessagesSent++;
    PacketNode *sendList = convertMessageToPackets(newMessage);

    if (sendList == NULL) //errors converting message
        return false;

    int bytesWritten = 0;
    while (sendList != NULL) {
        statsPacketsSent++;
        statsBytesSent += MAX_PACKET_SIZE;

        if (sendList->data->type == Packet_Data)
            statsBytesOverheadSent += (MAX_PACKET_SIZE - MAX_PACKET_PAYLOAD);
        else
            statsBytesOverheadSent += MAX_PACKET_SIZE;

        char *sendStream = sendList->data->serialize();

        if (!guaranteedSocketWrite(internalFD, sendStream, MAX_PACKET_SIZE))
            printf("ERROR writing to socket, entire message not sent or error encountered\n");

        bytesWritten += MAX_PACKET_SIZE;

        printf("Sending Packet (%i) to Datalink Layer\n", sendList->data->packetId);

        delete sendList->data;
        sendList = sendList->next;
    }

    return true;
}


PacketNode* NetworkLayer::convertControlMessageToPacket(Message *inMessage) {
    if (inMessage->type != Message_Stack_Control)
        return NULL;

    PacketNode *retval = new PacketNode();
    retval->next = NULL;
    Packet_Type newType;
    retval->data = new Packet(curPacketId++, true, Packet_Stack_Control, inMessage->sourceName, inMessage->targetName);
    retval->data->setPayload(inMessage->data, inMessage->dataLength);
    retval->data->type = Packet_Stack_Control;
    return retval;
}


PacketNode* NetworkLayer::convertMessageToPackets(Message *inMessage) {
    if (inMessage == NULL)
        return NULL;

    if (inMessage->type == Message_Stack_Control)
        return convertControlMessageToPacket(inMessage);

    int serializedLength;
    char *byteStream = inMessage->serialize(serializedLength);

    PacketNode *headPtr = new PacketNode();
    headPtr->next = NULL;

    if (inMessage->type == Message_Join)
        headPtr->data = new Packet(curPacketId++, false, Packet_Join, inMessage->sourceName, inMessage->targetName);
    else
        headPtr->data = new Packet(curPacketId++, false, Packet_Data, inMessage->sourceName, inMessage->targetName);

    int bytesAdded = headPtr->data->setPayload(byteStream, serializedLength);
    byteStream += bytesAdded;
    serializedLength -= bytesAdded;

    PacketNode *cursor = headPtr;

    while (serializedLength > 0) {
        cursor->next = new PacketNode();
        cursor = cursor->next;
        cursor->next = NULL;
        cursor->data = new Packet(curPacketId++, false, Packet_Data, inMessage->sourceName, inMessage->targetName);
        bytesAdded = cursor->data->setPayload(byteStream, serializedLength);
        byteStream += bytesAdded;
        serializedLength -= bytesAdded;
    }

    cursor->data->finalPacket = true;

    return headPtr;
}

Message* NetworkLayer::checkForMessages(bool &hasError) {
    if (internalFD == 0)
        return NULL;

    int n;
    if (checkSocketHasData(internalFD)) {
        char generalBuffer[255];
        memset(generalBuffer, '\0', 255);
        int n = read(internalFD, generalBuffer, 255);

        if (n == 0) {
            printf("Error, lost connection with Datalink Layer\n");
            hasError = true;
            return NULL;
        }
        else {
            memcpy(partialMessageBuffer+partialBufferUsed, generalBuffer, n);
            partialBufferUsed += n;
            memset(generalBuffer, '\0', MAX_PACKET_SIZE);
            statsBytesReceived += n;

            //process only complete messages
            while (partialBufferUsed >= MAX_PACKET_SIZE) {
                Packet *newPacket = new Packet(partialMessageBuffer);
                statsPacketsReceived++;

                if (!addPacket(newPacket)) {
                    printf("Error while recreating packet!\n");
                }

                printf("Received Packet (%i)\n", newPacket->packetId);

                if (newPacket->type == Packet_Data)
                    statsBytesOverheadReceived += (MAX_PACKET_SIZE - MAX_PACKET_PAYLOAD);
                else
                    statsBytesOverheadReceived += MAX_PACKET_SIZE;

                //shift buffer
                char *tmpBuffer = new char[DEFAULT_BUFFER_SIZE];
                memset(tmpBuffer, '\0', DEFAULT_BUFFER_SIZE);
                memcpy(tmpBuffer, partialMessageBuffer + MAX_PACKET_SIZE, (DEFAULT_BUFFER_SIZE-MAX_PACKET_SIZE));
                delete partialMessageBuffer;
                partialMessageBuffer = tmpBuffer;
                partialBufferUsed -= MAX_PACKET_SIZE;
            }
            while (hasFinalPacket()) {
                printf("Rebuilding Message\n");
                statsMessagesReceived++;

                PacketNode *packetList = extractPacketList();
                if (packetList == NULL) {
                    printf("Error rebuilding packet list\n");
                    hasError = true;
                    return NULL;
                }
                return convertPacketsToMessage(packetList);
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


Message* NetworkLayer::convertPacketJoinToMessage(PacketNode *headptr) {
    if (headptr == NULL)
        return NULL;
    if (headptr->data->type != Packet_Join)
        return NULL;

    return new Message(Message_Join, headptr->data->payload, headptr->data->payloadUsed, 0, headptr->data->sourceName, headptr->data->targetName);
}

Message* NetworkLayer::convertPacketsToMessage(PacketNode *headptr) {
    if (headptr == NULL)
        return NULL;
    if (headptr->data->type == Packet_Join)
        return convertPacketJoinToMessage(headptr);

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
    printf("\n");

    Message *newMessage = new Message(messagePayload);

    while (headptr != NULL) {
        cursor = headptr;
        headptr = headptr->next;
        delete cursor->data;
        delete cursor;
    }

    printf("Recovered Message (%i)\n", newMessage->messageId);

    return newMessage;
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

