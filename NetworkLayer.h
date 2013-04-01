#ifndef __NETWORK_H__
#define __NETWORK_H__

#include "common.h"
#include "Message.h"

/**
 *  This class encapsulates all the functionality of the network layer.  Unlike the Physical or Datalink Layer, this
 *  layer is directly incorporated into the application layer.
 */
class NetworkLayer {
public:
    NetworkLayer();
    ~NetworkLayer();

    /**
      *   Sends a message to the Datalink Layer returns true if successfully sent, false otherwise
      * @param newMessage - the message to be sent.  The message is not duplicated.
      * @return true if successfully sent, false otherwise
      */
    bool sendMessage(Message *newMessage);

    /**
     *   Initializes the Network Layer.
     *  @param portInternal - the port number of the local Datalink Layer service
     *  @return true if successful, false is initialization failed
     */
    bool initialize(int portInternal);

    /**
     *  Checks to see if the Datalink Layer has sent any messages.
     *  @return NULL if no messages are present, one complete Message otherwise
     */
    Message* checkForMessages();

//    bool receivePacket(Packet *newPacket);



//translates clientA to ip address

private:
    // The internal file descriptor for the socket used to communicate with the datalink layer
    int internalFD;

    // converts the message from the Application layer to a consumable message by the Datalink Layer
    //void convertMessageToFrame(Message *inMessage);


    //this won't work, as it takes several packets to create a frame, but it's a start...
    //void convertPacketToFrame(Packet *inPacket);

};


#endif