#ifndef __PHY_LAYER_H___
#define __PHY_LAYER_H___

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "common.h"
#include "DatalinkLayer.h"

class PhysicalLayer {
public:
    PhysicalLayer(int newPort, DatalinkLayer *newDLL);
    ~PhysicalLayer();

    bool sendFrame(Frame *outFrame);
    bool beginListening();
    bool receiveData();

private:
    Packet* convertFrameToPacket(Frame *inFrame);
    int readPacket(char* inString, Packet *outPacket);
    bool appropriatePort();
    char* stuffBits(char* inStream);

    int port;
    int listeningSocketFileDescriptor;
    DatalinkLayer myDLL;
};

#endif