#include "serialFlashPrint.h"

SerialFlashPrint::SerialFlashPrint(SerialFlashFile *file) : Print()
{
  this->_file = file;
  this->_buffer_count = 0;
  this->_current_byte = 0;
};

size_t SerialFlashPrint::write(uint8_t c)
{
  if (_buffer_count == BUFFER_SIZE)
  {
    this->writeBuf();
  };
  buf[_buffer_count] = c;
  _buffer_count++;
  _current_byte++;
  _data_to_write = 1;
  return 0;
};

size_t SerialFlashPrint::write(const uint8_t *buffer, size_t size)
{
  for (int i = 0; i < size; i++)
  {
    buf[_buffer_count] = buffer[i];
    _buffer_count++;
    _current_byte++;
    if (_buffer_count == BUFFER_SIZE)
    {
      if (_data_to_write == 1 && _buffer_count > _file_size)
      {
        return 0;
      };
      this->writeBuf();
    };
  };
  return 0;
};

void SerialFlashPrint::writeBuf()
{
  elapsedMicros timer123 = 0;
  _file->write(buf, BUFFER_SIZE);
  _data_to_write = 0;
  _buffer_count = 0;
  //Serial.println("writeBufTimerMicros: " + String(timer123) + "\t position: " + String(_file->position()));
};

void SerialFlashPrint::writePrepare(uint8_t channel, uint16_t file_size)
{
  _file_size = file_size;
  _file->seek(4096 * channel);
  //_file->erase4k();
  _buffer_count = 0;
};

void SerialFlashPrint::writeComplete()
{
  //Serial.println("_written_size: " + String(_file->position()));
  //Serial.println("_buffer_count: " + String(_buffer_count));
  for (int i = _buffer_count + 1; i < BUFFER_SIZE; i++)
  {
    buf[i] = '\0';
  };
  //free(buf);
  _file->write(buf, _buffer_count);
  _file->close();
};
