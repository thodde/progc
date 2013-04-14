
#include "Message.h"


Message::Message(Message_Type newType, char* newData, int size, unsigned int newId, char *newSourceName, char *newTargetName) {
    if (size < 0)
        return;

    messageId = newId;
    type = newType;
    data = new char[size];
    memset(data, '\0', size);
    memcpy(data, newData, size*sizeof(char));
    memset(sourceName, '\0', 10);
    memset(targetName, '\0', 10);
    memcpy(sourceName, newSourceName, 10);
    memcpy(targetName, newTargetName, 10);
    dataLength = size;
};

Message::Message(char* stream) {
    deSerialize(stream);
}

char* Message::serialize(int &serializedLength) {
    int messageSize = MESSAGE_HEADER_SIZE + dataLength;
    char* outMessage = new char[messageSize];
    memset(outMessage, '\0', messageSize);
    char* cursor = outMessage;

    memcpy(cursor, &type, sizeof(type));
    cursor += sizeof(type);
    memcpy(cursor, &messageId, sizeof(messageId));
    cursor += sizeof(messageId);
    memcpy(cursor, sourceName, sizeof(char)*10);
    cursor += sizeof(char)*10;
    memcpy(cursor, targetName, sizeof(char)*10);
    cursor += sizeof(char)*10;
    memcpy(cursor, &dataLength, sizeof(dataLength));
    cursor += sizeof(dataLength);
    memcpy(cursor, data, sizeof(char)*dataLength);
    cursor += sizeof(char)*dataLength;

    serializedLength = (int)(cursor - outMessage);  //sizeof(type)+sizeof(messageId)+sizeof(dataLength)+sizeof(char)*dataLength;
    return outMessage;
}

bool Message::deSerialize(char *stream) {
    char *cursor = stream;

    memcpy(&type, cursor, sizeof(type));
    cursor += sizeof(type);
    memcpy(&messageId, cursor, sizeof(messageId));
    cursor += sizeof(messageId);
    memcpy(sourceName, cursor, sizeof(char)*10);
    cursor += sizeof(char)*10;
    memcpy(targetName, cursor, sizeof(char)*10);
    cursor += sizeof(char)*10;
    memcpy(&dataLength, cursor, sizeof(dataLength));
    cursor += sizeof(dataLength);
    data = new char[dataLength];
    memset(data, '\0', dataLength);
    memcpy(data, cursor, sizeof(char)*dataLength);
    cursor += sizeof(char)*dataLength;

    return true;
}


Message* createConnectToServerMessage(char *serverAddress, char *clientName, int serverPort) {
    char msgStr[80];
    memset(msgStr, '\0', 80);
    sprintf(msgStr, "connect %s %i as client name: %s", serverAddress, serverPort, clientName);
    char serverName[25];
    memset(serverName, '\0', 25);
    strcpy(serverName, "server");

    return new Message(Message_Stack_Control, msgStr, strlen(msgStr), 0, clientName, serverName);
}

Message* createListenForClientsMessage(int serverPort) {
    char msgStr[80];
    memset(msgStr, '\0', 80);
    sprintf(msgStr, "listen %i", serverPort);
    char myName[25];
    memset(myName, '\0', 25);
    strcpy(myName, "server");


    return new Message(Message_Stack_Control, msgStr, strlen(msgStr), 0, myName, (char*)"");
}
