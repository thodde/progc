
#include "Message.h"


Message::Message(Message_Type newType, char* newData, int size, unsigned int newId) {
    if (size < 0)
        return;

    messageId = newId;
    type = newType;
    data = new char[size];
    memset(data, '\0', size);
    memcpy(data, newData, size*sizeof(char));
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
    memcpy(&dataLength, cursor, sizeof(dataLength));
    cursor += sizeof(dataLength);
    data = new char[dataLength];
    memset(data, '\0', dataLength);
    memcpy(data, cursor, sizeof(char)*dataLength);
    cursor += sizeof(char)*dataLength;

    return true;
}
