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

char* Frame::serialize() {
    char* outFrame = new char[MAX_FRAME_SIZE];
    memset(outFrame, '\0', MAX_FRAME_SIZE);
    char* frameCursor = outFrame;

    memcpy(frameCursor, &frameId, sizeof(frameId));
    frameCursor += sizeof(frameId);
    memcpy(frameCursor, &type, sizeof(type));
    frameCursor += sizeof(type);
    memcpy(frameCursor, &finalFrame, sizeof(finalFrame));
    frameCursor += sizeof(finalFrame);
    memcpy(frameCursor, &payloadUsed, sizeof(payloadUsed));
    frameCursor += sizeof(payloadUsed);
    memcpy(frameCursor, payload, sizeof(char)*payloadUsed);
    frameCursor += sizeof(char)*payloadUsed;

    return outFrame;
}

bool Frame::deSerialize(char *stream) {
    char *frameCursor = stream;
    memset(payload, '\0', MAX_FRAME_PAYLOAD);

    memcpy(&frameId, frameCursor, sizeof(frameId));
    frameCursor += sizeof(frameId);
    memcpy(&type, frameCursor, sizeof(type));
    frameCursor += sizeof(type);
    memcpy(&finalFrame, frameCursor, sizeof(finalFrame));
    frameCursor += sizeof(finalFrame);
    memcpy(&payloadUsed, frameCursor, sizeof(payloadUsed));
    frameCursor += sizeof(payloadUsed);
    memcpy(payload, frameCursor, sizeof(char)*payloadUsed);
    frameCursor += sizeof(char)*payloadUsed;

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
