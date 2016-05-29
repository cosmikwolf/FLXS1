#include <Arduino.h>
#include <SPI.h>
#include "Zetaohm_MAX7301/Zetaohm_MAX7301.h"
#include <Encoder.h>
#include "DisplayModule.h"
#include "Sequencer.h"
#include "masterClock.h"
#include "fileOps.h"
#include "GameOfLife.h"
#include "global.h"

#ifndef _inputModule_h_
#define _inputModule_h_
//button stuff
#define SW_00    0
#define SW_01    1
#define SW_02    2
#define SW_03    3
#define SW_04    4
#define SW_05    5
#define SW_06    6
#define SW_07    7
#define SW_08    8
#define SW_09    9
#define SW_10    10
#define SW_11    11
#define SW_12    12
#define SW_13    13
#define SW_14    14
#define SW_15    15
#define SW_PLAY  16
#define SW_REC   17
#define SW_STOP  18
#define SW_M0    19
#define SW_M1    20
#define SW_M2    21
#define SW_M3    22
#define SW_PGDN  23
#define SW_PGUP  24
#define SW_MENU  25
#define SW_ALT   26
#define SW_SPARE   27
#define ENCODER1LEFTPIN 17
#define ENCODER1RIGHTPIN 16
#define MAX7301PIN  5
class inputModule
{
public:
  inputModule();
  void buttonSetup();
  void buttonLoop();
  void patternSelectHandler();
  void channelMenuHandler();
  void channelButtonHandler(uint8_t channel);
  void altButtonHandler();
  void stepModeMatrixHandler();
  void sequencerMenuHandler();
  void instrumentSelectInputHandler();
  void timingMenuInputHandler();
  void debugScreenInputHandler();
  void resetKnobValues();
  void changeState(uint8_t state);

  Encoder knob = Encoder(ENCODER1LEFTPIN, ENCODER1RIGHTPIN);
  Zetaohm_MAX7301 max7301;

  // Encoder vars
  int8_t knobRead = 0;
  int8_t knobBuffer = 0;
  int8_t knobPrevious = 0;
  int8_t knobChange = 0;
  int8_t menuSelector = 0;
  int8_t instBuffer;
  int16_t stepModeBuffer;

  unsigned long encoderLoopTime;
  unsigned long smallButtonLoopTime;
  unsigned long encoderButtonTime;
  unsigned long matrixButtonTime;
};

extern inputModule interface;

#endif
