#include <Arduino.h>
#include <MIDI.h>
#include "Sequencer.h"
#include "global.h"

#ifndef _midiModule_h_
#define _midiModule_h_


  void midiSetup();
  void midiStopHandler();
  void midiNoteOffHandler(byte channel, byte note, byte velocity);
  void midiNoteOnHandler(byte channel, byte note, byte velocity);
  void midiStartContinueHandler();
  void midiClockPulseHandler();
  void midiClockSyncFunc();

#endif
