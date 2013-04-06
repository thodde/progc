
#include "Message.h"


Message::Message(Message_Type newType, char* newData, int size) {
    if (size < 0)
        return;

/*
    printf("New message uninterpretted:");
    int i;
    for(i = 0; i < size; i++)
        printf("%c", newData[i]);
    printf("\n");
    */

    //no clue, but the error is between uninterpretted and convert to....memcpy?
    type = newType;
    data = new char[size];
    memset(data, '\0', size);
    memcpy(data, newData, size*sizeof(char));
    dataLength = size;
    //printf("final iteration: %i size: %i data length: %i\n", i, size, dataLength);

/*
    printf("convert to message:");
    for(int i = 0; i < dataLength; i++)
        printf("%c", data[i]);
    printf("\n");
    */
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
    memcpy(cursor, &dataLength, sizeof(dataLength));
    cursor += sizeof(dataLength);
    memcpy(cursor, data, sizeof(char)*dataLength);
    cursor += sizeof(char)*dataLength;

/*
    printf("Unedited message prior to conversion:");
    for(int i = 0; i < dataLength; i++)
        printf("%c", data[i]);
    printf("\n");
*/

    serializedLength = sizeof(type)+sizeof(dataLength)+sizeof(char)*dataLength;
/*
    printf("Serialized message:");
    for (int i = 0; i < (MESSAGE_HEADER_SIZE + dataLength); i++)
        printf("%c", outMessage[i]);
    printf("\n");
*/
    return outMessage;
}

bool Message::deSerialize(char *stream) {
    char *cursor = stream;

    memcpy(&type, cursor, sizeof(type));
    cursor += sizeof(type);
    memcpy(&dataLength, cursor, sizeof(dataLength));
    cursor += sizeof(dataLength);
    data = new char[dataLength];
    memset(data, '\0', dataLength);
    memcpy(data, cursor, sizeof(char)*dataLength);
    cursor += sizeof(char)*dataLength;

    return true;
}
