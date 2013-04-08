

#include <stdio.h>
#include "common.h"
#include "Message.h"
#include "NetworkLayer.h"


int main (int argc, char *argv[]) {
    printf("Starting test\n");

    int dllPort = DLL_PORT;

    if (argc > 1)
        dllPort = atoi(argv[1]);

    NetworkLayer *myNL = new NetworkLayer();
    if (!myNL->initialize(dllPort)) {
        delete myNL;
        return -1;
    }

    char *testChar = new char[2048];
    memset(testChar, '\0', 2048);
    strcat(testChar, "What's this? There are two types of Internet sockets? Yes. Well, no. I'm lying. There are more, but I didn't want to scare you.");
    strcat(testChar, "I'm only going to talk about two types here. ");
    strcat(testChar, "Except for this sentence, where I'm going to tell you that \"Raw Sockets\" are also very powerful and you should look them up.");
    strcat(testChar, "All right, already. What are the two types? One is \"Stream Sockets\"; the other is \"Datagram Sockets\", which may hereafter be referred to as ");
    strcat(testChar, "\"SOCK_STREAM\" and \"SOCK_DGRAM\", respectively. Datagram sockets are sometimes called \"connectionless sockets\". (Though they can be connect()'d if you really want. ");
    strcat(testChar, "See connect(), below.  Stream sockets are reliable two-way connected communication streams. If you output two items into the socket in the order \"1, 2\", they will arrive in ");
    strcat(testChar, "the order \"1, 2\" at the opposite end. They will also be error-free. I'm so certain, in fact, they will be error-free, that I'm just going to put my fingers in my ears and chant ");
    strcat(testChar, "la la la la if anyone tries to claim otherwise. What uses stream sockets? Well, you may have heard of the telnet application, yes? It uses stream sockets. ");

    //printf("Sending body of size (%i): \n%s\n", (int)strlen(testChar), testChar);

    unsigned int messagesSent = 0;

    Message *testMsg;
    //Message *testMsg = new Message(Message_Join, testChar, strlen(testChar), messagesSent++);
    //myNL->sendMessage(testMsg);

    Message *response = NULL;

    //printf("Waiting for response\n");
    bool hasError = false;

    bool done = false;
    bool waitingForResponse = false;
    char input;

    while (!done && !hasError) {
        if (waitingForResponse) {
            response = myNL->checkForMessages(hasError);

            if (response != NULL) {
                waitingForResponse = false;
                printf("successfully received message: \n%s\n", response->data);
            }
        }
        else {
            if (input != '\n')
                printf("Input (q - quit, r - send message, c - send connect message to PH, l - send listen message to PH, m - query for messages):\n");

            scanf("%c", &input);
            printf("\n");
            if (input == 'r') {
                testMsg = new Message(Message_Join, testChar, strlen(testChar), messagesSent++);
                myNL->sendMessage(testMsg);
                waitingForResponse = true;
                printf("\nSending\n");
            }
            else if (input == 'q')
                done = true;
            else if (input == 'c') {
                testMsg = createConnectToServerMessage((char*)"localhost", 2666);
                myNL->sendMessage(testMsg);
                printf("\nSent connect message\n");
            }
            else if (input == 'l') {
                testMsg = createListenForClientsMessage(2666);
                myNL->sendMessage(testMsg);
                printf("\nSent listen message\n");
            }
            else if (input == 'm') {
                response = myNL->checkForMessages(hasError);
                if (response != NULL) {
                    waitingForResponse = false;
                    printf("successfully received message: \n%s\n", response->data);
                }
            }
        }
    }

    //begin main execution loop
    delete myNL;
    return 1;
}
