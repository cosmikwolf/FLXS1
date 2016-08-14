#include <Arduino.h>

#include "midiModule.h"
void MidiModule::midiSetup(midi::MidiInterface<HardwareSerial>* serialMidi, Sequencer *sequenceArray, NoteDatum *noteData){
  serialMidi->begin(MIDI_CHANNEL_OMNI);
  this->sequenceArray = sequenceArray;
  this->serialMidi = serialMidi;
  this->noteData = noteData;
//  serialMidi->setHandleNoteOn( midiNoteOnHandler );
//  serialMidi->setHandleNoteOff( midiNoteOffHandler );
//  serialMidi->setHandleStart( midiStartContinueHandler );
//  serialMidi->setHandleContinue( midiStartContinueHandler );
//  serialMidi->setHandleStop(midiStopHandler);
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
    testTimer = 0;
    playing = 1;
    tempoBlip = !tempoBlip;
    blipTimer = 0;
    firstRun = true;
    startTime = 0;
    masterPulseCount = 0;
    masterTempoTimer = 0;
    for (int i=0; i< sequenceCount; i++){
  //    sequenceArray[i].clockStart(startTime);
    //  sequenceArray[i].beatPulse(beatLength, &life);
    }

    Serial.println("Midi Start / Continue");

    pulseTimer = 0;
  }
}

void MidiModule::midiClockPulseHandler(){

  if (extClock == true) {
    /*
    if (playing){
      Serial.print(" D2 ");
      pulseLength = pulseTimer;
      avgPulseLength = (pulseLength + 23* avgPulseLength) / 24;
      avgPulseJitter = (abs(int(lastPulseLength) - int(pulseLength)) + 23*avgPulseJitter)/24;
      Serial.print(" D3 ");

     // Serial.println(String(masterPulseCount) + "\tlastPulseLength: " + String(lastPulseLength) + "\tpulseTimer: " + String(pulseLength) + "\tjitter: " + String(abs(int(lastPulseLength) - int(pulseLength))) + "\tavgPulseLength: " + String(avgPulseLength) + "\tavgPulseJitter: " + String(avgPulseJitter) + "\tstartTime: " + String(startTime));
      lastPulseLength = pulseLength;
    }
    pulseTimer = 0;
    Serial.print(" D4 ");

    bpm = 60.0/(24.0*(float(masterTempoTimer)/float(masterPulseCount))/1000000.0);
    avgBpm = (9*avgBpm + bpm)/10;
    */
    //Serial.println(" bpm:" + String(bpm)  + "\tavg: " + Sturing(avgBpm) );
    //20bpm = 124825/1
    //120bpm = 20831/1

    // 1 beat = 24 puleses
    // = 24* avg pulse = 24*(masterTempoTimer/(masterPulseCount+1))
    // = 24*(20831/1) = 499944 microseconds per beat /1000000 = .4999 seconds per beat
    if (firstRun){
        firstRun = false;
    }
    //Serial.print(" D5 ");

    masterPulseCount = (masterPulseCount + 1) % 24;
  //  Serial.println("Midi Clock - mpc: " + String(masterPulseCount) + "\ttempotimer: " + String(masterTempoTimer) );

    if (masterPulseCount == 0){
      //this gets triggered every quarter note
      if (queuePattern != currentPattern) {
//          changePattern(queuePattern, true, true);
      }

      beatLength = masterTempoTimer;
      masterTempoTimer = 0;
      tempoBlip = !tempoBlip;
      blipTimer = 0;
      for (int i=0; i< sequenceCount; i++){
        sequenceArray[i].beatPulse(beatLength, &life);
      }
Serial.println("beatPulse - beatlength: " + String(beatLength ));
  //    Serial.println("beatPulse - beatlength: " + String(int(beatLength)) + "\tbeatLengthJitter: " + String(int(lastBeatLength) - int(beatLength))+ "\tavgPulseLength: " + String(avgPulseLength) + "\tavgPulseJitter: " + String(avgPulseJitter));
      lastBeatLength = beatLength;

    }
    //MasterClockFunc();
  }
}
