
#ifndef __DL_LAYER_H___
#define __DL_LAYER_H___

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "common.h"
//#include "Message.h"
//#include "PhysicalLayer.h"


//IP address of next hop, plus info for next level up
class DatalinkLayer {
public:

    DatalinkLayer();
    ~DatalinkLayer();

    bool initialize(int portInternalUp, int portInternalDown);
    bool run();


/*
    bool dl_send(Packet *outPacket);
    bool dl_recv(Packet *inPacket);
*/

    // error detection bits

private:
    int internalDownFD;
    int internalUpFD;
    //NetworkLayer myNL;
};

#endif