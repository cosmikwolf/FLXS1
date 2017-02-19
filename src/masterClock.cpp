#include <Arduino.h>
#include "MasterClock.h"

void MasterClock::initialize(OutputController * outputControl, Sequencer *sequenceArray, midi::MidiInterface<HardwareSerial>* serialMidi, MidiModule *midiControl){
	Serial.println("Initializing Master Clock");
	this->sequenceArray = sequenceArray;
	this->outputControl = outputControl;
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

void MasterClock::clockRunCheck(){
	if ((int)masterLoopTimer > kMasterClockInterval + 100){
		masterClockFunc();
	}
}

void MasterClock::masterClockFunc(){

	//	Serial.println(String((int)masterLoopTimer));
	//  masterLooptimeMin
	//  masterLooptimeMax

	uint32_t clockPeriod = (60000000/(tempoX100/100) )/(INTERNAL_PPQ_COUNT);
	if (clockMode == INTERNAL_CLOCK){

		if ((int)masterLoopTimer > kMasterClockInterval + 100){
			uint8_t countToAdd = (int)masterLoopTimer / kMasterClockInterval;
			clockCounter = clockCounter + countToAdd;
			//Serial.println("Adding " + String((int)masterLoopTimer / kMasterClockInterval) + " counts to the clock: " + String((int)masterLoopTimer) );
		//	for(int i=0;  i< countToAdd; i++){
		//		digitalWriteFast(PIN_EXT_AD_2, LOW);delayMicroseconds(10);
		//		digitalWriteFast(PIN_EXT_AD_2, HIGH);delayMicroseconds(10);
		//	}
		} else {
			clockCounter++;
		}

		// Make sure the LEDs do not refresh right before the clock needs to be triggered.
		if (extClockCounter > EXTCLOCKDIV - 1  && clockCounter * kMasterClockInterval>  clockPeriod - 3000){
			digitalWriteFast(PIN_EXT_AD_2, LOW);
			ledRunSwitch = false;
		}


		if (clockCounter * kMasterClockInterval >  clockPeriod){
			extClockCounter++;
			if( extClockCounter > EXTCLOCKDIV ){
				outputControl->setClockOutput(HIGH);
				extClockCounter = 0;
			}
			clockCounter = 0;
			pulseTrigger = 1;
		}



	}
	masterLoopTimer = 0;

	if (clockCounter > INTERNAL_PPQ_COUNT/2 && outputControl->clockValue) {
		outputControl->setClockOutput(LOW);
		ledRunSwitch = true;
		digitalWriteFast(PIN_EXT_AD_2, HIGH);
	}

	//digitalWriteFast(PIN_EXT_AD_2, LOW);

//  masterLooptimeAvg

};

void MasterClock::sequencerFunc(void){
//	digitalWriteFast(PIN_EXT_AD_2, HIGH);

	outputControl->inputRead();

	for(int i=0; i < 4; i++){
		for(int n=0; n<4; n++){
			if (gateInputRose[i] == true){
				sequenceArray[n].gateInputTrigger(i);
			}
		}
	}
	//	outputControl->setClockOutput(gateInputRaw[0]);
	//  avgInterval =((micros() - lastMicros) + 9* avgInterval) / 10;
	//  timerAvg = (lastTimer + 9*timerAvg) /10;
	//  lastMicros = micros();
	//
	//  intervalJitter = (abs(int(avgInterval) - int(lastAvgInterval)));
	//  avgIntervalJitter = (intervalJitter * 9 + avgIntervalJitter) / 10;
	//  lastAvgInterval = avgInterval;

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

  }

  wasPlaying = playing;

//	digitalWriteFast(PIN_EXT_AD_2, LOW);

}

bool MasterClock::gateTrigger(uint8_t gateNum){
}

void MasterClock::checkGateClock(){
	for (int i =0; i <4; i++){
		if (gateInputRaw[i] == 0 && gatePrevState[i] == 1 ){
			gateTrig[i] = true;
		} else {
			gateTrig[i] = false;
		};
		gatePrevState[i] = gateInputRaw[i];
	}
}

void MasterClock::externalClockTick(uint8_t gateNum){
	checkGateClock();

	if (playing){
		if (wasPlaying == false){
				for (int i=0; i< SEQUENCECOUNT; i++){
					outputControl->allNotesOff(i);
					sequenceArray[i].clockStart(startTime);
					Serial.println("Starting sequence: " + String(i));
				}
		}

		if (gateTrig[gateNum]){
			//Serial.print("PPQPULSE: ");
			for (int i=0; i< SEQUENCECOUNT; i++){
				sequenceArray[i].ppqPulse(4);
			}
		}
	}

	for (int i=0; i< SEQUENCECOUNT; i++){
		sequenceArray[i].runSequence();
	}
	ledRunSwitch = true;

}

void MasterClock::internalClockTick(){
 //digitalWriteFast(DEBUG_PIN, HIGH);
  debug("begin internal clock tick");
        // int clock

  if (wasPlaying == false){
        // if playing has just re-started, the master tempo timer and the master beat count must be reset
   // MIDI.send(Start, 0, 0, 1);  // MIDI.sendSongPosition(0);
    masterPulseCount = 0;
    startTime = 0;

    for (int i=0; i< SEQUENCECOUNT; i++){
			outputControl->allNotesOff(i);
    	sequenceArray[i].clockStart(startTime);
    }
  }

	if ( pulseTrigger ){

		Serial.println("Pulsetrigger " + String(pulseTimer));
		pulseTimer = 0;
		for (int i=0; i< SEQUENCECOUNT; i++){
			sequenceArray[i].ppqPulse(INTERNAL_PPQ_COUNT);
		}
		pulseTrigger = 0;
  }

  for (int i=0; i< SEQUENCECOUNT; i++){
    sequenceArray[i].runSequence();
  }


	debug("end internal clock tick");

  //digitalWriteFast(DEBUG_PIN, LOW);
}

void MasterClock::midiClockTick(){
  // ext clock sync
	  for (int i=0; i< SEQUENCECOUNT; i++){
			sequenceArray[i].runSequence();
	  }
}
/*
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

*/
