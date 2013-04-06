#ifndef __FRAME_H__
#define __FRAME_H__

#include "common.h"

#define FRAME_HEADER_SIZE         100

typedef enum { Frame_Init, Frame_Data, Frame_Ack, Frame_Final } Frame_Type;

struct Frame {
public:

    Frame(unsigned int newFrameId, bool isFinalFrame);
    Frame(char* instream);
    Frame_Type type;

    /**
     *  Read as far into the stream as possible and load into playload.  Return number of chars loaded into payload.
     *  TODO the problem here is that string functions have some delimiters that I want to ignore and copy over.  Is there a buffer copy?
     */
    int setPayload(char* inStream, int size);

    char* serialize(int serializedLength);
    bool deSerialize(char* stream);
    bool finalFrame;

    unsigned int frameId; // identifier used by sliding window protocol
    char payload[MAX_FRAME_SIZE];
    bool moreFrames;
    int payloadUsed;
//    unsigned int checkByte; // byte used for error detection.  MOD 255 of the Frame object to 0

};

struct FrameNode {
    Frame* data;
    FrameNode* next;
};

#endif
