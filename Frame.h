#ifndef __FRAME_H__
#define __FRAME_H__

#include "common.h"

typedef enum { Frame_Join, Frame_Data, Frame_Ack, Frame_Stack_Control } Frame_Type;

#define     MAX_FRAME_SIZE          140
#define     MAX_FRAME_PAYLOAD       100 //this will need to be lowered for the frame header

struct Frame {
public:

    Frame(unsigned int newFrameId, bool isFinalFrame, Frame_Type newType, char *newSourceName, char *newTargetName);
    Frame(char* instream);
    Frame_Type type;
    char sourceName[10];
    char targetName[10];

    /**
     *  Read as far into the stream as possible and load into playload.  Return number of chars loaded into payload.
     *  TODO the problem here is that string functions have some delimiters that I want to ignore and copy over.  Is there a buffer copy?
     */
    int setPayload(char* inStream, int size);

    char* serialize();
    bool deSerialize(char* stream);
    bool finalFrame;

    unsigned int frameId; // identifier used by sliding window protocol
    char payload[MAX_FRAME_PAYLOAD];
    int payloadUsed;
//    unsigned int checkByte; // byte used for error detection.  MOD 255 of the Frame object to 0

};

struct FrameNode {
    Frame* data;
    FrameNode* next;
};

#endif
