#include <Arduino.h>
#include "OutputController.h"
#include "Sequencer.h"
#include "midiModule.h"

#include "global.h"
#define INTERNAL_PPQ_COUNT 12
#define EXTCLOCKDIV  3  
// DAC Mapping for each channel:
#ifndef _MasterClock_h_
#define _MasterClock_h_


class MasterClock {

public:
  void initialize(OutputController* outputControl, Sequencer *sequenceArray, midi::MidiInterface<HardwareSerial>* serialMidi, MidiModule *midiControl);
  void changeTempo(uint32_t newTempoX100);
  void masterClockFunc();
  void sequencerFunc();
  void internalClockTick();
  void midiClockTick();
  void clockRunCheck();
  void externalClockTick(uint8_t gateNum);
//  void noteOffSwitch();
//  void noteOnSwitch();

  bool gateTrigger(uint8_t gateNum);
  void checkGateClock();

  bool masterDebugSwitch;

  bool gateTrig[4];
  bool gatePrevState[4];
  elapsedMicros pulseTimer;
  uint32_t clockCounter;
  uint32_t extClockCounter;

  uint32_t masterLooptimeMin;
  uint32_t masterLooptimeMax;
  uint32_t masterLooptimeAvg;
  elapsedMicros masterLoopTimer;
  elapsedMicros masterDebugTimer;
  boolean pulseTrigger;
  boolean ledRunSwitch;
  uint8_t beatPulseIndex;
  uint8_t clickCounter;
  boolean firstRun;

  uint8_t gateMap[4];
  uint8_t dacCvMap[4];
  uint8_t dacCcMap[4];

private:
  OutputController* outputControl;
  Sequencer *sequenceArray;
  midi::MidiInterface<HardwareSerial>* serialMidi;
  MidiModule *midiControl;
  elapsedMillis lfoTimer;
};
// https://isocpp.org/wiki/faq/pointers-to-members#fnptr-vs-memfnptr-types
//typedef void (MasterClock::*MasterClockMemFn)();

#endif
