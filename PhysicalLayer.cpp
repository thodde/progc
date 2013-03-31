
#include "PhysicalLayer.h"

/*
bool sendFrame(Frame *outFrame) {
    return true;
};
 */


PhysicalLayer::PhysicalLayer() {
    externalFD = 0;
    internalFD = 0;
    externalPort = 0;
}

PhysicalLayer::~PhysicalLayer() {
    if (externalFD != 0)
        close(externalFD);
    if (internalFD != 0)
        close(internalFD);
}


bool PhysicalLayer::initialize(int portExternal, int portInternal) {
    printf("Initializing Physical Layer\n");
    internalFD = 0;
    externalPort = portExternal;

    internalFD = listenForInternalService(portInternal, "Physical Layer");
    if (internalFD == 0){
        printf("Error, closing down\n");
        return false;
    }

/*
    internalDLLFD = connectToInternalService(portInternalDLL, "Datalink Layer");
    if (internalDLLFD == 0) {
        printf("Error, closing down\n");
        return false;
    }  */

    return true;
}


int main (int argc, char *argv[]) {
    printf("Starting Physical Layer\n");

    PhysicalLayer *myPH = new PhysicalLayer();
    if (!myPH->initialize(EXTERNAL_PORT, PH_PORT)) {
        delete myPH;
        return -1;
    }

    //begin main execution loop
    while (true) {
       ;
    }

    delete myPH;
    return 1;
}

/*
bool PhysicalLayer::sendFrame(Frame *outFrame) {
    return false;
}

bool PhysicalLayer::receiveData() {
    return false;
}

Packet* PhysicalLayer::convertFrameToPacket(Frame *inFrame) {
    //stuff bits

    //cut into XXX length segments, -3?  provided they don't break the EOL identifier
    return NULL;
}


// chars read?  or remaining?
int PhysicalLayer::readPacket(char* inString, Packet *outPacket) {
    return 0;
}



char* PhysicalLayer::stuffBits(char *inStream) {
    //find sequence

    //replace

    //advance

    //find sequence (repeat)

    return NULL;
}


*/