
#include "SlidingWindow.h"

SlidingWindow::SlidingWindow(char *newClientName) {
    strcpy(clientName, newClientName);
    overflowHead = NULL;
    for (int i = 0; i < MAX_WINDOW_SIZE; i++) {
        myWindow[i] = NULL;
    }

}

SlidingWindow::~SlidingWindow() {
    for (int i = 0; i < MAX_WINDOW_SIZE; i++) {
        if (myWindow[i] == NULL) {
            delete myWindow[i]->data;
            delete myWindow[i];
            myWindow[i] == NULL;
        }
    }

    while (overflowHead == NULL) {
        FrameWindowEntry *cursor = overflowHead;
        overflowHead = overflowHead->next;
        delete cursor->data;
        delete cursor;
    }
}

bool SlidingWindow::addFrameToSend(Frame *inFrame) {
    FrameWindowEntry *newEntry = new FrameWindowEntry();
    newEntry->data = inFrame;
    newEntry->isAcked = false;
    newEntry->transType = FN_ToSend;
    newEntry->timeout = 0; //initially timed out //clock() + FRAME_TIMEOUT_IN_MS;
    newEntry->next = NULL;

    bool currentlyReceiving = false;
    for (int i = 0; i < MAX_WINDOW_SIZE; i++) {
        if (myWindow[i] != NULL) {
            if (myWindow[i]->transType == FN_ToReceive)
                currentlyReceiving = true;
        }
    }

    if (currentlyReceiving) {
        //I'm currently receiving a message, buffer this message to be sent
        return addFrameWindowEntryToBuffer(newEntry);
    }

    for (int i = 0; i < MAX_WINDOW_SIZE; i++) {
        if (myWindow[i] == NULL) {
            myWindow[i] = newEntry;
            return true;
        }
    }

    //no available space in the window, add to the buffer
    return addFrameWindowEntryToBuffer(newEntry);
}


bool SlidingWindow::addFrameToReceive(Frame *inFrame) {
    //is this a returned Ack?
    if (inFrame->type == Frame_Ack) {
        //TODO implement
        //receive ack
        //adjust window
        //add more to current window?

        return true;
    }
    else {
        FrameWindowEntry *newEntry = new FrameWindowEntry();
        newEntry->data = inFrame;
        newEntry->isAcked = false;
        newEntry->transType = FN_ToReceive;
        newEntry->timeout = 0; //initially timed out //clock() + FRAME_TIMEOUT_IN_MS;
        newEntry->next = NULL;

        //Always create an Ack entry to be returned to sending machine
        FrameWindowEntry *ackEntry = new FrameWindowEntry();
        //unsigned int newFrameId, bool isFinalFrame, Frame_Type newType, char *newSourceName, char *newTargetName
        ackEntry->data = new Frame(inFrame->frameId, true, Frame_Ack, inFrame->targetName, inFrame->sourceName);
        ackEntry->isAcked = false;
        ackEntry->transType = FN_ToSend;
        ackEntry->timeout = 0;
        ackEntry->next = NULL;
        addFrameWindowEntryToBuffer(ackEntry);

        bool currentlySending = false;
        for (int i = 0; i < MAX_WINDOW_SIZE; i++) {
            if (myWindow[i] != NULL) {
                if (myWindow[i]->transType == FN_ToSend)
                    currentlySending = true;
            }
        }

        if (currentlySending) {
            //I'm currently sending a message, buffer this message to be received
            return addFrameWindowEntryToBuffer(newEntry);
        }

        for (int i = 0; i < MAX_WINDOW_SIZE; i++) {
            if (myWindow[i] == NULL) {
                myWindow[i] = newEntry;
                return true;
            }
        }

        //no available space in the window, default add to the buffer
        return addFrameWindowEntryToBuffer(newEntry);
    }
}

Frame* SlidingWindow::getFrameToSend() {

    return NULL;
}

Packet* SlidingWindow::getPacketToReceive() {
    return NULL;
}


bool SlidingWindow::addFrameWindowEntryToBuffer(FrameWindowEntry *newEntry) {
    if (overflowHead == NULL) {
        overflowHead = newEntry;
        overflowHead->next = NULL;
        return true;
    }
    FrameWindowEntry *cursor = overflowHead;
    while (cursor->next != NULL)
        cursor = cursor->next;

    cursor->next = newEntry;
    cursor = cursor->next;
    cursor->next = NULL;
    return true;
}