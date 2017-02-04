/*
 * This is the definition of the Message class whose instance (object) is going
 * to be serialized and sent via the serial connection.
 */

#include "Message.h"

int Message::getDataSize() {
  int size = SerializationTypes::SIZEOF_INTEGER + strlen(message);
  size += SerializationTypes::SIZEOF_FLOAT;
  return size;
}

void Message::readData(SerializationInputStream& input) {
  input.readString(message);
  pi = input.readFloat();
}

void Message::writeData(SerializationOutputStream& output) {
  output.writeString(message);
  output.writeFloat(pi);
}
