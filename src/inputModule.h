#include <Arduino.h>
#include <SPI.h>
#include "Zetaohm_MAX7301/Zetaohm_MAX7301.h"
#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Entropy.h>
#include <Encoder.h>
#include "OutputController.h"
#include "DisplayModule.h"
#include "Sequencer.h"
#include "MasterClock.h"
#include "FlashMemory.h"
#include "globalVariable.h"
#include "global.h"

#define DOUBLECLICKMS  250
#define HOLDMS  1000

#define BUTTON_MODE_WAIT        0
#define BUTTON_MODE_XOX         1
#define BUTTON_MODE_PLAYRANGE  2
#define BUTTON_MODE_MULTISELECT 3

#ifndef _InputModule_h_
#define _InputModule_h_
//button stuff

class InputModule
{
public:
  InputModule();
  Encoder knob;
  //Encoder knob;
  Zetaohm_MAX7301* midplaneGPIO;
  Zetaohm_MAX7301* backplaneGPIO;
  OutputController* outputControl;
  MasterClock* clockMaster;

  void initialize(OutputController* outputControl, Zetaohm_MAX7301* midplaneGPIO, Zetaohm_MAX7301* backplaneGPIO, FlashMemory* saveFile, Sequencer *sequenceArray, MasterClock* clockMaster, DisplayModule* display, GlobalVariable* globalObj);

  void loop(uint16_t frequency);

  void patternSelectHandler();
  //void channelMenuHandler();
  void channelButtonHandler(uint8_t channel);
  void channelButtonShortcutHandler(uint8_t channel);
  void channelButtonChannelSelectorHandler(uint8_t channel);
  void channelButtonShiftHandler(uint8_t channel);
  void channelButtonShiftMenuHandler(uint8_t channel);

  bool altButtonHandler();
  void altButtonChannelHandler(uint8_t switchIndex);
  void altButtonPlayHandler();
  void altButtonRecHandler();
  void altButtonStopHandler();
  void altButtonPatternHandler();
  void altButtonTempoHandler();
  void altButtonShiftHandler();
  void altButtonPgupHandler();
  void altButtonPgdnHandler();

  void stepModeMatrixHandler();

  void channelPitchModeInputHandler();
  void multiSelectInputHandler();

  void randomShortcutHandler();

  void saveMenuInputHandler();
  void patternChainInputHandler();
  void sequenceMenuHandler();
  void scaleMenuHandler();
  void inputMenuHandler();
  void globalMenuHandler();
  void sysexMenuHandler();
  void tempoMenuHandler();
  void changeStepData(uint8_t channel, uint8_t stepNum, int change);

  void modMenu1_InputHandler();
  void modMenu2_InputHandler();
  void debugScreenInputHandler();
  void calibrationMenuHandler();
  void calibrationSaveHandler();

  void resetKnobValues();
  void changeState(uint8_t state);
  uint16_t generateRandomNumber();

  uint8_t getChannelButtonSw(uint8_t channel);
  int getChannelFromSw(int switchNum);
  elapsedMillis chRecEraseTimer;
  bool chRecEraseSwitch;
  uint8_t chPressedSelector;
  uint8_t buttonMode;
  uint8_t lastselectedStep;
  elapsedMillis selectedStepTimer;
  // Encoder vars
  int8_t knobRead;
  int8_t knobBuffer;
  int8_t knobPrevious;
  int8_t knobChange;
  int8_t menuSelector;
  int8_t instBuffer;

  uint8_t shortcutRandomOctaveSpan;
  elapsedMillis shortcutRandomSwitch;

  int16_t stepModeBuffer;
  uint8_t previousMenu;

  unsigned long encoderLoopTime;
  unsigned long smallButtonLoopTime;
  unsigned long encoderButtonTime;
  unsigned long matrixButtonTime;

private:
  Sequencer *sequenceArray;
  FlashMemory *saveFile;
  DisplayModule *display;
  GlobalVariable* globalObj;
  elapsedMicros inputTimer;

  bool channelButtonOperationInProgress;


};

#endif
