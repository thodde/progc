
#include "Message.h"


Message::Message(Message_Type newType, char* newData) {
    type = newType;
    data = newData;
    dataLength = strlen(data);
};


char* Message::serialize() {
    int messageSize = MESSAGE_HEADER_SIZE + dataLength;
    char* outMessage = new char[messageSize];

    //this mechanism doesn't feel right
    memset(outMessage, '\0', messageSize);
    sprintf(outMessage, "%i%i%s", type, dataLength, data);

    return outMessage;
}
