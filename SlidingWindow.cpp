
#include "SlidingWindow.h"

SlidingWindow::SlidingWindow(char *newClientName) {
    memset(clientName, '\0', 80);
    strcpy(clientName, newClientName);
    overflowHead = NULL;
    for (int i = 0; i < MAX_WINDOW_SIZE; i++) {
        myWindow[i] = NULL;
    }
    lastFrameIdReceived = 0;
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
        for (int i = 0; i < MAX_WINDOW_SIZE; i++) {
            if (myWindow[i] != NULL) {
                if (inFrame->frameId == inFrame->frameId) {
                    delete myWindow[i]->data;
                    delete myWindow[i];
                    delete inFrame;
                    myWindow[i] = NULL;
                    orderFramesById();
                    return true;
                }
            }
        }
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
        ackEntry->data = new Frame(inFrame->frameId, true, Frame_Ack, inFrame->targetName, inFrame->sourceName, true);
        ackEntry->isAcked = false;
        ackEntry->transType = FN_ToSend;
        ackEntry->timeout = 0;
        ackEntry->next = NULL;
        addFrameWindowEntryToBuffer(ackEntry);

        if (lastFrameIdReceived != 0 && lastFrameIdReceived >= newEntry->data->frameId) {
            printf("dropping packet, Id already encountered and sent to network layer\n");
            return true;
        }

        //also, have I already received this Frame?
        bool currentlySending = false;
        for (int i = 0; i < MAX_WINDOW_SIZE; i++) {
            if (myWindow[i] != NULL) {
                if (myWindow[i]->transType == FN_ToSend)
                    currentlySending = true;
                else if (myWindow[i]->data->frameId == inFrame->frameId)
                    return true; //already received
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
    time_t currentTime = time(NULL) + FRAME_TIMEOUT_IN_MS;
    bool hasFramesInWindow = false;

    for (int i = 0; i < MAX_WINDOW_SIZE; i++) {
        if (myWindow[i] != NULL) {
            hasFramesInWindow = true;
            if (myWindow[i]->timeout < currentTime && myWindow[i]->transType == FN_ToSend) {
                if (myWindow[i]->timeout != 0)
                    printf("Frame %i timed out!\n", myWindow[i]->data->frameId);
                myWindow[i]->timeout = time(NULL) + FRAME_TIMEOUT_IN_MS;
                return myWindow[i]->data;
            }
        }
    }

    FrameWindowEntry *cursor = overflowHead;
    FrameWindowEntry *prevCursor = NULL;

    while (cursor != NULL) {
        if (cursor->data->type == Frame_Ack) {
            Frame *retval = cursor->data;
            if (prevCursor == NULL) {
                overflowHead = cursor->next;
                delete cursor;
            }
            else {
                prevCursor->next = cursor->next;
                delete cursor;
            }
            return retval;
        }
        else {
            prevCursor = cursor;
            cursor = cursor->next;
        }
    }

    if (!hasFramesInWindow) {
        if (!moveFramesIntoWindow())
            return NULL;
        else
            return getFrameToSend();
    }

    return NULL;
}

bool SlidingWindow::moveFramesIntoWindow() {
    bool movedDownFrames = false;
    TransmissionType curType = FN_Unknown;
    int lastUsed = -1;

    for (int y = 0; y < MAX_WINDOW_SIZE; y++) {
        for (int x = 1; x < MAX_WINDOW_SIZE; x++) {
            if (myWindow[x-1] == NULL && myWindow[x] != NULL) {
                myWindow[x-1] = myWindow[x];
                myWindow[x] = NULL;
            }
        }
    }
    printf("Shuffled down all necessary frames, current ordering\n");
    for (int y = 0; y < MAX_WINDOW_SIZE; y++) {
        if (myWindow[y] != NULL) {
            printf("(%i)", myWindow[y]->data->frameId);
            lastUsed = y;
        }
    }
    printf("\n");


    for (int i = lastUsed + 1; i < MAX_WINDOW_SIZE; i++) {
        FrameWindowEntry *cursor = overflowHead;
        FrameWindowEntry *prevCursor = NULL;

        while(cursor != NULL) {
            //printf(" cursor trans: %i, matching to type: %i, frame type: %i\n", cursor->transType, curType, cursor->data->type);
            if ((cursor->transType == curType || curType == FN_Unknown) && cursor->data->type != Frame_Ack) {
                printf("Moving frame from buffer into window\n");
                movedDownFrames = true;
                curType = cursor->transType;
                myWindow[i] = cursor;

                if (prevCursor == NULL)
                    overflowHead = cursor->next;
                else
                    prevCursor->next = cursor->next;
                cursor->next = NULL;

                break;
            }
            prevCursor = cursor;
            cursor = cursor->next;
        }
    }
    printf("filled in frames frame buffer\n");

    return movedDownFrames;
}

void SlidingWindow::orderFramesById() {
    int maxFrameId = 0;
    int usedFrames = 0;
    int highestWindowUsed = -1;

    bool inOrder = false;
    printf("List before\n");
    for (int i = 0; i < MAX_WINDOW_SIZE; i++) {
        if (myWindow[i] != NULL)  {
            printf("(%i)", myWindow[i]->data->frameId);
            usedFrames++;
        }
    }
    printf("\n");
    if (usedFrames < MAX_WINDOW_SIZE) {
        printf("Buffer not full, moving frames into window\n");
        moveFramesIntoWindow();
    }



    printf("\nRunning reordering list\n");
    while (!inOrder) {
        inOrder = true;

        for (int i = 0; i < MAX_WINDOW_SIZE; i++) {
            if (myWindow[i] != NULL) {
                highestWindowUsed = i;
                if (myWindow[i]->transType == FN_ToSend) {
                    printf("Send list, doesn't reorder, breaking out\n");
                    return;
                }
                //if (frameId = 0)
                //    frameId = myWindow[i]->data->frameId;

                if (i > 0) {
                    if (myWindow[i-1]->data->frameId > (myWindow[i]->data->frameId)) {
                        printf("Frames out of order, swapping %i with %i\n", myWindow[i-1]->data->frameId, myWindow[i]->data->frameId);
                        FrameWindowEntry *tmp = myWindow[i-1];
                        myWindow[i-1] = myWindow[i];
                        myWindow[i] = tmp;
                        //frameId = myWindow[i]->data->frameId;
                        inOrder = false;
                    }
                    //else
                    //    frameId = myWindow[i]->data->frameId;
                }
                if (myWindow[i]->data->frameId > maxFrameId)
                    maxFrameId = myWindow[i]->data->frameId;
            }
        }
    }
    printf("Done reordering\n");
    for (int i = 0; i < MAX_WINDOW_SIZE; i++) {
        if (myWindow[i] != NULL)
            printf("(%i)", myWindow[i]->data->frameId);
    }
    printf("\n");
    FrameWindowEntry *prevCursor = NULL;
    FrameWindowEntry *cursor = overflowHead;
    inOrder = true;

    if (highestWindowUsed == -1)
        return;

    while (cursor != NULL) {
        if (cursor->transType == FN_ToReceive) {
            if (maxFrameId > cursor->data->frameId) {
                FrameWindowEntry *tmp = myWindow[highestWindowUsed];
                myWindow[highestWindowUsed] = cursor;
                cursor = tmp;
                cursor->next = myWindow[highestWindowUsed]->next;
                myWindow[highestWindowUsed]->next = NULL;

                if (prevCursor == NULL)
                    overflowHead->next = cursor;
                else
                    prevCursor->next = cursor;
                return orderFramesById();
            }
        }
        prevCursor = cursor;
        cursor = cursor->next;
    }
}

FrameNode* SlidingWindow::getFullPacketToReceive() {
    bool foundFirstFrame = false;
    bool foundLastFrame = false;
    bool completeFrame = true;

    int expectedFrameId;
    if (lastFrameIdReceived == 0)
        expectedFrameId = 0;
    else
        expectedFrameId = lastFrameIdReceived + 1;
    orderFramesById();

    printf("Frame ordering: ");
    for (int i = 0; i < MAX_WINDOW_SIZE; i++) {
        if (myWindow[i] != NULL) {
            printf("(%i-", myWindow[i]->data->frameId);
            if (myWindow[i]->transType == FN_ToReceive) {
                if (myWindow[i]->data->isFirstFrame || myWindow[i]->data->finalFrame) {
                    if (myWindow[i]->data->isFirstFrame)
                        foundFirstFrame = true;
                    if (myWindow[i]->data->finalFrame)
                        foundLastFrame = true;

                    if (expectedFrameId != 0)
                        if (myWindow[i]->data->frameId != expectedFrameId) { //final found and is not the expected id
                            //printf(" (received %i expected %i) ", myWindow[i]->data->frameId, expectedFrameId);
                            completeFrame = false;
                        }

                    expectedFrameId = myWindow[i]->data->frameId + 1;
                }
                else if (myWindow[i]->data->frameId == expectedFrameId) {
                    if (myWindow[i]->data->finalFrame) {
                        foundLastFrame = true;
                    }
                    expectedFrameId++;
                }
                else
                    completeFrame = false;
            }
            printf("%s-%s-%s)", (foundFirstFrame ? "T" : "F") , (foundLastFrame ? "T" : "F") , (completeFrame ? "T" : "F"));
        }
        if(foundFirstFrame && foundLastFrame && completeFrame)
            break;
    }
    printf("\n");

    if (foundFirstFrame && foundLastFrame && completeFrame) {
        printf("Found frame list to send\n");
        FrameNode *headptr = NULL;
        FrameNode *cursor = NULL;

        for (int i = 0; i < MAX_WINDOW_SIZE; i++) {
            if (myWindow[i] != NULL) {
                if (myWindow[i]->data->finalFrame) {
                    if (headptr == NULL) {
                        headptr = new FrameNode();
                        headptr->next = NULL;
                        headptr->data = myWindow[i]->data;
                        myWindow[i]->data = NULL;
                        delete myWindow[i];
                        myWindow[i] = NULL;
                        cursor = headptr;
                    }
                    else {
                        cursor->next = new FrameNode();
                        cursor = cursor->next;
                        cursor->next = NULL;
                        cursor->data = myWindow[i]->data;
                        myWindow[i]->data = NULL;
                        delete myWindow[i];
                        myWindow[i] = NULL;
                    }
                    lastFrameIdReceived = cursor->data->frameId;
                    //moveFramesIntoWindow();

                    return headptr;
                }
                else {
                    if (headptr == NULL) {
                        headptr = new FrameNode();
                        headptr->next = NULL;
                        headptr->data = myWindow[i]->data;
                        myWindow[i]->data = NULL;
                        delete myWindow[i];
                        myWindow[i] = NULL;
                        cursor = headptr;
                    }
                    else {
                        cursor->next = new FrameNode();
                        cursor = cursor->next;
                        cursor->next = NULL;
                        cursor->data = myWindow[i]->data;
                        myWindow[i]->data = NULL;
                        delete myWindow[i];
                        myWindow[i] = NULL;
                    }
                }
            }
        }
    }

    return NULL;
}

bool SlidingWindow::addFrameWindowEntryToBuffer(FrameWindowEntry *newEntry) {
    if (overflowHead == NULL) {
        overflowHead = newEntry;
        overflowHead->next = NULL;
        return true;
    }
    FrameWindowEntry *cursor = overflowHead;
    while (cursor->next != NULL) {
        if (newEntry->data->frameId == cursor->data->frameId && newEntry->transType == cursor->transType) //already received this frame
            return true;
        cursor = cursor->next;
    }

    cursor->next = newEntry;
    cursor = cursor->next;
    cursor->next = NULL;
    return true;
}