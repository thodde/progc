
#ifndef __DL_LAYER_H___
#define __DL_LAYER_H___

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "common.h"
#include "Message.h"
//#include "PhysicalLayer.h"

class DatalinkLayer {
public:

    DatalinkLayer(); //will need a link to the ApplicationClient

    bool sendMessage(Message *newMessage);
    bool receivePacket(Packet *newPacket);


private:
    void convertMessageToFrame(Message *inMessage);
    //this won't work, as it takes several packets to create a frame, but it's a start...
    void convertPacketToFrame(Packet *inPacket);
};

#endif