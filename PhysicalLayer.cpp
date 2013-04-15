
#include <math.h>
#include "PhysicalLayer.h"


PhysicalLayer::PhysicalLayer(int newByteErrorRate, int newFrameErrorRate) {
    myActivity = PH_Inactive;
    internalFD = 0;
    externalFD = 0;
    byteErrorRate = newByteErrorRate;
    frameErrorRate = newFrameErrorRate;
    statsFramesSent = 0;
    statsFramesReceived = 0;
    statsBytesSent = 0;
    statsBytesReceived = 0;
    statsFramesDropped = 0;
    statsBytesChanged = 0;

    for (int i = 0; i < MAX_CONNECTIONS; i++)
        clientFDs[i] = NULL;
}

PhysicalLayer::~PhysicalLayer() {
    printf("Runtime Stats\nSent\tFrames: %li Bytes %li\n", statsFramesSent, statsBytesSent);
    printf("Received\tFrames: %li Bytes %li\nErrors\tBytes Changed: %li Frames Dropped: %li\n", statsFramesReceived,
        statsBytesReceived, statsBytesChanged, statsFramesDropped);

    if (externalFD != 0)
        close(externalFD);
    if (internalFD != 0)
        close(internalFD);
    for (int i = 0; i < MAX_CONNECTIONS; i++)
        if (clientFDs[i] != NULL) {
            close(clientFDs[i]->socketFD);
            delete clientFDs[i];
            clientFDs[i] = NULL;
        }
}


bool PhysicalLayer::initialize(int portInternal) {
    printf("Initializing Physical Layer\n");
    internalFD = 0;
    externalFD = 0;

    internalFD = listenForService(portInternal, "Physical Layer", true);
    if (internalFD == 0){
        printf("Error, closing down\n");
        return false;
    }

    return true;
}


bool PhysicalLayer::run() {
    //begin main execution loop
    upBuffer = new char[DEFAULT_BUFFER_SIZE];
    memset(upBuffer, '\0', DEFAULT_BUFFER_SIZE);
    downBuffer = new char[DEFAULT_BUFFER_SIZE];
    memset(downBuffer, '\0', DEFAULT_BUFFER_SIZE);
    char generalBuffer[255];
    int n;
    upBufferUsed = 0;
    downBufferUsed = 0;
    bufferFramesReceived = 0;
    int serializedLength;

    while (internalFD != 0) {
        //TODO rewrite this so that I can check all available sockets
        if  (checkSocketHasData(internalFD)) {
            processUpstreamData();
        }

        //TODO write this to allow simultaneous checking of all sockets
        if (myActivity == PH_Server) {
            if (externalFD != 0) {
                if(checkSocketHasData(externalFD)) {
                    int newsockfd;
                    socklen_t clilen;
                    struct sockaddr_in cli_addr;
                    clilen = sizeof(cli_addr);

                    //TODO allow more than 1 client
                    newsockfd = accept(externalFD, (struct sockaddr *) &cli_addr,&clilen);

                    char buffer[256];
                    bzero(buffer,256);
                    n = read(newsockfd,buffer,255);
                    //TODO change this to finalize handshake?

                    if (n < 0) {
                        printf("ERROR reading from socket\n");
                        return 0;
                    }

                    printf("Here is the message: %s\n",buffer);
                    n = write(newsockfd,"I got your message",18);
                    if (n < 0) {
                        printf("ERROR writing to socket");
                        return 0;
                    }
                    for (int i = 0; i < MAX_CONNECTIONS; i++){
                        if (clientFDs[i] == NULL) {
                            clientFDs[i] = new ClientConnection();
                            clientFDs[i]->socketFD = newsockfd;
                            memset(clientFDs[i]->clientName, '\0', 10);
                            break;
                        }
                    }
                }
            }
            for (int i = 0; i < MAX_CONNECTIONS; i++) {
                if (clientFDs[i] != NULL) {
                    if(checkSocketHasData(clientFDs[i]->socketFD)) {
                        if(!processDownstreamData(clientFDs[i]->socketFD)) {
                            printf("Error while processing incoming data from client: %s.  Closing connection\n", clientFDs[i]->clientName);
                            close(clientFDs[i]->socketFD);
                            delete clientFDs[i];
                            clientFDs[i] = NULL;
                        }
                    }
                }
            }
        }
        if ((myActivity == PH_Client) && (externalFD != 0)) {
            if (checkSocketHasData(externalFD))
                if (!processDownstreamData(externalFD))
                    close(externalFD);
        }
    }
    return true;
}


bool PhysicalLayer::processDownstreamData(int socketId) {
    char generalBuffer[255];
    memset(generalBuffer, '\0', 255);
    int n = read(socketId, generalBuffer, 255);
    if (n == 0) {
        printf("Connection to External lost, exiting...\n");
        return false;
    }
    else {
        statsBytesReceived += n;
        memcpy(downBuffer+downBufferUsed, generalBuffer, n);
        downBufferUsed += n;
        memset(generalBuffer, '\0', MAX_FRAME_SIZE);

        //process only complete messages
        while (downBufferUsed >= MAX_FRAME_SIZE) {
            receivedFrame[bufferFramesReceived++] = new Frame(downBuffer);
            statsFramesReceived++;
            printf("Received From External Frame (%i) with payload (%i)\n", receivedFrame[bufferFramesReceived - 1]->frameId, receivedFrame[bufferFramesReceived - 1]->payloadUsed);

            //shift buffer
            char *tmpBuffer = new char[DEFAULT_BUFFER_SIZE];
            memset(tmpBuffer, '\0', DEFAULT_BUFFER_SIZE);
            memcpy(tmpBuffer, downBuffer + MAX_FRAME_SIZE, (DEFAULT_BUFFER_SIZE-MAX_FRAME_SIZE));
            delete downBuffer;
            downBuffer = tmpBuffer;
            downBufferUsed -= MAX_FRAME_SIZE;
        }

        for (int i = 0; i < bufferFramesReceived; i++) {
            if (receivedFrame[i]->type == Frame_Join && myActivity == PH_Server) {
                printf("assigning name to PH local: %s\n", receivedFrame[i]->sourceName);
                for (int x = 0; x < MAX_CONNECTIONS; x++) {
                    if (clientFDs[x] != NULL) {
                        if (clientFDs[x]->socketFD == socketId)
                            strcpy(clientFDs[x]->clientName, receivedFrame[i]->sourceName);
                    }
                }
            }

            if (internalFD != 0) {
                printf("Forwarding to internal DLL\n");
                char* serializedFrame = receivedFrame[i]->serialize();
                if (!guaranteedSocketWrite(internalFD, serializedFrame, MAX_FRAME_SIZE))
                     printf("ERROR writing to socket");
            }
        }

        for (int i = 0; i < bufferFramesReceived; i++) {
            delete receivedFrame[i];
            receivedFrame[i] = NULL;
        }
        bufferFramesReceived = 0;
    }
    return true;
}

bool PhysicalLayer::processUpstreamData() {
    char generalBuffer[255];
    memset(generalBuffer, '\0', 255);
    int n = read(internalFD, generalBuffer, 255);
    if (n == 0) {
        printf("Connection to Datalink Layer lost, exiting...\n");
        internalFD = 0;
    }
    else {
        memcpy(upBuffer+upBufferUsed, generalBuffer, n);
        upBufferUsed += n;
        memset(generalBuffer, '\0', MAX_FRAME_SIZE);

        //process only complete messages
        while (upBufferUsed >= MAX_FRAME_SIZE) {
            receivedFrame[bufferFramesReceived++] = new Frame(upBuffer);
            printf("Received From Datalink Layer Frame (%i) with payload (%i) and type %i\n",
                receivedFrame[bufferFramesReceived - 1]->frameId, receivedFrame[bufferFramesReceived - 1]->payloadUsed, receivedFrame[bufferFramesReceived-1]->type);

            //shift buffer
            char *tmpBuffer = new char[DEFAULT_BUFFER_SIZE];
            memset(tmpBuffer, '\0', DEFAULT_BUFFER_SIZE);
            memcpy(tmpBuffer, upBuffer + MAX_FRAME_SIZE, (DEFAULT_BUFFER_SIZE-MAX_FRAME_SIZE));
            delete upBuffer;
            upBuffer = tmpBuffer;
            upBufferUsed -= MAX_FRAME_SIZE;
        }

        for (int i = 0; i < bufferFramesReceived; i++) {
            if (receivedFrame[i]->type == Frame_Stack_Control)
                processControlFrame(receivedFrame[i]);
            else {
                if (myActivity == PH_Server) {
                    for (int x = 0; x < MAX_CONNECTIONS; x++) {
                        if (clientFDs[x] != NULL)
                            if (strcmp(clientFDs[x]->clientName, receivedFrame[i]->targetName) == 0) {
                                printf("Forwarding to client %s\n", clientFDs[x]->clientName);
                                char* serializedFrame = receivedFrame[i]->serialize();
                                statsBytesSent += MAX_FRAME_SIZE;
                                statsFramesSent++;
                                if (!unreliableSend(clientFDs[x]->socketFD, serializedFrame, MAX_FRAME_SIZE))
                                     printf("ERROR writing to socket");
                            }
                    }
                }
                else if (myActivity == PH_Client) {
                    if (externalFD != 0) {
                        printf("Forwarding to server\n");
                        char* serializedFrame = receivedFrame[i]->serialize();
                        statsBytesSent += MAX_FRAME_SIZE;
                        statsFramesSent++;
                        if (!unreliableSend(externalFD, serializedFrame, MAX_FRAME_SIZE))
                             printf("ERROR writing to socket");
                    }
                }
            }
        }

        for (int i = 0; i < bufferFramesReceived; i++) {
            delete receivedFrame[i];
            receivedFrame[i] = NULL;
        }
        bufferFramesReceived = 0;
    }
    return true;
}


bool PhysicalLayer::unreliableSend(int socket, char* stream, int length) {
    if (frameErrorRate > 0) {
        int check = rand() % 100;
        if (check < frameErrorRate) {// drop frame
            statsFramesDropped++;
            printf("Shhh....Dropping frame\n");
            return true;
        }
    }

    if (byteErrorRate > 0) {
        int check = rand() % 100;
        if (check < byteErrorRate) {
            int bytesChanges = rand() % length;

            while (bytesChanges > 0) {
                //change it so that we have some safety in our delimiters
                int changedByte = (rand() % (length - 6))+3;
                int newByte = rand() % 255;
                //again, same as above
                if (newByte == FRAME_START_CHAR)
                    newByte++;
                else if (newByte == FRAME_END_CHAR)
                    newByte++;
                else if (newByte == FRAME_ESCAPE)
                    newByte++;

                stream[changedByte] = (char)newByte;
                statsBytesChanged++;

                bytesChanges--;
            }
        }
    }

    return guaranteedSocketWrite(socket, stream, length);
}

bool PhysicalLayer::processControlFrame(Frame *inFrame) {
    if (inFrame == NULL)
        return false;

    if (inFrame->type != Frame_Stack_Control)
        return false;

    printf("Received control frame from application layer\n");

    if (strcmp(inFrame->payload, "listen") >= 0) {
        char *cindex = inFrame->payload + 7;
        int listenPort = atoi(cindex);

        printf("Received command to listen for chat client connections on port %i\n", listenPort);
        externalFD = listenForService(listenPort, "Physical Layer", false);

        if (externalFD == 0) {
            printf("Port not created\n");
            return false;
        }
        myActivity = PH_Server;
    }
    else if (strcmp(inFrame->payload, "connect") >= 0 && (externalFD == 0)) {
        int index = 8;

        while (inFrame->payload[index] != ' ')
            index++;

        char serverName[100];
        memset(serverName, '\0', 100);
        strncpy(serverName, inFrame->payload+8, index-8);
        char *cindex = inFrame->payload+index+1;
        int connectPort = atoi(cindex);

        printf("Received command to connect to chat server %s on port %i\n", serverName, connectPort);
        externalFD = connectToService(connectPort, "Physical Layer", serverName);
        if (externalFD == 0) {
            printf("Port not created\n");
            return false;
        }
        myActivity = PH_Client;

        Frame *joinFrame = new Frame(inFrame->frameId, true, Frame_Join, inFrame->sourceName, (char*)"server", true);
        joinFrame->setPayload(inFrame->sourceName, 10);
        char *serialized = joinFrame->serialize();
        if (!guaranteedSocketWrite(externalFD, serialized, MAX_FRAME_SIZE)) {
            printf("ERROR writing to socket");
            return false;
        }

    }
    return true;
}

int main (int argc, char *argv[]) {
    printf("Starting Physical Layer\n");

    if (argc < 3) {
        printf("Usage: PhysicalLayer.exe ByteErrorRate FrameErrorRate [listening port]\nError rates are out of 100\n");
        return -1;
    }

    int byteErrorRate = atoi(argv[1]);
    int frameErrorRate = atoi(argv[2]);

    PhysicalLayer *myPH = new PhysicalLayer(byteErrorRate, frameErrorRate);
    int portUsed = PH_PORT;


    if (argc > 3) {
        portUsed = atoi(argv[3]);
    }

    if (!myPH->initialize(portUsed)) {
        delete myPH;
        return -1;
    }

    myPH->run();

    delete myPH;
    return 1;
}


