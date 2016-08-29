#include <Arduino.h>

#include "midiModule.h"


void MidiModule::midiSetup(midi::MidiInterface<HardwareSerial>* serialMidi, Sequencer *sequenceArray, NoteDatum *noteData){
  serialMidi->begin(MIDI_CHANNEL_OMNI);
  this->sequenceArray = sequenceArray;
  this->serialMidi = serialMidi;
  this->noteData = noteData;

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
  if (extClock == true) {
    Serial.println("Midi Start / Continue");

    testTimer = 0;
    playing = 1;
    firstRun = true;
    startTime = 0;
  }
}

void MidiModule::midiClockPulseHandler(){
  if (extClock == false) {
    return; // no need to run clock pulse handler if using internal clock.
  }

/*
If device is receiving clock pulses,
  then set beatLength based upon pulse lengths.

If devices is not receiving any pulses
  beatLength should remain the same.

  How to determine if device is receiving clock pulses?

  If pulse length is greater than a certain amount, the data should be thrown out.



*/

    // If the time since the last midi pulse is too long, beatLength should not be changed.
    if (pulseTimer > MIDI_PULSE_TIMEOUT) {
      masterPulseCount = 23;
      masterTempoTimer = beatLength;
      Serial.println("pulse Timer exceeded timeout: " + String(pulseTimer));
    }
    pulseTimer = 0; // pulse timer needs to be reset after beatLength calculations

    // Keep track of how many midi clock pulses have been received since the last beat -> 1 beat = 24 pulses
    masterPulseCount = (masterPulseCount + 1) % 24;

    if (masterPulseCount == 0) {
      beatLength = (masterTempoTimer + beatLength) / 2;

      Serial.println("setting beatlength: " + String(beatLength ));
      masterTempoTimer = 0;
    }

    if (firstRun){
        firstRun = false;
        beatPulseIndex = masterPulseCount;

        for (int i=0; i< sequenceCount; i++){
          sequenceArray[i].clockStart(startTime);
          sequenceArray[i].beatPulse(beatLength, &life);
        }

    } else {
      if (masterPulseCount == beatPulseIndex){
        //this gets triggered every quarter note
        if (queuePattern != currentPattern) {
  //          changePattern(queuePattern, true, true);
        }

        for (int i=0; i< sequenceCount; i++){
          sequenceArray[i].beatPulse(beatLength, &life);
        }

      }
    }





//    Serial.println("Midi Clock - mpc: " + String(masterPulseCount) + "\ttempotimer: " + String(masterTempoTimer) + "\tbeatLength: " + String(beatLength) + "\tbeatPulseIndex: " + String(beatPulseIndex));


}
