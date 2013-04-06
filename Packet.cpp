#include "Packet.h"

Packet::Packet(unsigned int newPacketId, bool isFinalPacket) {
    //default values
    payloadUsed = 0;
    packetId = newPacketId;
    memset(payload, '\0', MAX_PACKET_PAYLOAD);
    finalPacket = isFinalPacket;
}

Packet::Packet(char* instream) {
    memset(payload, '\0', MAX_PACKET_PAYLOAD);
    payloadUsed = 0;
    deSerialize(instream);
}

char* Packet::serialize() {
    char* outMessage = new char[MAX_PACKET_SIZE];
    memset(outMessage, '\0', MAX_PACKET_SIZE);
    char* msgCursor = outMessage;
    memcpy(msgCursor, &packetId, sizeof(packetId));
    msgCursor += sizeof(packetId);
    memcpy(msgCursor, &type, sizeof(type));
    msgCursor += sizeof(type);
    memcpy(msgCursor, &finalPacket, sizeof(finalPacket));
    msgCursor += sizeof(finalPacket);
    memcpy(msgCursor, &payloadUsed, sizeof(payloadUsed));
    msgCursor += sizeof(payloadUsed);
    memcpy(msgCursor, payload, sizeof(char)*payloadUsed);
    msgCursor += sizeof(char)*payloadUsed;

    return outMessage;
}

bool Packet::deSerialize(char *stream) {
    char *msgCursor = stream;

    memcpy(&packetId, msgCursor, sizeof(packetId));
    msgCursor += sizeof(packetId);
    memcpy(&type, msgCursor, sizeof(type));
    msgCursor += sizeof(type);
    memcpy(&finalPacket, msgCursor, sizeof(finalPacket));
    msgCursor += sizeof(finalPacket);
    memcpy(&payloadUsed, msgCursor, sizeof(payloadUsed));
    msgCursor += sizeof(payloadUsed);
    memcpy(payload, msgCursor, sizeof(char)*payloadUsed);
    msgCursor += sizeof(char)*payloadUsed;

    //printf("Deserialized Packet Id: %i Payload Used: %i, Payload: %s\n", packetId, payloadUsed, payload);

    return true;
}

int Packet::setPayload(char* inStream, int size) {
    memset(payload, '\0', MAX_PACKET_PAYLOAD);
    if (size <= 0)
        return 0;

    payloadUsed = 0;

    if (size < MAX_PACKET_PAYLOAD) {
        memcpy(payload, inStream, size);
        payloadUsed = size;
    }
    else {
        memcpy(payload, inStream, MAX_PACKET_PAYLOAD);
        payloadUsed = MAX_PACKET_PAYLOAD;
    }
    return payloadUsed;
}
