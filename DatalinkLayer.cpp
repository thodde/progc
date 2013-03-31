
#include "DatalinkLayer.h"

/*
bool sendMessage(Message *outMessage) {
    return true;
};*/

DatalinkLayer::DatalinkLayer() {
    internalUpFD = 0;
    internalDownFD = 0;
};


DatalinkLayer::~DatalinkLayer() {
    if (internalUpFD != 0)
        close(internalUpFD);
    if (internalDownFD != 0)
        close(internalDownFD);
}

bool DatalinkLayer::initialize(int portInternalUp, int portInternalDown) {
    printf("Initializing Datalink Layer\n");
    internalUpFD = 0;
    internalDownFD = 0;


    internalDownFD = connectToInternalService(portInternalDown, "Physical Layer");
    if (internalDownFD == 0) {
        printf("Error, closing down\n");
        return false;
    }

/*  hold off on this code until the application code is at a point where it is ready to connect
    internalUpFD = listenForInternalService(portInternalUp, "Datalink Layer");
    if (internalUpFD == 0){
        printf("Error, closing down\n");
        return false;
    }
    */


    return true;
}


int main (int argc, char *argv[]) {
    printf("Starting Datalink Layer\n");

    DatalinkLayer *myDL = new DatalinkLayer();
    if (!myDL->initialize(DLL_PORT, PH_PORT)) {
        delete myDL;
        return -1;
    }

    //begin main execution loop
    while (true) {
       ;
    }
    //begin main execution loop
    delete myDL;
    return 1;
}


  /*
bool DatalinkLayer::dl_send(Packet *outPacket) {
    return true;
}

bool DatalinkLayer::dl_recv(Packet *inPacket) {
    return true;
}
*/


