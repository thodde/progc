///////////////////////////////////////////////////////////////////////////
///
/// @Project: WPIRC v1.0
/// @File: README
/// @Authors: Trevor Hodde, James Forkey, Peiwei Ge
/// @Copyright: 2013, All rights reserved.
///
///////////////////////////////////////////////////////////////////////////

A C++ chat client/server program that implements the sliding window protocol 
with a size of 4. 


***Instructions to use by script:***

Launch the StartupLayers.sh script with:

        ./ServerStartup.sh

    In a separate terminal:
        ./ClientStartup.sh

***Instructions to use by manually launching layers:***

    Server
        ./PhysicalLayer.exe A B C
            Where:
                A is the error rate of dropped frames as a percentage
                    valid entries are integers: 0-100
                B is the error rate of changed bytes as a percentage
                    valid entries are integers: 0-100
                C  is the port the physical layer should listen on, eg 2001
                    valid entries are integers: 1025-66535
    
        ./DatalinkLayer.exe D C
            Where:
                D is the port the data link layer will listen on (can not be the same as C)
                    valid entries are integers: 1025-66535
                C is the port the data link layer will transmit data to the physical layer on (Must be the same as C from previous command)
                    valid entries are integers: 1025-66535

        ./ChatServer.exe D
            Where:
                D is the port the data link layer is listening on (Must be the same as D from previous command)
                    valid entries are integers: 1025-66535

    Client
        ./PhysicalLayer.exe A B C
            Where:
                A is the error rate of dropped frames as a percentage
                    valid entries are integers: 0-100
                B is the error rate of changed bytes as a percentage
                    valid entries are integers: 0-100
                C  is the port the physical layer should listen on, eg 2001
                    valid entries are integers: 1025-66535
    
        ./DatalinkLayer.exe D C
            Where:
                D is the port the data link layer will listen on (can not be the same as C)
                    valid entries are integers: 1025-66535
                C is the port the data link layer will transmit data to the physical layer on(Must be the same as C from previous command)
                    valid entries are integers: 1025-66535
        
        ./ChatClient.exe E  D
            Where:
                E is the IP Address of the server machine.
                    Supports hostname or IP address of the server machine
                D is the port the data link layer is listening on (Must be the same as D from previous command)
                    valid entries are integers: 1025-66535

***Supported Commands***

*** help 
         Display a list of available commands.
         Example:
            help 
**  join 
        Command used to join a server.
        Example: 
            join Trevor
    
**  kick
        Forcibly remove a user from the chat room by specifying their username.
        Example:
            kick James
**  speak    
        Send a message to all users in the chat room.
        Example:
            speak Hello fellow chat room users!
**  whisper
        Send a message to a single user.  No other users in the chat room will be able
        to see this message.  It is exclusive to you and the recipient.
        Example:
            whisper Peiwei Hi Peiewi! How are you?
**  list
        Display a list of all users in the chat room.
        Example:
            list
**  sendfile
	Allows the users to exchange files.
	Example:
		sendfile /home/Desktop/test.txt
**  quit
        Allow a user to gracefully exit the chat room.
        Example:
            quit
 
***Extras***

 Default Values

        Physical layer listening port - 2001
        Data link layer listening port - 2002
Changes
       1)  We removed the admin command and the op flag.  Anyone can kick a user.
       2)  We did not use a Go-Back-N sliding window, instead we decided to
           implement a selective repeat sliding sliding window.
