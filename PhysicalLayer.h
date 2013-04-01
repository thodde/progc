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
//#include "DatalinkLayer.h"

class PhysicalLayer {
public:
    PhysicalLayer();
    ~PhysicalLayer();

    bool initialize(int portExternal, int portInternal);
    bool run();

//    bool beginListening();

    //bool sendFrame(Frame *outFrame);
    //bool receiveData();

private:
    int externalPort;
    int externalFD;
    int internalFD;

    //Packet* convertFrameToPacket(Frame *inFrame);
    //int readPacket(char* inString, Packet *outPacket);
    //bool appropriatePort();
    //char* stuffBits(char* inStream);

    //int internalPort;
    //int listeningSocketFileDescriptor;
    //DatalinkLayer myDLL;
};

#endif