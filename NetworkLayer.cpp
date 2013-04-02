
#include "NetworkLayer.h"

NetworkLayer::NetworkLayer() {
    internalFD = 0;
}

NetworkLayer::~NetworkLayer() {
    if (internalFD != 0)
        close(internalFD);
}

bool NetworkLayer::initialize(int portInternal) {
    printf("Initializing Network Layer\n");
    internalFD = 0;

    internalFD = connectToInternalService(portInternal, "Datalink Layer");
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

        //TODO more problems here if sendStream contains valid '\0's
        printf("Network Layer sending: %s\n", sendStream);
        n = write(internalFD, sendStream, strlen(sendStream));
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

    char *byteStream = inMessage->serialize();

    PacketNode *headPtr = new PacketNode();
    headPtr->next = NULL;

    headPtr->data = new Packet();
    //TODO update the strlen function to be something more appropriate.  Say a decremented variable based on the size from the message object?
    int bytesAdded = headPtr->data->setPayload(byteStream, strlen(byteStream));
    byteStream += bytesAdded;

    PacketNode *cursor = headPtr;

    while (strlen(byteStream) > 0) {
        cursor->next = new PacketNode();
        cursor = cursor->next;
        cursor->next = NULL;
        cursor->data = new Packet();
        bytesAdded = cursor->data->setPayload(byteStream, strlen(byteStream));
        byteStream += bytesAdded;
    }

    return headPtr;
}

/**
 *  TODO implement
 */
Message* NetworkLayer::checkForMessages() {
    if (internalFD == 0)
        return NULL;



    return NULL;
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