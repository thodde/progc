

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

    /*
    blocking
    #include <fcntl.h>
    If you don't want a socket to be blocking, you have to make a call to fcntl()
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    selecting
    select() gives you the power to monitor several sockets at the same time. It'll tell you which ones are ready for reading, which are ready for writing, and which sockets have raised exceptions,
    if you really want to know that.

    This being said, in modern times select(), though very portable, is one of the slowest methods for monitoring sockets. One possible alternative is libevent, or something similar, that encapsulates all the
    system-dependent stuff involved with getting socket notifications.

    Without any further ado, I'll offer the synopsis of select():

    #include <sys/time.h>
    #include <sys/types.h>
    #include <unistd.h>

    int select(int numfds, fd_set *readfds, fd_set *writefds,
               fd_set *exceptfds, struct timeval *timeout);

    When select() returns, readfds will be modified to reflect which of the file descriptors you selected which is ready for reading. You can test them with the macro FD_ISSET(), below.

    Before progressing much further, I'll talk about how to manipulate these sets. Each set is of the type fd_set. The following macros operate on this type:

Finally, what is this weirded out struct timeval? Well, sometimes you don't want to wait forever for someone to send you some data. Maybe every 96 seconds you want to print "Still Going..." to the terminal even though nothing has happened. This time structure allows you to specify a timeout period. If the time is exceeded and select() still hasn't found any ready file descriptors, it'll return so you can continue processing.


 The following code snipped waits 2.5 seconds for something to appear on standard input:
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define STDIN 0  // file descriptor for standard input

int main(void)
{
    struct timeval tv;
    fd_set readfds;

    tv.tv_sec = 2;
    tv.tv_usec = 500000;

    FD_ZERO(&readfds);
    FD_SET(STDIN, &readfds);

    // don't care about writefds and exceptfds:
    select(STDIN+1, &readfds, NULL, NULL, &tv);

    if (FD_ISSET(STDIN, &readfds))
        printf("A key was pressed!\n");
    else
        printf("Timed out.\n");

    return 0;
}

What happens if a socket in the read set closes the connection? Well, in that case, select() returns with that socket descriptor set as "ready to read". When you actually do recv() from it, recv() will return 0. That's how you know the client has closed the connection.

One more note of interest about select(): if you have a socket that is listen()ing, you can check to see if there is a new connection by putting that socket's file descriptor in the readfds set.


    RECV notes
    recv() returns the number of bytes actually read into the buffer, or -1 on error (with errno set, accordingly.)

    Wait! recv() can return 0. This can mean only one thing: the remote side has closed the connection on you! A return value of 0 is recv()'s way of letting you know this has occurred.


    // lose the pesky "Address already in use" error message
    if (setsockopt(listener,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

    IP address conversion
    Instead of gethostbyname(), use the superior getaddrinfo().

    struct sockaddr_in sa; // IPv4
    inet_pton(AF_INET, "192.0.2.1", &(sa.sin_addr)); // IPv4
    */

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

    printf("Sending: %s\n", testChar);

    Message *testMsg = new Message(Message_Join, testChar, strlen(testChar));
    myNL->sendMessage(testMsg);

    Message *response = NULL;

    printf("Waiting for response\n");

    while (response == NULL) {
        response = myNL->checkForMessages();
    }

    if (response != NULL)
        printf("successfully received message: %s\n", response->data);


    //begin main execution loop
    delete myNL;
    return 1;
}
