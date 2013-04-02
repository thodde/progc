
#include "NetworkLayer.h"

NetworkLayer::NetworkLayer() {
    internalFD = 0;
    curPacketId = 0;
    memset(partialMessageBuffer, '\0', 1024);
    partialBufferUsed = 0;
    packetsReceived = 0;
}

NetworkLayer::~NetworkLayer() {
    if (internalFD != 0)
        close(internalFD);
}

bool NetworkLayer::initialize(int portInternal) {
    printf("Initializing Network Layer\n");
    internalFD = 0;

//TODO set to block during testing, turn this back aftewards...
//    internalFD = connectToInternalService(portInternal, "Datalink Layer", true);
    internalFD = connectToInternalService(portInternal, "Datalink Layer", false);
    if (internalFD == 0) {
        printf("Error, closing down\n");
        return false;
    }

    return true;
}


bool NetworkLayer::sendMessage(Message *newMessage) {
    if (internalFD == 0)
        return false;

    //validate message parameters (like length > 0
    // break into packets
    PacketNode *sendList = convertMessageToPackets(newMessage);

    if (sendList == NULL) //errors converting message
        return false;

    int n;
    while (sendList != NULL) {

        char *sendStream = sendList->data->serialize();

/*
        printf("Network Layer sending:");
        for(int i = 0; i < MAX_PACKET_SIZE; i++)
            printf("%c", sendStream[i]);
        printf("\n");
*/

        n = write(internalFD, sendStream, MAX_PACKET_SIZE);
        if (n < 0)
             printf("ERROR writing to socket");

        delete sendList->data;
        sendList = sendList->next;
    }

    return true;
}

PacketNode* NetworkLayer::convertMessageToPackets(Message *inMessage) {
    if (inMessage == NULL)
        return NULL;

    int serializedLength;
    char *byteStream = inMessage->serialize(serializedLength);

    PacketNode *headPtr = new PacketNode();
    headPtr->next = NULL;

    headPtr->data = new Packet(curPacketId++, false);
    //TODO update the strlen function to be something more appropriate.  Say a decremented variable based on the size from the message object?

    int bytesAdded = headPtr->data->setPayload(byteStream, serializedLength);
    byteStream += bytesAdded;
    serializedLength -= bytesAdded;

    PacketNode *cursor = headPtr;

/*
    printf("Packet'ed message (%i): ", cursor->data->packetId);
    for (int i = 0; i < MAX_PACKET_PAYLOAD; i++)
        printf("%c", cursor->data->payload[i]);
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

/*
        printf("Packet'ed message (%i): ", cursor->data->packetId);
        for (int i = 0; i < MAX_PACKET_PAYLOAD; i++)
            printf("%c", cursor->data->payload[i]);
        printf("\n");
*/

    }
    cursor->data->finalPacket = true;

    return headPtr;
}

Message* NetworkLayer::checkForMessages() {
    if (internalFD == 0)
        return NULL;

    char generalBuffer[255];
    memset(generalBuffer, '\0', 255);
    int n = read(internalFD, generalBuffer, 255);

    if (n == 0) {

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
            receivedPackets[packetsReceived] = new Packet(partialMessageBuffer);
            packetsReceived++;

            //shift buffer
            memmove(partialMessageBuffer, (partialMessageBuffer + MAX_PACKET_SIZE), (1024-MAX_PACKET_SIZE));
            //null out end bytes
            memset(partialMessageBuffer+1024-MAX_PACKET_SIZE, '\0', MAX_PACKET_SIZE);
            partialBufferUsed -= MAX_PACKET_SIZE;

            /*
            printf("Packet Rebuilt.  Payload:");
            for (int i = 0; i < MAX_PACKET_PAYLOAD; i++) {
                printf("%c", receivedPackets[packetsReceived - 1]->payload[i]);
            }
            printf("\n");
            */
        }

/*
        printf("packets received: %i\n", packetsReceived);
*/

        //TODO this really doesn't belong here, but for testing purposes...
        while (hasFinalPacket()) {
            printf("Rebuilding Message\n");

            //TODO arbitrary message length.  Should be managed, as this is used in multiple places
            char msg_string[2048];
            memset(msg_string, '\0', 2048);
            int charsRead = 0;

            int i = 0;
            for (i = 0; i < packetsReceived; i++) {
                memcpy(msg_string+charsRead, receivedPackets[i]->payload, MAX_PACKET_PAYLOAD);
                charsRead += MAX_PACKET_PAYLOAD;

                bool finalPacket = receivedPackets[i]->finalPacket;
//TODO MEMORY LEAK!!!
//                delete receivedPackets[i];
                receivedPackets[i] = NULL;

                if (finalPacket) {
                    break;
                }
            }
            i++;


            //todo arbitrary, this has to be tied to the number of packets available
            //shuffle down the packets not used
            for (int x = 0; x < 100; x++) {
                if ((x+i) < 100)
                    receivedPackets[x] = receivedPackets[x+i];
                else
                    receivedPackets[x] = NULL;
            }

            packetsReceived -= i;

            Message *myMessage = new Message(msg_string);
            return myMessage;
        }
    }




    return NULL;
}

bool NetworkLayer::hasFinalPacket() {
    for (int i = 0; i < packetsReceived; i++) {
        if (receivedPackets[i] != NULL)
            if (receivedPackets[i]->finalPacket)
                return true;
    }
    return false;
}

    /*
bool NetworkLayer::receivePacket(Packet *newPacket) {
    return true;
} */


/*
void NetworkLayer::convertPacketToFrame(Packet *inPacket) {
    return;
}
*/