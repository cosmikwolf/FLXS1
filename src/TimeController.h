#ifndef _TIME_CONTROLLER_H_
#define _TIME_CONTROLLER_H_

#include <Arduino.h>
#include <Encoder.h>
#include "Zetaohm_MAX7301/Zetaohm_MAX7301.h"
#include <ADC.h>

#include "DisplayModule.h"
#include "LEDArray.h"
#include "InputModule.h"
#include "FlashMemory.h"
#include "MasterClock.h"
#include "OutputController.h"
#include "globalVariable.h"
#include "Sequencer.h"
#include "global.h"

class TimeController
{
public:
  TimeController();
  void initialize(midi::MidiInterface<HardwareSerial> *serialMidi, MidiModule *midiControl, Sequencer *sequencerArray, ADC *adc, GlobalVariable *globalObj);

  void runLoopHandler();
  void masterClockHandler();
  void sequencerHandler();
  void ledClockHandler();
  void midiClockHandler();
  void cacheWriteHandler();
  void sysexMessageHandler(char *sysex_message);
  int flashMemoryControl(int value);
  void runDisplayLoop();
  uint8_t midiTestValue;

private:
  GlobalVariable *globalObj;
  MasterClock clockMaster;
  OutputController outputControl;
  InputModule buttonIo;
  DisplayModule display;
  LEDArray ledArray;
  FlashMemory saveFile;
  Zetaohm_MAX7301 midplaneGPIO;
  Zetaohm_MAX7301 backplaneGPIO;
  MidiModule *midiControl;
  midi::MidiInterface<HardwareSerial> *serialMidi;
  Sequencer *sequencerArray;
  ADC *adc;
  elapsedMicros cacheWriteTimer;
};
//extern Sequencer sequence[4];

#endif
