#include "Zetaohm_MAX7301.h"
#include <SPI.h>//this chip needs SPI

uint8_t portConfig[7] = {0,0,0,0,0,0,0};


Zetaohm_MAX7301::Zetaohm_MAX7301(){
}

void Zetaohm_MAX7301::begin(uint8_t csPin) {
  Serial.println("=======initializing max7301 cspin: " + String(csPin));
  _spiTransactionsSpeed = 36000000;
  _cs = csPin;
  //	postSetup(csPin);
	MAX7301_SPI = SPISettings(24000000, MSBFIRST, SPI_MODE0);//mode3
  skipRiseCount = 0;
	SPI.begin();
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE0);

	pinMode(_cs, OUTPUT);
	digitalWrite(_cs, HIGH);
	delay(100);
	fellBuffer = 0x0;
	roseBuffer = 0x0;
	// disable shutdown so chip works!
	writeByte(0x04, 0x01);
}

void Zetaohm_MAX7301::initPort(uint8_t index, uint8_t port, uint16_t mode) {
  uint8_t setting = 0;
  uint8_t offset = port-4*floor(port/4);
  uint8_t portConfigIndex = floor(port/4);

  switch(mode){
    case INPUT:
      setting = 0x2;
      break;
    case OUTPUT:
      setting = 0x1;
      break;
    default: //input pullup
      setting = 0x3;
      break;
  }

  indexMap[index] = port;
  indexCache[index] = 0;

  portConfig[portConfigIndex] &= uint8_t(~(0x3 << offset*2) ) ;   // clear existing setting
  portConfig[portConfigIndex] |= setting << offset*2;        // set mode at correct offset

  //Serial.println("Port Config Index: " + String(portConfigIndex) + "\tsetting: " + String(setting) + "\tvalue: " + String(portConfig[portConfigIndex], HEX) + "\toffset: " + String(offset) + "\tclear: 0b" + String(uint8_t(~(0x3 << offset*2) ), BIN) + "\tassignment: 0x" + String(setting << offset, HEX));
};

void Zetaohm_MAX7301::updateGpioPinModes(){
  Serial.println("==== Setting GPIO Pinmodes ====");
  for (int i=0; i<7; i++){
    writeByte(0x09 + i, portConfig[i]);
    Serial.print("Address: 0x");
    Serial.print(0x09 + i, HEX);
    Serial.print("\tConfig:");
    Serial.println(portConfig[i], HEX);
  }

/*	if(mode == INPUT){
  		writeByte(0x09, 0xAA);
  		writeByte(0x0A, 0xAA);
  		writeByte(0x0B, 0xAA);
  		writeByte(0x0C, 0xAA);
  		writeByte(0x0D, 0xAA);
  		writeByte(0x0E, 0xAA);
  		writeByte(0x0F, 0xAA);
	} else if (mode == OUTPUT) {
  		writeByte(0x09, 0x55);
  		writeByte(0x0A, 0x55);
  		writeByte(0x0B, 0x55);
  		writeByte(0x0C, 0x55);
  		writeByte(0x0D, 0x55);
  		writeByte(0x0E, 0x55);
  		writeByte(0x0F, 0x55);
	} else { // INPUT_PULLUP
  		writeByte(0x09, 0xFF);
  		writeByte(0x0A, 0xFF);
  		writeByte(0x0B, 0xFF);
  		writeByte(0x0C, 0xFF);
  		writeByte(0x0D, 0xFF);
  		writeByte(0x0E, 0xFF);
  		writeByte(0x0F, 0xFF);
	}*/
}

void Zetaohm_MAX7301::setSPIspeed(uint32_t spispeed){
	_spiTransactionsSpeed = spispeed;
}


uint16_t Zetaohm_MAX7301::readAddress(byte addr){
	waitFifoEmpty();
	startTransaction();
	digitalWriteFast(_cs, LOW);

	SPI.transfer( addr | 0x80);
	SPI.transfer(0x00);

	digitalWriteFast(_cs, HIGH);
	digitalWriteFast(_cs, LOW);

	byte low_byte  = SPI.transfer(0x00);
	byte high_byte = SPI.transfer(0x00);

	digitalWriteFast(_cs, HIGH);
	endTransaction();

	uint16_t temp = low_byte | (high_byte << 8);
	return temp;
}

uint16_t Zetaohm_MAX7301::writeByte(byte addr, byte data){
  uint16_t returnData = 0;
  waitFifoEmpty();
	startTransaction();

	digitalWriteFast(_cs, LOW);
	SPI.transfer(addr & 0x7F);
	SPI.transfer(data);
	digitalWriteFast(_cs, HIGH);

	digitalWriteFast(_cs, LOW);
	returnData = SPI.transfer(0x00) << 8;
	returnData = returnData | SPI.transfer(0x00);
	digitalWriteFast(_cs, HIGH);

	endTransaction();

  return returnData;
}



void Zetaohm_MAX7301::update(){
	// load the 32 bit integer with the status of all buttons.
	uint32_t previousState = inputBuffer;

	if (debounceTimer > DEBOUNCE_THRESHOLD){
		inputBuffer =
			~( (readAddress(0x44) >> 8 )
			|  (readAddress(0x4C) & 0xFF00)
			| ((readAddress(0x54) & 0xFF00) << 8 )
			| ((readAddress(0x5C) & 0xFF00) << 16 ) );

		fellBuffer = (inputBuffer & ~previousState) | fellBuffer;
		roseBuffer = (~inputBuffer & previousState) | roseBuffer;
		if (previousState != inputBuffer){
		//	Serial.println("debounce timer: " + String(debounceTimer) );
			debounceTimer = 0;
		}
	}
};

void Zetaohm_MAX7301::clearBuffers(){
  // fellBuffer = 0x0;
  // roseBuffer = 0x0;
  // inputBuffer = 0x0;
  for(int i = 0; i<32; i++){
    if(this->rose(i)){};
    if(this->fell(i)){};
  }
}

void Zetaohm_MAX7301::skipNextRises(uint8_t count){
  skipRiseCount += count;
}

void Zetaohm_MAX7301::digitalWrite(uint8_t index, bool value){
  //  noInterrupts();
    writeByte(0x24 + indexMap[index], value);
    indexCache[index] = value;

  //  interrupts();
}

void Zetaohm_MAX7301::digitalWritePrint(uint8_t index, bool value){
  //  noInterrupts();
    Serial.println("digitalWrite return: " + String( writeByte(0x24 + indexMap[index], value) , BIN) );
  //  interrupts();
}

uint16_t Zetaohm_MAX7301::digitalRead(uint8_t index){
    uint16_t temp =  readAddress(0x24 + indexMap[index]);
    //Serial.println("READING " + String(index));
    //Serial.println(temp, BIN);
    return temp;
}

bool Zetaohm_MAX7301::fell(uint8_t index){
	if ( fellBuffer & (1 << indexMap[index]) ) {
		fellBuffer = fellBuffer & ~(1 << indexMap[index]) ;
  //  Serial.println("Button Press index: " + String(index) + "\tindexMap: " + String(indexMap[index]));
		return true;
	} else {
		return false;
	}
};

bool Zetaohm_MAX7301::rose(uint8_t index){
  if(skipRiseCount){
    skipRiseCount--;
    return true;
  }
	if ( roseBuffer & (1 << indexMap[index]) ) {
		roseBuffer = roseBuffer & ~(1 << indexMap[index]) ;
		return true;
	} else {
		return false;
	}
};

bool Zetaohm_MAX7301::pressed(uint8_t index){
	if (inputBuffer & (1 << indexMap[index])){
		return true;
	} else {
		return false;
	}
};

bool Zetaohm_MAX7301::cacheCheck(uint8_t index){
  return indexCache[index];
}
