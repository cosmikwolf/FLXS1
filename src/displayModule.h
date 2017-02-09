#include <Arduino.h>
#include <SPI.h>
#include "Sequencer.h"
#include "global.h"
#include <SSD_13XX.h>
#include "_fonts/unborn_small.c"
//#include "_fonts/msShell14.c"
#include "_fonts/Visitor_18.c"
#include "_fonts/F_Zero_18.c"
#include "_fonts/LadyRadical_12.c"
#include "_fonts/akashi20.c"
#include "_fonts/PixelSquare_10.c"
#include "_fonts/PixelSquareBold_10.c"
#include "_fonts/PixelTech_14.c"
//#include "_fonts/mono_mid.c"

#ifndef _display_h_
#define _display_h_

/* in addition to:
  BLACK  BLUE  RED ORANGE  GREEN CYAN MAGENTA YELLOW WHITE
  more colors available at: http://www.rinkydinkelectronics.com/calc_rgb565.php
  */
#define NAVY            0x000F      /*   0,   0, 128 */
#define DARKGREEN       0x03E0      /*   0, 128,   0 */
#define DARKCYAN        0x03EF      /*   0, 128, 128 */
#define MAROON          0x7800      /* 128,   0,   0 */
#define PURPLE          0x780F      /* 128,   0, 128 */
#define OLIVE           0x7BE0      /* 128, 128,   0 */
#define LIGHTGREY       0xC618      /* 192, 192, 192 */
#define DARKGREY        0x7BEF      /* 128, 128, 128 */
#define ORANGE          0xFD20      /* 255, 165,   0 */
#define GREENYELLOW     0xAFE5      /* 173, 255,  47 */
#define PINK            0xF81F
#define DARKPURPLE 0x48B3

#define MAX_DISPLAY_ELEMENTS 30

#define LCD_DC        10//out (SSD1351/sharp refresh todo) OK
#define LCD_CS        6//out (SSD1351/sharp) OK
#define LCD_RST       9//out (SSD1351)/sharp disp todo) OK

#define DISPLAY_LABEL  255

#define REGULAR1X      0
#define BOLD1X         1
#define REGULAR2X      2
#define BOLD2X         3
#define REGULAR3X      4
#define BOLD3X         5
#define REGULAR4X      6
#define BOLD4X         7
#define STYLE1X         8

class DisplayModule
{
  public:
    DisplayModule();

    int color = 0;
    uint32_t runcount;

    char *displayCache[MAX_DISPLAY_ELEMENTS];
    char *displayElement[MAX_DISPLAY_ELEMENTS];
    char *buf = new char[51]; // sprintf buffer

    uint8_t highlight;
    uint8_t previouslySelectedChannel;
    uint8_t previousStepMode;
    SSD_13XX oled = SSD_13XX(LCD_CS, LCD_DC, LCD_RST);

    elapsedMicros displayTimer;
    void initialize(Sequencer *sequenceArray);
    void displayLoop(uint16_t frequency);

    void clearDisplay();
    void freeDisplayCache();

    void stepDisplay(char *buf);
    void patternSelectDisplay();

    void channelSequenceDisplay(char *buf);
    void stepMode_pitch(char*buf);
    void stepMode_gateLength(char *buf);
    void stepMode_chord(char *buf);
    void stepMode_glide(char *buf);
    void stepMode_stepCount(char *buf);
    void stepMode_beatCount(char *buf);
    void stepMode_gateType(char *buf);
    void stepMode_arpType(char *buf);
    void stepMode_arpSpeed(char *buf);
    void stepMode_common(char *buf);
    void stepMode_arpOctave(char *buf);

    void channelPitchMenuDisplay(char *buf);
    void channelPitchMenuDisplay2(char *buf);
    void channelVelocityMenuDisplay(char *buf);
    void channelEnvelopeMenuDisplay(char *buf);
    void channelStepMenuDisplay(char *buf);

    void channelTunerDisplay(char *buf);
    void channelInputDisplay(char *buf);

    void sequenceMenuDisplay();
    void globalMenuDisplay();
    void tempoMenuDisplay();

    void gameOfLifeDisplay();
    void deleteMenuDisplay();
    void cleanupTextBuffers();
    void timingMenuDisplay();
    void calibrationMenuDisplay();
    void inputDebugMenuDisplay();
    uint16_t foreground, background;

    void renderOnce_StringBox(uint8_t index, uint8_t highlight, uint8_t previousHighlight, int16_t x, int16_t y, int16_t w, int16_t h, bool border, uint8_t textSize, uint16_t color, uint16_t bgColor) ;

    void renderStringBox(uint8_t index, uint8_t highlight, int16_t x, int16_t y, int16_t w, int16_t h, bool border, uint8_t textSize, uint16_t color, uint16_t bgColor) ;

    //void renderOnce_StringBox(Element element);
private:

    Sequencer *sequenceArray;

  /*  struct element {
      uint8_t index;
      uint8_t highlight;
      uint8_t previousHighlight;
      int16_t x;
      int16_t y;
      int16_t w;
      int16_t h;
      bool border;
      uint8_t textSize;
      uint16_t color;
      uint16_t bgColor;
    };
    typedef struct element Element;
    */

    const char* midiNotes[128] = {
    "C -2","C#-2","D -2","D#-2","E -2","F -2","F#-2","G -2","G#-2","A -2","A#-2","B -2",
    "C -1","C#-1","D -1","D#-1","E -1","F -1","F#-1","G -1","G#-1","A -1","A#-1","B -1",
    "C  0","C# 0","D  0","D# 0","E   0","F  0","F# 0","G  0","G# 0","A  0","A# 0","B  0",
    "C  1","C# 1","D  1","D# 1","E   1","F  1","F# 1","G  1","G# 1","A  1","A# 1","B  1",
    "C  2","C# 2","D  2","D# 2","E   2","F  2","F# 2","G  2","G# 2","A  2","A# 2","B  2",
    "C  3","C# 3","D  3","D# 3","E   3","F  3","F# 3","G  3","G# 3","A  3","A# 3","B  3",
    "C  4","C# 4","D  4","D# 4","E   4","F  4","F# 4","G  4","G# 4","A  4","A# 4","B  4",
    "C  5","C# 5","D  5","D# 5","E   5","F  5","F# 5","G  5","G# 5","A  5","A# 5","B  5",
    "C  6","C# 6","D  6","D# 6","E   6","F  6","F# 6","G  6","G# 6","A  6","A# 6","B  6",
    "C  7","C# 7","D  7","D# 7","E   7","F  7","F# 7","G  7","G# 7","A  7","A# 7","B  7",
    "C  8","C# 8","D  8","D# 8","E   8","F  8","F# 8","G  8" };


};


#endif
