#include <Arduino.h>

#include "midiModule.h"


void MidiModule::midiSetup( Sequencer *sequenceArray){
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
  if (playing == 0){
    for (int i=0; i< SEQUENCECOUNT; i++){
      sequenceArray[i].activeStep = 0;
      sequenceArray[i].zeroBeat = 0;
    }
  }
  playing = 0;

}

void MidiModule::midiNoteOffHandler(byte channel, byte note, byte velocity){
  Serial.println(String("Note Off: ch=") + channel + ", note=" + note + ", velocity=" + velocity);
}

void MidiModule::midiNoteOnHandler(byte channel, byte note, byte velocity){
  if (velocity > 0) {
    Serial.println(String("Note On:  ch=") + channel + ", note=" + note + ", velocity=" + velocity);
  } else {
    Serial.println(String("Note Off: ch=") + channel + ", note=" + note);
  }
}

void MidiModule::midiStartContinueHandler(){
  if (clockMode == EXTERNAL_MIDI_CLOCK) {
    Serial.println("Midi Start / Continue");

    testTimer = 0;
    firstRun = true;
    startTime = 0;
    masterPulseCount = 0;
  }
}

void MidiModule::midiClockPulseHandler(){
  if (clockMode != EXTERNAL_MIDI_CLOCK) {
    return; // no need to run clock pulse handler if using internal clock.
  }
    // If the time since the last midi pulse is too long, beatLength should not be changed.
    if (pulseTimer > MIDI_PULSE_TIMEOUT) {
      masterPulseCount = MIDI_PULSE_COUNT-1;
      masterTempoTimer = beatLength;
      Serial.println("pulse Timer exceeded timeout: " + String(pulseTimer));
    }
    avgPulseTimer = (avgPulseTimer*9 + (int)pulseTimer)/10;

    pulseTimer = 0; // pulse timer needs to be reset after beatLength calculations

    // Keep track of how many midi clock pulses have been received since the last beat -> 1 beat = 24 pulses
    masterPulseCount = (masterPulseCount + 1) % MIDI_PULSE_COUNT;

    if (firstRun){
        Serial.println("First run! ---->-._._>>>>>>____>_>_>_>_>>>>>>>>__>__>>___>_>_>>_>___>__> MPC: " + String(masterPulseCount));
        playing = 1;
        firstRun = false;
        //beatPulseIndex = masterPulseCount;
        masterPulseCount = 0;
        masterTempoTimer = 0;
        for (int i=0; i< SEQUENCECOUNT; i++){
          sequenceArray[i].clockStart(startTime);
          sequenceArray[i].beatPulse(beatLength);
        }
    } else {

      //if (masterPulseCount == beatPulseIndex){
      if (masterPulseCount == 0){
          //this gets triggered every quarter note
        beatLength = (beatLength+ masterTempoTimer)/2;
        masterTempoTimer = 0;

        Serial.println("masterPulseCount is 0 masterTempoTimer: " + String((int)masterTempoTimer) + "\tactiveStep: " + String(sequenceArray[0].activeStep) + "\tbeatLength:" + String(beatLength) + "\tBPIndex: " + String(masterPulseCount) + "\tTempo: " + String(int(6000000000 / beatLength) ) + "\tAvgPulseTimer: " + String(avgPulseTimer) );

        if (queuePattern != currentPattern) {
          //changePattern(queuePattern, true, true);
        }
        for (int i=0; i< SEQUENCECOUNT; i++){
          sequenceArray[i].beatPulse(beatLength);
        }

      }
    }

    if (playing == 1){
      for (int i=0; i< SEQUENCECOUNT; i++){
        //sequenceArray[i].ppqPulse(MIDI_PULSE_COUNT);
      }

    }
//    Serial.println("Midi Clock - mpc: " + String(masterPulseCount) + "\ttempotimer: " + String(masterTempoTimer) + "\tbeatLength: " + String(beatLength) + "\tbeatPulseIndex: " + String(beatPulseIndex));
}
