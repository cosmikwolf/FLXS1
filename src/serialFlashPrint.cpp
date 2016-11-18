#include "serialFlashPrint.h"

SerialFlashPrint::SerialFlashPrint(SerialFlashFile * file){
  _file = file;
  _current_byte = 0;
}

size_t SerialFlashPrint::write(uint8_t c) {
  if(_current_byte == 0){
    _file->erase();
    _file->seek(0);
  }
  sprintf(buf, "%c", c);
  _file->write(buf, 1);
  _current_byte++;
  return 0;
}

size_t SerialFlashPrint::write(const uint8_t *buffer, size_t size){
  _file->erase();
  _file->seek(0);
  _file->write(buffer, size);
  _file->write(NULL, 1);
  return 0;
};
