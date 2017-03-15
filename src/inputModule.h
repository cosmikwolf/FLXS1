#include <Arduino.h>
#include <SPI.h>
#include "Zetaohm_MAX7301/Zetaohm_MAX7301.h"
#define ENCODER_OPTIMIZE_INTERRUPTS

#include <Encoder.h>
#include "OutputController.h"
#include "DisplayModule.h"
#include "Sequencer.h"
#include "MasterClock.h"
#include "global.h"

#define DOUBLECLICKMS  250

#ifndef _InputModule_h_
#define _InputModule_h_
//button stuff

class InputModule
{
public:
  InputModule();
  Encoder knob;
  Zetaohm_MAX7301* midplaneGPIO;
  Zetaohm_MAX7301* backplaneGPIO;
  OutputController* outputControl;
  MasterClock* clockMaster;

  void initialize(OutputController* outputControl, Zetaohm_MAX7301* midplaneGPIO, Zetaohm_MAX7301* backplaneGPIO, FlashMemory* saveFile, Sequencer *sequenceArray, MasterClock* clockMaster);

  void loop(uint16_t frequency);

  void patternSelectHandler();
  //void channelMenuHandler();
  void channelButtonHandler(uint8_t channel);
  void channelButtonShiftHandler(uint8_t channel);
  void altButtonHandler();
  void stepModeMatrixHandler();

  void channelPitchModeInputHandler();
  void channelVelocityModeInputHandler();
  void channelEnvelopeModeInputHandler();
  void channelStepModeInputHandler();
  void channelInputInputHandler();

  void sequenceMenuHandler();
  void inputMenuHandler();
  void globalMenuHandler();
  void tempoMenuHandler();

  void modMenu1_InputHandler();
  void modMenu2_InputHandler();

  void timingMenuInputHandler();
  void debugScreenInputHandler();
  void calibrationMenuHandler();

  void resetKnobValues();
  void changeState(uint8_t state);


  uint8_t lastSelectedStep;
  elapsedMillis selectedStepTimer;
  // Encoder vars
  int8_t knobRead;
  int8_t knobBuffer;
  int8_t knobPrevious;
  int8_t knobChange;
  int8_t menuSelector;
  int8_t instBuffer;
  int16_t stepModeBuffer;

  unsigned long encoderLoopTime;
  unsigned long smallButtonLoopTime;
  unsigned long encoderButtonTime;
  unsigned long matrixButtonTime;


private:
  Sequencer *sequenceArray;
  FlashMemory *saveFile;
  elapsedMicros inputTimer;
};

#endif
