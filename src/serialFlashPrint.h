#include <Arduino.h>
#include <SerialFlash.h>
#include <Print.h>

#ifndef _SerialFlashPrint_h_
#define _SerialFlashPrint_h_

class SerialFlashPrint : public Print {
  public:
    SerialFlashPrint(SerialFlashFile * file);

    virtual size_t write(uint8_t c);
    virtual size_t write(const uint8_t *buffer, size_t size);

  private:
    char buf[1];
    uint16_t _current_byte;
    SerialFlashFile * _file;
};

#endif
