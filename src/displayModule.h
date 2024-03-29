#include <Arduino.h>
#include <SPI.h>
#include "Sequencer.h"
#include "global.h"
#include "globalVariable.h"
#include "MasterClock.h"

#include <SSD_13XX.h>
#include "_fonts/unborn_small.c"
//#include "_fonts/msShell14.c"
#include "_fonts/Visitor_18.c"
#include "_fonts/F_Zero_18.c"
#include "_fonts/LadyRadical_16.c"
#include "_fonts/OrbitB_12.c"
#include "_fonts/OrbitBold_14.c"
#include "_fonts/PixelSquare_10.c"
#include "_fonts/PixelSquareBold_10.c"
#include "_fonts/a04b03.c"
#include "_fonts/flxs1_menu.c"
#include "_fonts/NeueHaasXBlack_28.c"

#ifndef _display_h_
#define _display_h_

/* in addition to:
  BLACK  BLUE  RED ORANGE  GREEN CYAN MAGENTA YELLOW WHITE
  more colors available at: http://www.rinkydinkelectronics.com/calc_rgb565.php
  */
#define NAVY 0x000F /*   0,   0, 128 */
#define LIGHTBLUE 0x421F
#define DARKGREEN 0x67E0   /*   0, 128,   0 */
#define DARKCYAN 0x03EF    /*   0, 128, 128 */
#define MAROON 0x7800      /* 128,   0,   0 */
#define PURPLE 0x780F      /* 128,   0, 128 */
#define OLIVE 0x7BE0       /* 128, 128,   0 */
#define LIGHTGREY 0xC618   /* 192, 192, 192 */
#define DARKGREY 0x7BEF    /* 128, 128, 128 */
#define ORANGE 0xFD20      /* 255, 165,   0 */
#define LIGHTORANGE 0xFEAF /* 255, 165,   0 */
#define LIGHTGREEN 0xAFEF  /* 255, 165,   0 */
#define GREENYELLOW 0xAFE5 /* 173, 255,  47 */
#define PINK 0xF81F
#define LIGHTPINK 0xFCFF
#define DARKPURPLE 0x48B3

#define MAX_DISPLAY_ELEMENTS 30

#define LCD_DC 10 //out (SSD1351/sharp refresh todo) OK
#define LCD_CS 6  //out (SSD1351/sharp) OK
#define LCD_RST 9 //out (SSD1351)/sharp disp todo) OK

#define DISPLAY_LABEL 255

#define REGULAR1X 0
#define BOLD1X 1
#define REGULAR2X 2
#define BOLD2X 3
#define REGULAR3X 4
#define BOLD3X 5
#define REGULAR4X 6
#define BOLD4X 7
#define STYLE1X 8
#define MODALBOLD 9
#define REGULARSPECIAL 10

#define COLUNDINOTECOUNT 55

class DisplayModule
{
public:
  DisplayModule();

  int color = 0;
  uint32_t runcount;

  char *displayCache[MAX_DISPLAY_ELEMENTS];
  char *displayElement[MAX_DISPLAY_ELEMENTS];
  uint16_t previousColor[MAX_DISPLAY_ELEMENTS];
  char *buf = new char[51]; // sprintf buffer

  uint8_t highlight;
  uint8_t previousMenu;
  uint8_t previouslySelectedChannel;
  uint8_t previousStepMode;
  bool previousParameterSelect;
  SSD_13XX oled = SSD_13XX(LCD_CS, LCD_DC, LCD_RST);

  elapsedMicros displayTimer;
  void initialize(Sequencer *sequenceArray, MasterClock *clockMaster, GlobalVariable *globalObj, MidiModule *midiControl);
  void displayLoop(uint16_t frequency);
  void screenSaver(uint16_t frequency);
  void calibrationWarning();
  void saveFileWarning();
  void clearDisplay();
  void freeDisplayCache();

  void stepDisplay(char *buf);
  void patternSelectDisplay();

  void channelSequenceDisplay(char *buf);
  void stateDisplay_pitch(char *buf);
  void stateDisplay_pitchRender();
  void stateDisplay_arp(char *buf);
  void stateDisplay_velocity(char *buf);

  void stepMode_gateLength(char *buf);
  void stepMode_chord(char *buf);
  void stepMode_glide(char *buf);
  void stepMode_stepCount(char *buf);
  void stepMode_beatCount(char *buf);
  void stepMode_gateType(char *buf);
  void stepMode_arpSpeed(char *buf);
  void stepMode_common(char *buf);
  void stepMode_arpOctave(char *buf);

  void channelPitchMenuDisplay(char *buf);
  void channelPitchMenuDisplay2(char *buf);
  void channelEnvelopeMenuDisplay(char *buf);
  void channelStepMenuDisplay(char *buf);

  void channelTunerDisplay(char *buf);
  void channelInputDisplay(char *buf);

  void multiSelectMenu(char *buf);
  void modalPopup();
  void gateMappingText(char *buf, int8_t mapping);
  void cvMappingText(char *buf, int8_t mapping);

  void modalDisplay();
  void displayModal(uint16_t ms, uint8_t select, uint8_t chSelector);
  void displayModal(uint16_t ms, uint8_t selector);
  void resetModalTimer();
  void saveMenuDisplayHandler();

  void sequenceMenuDisplay();
  void scaleMenuDisplay();
  void inputMenuDisplay();
  void globalMenuDisplay();
  void globalMenuDisplay2();
  void globalMenuDisplay3();
  void sysexMenuHandler();
  void sysexImportHandler();
  void patternChainMenuHandler();
  void patternChainHelpHandler();

  void tempoMenuDisplay();
  void modMenu1_DisplayHandler();
  void modMenu2_DisplayHandler();
  void noteDisplayHandler();
  void gameOfLifeDisplay();
  void deleteMenuDisplay();
  void cleanupTextBuffers();
  void timingMenuDisplay();
  void inputCalibrationMenuDisplay();
  void outputCalibrationMenuDisplay();
  void midiTestDisplay();
  void gateTestDisplay();
  void rheostatTestDisplay();
  void inputDebugMenuDisplay();

  void shortcutRandomMenu();
  void shortcutTransposeMenu();
  void shortcutTunerMenu();
  void shortcutFillMenu();
  void shortcutSkipMenu();
  void shortcutClkdivMenu();
  void shortcutResetMenu();
  void shortcutReverseMenu();
  void shortcutPageMenu();
  void shortcutFn1Menu();
  void shortcutFn2Menu();
  void shortcutFn3Menu();
  void shortcutFn4Menu();

  void cvOutputRangeText(uint8_t dispElement, uint8_t outputRangeValue);
  void voltageToText(char *buf, int voltageValue);
  void createChList(char *buf);

  uint16_t foreground, background, contrastColor;

  void renderOnce_StringBox(uint8_t index, uint8_t highlight, uint8_t previousHighlight, int16_t x, int16_t y, int16_t w, int16_t h, bool border, uint8_t textSize, uint16_t color, uint16_t bgColor);

  void renderStringBox(uint8_t index, uint8_t highlight, int16_t x, int16_t y, int16_t w, int16_t h, bool border, uint8_t textSize, uint16_t color, uint16_t bgColor);
  void renderStringBox(uint8_t index, uint8_t highlight, int16_t x, int16_t y, int16_t w, int16_t h, bool border, uint8_t textSize, uint16_t color, uint16_t bgColor, bool forceHighlight);

  void renderString(uint8_t index, uint8_t highlight, int16_t x, int16_t y, bool border, uint8_t textSize, uint16_t color, uint16_t bgColor);

  //void renderOnce_StringBox(Element element);
private:
  MidiModule *midiControl;
  elapsedMillis modaltimer;
  bool modalRefreshSwitch;
  uint16_t modalMaxTime;
  uint8_t modalSelect;
  uint8_t chSelector;
  Sequencer *sequenceArray;
  MasterClock *clockMaster;
  GlobalVariable *globalObj;
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

  const char *const midiNotes[128] = {
      "C -2", "C#-2", "D -2", "D#-2", "E -2", "F -2", "F#-2", "G -2", "G#-2", "A -2", "A#-2", "B -2",
      "C -1", "C#-1", "D -1", "D#-1", "E -1", "F -1", "F#-1", "G -1", "G#-1", "A -1", "A#-1", "B -1",
      "C  0", "C# 0", "D  0", "D# 0", "E   0", "F  0", "F# 0", "G  0", "G# 0", "A  0", "A# 0", "B  0",
      "C  1", "C# 1", "D  1", "D# 1", "E   1", "F  1", "F# 1", "G  1", "G# 1", "A  1", "A# 1", "B  1",
      "C  2", "C# 2", "D  2", "D# 2", "E   2", "F  2", "F# 2", "G  2", "G# 2", "A  2", "A# 2", "B  2",
      "C  3", "C# 3", "D  3", "D# 3", "E   3", "F  3", "F# 3", "G  3", "G# 3", "A  3", "A# 3", "B  3",
      "C  4", "C# 4", "D  4", "D# 4", "E   4", "F  4", "F# 4", "G  4", "G# 4", "A  4", "A# 4", "B  4",
      "C  5", "C# 5", "D  5", "D# 5", "E   5", "F  5", "F# 5", "G  5", "G# 5", "A  5", "A# 5", "B  5",
      "C  6", "C# 6", "D  6", "D# 6", "E   6", "F  6", "F# 6", "G  6", "G# 6", "A  6", "A# 6", "B  6",
      "C  7", "C# 7", "D  7", "D# 7", "E   7", "F  7", "F# 7", "G  7", "G# 7", "A  7", "A# 7", "B  7",
      "C  8", "C# 8", "D  8", "D# 8", "E   8", "F  8", "F# 8", "G  8"};

  const char *const colundiNotes[56] = {
      "10.8", "33.0", "33.8", "47.0", "55.0", "59.9", "62.6", "63.0", "70.0", "73.6", "83.0", "98.4", "105.0", "110.0", "111.0", "147.0", "147.9", "172.1", "210.4", "221.2", "264.0", "293.0", "342.0", "396.0", "404.0", "408.0", "410.0", "413.0", "416.0", "417.0", "420.8", "440.0", "445.0", "448.0", "464.0", "528.0", "586.0", "630.0", "639.0", "685.0", "741.0", "852.0", "880.0", "1033.0", "1052.0", "1185.0", "3240.0", "3835.0", "3975.0", "4129.0", "4280.0", "4444.0", "5907.0", "6051.0", "8000.0", "12000.0"};
};

#endif
