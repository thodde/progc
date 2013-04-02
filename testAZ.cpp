

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

    char *testChar = new char[1024];
    memset(testChar, '\0', 1024);
    strcat(testChar, "this is a test message");
    strcat(testChar, " and another and another and another and another and another and another 1");
    strcat(testChar, " and another and another and another and another and another and another 2");
    strcat(testChar, " and another and another and another and another and another and another 3");
    strcat(testChar, " and another and another and another and another and another and another 4");
    strcat(testChar, " and another and another and another and another and another and another 5");
    strcat(testChar, " and another and another and another and another and another and another 6");
    strcat(testChar, " and another and another and another and another and another and another 7");
    strcat(testChar, " and another and another and another and another and another and another");

    Message *testMsg = new Message(Message_Join, testChar);
    myNL->sendMessage(testMsg);

    //begin main execution loop
    delete myNL;
    return 1;
}
