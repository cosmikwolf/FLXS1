#include <Arduino.h>
#include "masterClock.h"


masterClock::masterClock(){

};

void masterClock::changeTempo(uint32_t newTempoX100){
	tempoX100 = newTempoX100;
	beatLength = 60000000/(tempoX100/100);
  for (int i = 0; i < sequenceCount; i++ ){
    sequence[i].setTempo(tempoX100);
  }
}

void masterClock::masterClockFunc(void){
 elapsedMicros loopTimer = 0;


if (inputTimer > 10000){
  inputTimer = 0;
}
//  avgInterval =((micros() - lastMicros) + 9* avgInterval) / 10;
//  timerAvg = (lastTimer + 9*timerAvg) /10;
//  lastMicros = micros();
//
//  intervalJitter = (abs(int(avgInterval) - int(lastAvgInterval)));
//  avgIntervalJitter = (intervalJitter * 9 + avgIntervalJitter) / 10;
//  lastAvgInterval = avgInterval;

  if(playing){

    if( extClock == false ){
      internalClockTick();
    } else {
      externalClockTick();
    }

    noteOffSwitch();

    noteOnSwitch();

  }

  wasPlaying = playing;
  lastTimer = loopTimer;

//if (pixelTimer > 2000){
//  pixelTimer = 0;
//}
  midiClockSyncFunc();
}


void masterClock::internalClockTick(){
 //digitalWriteFast(DEBUG_PIN, HIGH);

  debug("begin internal clock tick");
        // int clock
  if (wasPlaying == false){
        // if playing has just re-started, the master tempo timer and the master beat count must be reset
   // MIDI.send(Start, 0, 0, 1);  // MIDI.sendSongPosition(0);
    masterTempoTimer = 0;
    masterPulseCount = 0;
    internalClockTimer = 0;
    startTime = 0;

    for (int i=0; i< sequenceCount; i++){
      sequence[i].clockStart(startTime);
      sequence[i].beatPulse(beatLength, &life);
      sequence[i].runSequence(&noteData[i], &life);
    }
  } else if (internalClockTimer > 60000000/(tempoX100/100)){
       // Serial.print(" b4 ");
    if (queuePattern != currentPattern) {
      //changePattern(queuePattern, true, true);
    }
    for (int i=0; i< sequenceCount; i++){
      sequence[i].runSequence(&noteData[i], &life);
      sequence[i].beatPulse(beatLength, &life);
    }
    tempoBlip = !tempoBlip;
    internalClockTimer = 0;

       // Serial.print(" b5 ");
  }  else {
    for (int i=0; i< sequenceCount; i++){
      sequence[i].runSequence(&noteData[i], &life);
    }
  }
    debug("end internal clock tick");
  //digitalWriteFast(DEBUG_PIN, LOW);

}

void masterClock::externalClockTick(){
  // ext clock sync
  for (int i=0; i< sequenceCount; i++){
    sequence[i].runSequence(&noteData[i], &life);
  }
}

void masterClock::noteOffSwitch(){
  for (int i=0; i< sequenceCount; i++){
    if (noteData[i].noteOff == true){
      for (int n=0; n< 16; n++){
        if (!noteData[i].noteOffArray[n]){
          continue;
        }
         // noteOn(noteData[i].channel,noteData[i].noteOffArray[n]);
       // mcp.digitalWrite(noteData[i].channel+4, LOW);
        mcp.digitalWrite(gateMap[noteData[i].channel], LOW);

        MIDI.sendNoteOff(noteData[i].noteOffArray[n], 64, noteData[i].channel);
        sam2695.noteOff(noteData[i].channel, noteData[i].noteOffArray[n]);
         // usbMIDI.sendNoteOff(noteData[i].noteOffArray[n], 64, noteData[i].channel);
          //Serial.println("noteOff: " + String(noteData[i].noteOffArray[n]) + "\tbt: " + String(sequence[selectedChannel].beatTracker) ) ;
      }
    }
  }
}

void masterClock::noteOnSwitch(){
    debug( "\tbegin note on switch");

  for (int i=0; i< sequenceCount; i++){
    if (noteData[i].noteOn == true){
      for (int n=0; n< 128; n++){
        if (!noteData[i].noteOnArray[n]){
          continue;
        }
    digitalWriteFast(DEBUG_PIN, HIGH);

// calibration numbers: 0v: 22180   5v: 43340

        ad5676.setVoltage(dacCvMap[noteData[i].channel],  map(noteData[i].noteOnArray[n], 0,127,13716, 58504 ) );
       // digitalWriteFast(15, HIGH);

        MIDI.sendNoteOn(noteData[i].noteOnArray[n], noteData[i].noteVelArray[n], noteData[i].channel);
        sam2695.noteOn(noteData[i].channel, noteData[i].noteOnArray[n], noteData[i].noteVelArray[n]);
        ad5676.setVoltage(dacCcMap[noteData[i].channel],  map(noteData[i].noteVelArray[n], 0,127,0, 43340 ) );

      //  Serial.println("Note Data: " + String(noteData[i].noteOnArray[n]) + " \t\tsetting voltage to: " + String( map(noteData[i].noteOnArray[n], 0,127,0, 65535 )));
        //mcp.digitalWrite(noteData[i].channel+4, HIGH);

        mcp.digitalWrite(gateMap[noteData[i].channel], HIGH);
//        Serial.println("i: " + String(i) + "\tn: " + String(n) + "\tchan: " + String(noteData[i].channel) + "\tnoteOn: " + String(noteData[i].noteOnArray[n]) + "\tvel: " + noteData[i].noteVelArray[n] );
        //digitalWriteFast(15, LOW);

        /*
        digitalWriteFast(DEBUG_PIN, HIGH);

         Serial.println("triggering Note");
         Serial.println( "ch: " + String(noteData[i].channel)
         // + "\tbt: " + String(sequence[selectedChannel].beatTracker)
          + "\tnoteOn: " + String(noteData[i].noteOnArray[n])
          + "\tvel: " + String(map(noteData[i].noteVelArray[n], 0,127,22180, 43340 ))
          + "\tgate: " + String(gateMap[noteData[i].channel])
          + "\tcv: " + String(dacCvMap[noteData[i].channel])
          + "\tcc: " + String(dacCcMap[noteData[i].channel]) );
        digitalWriteFast(DEBUG_PIN, LOW);
        */
    digitalWriteFast(DEBUG_PIN, LOW);

      }
    }
  }
      debug("end note on switch");

}
