#ifndef _OLED_SSD1351_REGISTERS_H
#define _OLED_SSD1351_REGISTERS_H


#define SSD1351_DELAYS_HWFILL	    (3)
#define SSD1351_DELAYS_HWLINE       (1)

const static byte _CMD_SETCOLUMN = 		0x15;
const static byte _CMD_WRITERAM =   	0x5C;
const static byte _CMD_READRAM =   		0x5D;
const static byte _CMD_SETROW =    		0x75;
const static byte _CMD_HORIZSCROLL =    0x96;
const static byte _CMD_STOPSCROLL =     0x9E;
const static byte _CMD_STARTSCROLL =    0x9F;
const static byte _CMD_SETREMAP = 		0xA0;
const static byte _CMD_STARTLINE = 		0xA1;
const static byte _CMD_DISPLAYOFFSET = 	0xA2;
const static byte _CMD_DISPLAYALLOFF = 	0xA4;
const static byte _CMD_DISPLAYALLON =  	0xA5;
const static byte _CMD_NORMALDISPLAY = 	0xA6;
const static byte _CMD_INVERTDISPLAY = 	0xA7;
const static byte _CMD_FUNCTIONSELECT = 0xAB;
const static byte _CMD_NOP = 			0xAD;
const static byte _CMD_DISPLAYOFF = 	0xAE;
const static byte _CMD_DISPLAYON =     	0xAF;
const static byte _CMD_PRECHARGE = 		0xB1;
const static byte _CMD_DISPLAYENHANCE =	0xB2;
const static byte _CMD_CLOCKDIV = 		0xB3;
const static byte _CMD_SETVSL = 		0xB4;
const static byte _CMD_SETGPIO = 		0xB5;
const static byte _CMD_PRECHARGE2 = 	0xB6;
const static byte _CMD_SETGRAY = 		0xB8;
const static byte _CMD_USELUT = 		0xB9;
const static byte _CMD_PRECHARGELEVEL = 0xBB;
const static byte _CMD_VCOMH = 			0xBE;
const static byte _CMD_CONTRASTABC =	0xC1;
const static byte _CMD_CONTRASTMASTER =	0xC7;
const static byte _CMD_MUXRATIO =       0xCA;
const static byte _CMD_COMMANDLOCK =    0xFD;



#endif