#include <Arduino.h>
#include "MasterClock.h"

void MasterClock::initialize(OutputController * outputControl, Sequencer *sequenceArray, NoteDatum *noteData, midi::MidiInterface<HardwareSerial>* serialMidi, MidiModule *midiControl){
	Serial.println("Initializing Master Clock");
	this->sequenceArray = sequenceArray;
	this->outputControl = outputControl;
	this->noteData = noteData;
	this->serialMidi = serialMidi;
	this->midiControl = midiControl;
	Serial.println("Master Clock Initialized");
	lfoTimer = 0;
};

void MasterClock::changeTempo(uint32_t newTempoX100){
	tempoX100 = newTempoX100;
	beatLength = 60000000/(tempoX100/100);
  for (int i = 0; i < SEQUENCECOUNT; i++ ){
    sequenceArray[i].setTempo(tempoX100);
  }
}

void MasterClock::masterClockFunc(void){
 //elapsedMicros loopTimer = 0;

//  avgInterval =((micros() - lastMicros) + 9* avgInterval) / 10;
//  timerAvg = (lastTimer + 9*timerAvg) /10;
//  lastMicros = micros();
//
//  intervalJitter = (abs(int(avgInterval) - int(lastAvgInterval)));
//  avgIntervalJitter = (intervalJitter * 9 + avgIntervalJitter) / 10;
//  lastAvgInterval = avgInterval;
	midiControl->midiClockSyncFunc(serialMidi);

  if(playing){
    if( extClock == false ){
      internalClockTick();
    } else {
      externalClockTick();
    }
		if(lfoTimer > 10){
			for (int i=0; i< SEQUENCECOUNT; i++){
				outputControl->lfoUpdate(i);
			}
			lfoTimer = 0;
		}
    noteOffSwitch();
    noteOnSwitch();
  }

	if (outputControl->clockOutputTimer > 2) {
		outputControl->setClockOutput(LOW);
	}
  wasPlaying = playing;
  //lastTimer = loopTimer;

}


void MasterClock::internalClockTick(){
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

   for (int i=0; i<4; i++){
 		 outputControl->allNotesOff(i);
 		 noteData[i].noteOn = false;
 		 noteData[i].noteOff = false;
 		 for (int n=0; n<MAX_STEPS_PER_SEQUENCE; n++){
 		   	noteData[i].noteOffArray[n] = NULL;
 				noteData[i].noteOnArray[n] = NULL;
 		 }
 	 }
		outputControl->setClockOutput(HIGH);

    for (int i=0; i< SEQUENCECOUNT; i++){
      sequenceArray[i].clockStart(startTime);
      sequenceArray[i].beatPulse(beatLength, &life);
      sequenceArray[i].runSequence(&noteData[i], &life);
    }
  } else if (internalClockTimer > 60000000/(tempoX100/100)){
       // Serial.print(" b4 ");
    if (queuePattern != currentPattern) {
      //changePattern(queuePattern, true, true);
    }
    for (int i=0; i< SEQUENCECOUNT; i++){
      sequenceArray[i].runSequence(&noteData[i], &life);
      sequenceArray[i].beatPulse(beatLength, &life);

    }
		outputControl->setClockOutput(HIGH);
    internalClockTimer = 0;

  }  else {
    for (int i=0; i< SEQUENCECOUNT; i++){
      sequenceArray[i].runSequence(&noteData[i], &life);
    }
  }

	debug("end internal clock tick");
  //digitalWriteFast(DEBUG_PIN, LOW);

}

void MasterClock::externalClockTick(){
  // ext clock sync

	  for (int i=0; i< SEQUENCECOUNT; i++){
	    sequenceArray[i].runSequence(&noteData[i], &life);
	  }
}

void MasterClock::noteOffSwitch(){
  for (int i=0; i< SEQUENCECOUNT; i++){
    if (noteData[i].noteOff == true){
      for (int n=0; n< MAX_STEPS_PER_SEQUENCE; n++){
        if (!noteData[i].noteOffArray[n]){
          continue;
        }

				outputControl->noteOff(noteData[i].channel, noteData[i].noteOffArray[n],noteData[i].noteGateOffArray[n] );
      }
    }
  }
}

void MasterClock::noteOnSwitch(){

  for (int i=0; i< SEQUENCECOUNT; i++){
    if (noteData[i].noteOn == true){
      for (int n=0; n< MAX_STEPS_PER_SEQUENCE; n++){
        if (!noteData[i].noteOnArray[n]){
          continue;
        }
				outputControl->noteOn(noteData[i].channel,noteData[i].noteOnArray[n],noteData[i].noteVelArray[n],noteData[i].noteVelTypeArray[n], noteData[i].noteLfoSpeed[n], noteData[i].noteGlideArray[n], noteData[i].noteGateArray[n] );
      }
    }
  }
}
