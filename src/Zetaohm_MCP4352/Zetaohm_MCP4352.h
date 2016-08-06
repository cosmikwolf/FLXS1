#include <Arduino.h>
#include <SPI.h>//this chip needs SPI

#ifndef _ZETAOHM_MCP4352_H_
#define _ZETAOHM_MCP4352_H_

#if defined(SPI_HAS_TRANSACTION)
	static SPISettings MCP4352_SPI;
#endif


class Zetaohm_MCP4352 {

public:
	Zetaohm_MCP4352();
  void initialize(uint8_t csPin);
  void setResistance(uint8_t port, uint8_t value);
  uint8_t readResistance(uint8_t port);

  uint16_t 	readAddress(byte addr);
  void			writeByte(byte addr, byte data);

private:
    uint8_t 		_cs;

protected:
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		void startTransaction(void)
		__attribute__((always_inline)) {
			#if defined(SPI_HAS_TRANSACTION)
				SPI.beginTransaction(MCP4352_SPI);
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
