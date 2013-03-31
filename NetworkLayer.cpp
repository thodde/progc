
#include "NetworkLayer.h"

NetworkLayer::NetworkLayer() {

}


bool NetworkLayer::sendMessage(Message *newMessage) {
    return true;
}


bool NetworkLayer::receivePacket(Packet *newPacket) {
    return true;
}


void NetworkLayer::convertMessageToFrame(Message *inMessage) {
    return;
}

void NetworkLayer::convertPacketToFrame(Packet *inPacket) {
    return;
}