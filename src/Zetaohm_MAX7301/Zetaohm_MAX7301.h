#ifndef _ZETAOHM_MAX7301_H_
#define _ZETAOHM_MAX7301_H_

#include <inttypes.h>
#include <SPI.h>//this chip needs SPI

#define DEBOUNCE_THRESHOLD 	500

#if defined(SPI_HAS_TRANSACTION)
	static SPISettings MAX7301_SPI;
#endif

class Zetaohm_MAX7301 {

public:
	Zetaohm_MAX7301();
	void 	    begin(uint8_t csPin); //protocolInitOverride=true	will not init the SPI
	void			initPort(uint8_t index, uint8_t port, uint16_t mode);		// initialize a new button
	void 			updateGpioPinModes();					//OUTPUT=all out,INPUT=all in,0xxxx=you choose
	uint16_t 	readAddress(byte addr);
	void			digitalWrite(uint8_t index, bool value);
	void			digitalWritePrint(uint8_t index, bool value);
	uint16_t			digitalRead(uint8_t index);
	void			setSPIspeed(uint32_t spispeed);//for SPI trans0actions
	uint16_t			writeByte(byte addr, byte data);
	void			update();								// update the input buffer
	bool			fell(uint8_t index);					// was the button pressed since the last check?
	bool			rose(uint8_t index);					// was the button pressed since the last check?
	bool			pressed(uint8_t index);
	uint32_t		inputBuffer;
	uint32_t		fellBuffer;
	uint32_t		roseBuffer;
	uint8_t			indexMap[32];
	uint8_t			portConfig[7];

	elapsedMicros 	debounceTimer;
protected:
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		void startTransaction(void)
		__attribute__((always_inline)) {
			#if defined(SPI_HAS_TRANSACTION)
				SPI.beginTransaction(MAX7301_SPI);
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

private:
	uint32_t		_spiTransactionsSpeed;//for SPI transactions
    uint8_t 		_cs;
	uint8_t 		_adrs;
};

#endif
