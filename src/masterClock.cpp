#include <Arduino.h>
#include "MasterClock.h"

void MasterClock::initialize(OutputController * outputControl, Sequencer *sequenceArray, NoteDatum *noteData, midi::MidiInterface<HardwareSerial>* serialMidi, MidiModule *midiControl){
	Serial.println("Initializing Master Clock");
	this->sequenceArray = sequenceArray;
	this->outputControl = outputControl;
	this->noteData = noteData;
	this->serialMidi = serialMidi;
	this->midiControl = midiControl;
	gatePrevState[0] = false;
	gatePrevState[1] = false;
	gatePrevState[2] = false;
	gatePrevState[3] = false;
	clickCounter = 0;
	firstRun = false;
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
	outputControl->setClockOutput(HIGH);

 elapsedMicros masterClockDebugTimer = 0;

//  avgInterval =((micros() - lastMicros) + 9* avgInterval) / 10;
//  timerAvg = (lastTimer + 9*timerAvg) /10;
//  lastMicros = micros();
//
//  intervalJitter = (abs(int(avgInterval) - int(lastAvgInterval)));
//  avgIntervalJitter = (intervalJitter * 9 + avgIntervalJitter) / 10;
//  lastAvgInterval = avgInterval;
if(masterDebugSwitch == HIGH){
	Serial.println("MASTERDEBUG HIGH!");
}
	masterDebugSwitch = HIGH;

	masterDebugCounter++;

//	midiControl->midiClockSyncFunc(serialMidi);

  if(playing){
		switch(clockMode){
	    case INTERNAL_CLOCK:
				internalClockTick();
	    	break;
	    case EXTERNAL_MIDI_CLOCK:
				midiClockTick();
		    break;
			case EXTERNAL_CLOCK_GATE_0:
				externalClockTick(0);
				break;
	    case EXTERNAL_CLOCK_GATE_1:
				externalClockTick(1);
			  break;
	    case EXTERNAL_CLOCK_GATE_2:
				externalClockTick(2);
		    break;
	    case EXTERNAL_CLOCK_GATE_3:
				externalClockTick(3);
			  break;
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
	masterDebugSwitch = LOW;
	outputControl->setClockOutput(LOW);

  masterClockDebugValue = ((int)masterClockDebugTimer + masterClockDebugValue*9)/10 ;
	if ( (int)masterClockDebugTimer > masterClockDebugHigh){
		masterClockDebugHigh = (int)masterClockDebugTimer;
	}
	if (masterClockDebugTimer2 > 3000000){
		masterClockDebugTimer2 = 0;
		Serial.println("$%^&^%$%^&^%$%^%$#$%^&^%$#$%^&  Masterclock interval: " + String(masterClockDebugValue) + "\thigh val; " + String(masterClockDebugHigh));
		masterClockDebugHigh = 0;
	}
}

bool MasterClock::gateTrigger(uint8_t gateNum){

}

void MasterClock::checkGateClock(){
	for (int i =0; i <4; i++){
		if (gateInputRaw[i] == 0 && gatePrevState[i] != gateInputRaw[i] ){
			gateTrig[i] = true;
		} else {
			gateTrig[i] = false;
		};
		gatePrevState[i] = gateInputRaw[i];
	}
}

void MasterClock::externalClockTick(uint8_t gateNum){
	checkGateClock();

	if (gateTrig[gateNum]){
			Serial.println("gate trig!");
	    // If the time since the last midi pulse is too long, beatLength should not be changed.
	    if (pulseTimer > GATE_CLOCK_TIMEOUT) {
	      masterPulseCount = 4;
	      masterTempoTimer = beatLength;
	      Serial.println("pulse Timer exceeded timeout: " + String(pulseTimer));
	    }
	    pulseTimer = 0; // pulse timer needs to be reset after beatLength calculations

	    // Keep track of how many midi clock pulses have been received since the last beat -> 1 beat = 24 pulses
	    masterPulseCount = (masterPulseCount + 1) % 4;

	    if (firstRun){
	        firstRun = false;
	        beatPulseIndex = masterPulseCount;
    			masterTempoTimer = 0;
	        for (int i=0; i< SEQUENCECOUNT; i++){
	          sequenceArray[i].clockStart(startTime);
	          sequenceArray[i].beatPulse(beatLength);
	        }
	    } else {
	      if (masterPulseCount == beatPulseIndex){
	        //this gets triggered every quarter note
					beatLength = (masterTempoTimer + beatLength) / 2;
					Serial.println("setting beatlength: " + String(beatLength ));
					masterTempoTimer = 0;

	        if (queuePattern != currentPattern) {
	  //          changePattern(queuePattern, true, true);
	        }

	        for (int i=0; i< SEQUENCECOUNT; i++){
	          sequenceArray[i].beatPulse(beatLength);
	        }

	      }
	    }


		}

		for (int i=0; i< SEQUENCECOUNT; i++){
			sequenceArray[i].runSequence(&noteData[i]);
		}

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
      sequenceArray[i].beatPulse(beatLength);
      sequenceArray[i].runSequence(&noteData[i]);
    }
  } else if (internalClockTimer > 60000000/(tempoX100/100)){
       // Serial.print(" b4 ");
	  masterTempoTimer = 0;
    if (queuePattern != currentPattern) {
      //changePattern(queuePattern, true, true);
    }
    for (int i=0; i< SEQUENCECOUNT; i++){
			sequenceArray[i].beatPulse(beatLength);
      sequenceArray[i].runSequence(&noteData[i]);
    }
		outputControl->setClockOutput(HIGH);
    internalClockTimer = 0;

  }  else {
    for (int i=0; i< SEQUENCECOUNT; i++){
      sequenceArray[i].runSequence(&noteData[i]);
    }
  }

	debug("end internal clock tick");

  //digitalWriteFast(DEBUG_PIN, LOW);
}

void MasterClock::midiClockTick(){
  // ext clock sync
		clickCounter = (clickCounter + 1) % SEQUENCECOUNT;
	//	sequenceArray[clickCounter].runSequence(&noteData[clickCounter]);

	  for (int i=0; i< SEQUENCECOUNT; i++){
			sequenceArray[i].runSequence(&noteData[i]);
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

				//Serial.print(String(noteData[i].noteOnStep) + "\t"  + String(noteData[i].noteGlideArray[n]) + "\t") ;
				outputControl->noteOn(noteData[i].channel,noteData[i].noteOnArray[n],noteData[i].noteVelArray[n],noteData[i].noteVelTypeArray[n], noteData[i].noteLfoSpeed[n], noteData[i].noteGlideArray[n], noteData[i].noteGateArray[n] );

      }
    }
  }
}
