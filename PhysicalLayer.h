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

    bool sendFrame(Frame *outFrame);
    bool beginListening();
    bool receiveData();

private:
    void convertFrameToPacket(Frame *inFrame);

    int port;
    DatalinkLayer myDLL;
};

#endif