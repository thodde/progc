
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


bool PhysicalLayer::run() {
    char buffer[255];
    memset(buffer, '\0', 255);
    int n;

    //begin main execution loop
    while (true) {
//        n = read(internalFD, buffer,255);
        n = recv(internalFD, buffer,255, 0);
        if (n > 0) {
            printf("Received message from Datalink Layer\n");
            printf("Message content: %s\n", buffer);

            //logic to be determined

            memset(buffer, '\0', 255);
        }

        /* to be written
        n = read(externalFD,buffer,255);
        if (n > 0) {
            printf("Received message from external machine\n");
            printf("Message content: %s\n", buffer);
            printf("To be written\n");
            memset(buffer, '\0', 255);
        }
        */
    }

    return true;
}

int main (int argc, char *argv[]) {
    printf("Starting Physical Layer\n");

    PhysicalLayer *myPH = new PhysicalLayer();
    if (!myPH->initialize(EXTERNAL_PORT, PH_PORT)) {
        delete myPH;
        return -1;
    }

    myPH->run();

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