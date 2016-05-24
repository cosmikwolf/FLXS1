#ifndef _OLED_SSD1351_SETTINGS_H
#define _OLED_SSD1351_SETTINGS_H


#define SSD1351_COLORORDER_RGB
// #define SSD1351_COLORORDER_BGR


//#define SSD1351_128_128
#define SSD1351_128_96
	
#define _SSD1351_BACKGROUND BLACK
#define _SSD1351_FOREGROUND WHITE
	

#if defined SSD1351_COLORORDER_RGB && defined SSD1351_COLORORDER_BGR
  #error "RGB and BGR can not both be defined for SSD1351_COLORODER."
#endif

#if defined SSD1351_128_128 && defined SSD1351_128_96
  #error "Only one SSD1351 display can be specified at once in TFT_SSD1351_settings.h"
#endif
#if !defined SSD1351_128_128 && !defined SSD1351_128_96
  #error "At least one SSD1351 display must be specified in TFT_SSD1351_settings.h"
#endif


#if defined SSD1351_128_128
  #define OLED_WIDTH                  128
  #define OLED_HEIGHT                 128
#endif
#if defined SSD1351_128_96
  #define OLED_WIDTH                  128
  #define OLED_HEIGHT                 96
#endif

//this chip uses fixed resolution but in case...
#define OLED_MW						OLED_WIDTH -1
#define OLED_MH						OLED_HEIGHT -1


#endif