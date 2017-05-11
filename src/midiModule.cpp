#include <Arduino.h>

#include "midiModule.h"


void MidiModule::midiSetup( Sequencer *sequenceArray){ // backplane is for debug purposes only
  this->sequenceArray = sequenceArray;
//  this->noteData = noteData;
  beatPulseIndex = 0;
  firstRun = false;
  pulseTimer = 0;
}

void MidiModule::midiClockSyncFunc(midi::MidiInterface<HardwareSerial>* serialMidi){
  noInterrupts();
  serialMidi->read();
  interrupts();
}

void MidiModule::midiStopHandler(){
  if (clockMode == EXTERNAL_MIDI_CLOCK) {
    Serial.println("Midi Stop");
    if (playing == 0){
      for (int i=0; i< SEQUENCECOUNT; i++){
        sequenceArray[i].clockReset(true);
      }
    }
    playing = 0;
  }
}

void MidiModule::midiNoteOffHandler(byte channel, byte note, byte velocity){
//  Serial.println(String("Note Off: ch=") + channel + ", note=" + note + ", velocity=" + velocity);
}

void MidiModule::midiNoteOnHandler(byte channel, byte note, byte velocity){
  if (velocity > 0) {
    //Serial.println(String("Note On:  ch=") + channel + ", note=" + note + ", velocity=" + velocity);
  } else {
  //  Serial.println(String("Note Off: ch=") + channel + ", note=" + note);
  }
}

void MidiModule::midiStartContinueHandler(){
  if (clockMode == EXTERNAL_MIDI_CLOCK) {
    Serial.println("Midi Start / Continue");
    playing = 1;
  }
}

void MidiModule::midiClockPulseHandler(){
  if (clockMode != EXTERNAL_MIDI_CLOCK) {
    return; // no need to run clock pulse handler if using internal clock.
  }

  if(playing){
    for (int i=0; i< SEQUENCECOUNT; i++){
      sequenceArray[i].ppqPulse(MIDI_PULSE_COUNT);
    }
  }

//    Serial.println("Midi Clock - mpc: " + String(masterPulseCount) + "\ttempotimer: " + String(masterTempoTimer) + "\tbeatLength: " + String(beatLength) + "\tbeatPulseIndex: " + String(beatPulseIndex));
}
