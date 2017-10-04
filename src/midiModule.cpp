#include <Arduino.h>

#include "midiModule.h"


void MidiModule::midiSetup( Sequencer *sequenceArray, GlobalVariable* globalObj){ // backplane is for debug purposes only
  this->sequenceArray = sequenceArray;
  this->globalObj = globalObj;
//  this->noteData = noteData;
  firstRun = false;
  pulseTimer = 0;
  for(int i=0; i<10; i++){
    midiTestArray[i] = false;
  }
}

void MidiModule::midiClockSyncFunc(midi::MidiInterface<HardwareSerial>* serialMidi){
  //noInterrupts();
  // if(  ){
  //  Serial.println("1: " + String(serialMidi->getData1()));
  //  Serial.println("2: " + String(serialMidi->getData2()));
  // }
  serialMidi->read();
  //interrupts();
}

void MidiModule::midiStopHandler(){
  if (globalObj->clockMode == EXTERNAL_MIDI_CLOCK) {
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
  bool exitTestLoop;
  if(stepMode == STATE_TEST_MIDI){
    if(midiTestActive == false){
      for(int i=0; i<128; i++){
        midiTestArray[i] = false;
      }
    }
    Serial.println("Recieved test note " + String(note));
    if(note < 128){
      midiTestArray[note] = true;
    }
    exitTestLoop = true;
    for(int n=0; n <128; n++){
      if (midiTestArray[n] == false){
        exitTestLoop = false;
      }
    }
    if(exitTestLoop){
      midiTestActive = false;
      Serial.println("midi Test loop complete");
    }
  } else {
    if (velocity > 0) {
      Serial.println(String("Note On:  ch=") + channel + ", note=" + note + ", velocity=" + velocity);
    } else {
      Serial.println(String("Note Off: ch=") + channel + ", note=" + note);
    }
  }

}

void MidiModule::midiStartContinueHandler(){
  if (globalObj->clockMode == EXTERNAL_MIDI_CLOCK) {
    Serial.println("Midi Start / Continue");
    playing = 1;
  }
}

void MidiModule::midiClockPulseHandler(){
  if (globalObj->clockMode != EXTERNAL_MIDI_CLOCK) {
    return; // no need to run clock pulse handler if using internal clock.
  }

  if(playing){
    for (int i=0; i< SEQUENCECOUNT; i++){
      sequenceArray[i].ppqPulse(MIDI_PULSE_COUNT);
    }
  }

  //Serial.println("Midi Clock - mpc: " + String(masterPulseCount) + "\tbeatLength: " + String(beatLength) );
}
