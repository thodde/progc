#include "common.h"

int listenForService(int port, const char *serviceName, bool waitForConnection) {
    printf("Starting to listen on port %i for internal service '%s'\n", port, serviceName);

    int sockfd, newsockfd;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    if (port == 0) {
        printf("%s ERROR, no port provided\n", serviceName);
        return 0;
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        printf("%s ERROR opening socket\n", serviceName);
        return 0;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("%s ERROR on binding\n", serviceName);
        return 0;
    }

    listen(sockfd,5);
    clilen = sizeof(cli_addr);

    if (!waitForConnection)
        return sockfd;

    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,&clilen);
    if (newsockfd < 0) {
        printf("%s ERROR on accept\n", serviceName);
        return 0;
    }

    bzero(buffer,256);
    n = read(newsockfd,buffer,255);

    if (n < 0) {
        printf("%s ERROR reading from socket\n", serviceName);
        return 0;
    }

    printf("Here is the message: %s\n",buffer);
    n = write(newsockfd,"I got your message",18);
    if (n < 0) {
        printf("%s ERROR writing to socket", serviceName);
        return 0;
    }

    return newsockfd;
}

/*
struct addrinfo {
    int     ai_flags;
    int     ai_family;
    int     ai_socktype;
    int     ai_protocol;
    size_t  ai_addrlen;
    struct  sockaddr *ai_addr;
    char    *ai_canonname;     /* canonical name */
//    struct  addrinfo *ai_next; /* this struct can form a linked list */
//};


int connectToService(int port, const char *serviceName, const char *serverName) {
    if (strcmp(serverName, "localhost") >= 0)
        printf("Connecting to Service '%s' at internal port %i\n", serviceName, port);
    else
        printf("Connecting to Service '%s' at host %s on port %i\n", serviceName, serverName, port);


    int sockfd, n;

    char buffer[256];
    if (port == 0) {
       printf("%s ERROR please enter a valid port\n", serviceName);
       return 0;
    }

    if (strcmp(serverName, "localhost") >= 0) {
        struct sockaddr_in serv_addr;
        bzero((char *) &serv_addr, sizeof(serv_addr));

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            printf("%s ERROR opening socket\n", serviceName);
            return 0;
        }
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(port);

        if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
            printf("%s ERROR (%i) connecting with message: %s\n", serviceName, errno, strerror(errno));
            return 0;
        }
    }
    else {
        struct sockaddr_in serv_addr;
        bzero((char *) &serv_addr, sizeof(serv_addr));

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            printf("%s ERROR opening socket\n", serviceName);
            return 0;
        }
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;

        if(inet_pton(AF_INET, serverName, &serv_addr.sin_addr)<=0) {
            printf("\n inet_pton error occured\n");
            return 0;
        }

        serv_addr.sin_port = htons(port);

        if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
            printf("%s ERROR (%i) connecting with message: %s\n", serviceName, errno, strerror(errno));
            return 0;
        }
    }

    bzero(buffer,256);
    sprintf(buffer, "checking in");
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) {
         printf("%s ERROR writing to socket\n", serviceName);
         return 0;
    }

    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) {
         printf("%s ERROR reading from socket\n", serviceName);
         return 0;
    }
    return sockfd;
}

bool guaranteedSocketWrite(int sockfd, char *stream, int length) {
    if (length <= 0)
        return false;

    int thisWrite = 0;
    int totalWritten = 0;

    while (totalWritten < length) {
        thisWrite = write(sockfd, stream+totalWritten, length-totalWritten);

        if (thisWrite < 0) //error while writing
            return false;
        else if (thisWrite == 0) //socket closed?  or is it just in use?  or maybe just full?
            return false;

        totalWritten += thisWrite;

        if (totalWritten > length) //error wrote too much
            return false;
    }

    return true;
}

bool checkSocketHasData(int sockfd) {
    if (sockfd <= 0)
        return false;

    struct timeval tv;
    fd_set readfds;
    int socketsReady;
    //timeout of 1/2 a second
//    tv.tv_sec = 1;
//    tv.tv_usec = 500000;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;
    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);
    socketsReady = select(sockfd+1, &readfds, NULL, NULL, &tv);

    if (socketsReady > 0)
        if (FD_ISSET(sockfd, &readfds))
            return true;

    return false;
}


