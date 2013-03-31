#ifndef __NETWORK_H__
#define __NETWORK_H__

#include "common.h"
#include "Message.h"

class NetworkLayer {
public:
    NetworkLayer();

    bool sendMessage(Message *newMessage);
    bool receivePacket(Packet *newPacket);

//translates clientA to ip address

private:
    // converts the message from the Application layer to a consumable message by the Datalink Layer
    void convertMessageToFrame(Message *inMessage);


    //this won't work, as it takes several packets to create a frame, but it's a start...
    void convertPacketToFrame(Packet *inPacket);

};


#endif