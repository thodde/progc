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
#include "Frame.h"
//#include "DatalinkLayer.h"

/**
 *  The class encapsulates the Physical Layer (managing bits on the "wire").  It utilizes the same internal
 *  communication mechanisms as the Datalink Layer above.  As with the Datalink Layer, the Physical Layer runs in an
 *  independent process and makes be kill from the OS to stop.
 */
class PhysicalLayer {
public:
    /**
     *  Default constructor
     */
    PhysicalLayer();

    /**
     *  Default destructor
     */
    ~PhysicalLayer();

    /**
     *  Initializes the Physical Layer and makes the appropriate connections
     *  @param portExternal - the external port used to receive communications from external machines.  Leave as 0
     *  if this application is a client.
     *  @param portInternal - the internal port used to communicate with the Datalink Layer
     *  @return true if successful, false otherwise
     */
    bool initialize(int portExternal, int portInternal);

    /**
     *  The main run body.  This function listens for communications on the external port or from the datalink layer
     *  and processes appropriately.  This function never terminates.
     *  @return true
     */
    bool run();

//    bool beginListening();

    //bool sendFrame(Frame *outFrame);
    //bool receiveData();

private:
    // the external port being listened to for new clients if this is a server
    int externalPort;

    // the socket description of the external port.  To be modified to allow multiple clients.
    int externalFD;

    // The file descriptor of the socket used to communicate with the datalink layer.
    int internalFD;

    //bool appropriatePort();
    //char* stuffBits(char* inStream);

    //int internalPort;
    //int listeningSocketFileDescriptor;
    //DatalinkLayer myDLL;
};

#endif
