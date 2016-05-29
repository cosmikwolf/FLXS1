/**************************************************************************/
/*! 
    @file     Zetaohm_AD5676.h
    @author   T. Kariya
		@license  BSD (see license.txt)

		@section  HISTORY
    v1.0  - First release
*/
/**************************************************************************/
#ifndef Zetaohm_AD5676_h_
#define Zetaohm_AD5676_h_

//#include <spi4teensy3.h>
#include <SPI.h>

#if defined(SPI_HAS_TRANSACTION)
  static SPISettings AD5676_SPI;
#endif


class Zetaohm_AD5676{
  public:
    Zetaohm_AD5676();
    void begin(uint8_t cs_pin);  
    void setVoltage( uint8_t dac, uint16_t output );
    void softwareReset();
	  void internalReferenceEnable(bool enable);
    void setLDACMaskRegister(byte n);
  private:
    uint8_t _cs_pin;   //AD5676 SYNC line
    uint8_t _ldac_pin;

  protected:
  #if defined(__MK20DX128__) || defined(__MK20DX256__)  
    void startTransaction(void)
    __attribute__((always_inline)) {
      #if defined(SPI_HAS_TRANSACTION)
        SPI.beginTransaction(AD5676_SPI);
      #endif
    }
  
    void endTransaction(void)
    __attribute__((always_inline)) {
      #if defined(SPI_HAS_TRANSACTION)
        SPI.endTransaction();
      #endif
    }
  
    void waitFifoNotFull(void) {
      uint32_t sr;
      uint32_t tmp __attribute__((unused));
      do {
        sr = KINETISK_SPI0.SR;
        if (sr & 0xF0) tmp = SPI0_POPR;  // drain RX FIFO
      } while ((sr & (15 << 12)) > (3 << 12));
    }
  
    void waitFifoEmpty(void) {
      uint32_t sr;
      uint32_t tmp __attribute__((unused));
      do {
        sr = KINETISK_SPI0.SR;
        if (sr & 0xF0) tmp = KINETISK_SPI0.POPR;  // drain RX FIFO
      } while ((sr & 0xF0F0) > 0);// wait both RX & TX empty
    }
  
      void waitTransmitComplete(uint32_t mcr)
      __attribute__((always_inline)) {
        uint32_t tmp __attribute__((unused));
        while (1) {
          uint32_t sr = KINETISK_SPI0.SR;
          if (sr & SPI_SR_EOQF) break;  // wait for last transmit
          if (sr &  0xF0) tmp = KINETISK_SPI0.POPR;
        }
        KINETISK_SPI0.SR = SPI_SR_EOQF;
        SPI0_MCR = mcr;
        while (KINETISK_SPI0.SR & 0xF0) { tmp = KINETISK_SPI0.POPR; }
      }
  
  #endif
  

};

#endif