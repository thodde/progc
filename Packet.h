#ifndef __PACKET_H__
#define __PACKET_H__

#include "common.h"

#define     MAX_PACKET_SIZE         192
#define     MAX_PACKET_PAYLOAD      150 //this will need to be lowered due to the packet header

typedef enum { Packet_Join, Packet_Data, Packet_Stack_Control } Packet_Type;

struct Packet {
    //the packet id will need to be removed later, this should only exist at the DLL layer.
    Packet(unsigned int newPacketId, bool isFinalPacket, Packet_Type newType, char *newSourceName, char *newTargetName);
    Packet(char* instream);

    /**
     *  Read as far into the stream as possible and load into playload.  Return number of chars loaded into payload.
     *  TODO the problem here is that string functions have some delimiters that I want to ignore and copy over.  Is there a buffer copy?
     */
    int setPayload(char* inStream, int size);

    char* serialize();
    bool deSerialize(char *stream);
    bool finalPacket;

    unsigned int packetId;
    int payloadUsed;
    char sourceName[10];
    char targetName[10];
    Packet_Type type;
    char payload[MAX_PACKET_PAYLOAD];    // minus overhead
};

struct PacketNode {
    Packet *data;
    PacketNode *next;
};

#endif
