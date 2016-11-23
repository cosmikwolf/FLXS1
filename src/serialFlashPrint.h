#include <Arduino.h>
#include <SerialFlash.h>
#include "Print.h"

#ifndef _SerialFlashPrint_h_
#define _SerialFlashPrint_h_

#define BUFFER_SIZE  256

class SerialFlashPrint : public Print {
  public:
    SerialFlashPrint(SerialFlashFile *file);

    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t *buffer, size_t size);
    void writePrepare(uint8_t channel, uint16_t file_size);
    void writeBuf();
    void writeComplete();

  private:
    char buf[BUFFER_SIZE]; //THIS IS PROBABLY A MEMORY LEAK!
    uint16_t _current_byte;
    uint16_t _buffer_count;
    uint16_t _file_size;
    bool    _data_to_write;
    SerialFlashFile * _file;
};

#endif
