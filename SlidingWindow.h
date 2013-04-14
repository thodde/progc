
#ifndef __SLIDING_WINDOW_H__
#define __SLIDING_WINDOW_H__

#include <string.h>
#include <time.h>
#include <stdio.h>

#include "Frame.h"
#include "Packet.h"

#define MAX_WINDOW_SIZE 4
#define FRAME_TIMEOUT_IN_MS     5000

typedef enum { FN_ToSend, FN_ToReceive, FN_Unknown } TransmissionType;


struct FrameWindowEntry {
    Frame *data;
    bool isAcked;
    TransmissionType transType;
    time_t timeout;
    FrameWindowEntry *next;    //!< Used only by the buffer overflow
};


/**
 *  Implied requirement: an entire packet's worth of frames can fit in the window
 */
class SlidingWindow {
public:
    SlidingWindow(char *newClientName);
    ~SlidingWindow();

    char clientName[80];

    /**
     *  Adds a frame to the window and sets its type to "To Be Sent".  Also used for the returning ACK messages.  Writes to the
     *    overflow buffer is a packet is currently being received.
     */
    bool addFrameToSend(Frame *inFrame);

    /**
     *  Adds a frame to the window and sets its type to be "To Be Received".  Generates an ACK message if Appropriate
     */
    bool addFrameToReceive(Frame *inFrame);

    /**
     *  Queries if a frame is available to be sent.  Also checks if a timeout has occurred.  Returns true if a frame is
     *    available to be sent (and updates the outFrame parameter), or false otherwise
     */
    Frame* getFrameToSend();

    /**
     *  Queries if a complete packet has been received.
     */
    FrameNode* getFullPacketToReceive();



private:
    bool addFrameWindowEntryToBuffer(FrameWindowEntry *newEntry);
    FrameWindowEntry* myWindow[MAX_WINDOW_SIZE];
    FrameWindowEntry* overflowHead;

    void orderFramesById();
    unsigned int lastFrameIdReceived;

    /**
     *  Returns true if there were frames to move into window.  False otherwise
     */
    bool moveFramesIntoWindow();


};





#endif
