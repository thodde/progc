
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
    listeningSocketFileDescriptor = 0;

    return;
}

PhysicalLayer::~PhysicalLayer() {
    if (listeningSocketFileDescriptor != 0)
        close(listeningSocketFileDescriptor);
}

bool PhysicalLayer::sendFrame(Frame *outFrame) {
    return false;
}

bool PhysicalLayer::receiveData() {
    return false;
}

bool PhysicalLayer::appropriatePort() {
    if (listeningSocketFileDescriptor != 0)
        close(listeningSocketFileDescriptor);

    struct sockaddr_in serv_addr;

    listeningSocketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (listeningSocketFileDescriptor < 0) {
        printf("Physical Layer ERROR opening socket\n");
        return false;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = port;

    if (bind(listeningSocketFileDescriptor, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("Physical Layer ERROR on binding\n");
        return false;
    }

    listen(listeningSocketFileDescriptor, 5);
};


Packet* PhysicalLayer::convertFrameToPacket(Frame *inFrame) {
    //stuff bits

    //cut into XXX length segments, -3?  provided they don't break the EOL identifier
    return NULL;
}


// chars read?  or remaining?
int PhysicalLayer::readPacket(char* inString, Packet *outPacket) {
    return 0;
}


bool PhysicalLayer::beginListening() {
    printf("I'm listening on port: %i\n", port);

    appropriatePort();

    socklen_t clilen;
    struct sockaddr_in cli_addr;
    clilen = sizeof(cli_addr);
    int newRequestSocketFileDescriptor;
    char buffer[1024];
    int charRead;

    while (1) {
        newRequestSocketFileDescriptor = accept(listeningSocketFileDescriptor, (struct sockaddr *) &cli_addr, &clilen);

        if (newRequestSocketFileDescriptor < 0) {
            printf("Physical Layer ERROR on accept\n");
            return false;
        }

        bzero(buffer, 1024);
        charRead = read(newRequestSocketFileDescriptor, buffer, 1024);

        // this portion needs to be rewritten as it is disguising a ACK response

        //process until a complete packet has been received
        //send up to datalink layer only after a full
        if (charRead < 0) {
            printf("Physical Layer ERROR reading from socket\n");
            return false;
        }

        printf("Physical Layer message received: %s\n",buffer);

        //override this here
        charRead = write(newRequestSocketFileDescriptor, "I got your message", 18);

        if (charRead < 0) {
            printf("Physical Layer ERROR writing to socket\n");
            return false;
        }

        close(newRequestSocketFileDescriptor);

    }

    return true;
};

char* PhysicalLayer::stuffBits(char *inStream) {
    //find sequence

    //replace

    //advance

    //find sequence (repeat)

    return NULL;
}

