#include <Arduino.h>
#include <MIDI.h>
#include "Sequencer.h"
#include "global.h"
#include "globalVariable.h"

#ifndef _midiModule_h_
#define _midiModule_h_

#define MIDI_PULSE_TIMEOUT 85000 //timeout length between midi clock pulses
#define MIDI_PULSE_COUNT 24

class MidiModule
{
public:
  void midiSetup(Sequencer *sequenceArray, GlobalVariable *globalObj);
  void midiStopHandler();
  void midiNoteOffHandler(byte channel, byte note, byte velocity);
  void midiNoteOnHandler(byte channel, byte note, byte velocity);
  void midiStartContinueHandler();
  void midiClockPulseHandler();
  void midiClockSyncFunc(midi::MidiInterface<HardwareSerial> *serialMidi);
  void midiSongPosition(int songPosition);

  bool midiTestArray[128];
  uint8_t midiClockCount;

private:
  int avgPulseTimer;
  elapsedMicros pulseTimer;
  boolean firstRun;
  Sequencer *sequenceArray;
  GlobalVariable *globalObj;
};
#endif
