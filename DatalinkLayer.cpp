
#include "DatalinkLayer.h"

/*
bool sendMessage(Message *outMessage) {
    return true;
};*/

DatalinkLayer::DatalinkLayer() {
    printf("hello world, this is the datalink layer\n");
};


bool DatalinkLayer::sendMessage(Message *newMessage) {
    return true;
}


bool DatalinkLayer::receivePacket(Packet *newPacket) {
    return true;
}

void DatalinkLayer::convertMessageToFrame(Message *inMessage) {
    return;
}

void DatalinkLayer::convertPacketToFrame(Packet *inPacket) {
    return;
}