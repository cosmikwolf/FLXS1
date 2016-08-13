#include "Zetaohm_MCP4352.h"


Zetaohm_MCP4352::Zetaohm_MCP4352(){

};

void Zetaohm_MCP4352::initialize(uint8_t csPin){
  _cs = csPin;
  MCP4352_SPI = SPISettings(10000000, MSBFIRST, SPI_MODE0);//mode3

  SPI.begin();

	pinMode(_cs, OUTPUT);
	digitalWrite(_cs, HIGH);
};

void Zetaohm_MCP4352::setResistance(uint8_t port, uint8_t value){
  uint8_t portAddress = 0x00;
  switch (port) {
    case 0:
      portAddress = 0x00;
    break;
    case 1:
      portAddress = 0x01;
    break;
    case 2:
      portAddress = 0x06;
    break;
    case 3:
      portAddress = 0x07;
    break;
  }

  writeByte(portAddress << 4, value);
};

uint8_t Zetaohm_MCP4352::readResistance(uint8_t port){
  uint8_t portAddress = 0x00;
  switch (port) {
    case 0:
      portAddress = 0x00;
    break;
    case 1:
      portAddress = 0x01;
    break;
    case 2:
      portAddress = 0x06;
    break;
    case 3:
      portAddress = 0x07;
    break;
  }

  readAddress((portAddress << 4) & 0x0C );
};


void Zetaohm_MCP4352::writeByte(byte addr, byte data){
  waitFifoEmpty();
	startTransaction();
	digitalWriteFast(_cs, LOW);

  SPI.transfer(addr);
	SPI.transfer(data);

	digitalWriteFast(_cs, HIGH);
	endTransaction();
}

uint16_t Zetaohm_MCP4352::readAddress(byte addr){
	waitFifoEmpty();
	startTransaction();
	digitalWriteFast(_cs, LOW);

  byte low_byte  = SPI.transfer( addr | 0x80);
  byte high_byte = SPI.transfer(0x00);

	digitalWriteFast(_cs, HIGH);
	endTransaction();

  uint16_t temp = low_byte | (high_byte << 8);
	return temp;
}
