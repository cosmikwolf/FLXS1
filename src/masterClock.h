#include <Arduino.h>
#include "OutputController.h"
#include "Sequencer.h"
#include "midiModule.h"
#include "globalVariable.h"

#include "global.h"
#define INTERNAL_PPQ_COUNT 24
#define EXTCLOCKDIV  6
// DAC Mapping for each channel:
#ifndef _MasterClock_h_
#define _MasterClock_h_

#define CPU_RESET_CYCLECOUNTER_MSTR    do { ARM_DEMCR |= ARM_DEMCR_TRCENA;          \
                                       ARM_DWT_CTRL |= ARM_DWT_CTRL_CYCCNTENA; \
                                       ARM_DWT_CYCCNT = 0; } while(0)

class MasterClock {

public:
  void initialize(OutputController* outputControl, Sequencer *sequenceArray, midi::MidiInterface<HardwareSerial>* serialMidi, MidiModule *midiControl, GlobalVariable* globalObj);
  void changeTempo(uint32_t newTempoX100);
  void masterClockFunc();
  void sequencerFunc();
  void internalClockTick();
  void midiClockTick();
  void externalClockTick(uint8_t gateNum);
  void checkGateClock();

  bool masterDebugSwitch;

  bool gateTrig[9];
  bool gatePrevState[9];
  elapsedMicros pulseTimer;
  uint32_t totalClockCount;
  uint32_t clockCounter;
  uint32_t extClockCounter;

  uint32_t masterClockCycleCount;
  uint32_t lastClockValue;
  uint32_t startingClockCount;

  uint32_t lastPulseClockCount;

  elapsedMicros masterDebugTimer;
  boolean pulseTrigger;
  boolean displayRunSwitch;

  uint8_t gateMap[4];
  uint8_t dacCvMap[4];
  uint8_t dacCcMap[4];

private:
  OutputController* outputControl;
  GlobalVariable* globalObj;

  Sequencer *sequenceArray;
  midi::MidiInterface<HardwareSerial>* serialMidi;
  MidiModule *midiControl;
  elapsedMillis lfoTimer;
};
// https://isocpp.org/wiki/faq/pointers-to-members#fnptr-vs-memfnptr-types
//typedef void (MasterClock::*MasterClockMemFn)();

#endif
