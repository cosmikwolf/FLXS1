/*
	OLED_SSD1351 - A fast SPI driver for OLED using SSD1351

	Features:
	- Very FAST!, expecially with Teensy 3.x where uses hyper optimized SPI.
	- It uses just 4 or 5 wires.
	- Full SPI Transaction compatible
	- Can use external font by Paul Stoffregen

	Background:
	An existant Adafruit library exists but it's ridicully slow so I decided to read datasheet
	and start over with this one that is much faster.
	The SSD1351 works in SPI Mode3 so it's not so nice if there's other devices sharing
	the same SPI bus!!! You will need an isolator chip (will be described shortly, check wiki soon).

	-------------------------------------------------------------------------------
    Copyright (c) 2015, .S.U.M.O.T.O.Y., coded by Max MC Costa.

    OLED_SSD1351 Library is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OLED_SSD1351 Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
	++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	Special Thanks:
	Thanks to Paul Stoffregen for his beautiful Teensy3 and DMA SPI.
	Thanks to Jnmattern & Marek Buriak for drawArc!
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	Version:
	0.1: Working beta version (only for Teensy 3.1 right now!)
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	BugList of the current version:

	Please report any!

*/
#ifndef _OLED_SSD1351H_
#define _OLED_SSD1351H_

#ifdef __cplusplus

	#include "Arduino.h"
	#include "Print.h"
	#include <limits.h>
	#include "pins_arduino.h"
	#include "wiring_private.h"
	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>
	#include <SPI.h>

	#ifdef __AVR__
		#include <avr/pgmspace.h>
	#elif defined(ESP8266)
		#include <pgmspace.h>
	#elif defined(__SAM3X8E__)
		#include <include/pio.h>
		#define PROGMEM
		#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
		#define pgm_read_word(addr) (*(const unsigned short *)(addr))
		typedef unsigned char prog_uchar;
	#endif

	#include "_settings/OLED_SSD1351_registers.h"
	#include "_settings/OLED_SSD1351_colors.h"
	#include "_settings/OLED_SSD1351_settings.h"

	#ifndef swap
		#define swap(a, b) { typeof(a) t = a; a = b; b = t; }
	#endif

	#if defined(SPI_HAS_TRANSACTION)
		static SPISettings SSD1351_SPI;
	#endif

#endif

#include "../commonFonts/commonFonts.h"

#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)
//ok, code support included
#else
#error this beta version works ONLY with teensy 3.x Please wait for a new beta
#endif

#ifdef __cplusplus

class OLED_SSD1351 : public Print {
 public:

	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		OLED_SSD1351(uint8_t cspin,uint8_t dcpin,uint8_t rstpin=255,uint8_t mosi=11,uint8_t sclk=13);
	#elif defined(__MKL26Z64__)
		OLED_SSD1351(uint8_t cspin,uint8_t dcpin,uint8_t rstpin=255,uint8_t mosi=11,uint8_t sclk=13);
	#else
		OLED_SSD1351(uint8_t cspin,uint8_t dcpin,uint8_t rstpin=255);
	#endif
	void 	begin(void);
	void 	goTo(int x, int y);
	uint8_t	getError(){return _initError;};
	//void 	setAddress(int xs, int xe, int ys, int ye);
	void 	sendData(uint16_t d) { startTransaction(); writecommand_cont(_CMD_WRITERAM); writedata16_last(d); endTransaction(); };
	uint16_t Color565(uint8_t r, uint8_t g, uint8_t b);
	void 	drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
	void 	drawRect(int16_t x,int16_t y,int16_t w,int16_t h,uint16_t color);
  // drawing primitives!
	void 	drawPixel(int16_t x, int16_t y, uint16_t color);
	void 	fillRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color);
	void 	drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
	void 	drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
	void 	fillScreen(uint16_t fillcolor);
	void 	clearScreen() {fillScreen(BLACK);};
	void 	drawMesh(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t spacing,uint16_t color);
	void 	drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
	void 	ellipse(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
	void 	fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
	void 	drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,int16_t x2, int16_t y2, uint16_t color);
	void 	fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,int16_t x2, int16_t y2, uint16_t color);
	void 	drawQuad(int16_t x0, int16_t y0,int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint16_t color);
	void 	fillQuad(int16_t x0, int16_t y0,int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint16_t color);
	void 	drawPolygon(int16_t cx, int16_t cy, uint8_t sides, int16_t diameter, float rot, uint16_t color);
	void 	drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,int16_t radius, uint16_t color);
	void 	fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,int16_t radius, uint16_t color);
	void 	drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap,int16_t w, int16_t h, uint16_t color);
	void 	drawBitmap(int16_t x, int16_t y,const uint8_t *bitmap, int16_t w, int16_t h,uint16_t color, uint16_t bg);
	void 	setCursor(int16_t x, int16_t y);
	void 	setTextColor(uint16_t c);
	void 	setTextColor(uint16_t c, uint16_t bg);
	void 	setTextSize(uint8_t s);
	void 	setTextWrap(boolean w);
	void 	setRotation(uint8_t r);
	int16_t height(void);
	int16_t width(void);
	void 	invert(boolean);
	virtual size_t write(uint8_t);
	void 	drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
	void 	setFont(const common_font_t &f) { _font = &f; }
	void 	setInternalFont(void) { _font = NULL; }
	void 	drawFontChar(unsigned int c);

 protected:
	int16_t 				_width, _height;
	boolean					_portrait;
	int16_t 				_cursor_x, _cursor_y;
	uint8_t 				_textsize,
							_rotation;
	uint16_t 				_textcolor, _textbgcolor;
	boolean 				_wrap;
	const common_font_t *_font;

	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		uint8_t 			pcs_data, pcs_command;
		uint8_t 			_mosi, _sclk;
		uint8_t 			_cs,_rs,_rst;

		void startTransaction(void)
		__attribute__((always_inline)) {
			#if defined(SPI_HAS_TRANSACTION)
				SPI.beginTransaction(SSD1351_SPI);
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

		void writecommand_cont(uint8_t c)
		__attribute__((always_inline)) {
			KINETISK_SPI0.PUSHR = c | (pcs_command << 16) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_CONT;
			waitFifoNotFull();
		}

		void writedata_cont(uint8_t d)
		__attribute__((always_inline)) {
			KINETISK_SPI0.PUSHR = d | (pcs_data << 16) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_CONT;
			waitFifoNotFull();
		}

		void writedata16_cont(uint16_t d)
		__attribute__((always_inline)) {
			KINETISK_SPI0.PUSHR = d | (pcs_data << 16) | SPI_PUSHR_CTAS(1) | SPI_PUSHR_CONT;
			waitFifoNotFull();
		}

		void writecommand_last(uint8_t c)
		__attribute__((always_inline)) {
			uint32_t mcr = SPI0_MCR;
			KINETISK_SPI0.PUSHR = c | (pcs_command << 16) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_EOQ;
			waitTransmitComplete(mcr);
		}

		void writedata_last(uint8_t d)
		__attribute__((always_inline)) {
			uint32_t mcr = SPI0_MCR;
			KINETISK_SPI0.PUSHR = d | (pcs_data << 16) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_EOQ;
			waitTransmitComplete(mcr);
		}

		void writedata16_last(uint16_t d)
		__attribute__((always_inline)) {
			uint32_t mcr = SPI0_MCR;
			KINETISK_SPI0.PUSHR = d | (pcs_data << 16) | SPI_PUSHR_CTAS(1) | SPI_PUSHR_EOQ;
			waitTransmitComplete(mcr);
		}

		void setAddr(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
		__attribute__((always_inline)) {
			writecommand_cont(_CMD_SETCOLUMN); // Column addr set
			writedata_cont(x0);   // XSTART
			writedata_cont(x1);   // XEND
			writecommand_cont(_CMD_SETROW); // Row addr set
			writedata_cont(y0);   // YSTART
			writedata_cont(y1);   // YEND
		}

		void fillData(int16_t val,uint16_t color)
		__attribute__((always_inline)) {
			writecommand_cont(_CMD_WRITERAM);
			do { writedata16_cont(color); } while (--val > 0);
			writecommand_last(_CMD_NOP);
		}

		void Pixel(int16_t x, int16_t y, uint16_t color)
		__attribute__((always_inline)) {
			setAddr(x, y, x, y);
			writecommand_cont(_CMD_WRITERAM);
			writedata16_cont(color);
		}

		void writeData(uint8_t d)
		__attribute__((always_inline)) {
			startTransaction();
			writedata_last(d);
			endTransaction();
		}

		void writeCommand(uint8_t c)
		__attribute__((always_inline)) {
			startTransaction();
			writecommand_last(c);
			endTransaction();
		}

		void setRegister(const byte reg,uint8_t val)
		__attribute__((always_inline)) {
			startTransaction();
			writecommand_cont(reg);
			writedata_last(val);
			endTransaction();
		}

 #endif
 private:
  uint8_t	_initError;
	void 	drawFontBits(uint32_t bits, uint32_t numbits, uint32_t x, uint32_t y, uint32_t repeat);
	void 	plot8points(uint16_t cx, uint16_t cy, uint16_t x, uint16_t y, uint16_t color);
	void 	plot4points(uint16_t cx, uint16_t cy, uint16_t x, uint16_t y, uint16_t color);
	void	drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,uint16_t color);
	void	fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,int16_t delta, uint16_t color);
	void 	drawFastVLineInt(int16_t x, int16_t y, int16_t h, uint16_t color);
	void 	drawFastHLineInt(int16_t x, int16_t y, int16_t w, uint16_t color);
};
#endif // __cplusplus

#endif
