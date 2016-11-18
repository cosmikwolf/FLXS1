#include <Arduino.h>
#include <SerialFlash.h>
#include <Print.h>

#ifndef _ChunkPrint_h_
#define _ChunkPrint_h_

class ChunkPrint : public Print {
  public:
      ChunkPrint(SerialFlashFile * file)
      : _file(file), _current_byte(0) {}

      virtual size_t write(uint8_t c) {
        if(_current_byte == 0){
          _file->erase();
          _file->seek(0);
        }
        sprintf(buf, "%c", c);
        _file->write(buf, 1);
        _current_byte++;
        return 0;
      }

      virtual size_t write(const uint8_t *buffer, size_t size){
        _file->erase();
        _file->seek(0);
        _file->write(buffer, size);
        _file->write(NULL, 1);
        return 0;
      };

 private:
  char buf[1];
  uint16_t _current_byte;
  SerialFlashFile * _file;
};

#endif
