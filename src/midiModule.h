#include <Arduino.h>
#include <MIDI.h>
#include "Sequencer.h"
#include "global.h"

#ifndef _midiModule_h_
#define _midiModule_h_

class MidiModule {
public:
  void midiSetup(midi::MidiInterface<HardwareSerial>* serialMidi, Sequencer *sequenceArray, NoteDatum *noteData);
  void midiStopHandler();
  void midiNoteOffHandler(byte channel, byte note, byte velocity);
  void midiNoteOnHandler(byte channel, byte note, byte velocity);
  void midiStartContinueHandler();
  void midiClockPulseHandler();
  void midiClockSyncFunc(midi::MidiInterface<HardwareSerial>* serialMidi);
private:
  Sequencer *sequenceArray;
  midi::MidiInterface<HardwareSerial>* serialMidi;
  NoteDatum *noteData;
};
#endif
