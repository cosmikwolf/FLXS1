
#include "OLED_SSD1351.h"


extern "C" const unsigned char glcdfont[];
/***********************************/


void OLED_SSD1351::goTo(int x, int y) {
	if ((x >= OLED_WIDTH) || (y >= OLED_HEIGHT)) return;
	startTransaction();
	setAddr(x,y,OLED_MW,OLED_MH);
	writecommand_last(_CMD_NOP);
	endTransaction();
}

uint16_t OLED_SSD1351::Color565(uint8_t r, uint8_t g, uint8_t b) {
	uint16_t c;
	c = r >> 3;
	c <<= 6;
	c |= g >> 2;
	c <<= 5;
	c |= b >> 3;
	return c;
}

void OLED_SSD1351::fillScreen(uint16_t fillcolor) {
	fillRect(0, 0, OLED_WIDTH, OLED_HEIGHT, fillcolor);
}

/**************************************************************************/
/*!
    @brief  Draws a filled rectangle using HW acceleration
*/
/**************************************************************************/
void OLED_SSD1351::fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t fillcolor) {
  // Transform x and y based on current rotation.
	switch (_rotation) {
		case 0:  // No rotation
			break;
		case 1:  // Rotated 90 degrees clockwise.
			swap(x, y);
			x = OLED_WIDTH - x - h;
			break;
		case 2:  // Rotated 180 degrees clockwise.
			x = OLED_WIDTH - x - w;
			y = OLED_HEIGHT - y - h;
			break;
		case 3:  // Rotated 270 degrees clockwise.
			swap(x, y);
			y = OLED_HEIGHT - y - w;
			break;
	}
	if (_portrait) swap(w, h);
	//rawFillRect(x, y, w, h, fillcolor);
	// Bounds check
	if ((x >= OLED_WIDTH) || (y >= OLED_HEIGHT)) return;
	// Y bounds check
	if (y+h > OLED_HEIGHT) h = OLED_MH - y;
	// X bounds check
	if (x+w > OLED_WIDTH) w = OLED_MW - x;
	uint16_t ww = w * h;
	startTransaction();
	setAddr(x,y,x+w-1,y+h-1);
	fillData(ww,fillcolor);
	// writecommand_cont(_CMD_WRITERAM); 
	// do { writedata16_cont(fillcolor); } while (--ww > 0);
	// writecommand_last(_CMD_NOP);
	endTransaction();
}



void OLED_SSD1351::drawFastHLineInt(int16_t x, int16_t y, int16_t w, uint16_t color)
{
	// Bounds check
	if ((x >= OLED_WIDTH) || (y >= OLED_HEIGHT)) return;
	// X bounds check
	if (x+w > OLED_WIDTH) w = OLED_MH - x;
	if (w < 0) return;
	startTransaction();
	setAddr(x,y,x+w-1,y);
	fillData(w,color);
	// writecommand_cont(_CMD_WRITERAM);
	// do { writedata16_cont(color); } while (--w > 0);
	// writecommand_last(_CMD_NOP);
	endTransaction();
}



void OLED_SSD1351::drawFastVLineInt(int16_t x, int16_t y, int16_t h, uint16_t color)
{
	// Bounds check
	if ((x >= OLED_WIDTH) || (y >= OLED_HEIGHT)) return;
	// X bounds check
	if (y+h > OLED_HEIGHT) h = OLED_MH - y;
	if (h < 0) return;
	startTransaction();
	setAddr(x,y,x,y+h-1);
	fillData(h,color);
	// writecommand_cont(_CMD_WRITERAM);
	// do { writedata16_cont(color); } while (--h > 0);
	// writecommand_last(_CMD_NOP);
	endTransaction();
}



void OLED_SSD1351::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) 
{
 	switch (_rotation) {
		case 0:  // No rotation
			break;
		case 1:  // Rotated 90 degrees clockwise.
			swap(x, y);
			x = OLED_WIDTH - x - h;
			break;
		case 2:  // Rotated 180 degrees clockwise.
			x = OLED_MW - x;
			y = OLED_HEIGHT - y - h;
			break;
		case 3:  // Rotated 270 degrees clockwise.
			swap(x, y);
			y = OLED_MH - y;
			break;
	}
	if (_portrait){
		drawFastHLineInt(x, y, h, color);
	} else {
		drawFastVLineInt(x, y, h, color);
	}
}




void OLED_SSD1351::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {

	switch (_rotation) {
		case 0:  // No rotation.
			break;
		case 1:  // Rotated 90 degrees clockwise.
			swap(x, y);
			x = OLED_MW - x;
			break;
		case 2:  // Rotated 180 degrees clockwise.
			x = OLED_WIDTH - x - w;
			y = OLED_MH - y;
			break;
		case 3:  // Rotated 270 degrees clockwise.
			swap(x, y);
			y = OLED_HEIGHT - y - w;
			break;
	}
	if (_portrait){
		drawFastVLineInt(x, y, w, color);
	} else {
		drawFastHLineInt(x, y, w, color);
	}
}



void OLED_SSD1351::drawPixel(int16_t x, int16_t y, uint16_t color) 
{
	// Transform x and y based on current rotation.
	switch (_rotation) {
	// Case 0: No rotation
		case 1:  // Rotated 90 degrees clockwise.
			swap(x, y);
			x = OLED_MW - x;
			break;
		case 2:  // Rotated 180 degrees clockwise.
			x = OLED_MW - x;
			y = OLED_MH - y;
			break;
		case 3:  // Rotated 270 degrees clockwise.
			swap(x, y);
			y = OLED_MH - y;
			break;
	}
	// Bounds check.
	if ((x >= OLED_WIDTH) || (y >= OLED_HEIGHT)) return;
	if ((x < 0) || (y < 0)) return;

	startTransaction();
	Pixel(x,y,color);
	endTransaction();
}


void OLED_SSD1351::begin(void) {
	_initError = 0b00000000;
	setTextColor(WHITE);
	_width = OLED_WIDTH;
	_height = OLED_HEIGHT;
	_wrap = true;
	_textsize  = 1;
	_font      = NULL;
	_cursor_x = 0;
	_cursor_y = 0;
	setRotation(0);
    #if defined(__AVR__) 
		SSD1351_SPI = SPISettings(30000000, MSBFIRST, SPI_MODE3);//mode3
		SPI.begin();
		pinMode(_cs, OUTPUT);
		pinMode(_rs, OUTPUT);
		digitalWrite(_cs, HIGH);
		digitalWrite(_rs, HIGH);
	#elif defined(__MK20DX128__) || defined(__MK20DX256__)
		SSD1351_SPI = SPISettings(30000000, MSBFIRST, SPI_MODE3);//mode3
		if ((_mosi == 11 || _mosi == 7) && (_sclk == 13 || _sclk == 14)) {
			SPI.setMOSI(_mosi);
			SPI.setSCK(_sclk);
		} else {
			bitSet(_initError,0);
			return;
		}
		SPI.begin();
		if (SPI.pinIsChipSelect(_cs, _rs)) {
			pcs_data = SPI.setCS(_cs);
			pcs_command = pcs_data | SPI.setCS(_rs);
		} else {
			pcs_data = 0;
			pcs_command = 0;
			bitSet(_initError,1);
			return;
		}
	#endif
	if (_rst && (_initError == 0)) {
		pinMode(_rst, OUTPUT);
		digitalWrite(_rst, HIGH);
		delay(500);
		digitalWrite(_rst, LOW);
		delay(500);
		digitalWrite(_rst, HIGH);
		delay(500);
	}
	
    // Initialization Sequence
    setRegister(_CMD_COMMANDLOCK,0x12);  // set command lock 
    setRegister(_CMD_COMMANDLOCK,0xB1);  // set command lock
    writeCommand(_CMD_DISPLAYOFF);  		// 0xAE
    writeCommand(_CMD_CLOCKDIV);  		// 0xB3
    writeCommand(0xF1);  						// 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
    setRegister(_CMD_MUXRATIO,0x7F);
    setRegister(_CMD_SETREMAP,0x74);

    writeCommand(_CMD_SETCOLUMN);
    writeData(0x00);
    if (OLED_HEIGHT == 96) {
    	writeData(0x60);//96 px
  	} else {
  	  writeData(0x7F);//127
	  }
    writeCommand(_CMD_SETROW);
    writeData(0x00);
    writeData(0x7F);
    setRegister(_CMD_STARTLINE,0x00);
    setRegister(_CMD_DISPLAYOFFSET,0x00); 	// 0xA2
    setRegister(_CMD_SETGPIO,0x00);
    setRegister(_CMD_FUNCTIONSELECT,0x01);// internal (diode drop)
    writeCommand(_CMD_PRECHARGE);  		// 0xB1
    writeCommand(0x32);
    writeCommand(_CMD_VCOMH);  			// 0xBE
    writeCommand(0x05);
    writeCommand(_CMD_NORMALDISPLAY);  	// 0xA6
    writeCommand(_CMD_CONTRASTABC);
    writeData(0xC8);
    writeData(0x80);
    writeData(0xC8);
    setRegister(_CMD_CONTRASTMASTER,0x0F);
    writeCommand(_CMD_SETVSL );
    writeData(0xA0);
    writeData(0xB5);
    writeData(0x55);
    setRegister(_CMD_PRECHARGE2,0x01);
    writeCommand(_CMD_DISPLAYON);		//--turn on oled panel    
	clearScreen();
	goTo(0, 0);
}

void  OLED_SSD1351::invert(boolean v) {
	if (v) {
		writeCommand(_CMD_INVERTDISPLAY);
	} else {
		writeCommand(_CMD_NORMALDISPLAY);
	}
 }

/********************************* low level pin initialization */



#if defined(__MK20DX128__) || defined(__MK20DX256__)
	OLED_SSD1351::OLED_SSD1351(uint8_t cspin,uint8_t dcpin,uint8_t rstpin,uint8_t mosi,uint8_t sclk)
	{
		_cs   = cspin;
		_rs   = dcpin;
		_rst  = rstpin;
		_mosi = mosi;
		_sclk = sclk;
	}
	
#elif defined(__MKL26Z64__)
	OLED_SSD1351::OLED_SSD1351(uint8_t cspin,uint8_t dcpin,uint8_t rstpin,uint8_t mosi,uint8_t sclk)
	{
		_cs   = cspin;
		_rs   = dcpin;
		_rst  = rstpin;
		_mosi = mosi;
		_sclk = sclk;
		_useSPI1 = false;
		if ((_mosi == 0 || _mosi == 21) && (_sclk == 20)) _useSPI1 = true;
	}
	
#else
	OLED_SSD1351::OLED_SSD1351(uint8_t cspin,uint8_t dcpin,uint8_t rstpin)
	{
		_cs   = cspin;
		_rs   = dcpin;
		_rst  = rstpin;
	}
	
#endif


void OLED_SSD1351::drawLine(int16_t x0, int16_t y0,int16_t x1, int16_t y1,uint16_t color) 
{
	if (y0 == y1) {
		if (x1 > x0) {
			drawFastHLine(x0, y0, x1 - x0 + 1, color);
		} else if (x1 < x0) {
			drawFastHLine(x1, y0, x0 - x1 + 1, color);
		} else {
			drawPixel(x0, y0, color);
		}
		return;
	} else if (x0 == x1) {
		if (y1 > y0) {
			drawFastVLine(x0, y0, y1 - y0 + 1, color);
		} else {
			drawFastVLine(x0, y1, y0 - y1 + 1, color);
		}
		return;
	}

	bool steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {swap(x0, y0);swap(x1, y1);}
	if (x0 > x1) {swap(x0, x1);swap(y0, y1);}

	int16_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int16_t err = dx / 2;
	int16_t ystep;

	if (y0 < y1) {
		ystep = 1;
	} else {
		ystep = -1;
	}


	int16_t xbegin = x0;
	if (steep) {
		for (; x0<=x1; x0++) {
			err -= dy;
			if (err < 0) {
				int16_t len = x0 - xbegin;
				if (len) {
					drawFastVLine(y0, xbegin, len + 1, color);
				} else {
					drawPixel(y0, x0, color);
				}
				xbegin = x0 + 1;
				y0 += ystep;
				err += dx;
			}
		}
		if (x0 > xbegin + 1) drawFastVLine(y0, xbegin, x0 - xbegin, color);
	} else {
		for (; x0<=x1; x0++) {
			err -= dy;
			if (err < 0) {
				int16_t len = x0 - xbegin;
				if (len) {
					drawFastHLine(xbegin, y0, len + 1, color);
				} else {
					drawPixel(x0, y0, color);
				}
				xbegin = x0 + 1;
				y0 += ystep;
				err += dx;
			}
		}
		if (x0 > xbegin + 1) drawFastHLine(xbegin, y0, x0 - xbegin, color);
	}
}


void OLED_SSD1351::drawRect(int16_t x,int16_t y,int16_t w,int16_t h,uint16_t color) 
{
	drawFastHLine(x, y, w, color);
	drawFastHLine(x, y+h-1, w, color);
	drawFastVLine(x, y, h, color);
	drawFastVLine(x+w-1, y, h, color);
}


void OLED_SSD1351::drawCircle(int16_t cx, int16_t cy, int16_t radius, uint16_t color)
{
	int error = -radius;
	int16_t x = radius;
	int16_t y = 0;
	while (x >= y){
		plot8points(cx, cy, x, y, color);
		error += y;
		++y;
		error += y;
		if (error >= 0){
			--x;
			error -= x;
			error -= x;
		}
	}
}



void OLED_SSD1351::plot8points(uint16_t cx, uint16_t cy, uint16_t x, uint16_t y, uint16_t color)
{
	plot4points(cx, cy, x, y, color);
	if (x != y) plot4points(cx, cy, y, x, color);
}

void OLED_SSD1351::plot4points(uint16_t cx, uint16_t cy, uint16_t x, uint16_t y, uint16_t color)
{
	drawPixel(cx + x, cy + y, color);
	if (x != 0) drawPixel(cx - x, cy + y, color);
	if (y != 0) drawPixel(cx + x, cy - y, color);
	if (x != 0 && y != 0) drawPixel(cx - x, cy - y, color);
}

void OLED_SSD1351::drawCircleHelper(int16_t x0,int16_t y0,int16_t r,uint8_t cornername,uint16_t color) 
{
	int16_t f     = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x     = 0;
	int16_t y     = r;

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f     += ddF_y;
		}
		x++;
		ddF_x += 2;
		f     += ddF_x;
		if (cornername & 0x4) {drawPixel(x0 + x, y0 + y, color); drawPixel(x0 + y, y0 + x, color);} 
		if (cornername & 0x2) {drawPixel(x0 + x, y0 - y, color);drawPixel(x0 + y, y0 - x, color);}
		if (cornername & 0x8) {drawPixel(x0 - y, y0 + x, color);drawPixel(x0 - x, y0 + y, color);}
		if (cornername & 0x1) {drawPixel(x0 - y, y0 - x, color);drawPixel(x0 - x, y0 - y, color);}
	}
}


void OLED_SSD1351::fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color) 
{
	int16_t f     = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x     = 0;
	int16_t y     = r;

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f     += ddF_y;
		}
		x++;
		ddF_x += 2;
		f     += ddF_x;

		if (cornername & 0x1) {
			drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
			drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
		}
		if (cornername & 0x2) {
			drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
			drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
		}
	}
}

void OLED_SSD1351::ellipse(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
	int16_t a = abs(x1 - x0), b = abs(y1 - y0), b1 = b & 1; /* values of diameter */
	long dx = 4 * (1 - a) * b * b, dy = 4 * (b1 + 1) * a * a; /* error increment */
	long err = dx + dy + b1 * a * a, e2; /* error of 1.step */

	if (x0 > x1) { x0 = x1; x1 += a; } /* if called with swapped points */
	if (y0 > y1) y0 = y1; /* .. exchange them */
	y0 += (b + 1) / 2; /* starting pixel */
	y1 = y0 - b1;
	a *= 8 * a;
	b1 = 8 * b * b;

	do {
		drawPixel(x1, y0, color); /*   I. Quadrant */
		drawPixel(x0, y0, color); /*  II. Quadrant */
		drawPixel(x0, y1, color); /* III. Quadrant */
		drawPixel(x1, y1, color); /*  IV. Quadrant */
		e2 = 2 * err;
		if (e2 >= dx) { x0++; x1--; err += dx += b1; } /* x step */
		if (e2 <= dy) { y0++; y1--; err += dy += a; }  /* y step */
	} while (x0 <= x1);

	while (y0 - y1 < b) {  /* too early stop of flat ellipses a=1 */
		drawPixel(x0 - 1, ++y0, color); /* -> complete tip of ellipse */
		drawPixel(x0 - 1, --y1, color);
	}
}

void OLED_SSD1351::fillCircle(int16_t x0, int16_t y0, int16_t r,uint16_t color) 
{
	drawFastVLine(x0, y0-r, 2*r+1, color);
	fillCircleHelper(x0, y0, r, 3, 0, color);
}

void OLED_SSD1351::drawTriangle(int16_t x0, int16_t y0,int16_t x1, int16_t y1,int16_t x2, int16_t y2, uint16_t color) 
{
	drawLine(x0, y0, x1, y1, color);
	drawLine(x1, y1, x2, y2, color);
	drawLine(x2, y2, x0, y0, color);
}

void OLED_SSD1351::fillTriangle(int16_t x0, int16_t y0,int16_t x1, int16_t y1,int16_t x2, int16_t y2, uint16_t color) 
{

	int16_t a, b, y, last;

	// Sort coordinates by Y order (y2 >= y1 >= y0)
	if (y0 > y1) swap(y0, y1); swap(x0, x1);
	if (y1 > y2) swap(y2, y1); swap(x2, x1);
	if (y0 > y1) swap(y0, y1); swap(x0, x1);

	if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
		a = b = x0;
		if (x1 < a)      a = x1;
		else if(x1 > b) b = x1;
		if (x2 < a)      a = x2;
		else if(x2 > b) b = x2;
		drawFastHLine(a, y0, b-a+1, color);
		return;
	}

	int16_t
		dx01 = x1 - x0,
		dy01 = y1 - y0,
		dx02 = x2 - x0,
		dy02 = y2 - y0,
		dx12 = x2 - x1,
		dy12 = y2 - y1;
	int32_t
		sa   = 0,
		sb   = 0;
	if (y1 == y2) last = y1;   // Include y1 scanline
	else         last = y1-1; // Skip it

	for (y=y0; y<=last; y++) {
		a   = x0 + sa / dy01;
		b   = x0 + sb / dy02;
		sa += dx01;
		sb += dx02;
		if (a > b) swap(a,b);
		drawFastHLine(a, y, b-a+1, color);
	}
	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);
	for (; y<=y2; y++) {
		a   = x1 + sa / dy12;
		b   = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;
		if (a > b) swap(a,b);
		drawFastHLine(a, y, b-a+1, color);
	}
}

void OLED_SSD1351::drawQuad(int16_t x0, int16_t y0,int16_t x1, int16_t y1,int16_t x2, int16_t y2,int16_t x3, int16_t y3, uint16_t color) 
{
	drawLine(x0, y0, x1, y1, color);//low 1
	drawLine(x1, y1, x2, y2, color);//high 1
	drawLine(x2, y2, x3, y3, color);//high 2
	drawLine(x3, y3, x0, y0, color);//low 2
}

void OLED_SSD1351::fillQuad ( int16_t x0, int16_t y0,int16_t x1, int16_t y1,int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint16_t color) 
{
    fillTriangle(x0,y0,x1,y1,x2,y2,color);
    fillTriangle(x0,y0,x2,y2,x3,y3,color);
}

void OLED_SSD1351::drawPolygon(int16_t cx, int16_t cy, uint8_t sides, int16_t diameter, float rot, uint16_t color)
{ 
	sides = (sides > 2? sides : 3);
	float dtr = (PI/180.0) + PI;
	float rads = 360.0 / sides;//points spacd equally
	uint8_t i;
	for (i = 0; i < sides; i++) { 
		drawLine(
			cx + (sin((i*rads + rot) * dtr) * diameter),
			cy + (cos((i*rads + rot) * dtr) * diameter),
			cx + (sin(((i+1)*rads + rot) * dtr) * diameter),
			cy + (cos(((i+1)*rads + rot) * dtr) * diameter),
			color);
	}
}


void OLED_SSD1351::drawRoundRect(int16_t x, int16_t y, int16_t w,int16_t h, int16_t r, uint16_t color) 
{
  // smarter version
	drawFastHLine(x+r  , y    , w-2*r, color); // Top
	drawFastHLine(x+r  , y+h-1, w-2*r, color); // Bottom
	drawFastVLine(x    , y+r  , h-2*r, color); // Left
	drawFastVLine(x+w-1, y+r  , h-2*r, color); // Right
  // draw four corners
	drawCircleHelper(x+r    , y+r    , r, 1, color);
	drawCircleHelper(x+w-r-1, y+r    , r, 2, color);
	drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
	drawCircleHelper(x+r    , y+h-r-1, r, 8, color);
}

void OLED_SSD1351::fillRoundRect(int16_t x, int16_t y, int16_t w,int16_t h, int16_t r, uint16_t color) 
{
	// smarter version
	fillRect(x+r, y, w-2*r, h, color);
	// draw four corners
	fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
	fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
}

void OLED_SSD1351::drawBitmap(int16_t x, int16_t y,const uint8_t *bitmap, int16_t w, int16_t h,uint16_t color) 
{
	int16_t i, j, byteWidth = (w + 7) / 8;
	for (j=0; j<h; j++) {
		for (i=0; i<w; i++ ) {
			if (pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) drawPixel(x+i, y+j, color);
		}
	}
}

void OLED_SSD1351::drawBitmap(int16_t x, int16_t y,const uint8_t *bitmap, int16_t w, int16_t h,uint16_t color, uint16_t bg) 
{
	int16_t i, j, byteWidth = (w + 7) / 8;
	for (j=0; j<h; j++) {
		for (i=0; i<w; i++ ) {
			if (pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
				drawPixel(x+i, y+j, color);
			} else {
				drawPixel(x+i, y+j, bg);
			}
		}
	}
}

void OLED_SSD1351::setCursor(int16_t x, int16_t y) 
{
	_cursor_x = x;
	_cursor_y = y;
}

void OLED_SSD1351::setTextColor(uint16_t c) 
{
	_textcolor = _textbgcolor = c;
}

void OLED_SSD1351::setTextColor(uint16_t c, uint16_t b) 
{
	_textcolor   = c;
	_textbgcolor = b; 
}

int16_t OLED_SSD1351::width(void)
{
	return _width;
}
 
int16_t OLED_SSD1351::height(void)
{
	return _height;
}

void OLED_SSD1351::setRotation(uint8_t x) 
{
	_rotation = (x & 3);
	switch(_rotation) {
		case 0:
		case 2:
			_width  = OLED_WIDTH;
			_height = OLED_HEIGHT;
			_portrait = false;
			break;
		case 1:
		case 3:
			_width  = OLED_HEIGHT;
			_height = OLED_WIDTH;
			_portrait = true;
			break;
	}
}

void OLED_SSD1351::setTextWrap(boolean w) 
{
	_wrap = w;
}

void OLED_SSD1351::setTextSize(uint8_t s) 
{
	_textsize = (s > 0) ? s : 1;
}

void OLED_SSD1351::drawMesh(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t spacing, uint16_t color)
{
	if (spacing < 2) spacing = 2;
	if (((x + w) - 1) >= _width)  w = _width  - x;
	if (((y + h) - 1) >= _height) h = _height - y;
	
	int16_t n, m;

	if (w < x) {n = w; w = x; x = n;}
	if (h < y) {n = h; h = y; y = n;}
	for (m = y; m <= h; m += spacing) {
		for (n = x; n <= w; n += spacing) {
			drawPixel(n, m, color);
		}
	}
}


size_t OLED_SSD1351::write(uint8_t c)
{
	if (_font) {
		if (c == '\n') {
			//cursor_y += ??
			_cursor_x = 0;
		} else {
			drawFontChar(c);
		}
	} else {
		if (c == '\n') {
			_cursor_y += _textsize*8;
			_cursor_x  = 0;
		} else if (c == '\r') {
			// skip em
		} else {
			drawChar(_cursor_x, _cursor_y, c, _textcolor, _textbgcolor, _textsize);
			_cursor_x += _textsize*6;
			if (_wrap && (_cursor_x > (_width - _textsize*6))) {
				_cursor_y += _textsize*8;
				_cursor_x = 0;
			}
		}
	}
	return 1;
}


// Draw a character
void OLED_SSD1351::drawChar(int16_t x, int16_t y, unsigned char c,
			    uint16_t fgcolor, uint16_t bgcolor, uint8_t size)
{
	if((x >= _width)            || // Clip right
	   (y >= _height)           || // Clip bottom
	   ((x + 6 * size - 1) < 0) || // Clip left  TODO: is this correct?
	   ((y + 8 * size - 1) < 0))   // Clip top   TODO: is this correct?
		return;

	if (fgcolor == bgcolor) {
		// This transparent approach is only about 20% faster
		if (size == 1) {
			uint8_t mask = 0x01;
			int16_t xoff, yoff;
			for (yoff=0; yoff < 8; yoff++) {
				uint8_t line = 0;
				for (xoff=0; xoff < 5; xoff++) {
					if (glcdfont[c * 5 + xoff] & mask) line |= 1;
					line <<= 1;
				}
				line >>= 1;
				xoff = 0;
				while (line) {
					if (line == 0x1F) {
						drawFastHLine(x + xoff, y + yoff, 5, fgcolor);
						break;
					} else if (line == 0x1E) {
						drawFastHLine(x + xoff, y + yoff, 4, fgcolor);
						break;
					} else if ((line & 0x1C) == 0x1C) {
						drawFastHLine(x + xoff, y + yoff, 3, fgcolor);
						line <<= 4;
						xoff += 4;
					} else if ((line & 0x18) == 0x18) {
						drawFastHLine(x + xoff, y + yoff, 2, fgcolor);
						line <<= 3;
						xoff += 3;
					} else if ((line & 0x10) == 0x10) {
						drawPixel(x + xoff, y + yoff, fgcolor);
						line <<= 2;
						xoff += 2;
					} else {
						line <<= 1;
						xoff += 1;
					}
				}
				mask = mask << 1;
			}
		} else {
			uint8_t mask = 0x01;
			int16_t xoff, yoff;
			for (yoff=0; yoff < 8; yoff++) {
				uint8_t line = 0;
				for (xoff=0; xoff < 5; xoff++) {
					if (glcdfont[c * 5 + xoff] & mask) line |= 1;
					line <<= 1;
				}
				line >>= 1;
				xoff = 0;
				while (line) {
					if (line == 0x1F) {
						fillRect(x + xoff * size, y + yoff * size,
							5 * size, size, fgcolor);
						break;
					} else if (line == 0x1E) {
						fillRect(x + xoff * size, y + yoff * size,
							4 * size, size, fgcolor);
						break;
					} else if ((line & 0x1C) == 0x1C) {
						fillRect(x + xoff * size, y + yoff * size,
							3 * size, size, fgcolor);
						line <<= 4;
						xoff += 4;
					} else if ((line & 0x18) == 0x18) {
						fillRect(x + xoff * size, y + yoff * size,
							2 * size, size, fgcolor);
						line <<= 3;
						xoff += 3;
					} else if ((line & 0x10) == 0x10) {
						fillRect(x + xoff * size, y + yoff * size,
							size, size, fgcolor);
						line <<= 2;
						xoff += 2;
					} else {
						line <<= 1;
						xoff += 1;
					}
				}
				mask = mask << 1;
			}
		}
	} else {
		// This solid background approach is about 5 time faster
		startTransaction();
		setAddr(x, y, x + 6 * size - 1, y + 8 * size - 1);
		writecommand_cont(_CMD_WRITERAM);
		uint8_t xr, yr;
		uint8_t mask = 0x01;
		uint16_t color;
		for (y=0; y < 8; y++) {
			for (yr=0; yr < size; yr++) {
				for (x=0; x < 5; x++) {
					if (glcdfont[c * 5 + x] & mask) {
						color = fgcolor;
					} else {
						color = bgcolor;
					}
					for (xr=0; xr < size; xr++) {
						writedata16_cont(color);
					}
				}
				for (xr=0; xr < size; xr++) {
					writedata16_cont(bgcolor);
				}
			}
			mask = mask << 1;
		}
		writecommand_last(_CMD_NOP);
		endTransaction();
	}
}


static uint32_t fetchbit(const uint8_t *p, uint32_t index)
{
	if (p[index >> 3] & (1 << (7 - (index & 7)))) return 1;
	return 0;
}

static uint32_t fetchbits_unsigned(const uint8_t *p, uint32_t index, uint32_t required)
{
	uint32_t val = 0;
	do {
		uint8_t b = p[index >> 3];
		uint32_t avail = 8 - (index & 7);
		if (avail <= required) {
			val <<= avail;
			val |= b & ((1 << avail) - 1);
			index += avail;
			required -= avail;
		} else {
			b >>= avail - required;
			val <<= required;
			val |= b & ((1 << required) - 1);
			break;
		}
	} while (required);
	return val;
}

static uint32_t fetchbits_signed(const uint8_t *p, uint32_t index, uint32_t required)
{
	uint32_t val = fetchbits_unsigned(p, index, required);
	if (val & (1 << (required - 1))) {
		return (int32_t)val - (1 << required);
	}
	return (int32_t)val;
}

void OLED_SSD1351::drawFontChar(unsigned int c)
{
	uint32_t bitoffset;
	const uint8_t *data;
	if (c >= _font->index1_first && c <= _font->index1_last) {
		bitoffset = c - _font->index1_first;
		bitoffset *= _font->bits_index;
	} else if (c >= _font->index2_first && c <= _font->index2_last) {
		bitoffset = c - _font->index2_first + _font->index1_last - _font->index1_first + 1;
		bitoffset *= _font->bits_index;
	} else if (_font->unicode) {
		return; // TODO: implement sparse unicode
	} else {
		return;
	}
	data = _font->data + fetchbits_unsigned(_font->index, bitoffset, _font->bits_index);

	uint32_t encoding = fetchbits_unsigned(data, 0, 3);
	if (encoding != 0) return;
	uint32_t width = fetchbits_unsigned(data, 3, _font->bits_width);
	bitoffset = _font->bits_width + 3;
	uint32_t height = fetchbits_unsigned(data, bitoffset, _font->bits_height);
	bitoffset += _font->bits_height;
	int32_t xoffset = fetchbits_signed(data, bitoffset, _font->bits_xoffset);
	bitoffset += _font->bits_xoffset;
	int32_t yoffset = fetchbits_signed(data, bitoffset, _font->bits_yoffset);
	bitoffset += _font->bits_yoffset;
	uint32_t delta = fetchbits_unsigned(data, bitoffset, _font->bits_delta);
	bitoffset += _font->bits_delta;
	// horizontally, we draw every pixel, or none at all
	if (_cursor_x < 0) _cursor_x = 0;
	int32_t origin_x = _cursor_x + xoffset;
	if (origin_x < 0) {
		_cursor_x -= xoffset;
		origin_x = 0;
	}
	if (origin_x + (int)width > _width) {
		if (!_wrap) return;
		origin_x = 0;
		if (xoffset >= 0) {
			_cursor_x = 0;
		} else {
			_cursor_x = -xoffset;
		}
		_cursor_y += _font->line_space;
	}
	if (_cursor_y >= _height) return;
	_cursor_x += delta;

	// vertically, the top and/or bottom can be clipped
	int32_t origin_y = _cursor_y + _font->cap_height - height - yoffset;
	// TODO: compute top skip and number of lines
	int32_t linecount = height;
	uint32_t y = origin_y;
	while (linecount) {
		uint32_t b = fetchbit(data, bitoffset++);
		if (b == 0) {
			uint32_t x = 0;
			do {
				uint32_t xsize = width - x;
				if (xsize > 32) xsize = 32;
				uint32_t bits = fetchbits_unsigned(data, bitoffset, xsize);
				drawFontBits(bits, xsize, origin_x + x, y, 1);
				bitoffset += xsize;
				x += xsize;
			} while (x < width);
			y++;
			linecount--;
		} else {
			uint32_t n = fetchbits_unsigned(data, bitoffset, 3) + 2;
			bitoffset += 3;
			uint32_t x = 0;
			do {
				uint32_t xsize = width - x;
				if (xsize > 32) xsize = 32;
				uint32_t bits = fetchbits_unsigned(data, bitoffset, xsize);
				drawFontBits(bits, xsize, origin_x + x, y, n);
				bitoffset += xsize;
				x += xsize;
			} while (x < width);
			y += n;
			linecount -= n;
		}
	}
}


void OLED_SSD1351::drawFontBits(uint32_t bits, uint32_t numbits, uint32_t x, uint32_t y, uint32_t repeat)
{
	// TODO: replace this *slow* code with something fast...
	if (bits == 0) return;
	do {
		uint32_t x1 = x;
		uint32_t n = numbits;
		do {
			n--;
			if (bits & (1 << n)) drawPixel(x1, y, _textcolor);
			x1++;
		} while (n > 0);
		y++;
		repeat--;
	} while (repeat);
}