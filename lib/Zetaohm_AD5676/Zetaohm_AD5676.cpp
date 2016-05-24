/**************************************************************************/
/*! 
    @file     Zetaohm_AD5676.h
    @author   T. Kariya
		@license  BSD (see license.txt)

		@section  HISTORY
    v1.0  - First release
*/
/**************************************************************************/

//#include <spi4teensy3.h>
#include <SPI.h>

#include <Zetaohm_AD5676.h>

/**************************************************************************/
/*! 
    @brief  Instantiates a new AD5676 class
*/
/**************************************************************************/
Zetaohm_AD5676::Zetaohm_AD5676() {
}

/**************************************************************************/
/*! 
    @brief  Setups the HW
*/
/**************************************************************************/

 void Zetaohm_AD5676::begin(uint8_t cs_pin) {
  _cs_pin = cs_pin;
  _ldac_pin = 0;
  pinMode(_cs_pin, OUTPUT);                        // cs_pin is also the SYNC pin
  digitalWriteFast(_cs_pin, HIGH);  //deactivate DAC
  AD5676_SPI = SPISettings(SPI_CLOCK_DIV2, MSBFIRST, SPI_MODE0);

  SPI.begin ();
}

/**************************************************************************/
/*! 
Command Bits
      C3  C2  C1  C0  Description
0x00  0   0   0   0   No operation
0x01  0   0   0   1   Write to Input Register n (where n = 1 to 8, depending on the DAC selected from the address bits in Table 8), dependent on LDAC
0x02  0   0   1   0   Update the DAC register with contents of Input Register n
0x03  0   0   1   1   Write to and update DAC Channel n
0x04  0   1   0   0   Power down/power up the DAC
0x05  0   1   0   1   Hardware LDAC mask register
0x06  0   1   1   0   Software reset (power-on reset)
0x07  0   1   1   1   Internal reference setup register
0x08  1   0   0   0   Set up the DCEN register (daisy-chain enable)
0x09  1   0   0   1   Set up the readback register (readback enable)
0x0A  1   0   1   0   Update all channels of the input register simultaneously with the input data
0x0B  1   0   1   1   Update all channels of the DAC register and input register simultaneously with the input data
0x0C  1   1   0   0   Reserved
0x0D  1   1   1   1   Reserved 

Address Bits
      A3  A2  A1  A0
0x00  0   0   0   0   DAC 0
0x01  0   0   0   1   DAC 1
0x02  0   0   1   0   DAC 2
0x03  0   0   1   1   DAC 3
0x04  0   1   0   0   DAC 4
0x05  0   1   0   1   DAC 5
0x06  0   1   1   0   DAC 6
0x07  0   1   1   1   DAC 7 
*/
/**************************************************************************/
void Zetaohm_AD5676::softwareReset(){
  uint8_t buffer[3];
  waitFifoEmpty();
  startTransaction();
  digitalWriteFast(_cs_pin, LOW);  // Begin transmission, bring SYNC line low
  buffer[0] = (0x06 << 4) + 0x00;
  buffer[1] = 0x00;
  buffer[2] = 0x00;
  //spi4teensy3::send(buffer, 3 );  //  FRAME 1 COMMAND BYTE - Command + DAC Address (C3 C2 C1 C0 A3 A2 A1 A0)
  SPI.transfer(buffer, 3);
  digitalWriteFast(_cs_pin, HIGH);  // End transmission, bring SYNC line high
  endTransaction();
}

void Zetaohm_AD5676::internalReferenceEnable(bool enable)
{
  waitFifoEmpty();
  startTransaction();
  digitalWriteFast(_cs_pin, LOW);  // Begin transmission, bring SYNC line low

  SPI.transfer( (0x07 << 4) + 0x00 );   //  FRAME 1 COMMAND BYTE - Command + DAC Address (C3 C2 C1 C0 A3 A2 A1 A0)
  SPI.transfer(0x00);                   //  FRAME 2 MOST SIGNIFICANT DATA BYTE

  //spi4teensy3::send( (0x07 << 4) + 0x00 );   //  FRAME 1 COMMAND BYTE - Command + DAC Address (C3 C2 C1 C0 A3 A2 A1 A0)
  //spi4teensy3::send(0x00);                   //  FRAME 2 MOST SIGNIFICANT DATA BYTE
  if (enable) {
    SPI.transfer(0x04);           //  FRAME 3 LEAST SIGNIFICANT DATA BYTE  
    //spi4teensy3::send(0x00);           //  FRAME 3 LEAST SIGNIFICANT DATA BYTE  
  } else {
    SPI.transfer(0x01);           //  FRAME 3 LEAST SIGNIFICANT DATA BYTE  
    //spi4teensy3::send(0x01);           //  FRAME 3 LEAST SIGNIFICANT DATA BYTE  
  }

  digitalWriteFast(_cs_pin, HIGH);  // End transmission, bring SYNC line high
  endTransaction();

}

void Zetaohm_AD5676::setVoltage( uint8_t dac, uint16_t output )
{
  uint8_t buffer[3];
  waitFifoEmpty();
  startTransaction();
  digitalWriteFast(_cs_pin, LOW);  // Begin transmission, bring SYNC line low

  buffer[0] = (0x03 << 4) + dac;
  buffer[1] = output / 256;
  buffer[2] = output % 256;
 // spi4teensy3::send(buffer, 3 );  //  FRAME 1 COMMAND BYTE - Command + DAC Address (C3 C2 C1 C0 A3 A2 A1 A0)
  SPI.transfer(buffer, 3 );  //  FRAME 1 COMMAND BYTE - Command + DAC Address (C3 C2 C1 C0 A3 A2 A1 A0)

  digitalWriteFast(_cs_pin, HIGH);  // End transmission, bring SYNC line high
  digitalWriteFast(_cs_pin, LOW);  // Begin transmission, bring SYNC line low

  buffer[0] = (0x03 << 4) + dac;
  buffer[1] = output / 256;
  buffer[2] = output % 256;
 // spi4teensy3::send(buffer, 3 );  //  FRAME 1 COMMAND BYTE - Command + DAC Address (C3 C2 C1 C0 A3 A2 A1 A0)

  SPI.transfer(buffer, 3 );  //  FRAME 1 COMMAND BYTE - Command + DAC Address (C3 C2 C1 C0 A3 A2 A1 A0)

  digitalWriteFast(_cs_pin, HIGH);  // End transmission, bring SYNC line high

  endTransaction();

 //     digitalWrite(_ldac_pin, HIGH); 
 //
 // digitalWrite(_ldac_pin, LOW); 
 // digitalWrite(_ldac_pin, HIGH); 
 // delay(1);
 // digitalWriteFast(_cs_pin, LOW);  // Begin transmission, bring SYNC line low
 // buffer[1] = 0xFF;
 // buffer[2] = 0xFF;
 //   buffer[0] = (0x02 << 4) + dac;
 //     spi4teensy3::send(buffer, 3 );  //  FRAME 1 COMMAND BYTE - Command + DAC Address (C3 C2 C1 C0 A3 A2 A1 A0)
 //
 // digitalWriteFast(_cs_pin, HIGH);  // End transmission, bring SYNC line high

}