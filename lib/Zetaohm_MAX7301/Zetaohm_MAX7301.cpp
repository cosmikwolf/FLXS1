#include "Zetaohm_MAX7301.h"
#include <SPI.h>//this chip needs SPI

max7301::max7301(){
}

max7301::max7301(const uint8_t csPin, uint32_t spispeed){
	_spiTransactionsSpeed = 36000000;
	_cs = csPin;
//	postSetup(csPin);
}


void max7301::begin(bool protocolInitOverride) {

	MAX7301_SPI = SPISettings(30000000, MSBFIRST, SPI_MODE0);//mode3

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


void max7301::setSPIspeed(uint32_t spispeed){
	_spiTransactionsSpeed = spispeed;
}


uint16_t max7301::readAddress(byte addr){
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

void max7301::writeByte(byte addr, byte data){
	startTransaction();
	digitalWriteFast(_cs, LOW);
	SPI.transfer(addr & 0x7F);

	SPI.transfer(data);

	digitalWriteFast(_cs, HIGH);
	digitalWriteFast(_cs, LOW);

	SPI.transfer(0x00);
	SPI.transfer(0x00);
	digitalWriteFast(_cs, HIGH);
	endTransaction();
}

void max7301::gpioPinMode(uint16_t mode){
	if(mode == INPUT){
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
	}
}

void max7301::update(){
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
			Serial.println("debounce timer: " + String(debounceTimer) );
			debounceTimer = 0;
		}
	}
	
};

void max7301::init(uint8_t index, uint8_t pin) {
	indexMap[index] = pin;
};

bool max7301::fell(uint8_t index){
	if ( fellBuffer & (1 << indexMap[index]) ) {
		fellBuffer = fellBuffer & ~(1 << indexMap[index]) ;
		return true;
	} else {
		return false;
	}
};

bool max7301::rose(uint8_t index){
	if ( roseBuffer & (1 << indexMap[index]) ) {
		roseBuffer = roseBuffer & ~(1 << indexMap[index]) ;
		return true;
	} else {
		return false;
	}
};

bool max7301::pressed(uint8_t index){
	if (inputBuffer & (1 << indexMap[index])){
		return true;
	} else {
		return false;
	}
};
