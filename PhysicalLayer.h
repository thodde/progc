#ifndef __PHY_LAYER_H___
#define __PHY_LAYER_H___

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "common.h"
#include "DatalinkLayer.h"

class PhysicalLayer {
public:
    PhysicalLayer(bool newIsServer, int isPort, DatalinkLayer *newDLL);

    bool sendFrame(Frame *outFrame);
    bool receivePacket(Packet *inPacket);

private:
    int port;
    bool isServer;
    DatalinkLayer myDLL;
};

#endif