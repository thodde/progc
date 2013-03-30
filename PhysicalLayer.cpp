
#include "PhysicalLayer.h"

/*
bool sendFrame(Frame *outFrame) {
    return true;
};
 */


PhysicalLayer::PhysicalLayer(int newPort, DatalinkLayer *newDLL) {
    printf("Hello world!  I'm a Physical Layer\n");
    port = newPort;
    myDLL = *newDLL;

    return;
}

bool PhysicalLayer::sendFrame(Frame *outFrame) {
    return false;
}

bool PhysicalLayer::receiveData() {
    return false;
}


void PhysicalLayer::convertFrameToPacket(Frame *inFrame) {
    return;
}


bool PhysicalLayer::beginListening() {
    printf("I'm listening on port: %i\n", port);

    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("Physical Layer ERROR opening socket\n");
        return false;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = port;

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("Physical Layer ERROR on binding\n");
        return false;
    }

    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

    if (newsockfd < 0) {
        printf("Physical Layer ERROR on accept\n");
        return false;
    }

    bzero(buffer,256);
    n = read(newsockfd,buffer,255);

//process until a complete packet has been received
//send up to datalink layer only after a full
    if (n < 0) {
        printf("Physical Layer ERROR reading from socket\n");
        return false;
    }

    printf("Physical Layer message received: %s\n",buffer);

//override this here
    n = write(newsockfd,"I got your message",18);

    if (n < 0) {
        printf("Physical Layer ERROR writing to socket\n");
        return false;
    }


    close(newsockfd);
    close(sockfd);
    return 0;
};

