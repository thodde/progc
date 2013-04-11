
#ifndef __DL_LAYER_H___
#define __DL_LAYER_H___

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

#include "common.h"
#include "Frame.h"
#include "Packet.h"

#define MAX_WINDOW_SIZE 4


//IP address of next hop, plus info for next level up

/**
 *  This class encapsulates the functionality of the Datalink Layer (connection between two neighboring nodes).
 *  It incorporates several Frame consistency
 *  mechanisms such as the delivery/confirmation mode (stop and wait/go back n/selective repeat) and maintains
 *  the checksum.  This layer runs as a stand alone process and interacts with the other layers via ports.
 *
 *  Each layer functions by connecting to the service below, and listening on a port for a connection from above.
 *  Due to this, layers must be built from the bottom of the OSI model up.  This also uses the constraint that only
 *  one service can be communicating with each end point of the layer at any time, essentially enforcing the
 *  tiered OSI structure.
 */
class DatalinkLayer {
public:

    /**
     *  Default constructor
     */
    DatalinkLayer();

    /**
     *  Default destructor
     */
    ~DatalinkLayer();

    /**
     *  Initializes the Datalink Layer and makes the appropriate connections to the other neighboring layers
     *  @param portInternalUp - the port used to listen to communications from  the network layer
     *  @param portInternalDown - the port used to connect to the Physical Layer service
     *  @return - true if successful, false otherwise
     */
    bool initialize(int portInternalUp, int portInternalDown);

    /**
     *  The main processing loop.  All of the base functionality of the DLL is called here (listening for frames from
     *  the network layer, listening for packets from the physical layer, error detection of incoming packets, timeout
     *  of unacknowledged packets.  This function has no completion mechanism currently, and the process must be killed
     *  to stop running.
     *  @return true
     */
    bool run();


/*
    bool dl_send(Packet *outPacket);
    bool dl_recv(Packet *inPacket);
*/

    // error detection bits

private:
    //the file descriptor for the socket leading to the physical layer
    int internalDownFD;

    //the file descriptor for the socket leading to the network layer
    int internalUpFD;

    char *upBuffer;
    char *downBuffer;
    
    unsigned int curFrameId;

//remove this 'public' later
public:
    FrameNode *framesReceived;

    // converts the packet from the Network layer to a consumable Frame by the Physical Layer
    FrameNode* convertPacketsToFrames(Packet* inPacket);
    FrameNode* convertControlPacketToFrame(Packet *inPacket);

    bool addFrameReceived(Frame *inFrame);
    bool hasFinalFrame();
    FrameNode* extractFrameList();
    Packet* convertFramesToPacket(FrameNode *headptr);
    Packet* convertFrameJoinToPacket(FrameNode *headptr);
    bool receiveDataFromPhysicalLayer();
    bool receiveDataFromNetworkLayer();

    int upBufferUsed;
    int downBufferUsed;
    Packet* receivedPackets[100];
    int packetsReceived;

};

#endif
