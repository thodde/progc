#ifndef __PACKET_H__
#define __PACKET_H__

#include "common.h"

typedef enum { Packet_Init, Packet_Data, Packet_Ack, Packet_Final } Packet_Type;

struct Packet {
    //the packet id will need to be removed later, this should only exist at the DLL layer.
    Packet(unsigned int newPacketId, bool isFinalPacket);
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
    Packet_Type type;
    char payload[MAX_PACKET_PAYLOAD];    // minus overhead
};

struct PacketNode {
    Packet *data;
    PacketNode *next;
};

#endif
