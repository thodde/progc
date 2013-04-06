#include "common.h"

int listenForInternalService(int port, const char *serviceName) {
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

int connectToInternalService(int port, const char *serviceName, bool blocking) {
    printf("Connecting to Service '%s' at internal port %i\n", serviceName, port);

    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (port == 0) {
       printf("%s ERROR please enter a valid port\n", serviceName);
       return 0;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("%s ERROR opening socket\n", serviceName);
        return 0;
    }
    /* Blocking/non-blocking request is ignored
    if (!blocking) {
        int x = fcntl(sockfd, F_GETFL, 0);
        fcntl(sockfd, F_SETFL, x | O_NONBLOCK);
    }
    */

    //server = gethostbyname("localhost");
    if (server == NULL) {
        printf("%s ERROR, no such host\n", serviceName);
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    //server->h_addr =
    //bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(port);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        printf("%s ERROR (%i) connecting with message: %s\n", serviceName, errno, strerror(errno));
        return 0;
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
        thisWrite = write(sockfd+totalWritten, stream, length-totalWritten);

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


