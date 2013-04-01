

#include <stdio.h>
#include "common.h"
#include "Message.h"
#include "NetworkLayer.h"


int main (int argc, char *argv[]) {
    printf("Starting test\n");

    NetworkLayer *myNL = new NetworkLayer();
    if (!myNL->initialize(DLL_PORT)) {
        delete myNL;
        return -1;
    }

    Message *testMsg = new Message(Message_Join, "this is a test message");
    myNL->sendMessage(testMsg);

    //begin main execution loop
    delete myNL;
    return 1;
}
