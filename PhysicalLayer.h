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


#define MAX_CONNECTIONS         10
typedef enum { PH_Inactive, PH_Server, PH_Client } LayerActivity;

struct ClientConnection {
    int socketFD;
    char clientName[10];
};

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
    bool initialize(int portInternal);

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
    /**
     *  Handles messages from the application layer
     */
    bool processControlFrame(Frame *inFrame);

    // the external port being listened to for new clients if this is a server
    //int externalPort;

    // the socket description of the external port.  To be modified to allow multiple clients.
    //int externalFD;

    // The file descriptor of the socket used to communicate with the datalink layer.
    int internalFD;

    //Used only by the server
    ClientConnection* clientFDs[MAX_CONNECTIONS];

    //Either the port the server is listening on, or the port the client is connected to the server on
    int externalFD;

    LayerActivity myActivity;


    bool processUpstreamData();
    bool processDownstreamData(int socketId);

    //char* stuffBits(char* inStream);


    char *upBuffer;
    int upBufferUsed;

    char *downBuffer;
    int downBufferUsed;
    Frame* receivedFrame[100];
    int framesReceived;
};

#endif
