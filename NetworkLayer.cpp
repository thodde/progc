
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
    // break into frames

    //stringify frames

    //send each frame to DLL
    printf("Network Layer sending: %s\n", newMessage->data);
    int n = write(internalFD, newMessage->data, strlen(newMessage->data));
    if (n < 0)
         printf("ERROR writing to socket");

    return true;
}

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

void NetworkLayer::convertMessageToFrame(Message *inMessage) {
    return;
}

void NetworkLayer::convertPacketToFrame(Packet *inPacket) {
    return;
}
*/