#include "common.h"
#include "Frame.h"

Frame::Frame(unsigned int newFrameId, bool isFinalFrame) {
    //default values
    payloadUsed = 0;
    frameId = newFrameId;
    memset(payload, '\0', MAX_FRAME_PAYLOAD);
    finalFrame = isFinalFrame;
}

Frame::Frame(char* instream) {
    memset(payload, '\0', MAX_FRAME_PAYLOAD);
    payloadUsed = 0;
    deSerialize(instream);
}

char* Frame::serialize(int serializedLength) {
    char* outPacket = new char[MAX_FRAME_SIZE];
    memset(outPacket, '\0', MAX_FRAME_SIZE);
    char* packetCursor = outPacket;
    memcpy(packetCursor, &frameId, sizeof(frameId));
    packetCursor += sizeof(frameId);
    memcpy(packetCursor, &type, sizeof(type));
    packetCursor += sizeof(type);
    memcpy(packetCursor, &finalFrame, sizeof(finalFrame));
    packetCursor += sizeof(finalFrame);
    memcpy(packetCursor, &payloadUsed, sizeof(payloadUsed));
    packetCursor += sizeof(payloadUsed);
    memcpy(packetCursor, payload, sizeof(char)*payloadUsed);
    packetCursor += sizeof(char)*payloadUsed;

    return outPacket;
}

bool Frame::deSerialize(char *stream) {
    char *packetCursor = stream;

    memcpy(&frameId, packetCursor, sizeof(frameId));
    packetCursor += sizeof(frameId);
    memcpy(&type, packetCursor, sizeof(type));
    packetCursor += sizeof(type);
    memcpy(&finalFrame, packetCursor, sizeof(finalFrame));
    packetCursor += sizeof(finalFrame);
    memcpy(&payloadUsed, packetCursor, sizeof(payloadUsed));
    packetCursor += sizeof(payloadUsed);
    memcpy(payload, packetCursor, sizeof(char)*payloadUsed);
    packetCursor += sizeof(char)*payloadUsed;

    return true;
}

int Frame::setPayload(char* inStream, int size) {
    memset(payload, '\0', MAX_FRAME_PAYLOAD);
    if (size <= 0)
        return 0;

    payloadUsed = 0;

    if (size < MAX_FRAME_PAYLOAD) {
        memcpy(payload, inStream, size);
        payloadUsed = size;
    }
    else {
        memcpy(payload, inStream, MAX_FRAME_PAYLOAD);
        payloadUsed = MAX_FRAME_PAYLOAD;
    }
    return payloadUsed;
}
