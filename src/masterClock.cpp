#include <Arduino.h>
#include "MasterClock.h"

void MasterClock::initialize(OutputController * outputControl, Sequencer *sequenceArray, midi::MidiInterface<HardwareSerial>* serialMidi, MidiModule *midiControl, GlobalVariable* globalObj){
	Serial.println("Initializing Master Clock");
	this->sequenceArray = sequenceArray;
	this->outputControl = outputControl;
	this->serialMidi = serialMidi;
	this->midiControl = midiControl;
	this->globalObj = globalObj;
	this->displayRunSwitch = true;
	gatePrevState[0] = false;
	gatePrevState[1] = false;
	gatePrevState[2] = false;
	gatePrevState[3] = false;
	this->totalClockCount = 0;
	Serial.println("Master Clock Initialized");
	lfoTimer = 0;
};

void MasterClock::changeTempo(uint32_t newtempoX100){
	globalObj->tempoX100 = newtempoX100;
	// beatLength = 58962000/(globalObj->tempoX100/100);

	//uint32_t clockPeriod = (58962000/(globalObj->tempoX100/100) )/(INTERNAL_PPQ_COUNT);
	// uint32_t clockPeriod = (60000000/(globalObj->tempoX100/100) )/(INTERNAL_PPQ_COUNT);

	// clockCounter = clockCounter % (clockPeriod/kMasterClockInterval);
	// totalClockCount = 0;
}

void MasterClock::masterClockFunc(){
	//	Serial.println(String((int)masterLoopTimer));
	//  masterLooptimeMin
	//  masterLooptimeMax
	digitalWriteFast(PIN_EXT_TX, HIGH);

	//uint32_t clockPeriod = (60000000/(globalObj->tempoX100/100) )/(INTERNAL_PPQ_COUNT);
	//clockPeriod = 550;
	//uint32_t clockPeriod = (58962000/(globalObj->tempoX100/100) )/(INTERNAL_PPQ_COUNT);

	uint32_t clockPeriod = 120000000;
	clockPeriod /= INTERNAL_PPQ_COUNT;
	clockPeriod /= (globalObj->tempoX100/100);
//	clockPeriod /= 1024;
	clockPeriod *= 60;

	uint32_t clockCycles = ARM_DWT_CYCCNT;

	if (globalObj->clockMode == INTERNAL_CLOCK){
		for (int i = 0; i < SEQUENCECOUNT; i++ ){
			sequenceArray[i].masterClockPulse();
		}
		globalObj->tapTempoMasterClkCounter++;
		// clockCounter++;

		// Make sure the LEDs do not refresh right before the clock needs to be triggered.
		if (extClockCounter > EXTCLOCKDIV / 2){
			//digitalWriteFast(PIN_EXT_AD_2, LOW);
			//displayRunSwitch = false;
			clearedToRunLoadOperation = false;
		} else {
      displayRunSwitch = true;
    }

		if(clockCycles - lastPulseClockCount > clockPeriod){
			extClockCounter++;

			if( extClockCounter >= EXTCLOCKDIV && globalObj->playing){
				outputControl->setClockOutput(HIGH);
				clearedToRunLoadOperation = true;
				digitalWriteFast(PIN_EXT_AD_3, HIGH);
				serialMidi->sendRealTime(midi::Clock);
				extClockCounter = 0;
				//Serial.println("Clock Fire debugTimer: " + String(masterDebugTimer) + "\tclockPeriod: " + String(clockPeriod) + "\tclockCounter: " + String(clockCounter) + "\tinterval:" + String(kMasterClockInterval) + "\ttotalTimer: " + String(clockCounter * kMasterClockInterval) + "\ttotalClockCount: " + String(totalClockCount));
				//masterDebugTimer = 0;
			  digitalWriteFast(PIN_EXT_AD_3, LOW);
			}
			//
			// totalClockCount++;
			// if(totalClockCount > INTERNAL_PPQ_COUNT){
			// 	clockCounter = clockCounter % (clockPeriod/kMasterClockInterval);
			// 	totalClockCount = 0;
			// }
			pulseTrigger = 1; //send ppq pulse to each internally sequenced sequence

			lastPulseClockCount += clockPeriod;
		}

		if(extClockCounter >= 15*EXTCLOCKDIV/16 && (extClockCounter < EXTCLOCKDIV) ){
			clearedToRunLoadOperation = false; //keep load operation from happening 1/16 of a clock cyle before it rises
		}
		if((extClockCounter >= 7*EXTCLOCKDIV/16) && (extClockCounter < EXTCLOCKDIV / 2) ){
			clearedToRunLoadOperation = false;//keep load operation from happening 1/16 of a clock cyle before it falls
		}
		if ((extClockCounter >= EXTCLOCKDIV / 2) && outputControl->clockValue) {
				outputControl->setClockOutput(LOW);
				displayRunSwitch = true;
				clearedToRunLoadOperation = true;
				digitalWriteFast(PIN_EXT_AD_2, HIGH);
		}


	} else if(globalObj->clockMode == EXTERNAL_MIDI_35_CLOCK || globalObj->clockMode == EXTERNAL_MIDI_USB_CLOCK ){
		clearedToRunLoadOperation = true;

		if (globalObj->midiSetClockOut && !outputControl->clockValue){
			outputControl->setClockOutput(HIGH);
		}
		if (outputControl->clockValue && !globalObj->midiSetClockOut) {
			outputControl->setClockOutput(LOW);
			displayRunSwitch = true;
			digitalWriteFast(PIN_EXT_AD_2, HIGH);
		}


		for (int i = 0; i < SEQUENCECOUNT; i++ ){
			sequenceArray[i].masterClockPulse();
		}
  } else if(globalObj->clockMode >= EXTERNAL_CLOCK_GATE_0){
		clearedToRunLoadOperation = true;

		for (int i = 0; i < SEQUENCECOUNT; i++ ){
			sequenceArray[i].masterClockPulse();
		}
	}

};

void MasterClock::sequencerFunc(void){
//	digitalWriteFast(PIN_EXT_AD_2, HIGH);

	outputControl->inputRead();
	this->songAndPatternLogic();

	if(currentMenu == CALIBRATION_MENU){
		globalObj->playing = 0;
		outputControl->dacTestLoop();
		return;
	}

	for(int i=0; i < 8; i++){
		for(int n=0; n<4; n++){
			if (globalObj->gateInputRose[i] == true){
				sequenceArray[n].gateInputTrigger(i+1); // add one to accomodate for mapping
			}
		}
	}
	//	outputControl->setClockOutput(globalObj->gateInputRaw[0]);
	//  avgInterval =((micros() - lastMicros) + 9* avgInterval) / 10;
	//  timerAvg = (lastTimer + 9*timerAvg) /10;
	//  lastMicros = micros();
	//
	//  intervalJitter = (abs(int(avgInterval) - int(lastAvgInterval)));
	//  avgIntervalJitter = (intervalJitter * 9 + avgIntervalJitter) / 10;
	//  lastAvgInterval = avgInterval;

	//	midiControl->midiClockSyncFunc(serialMidi);
	if (globalObj->playing && !globalObj->wasPlaying){
		//generate random seed
		globalObj->generateRandomNumber(0, 65535);
	}
	switch(globalObj->clockMode){
    case INTERNAL_CLOCK:
			internalClockTick();
    	break;
    case EXTERNAL_MIDI_35_CLOCK:
    case EXTERNAL_MIDI_USB_CLOCK:
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
		case EXTERNAL_CLOCK_BIDIRECTIONAL_INPUT:
			externalClockTick(4);
			break;
  }
	if(lfoTimer > 2){
		for (int i=0; i< SEQUENCECOUNT; i++){
			outputControl->cv2update(i, sequenceArray[i].currentFrame, sequenceArray[i].framesPerSequence(), sequenceArray[i].getStepLength(), sequenceArray[i].muteCV2 );
		}
		lfoTimer = 0;
	}
  globalObj->wasPlaying = globalObj->playing;

	if(clearedToRunLoadOperation || !globalObj->playing){
		outputControl->flashMemoryStaggeredLoadLoop();
	}


//	digitalWriteFast(PIN_EXT_AD_2, LOW);
}

void MasterClock::songAndPatternLogic(){
	if (sequenceArray[globalObj->chainModeMasterChannel[globalObj->chainModeIndex]].currentFrame >= sequenceArray[globalObj->chainModeMasterChannel[globalObj->chainModeIndex]].framesPerSequence()-(sequenceArray[globalObj->chainModeMasterChannel[globalObj->chainModeIndex]].getStepLength()*3/4) ){

		if(globalObj->chainModeActive){
 /* moved to clockReset
		if ((sequenceArray[globalObj->chainModeMasterChannel[globalObj->chainModeIndex]].currentFrame < sequenceArray[globalObj->chainModeMasterChannel[globalObj->chainModeIndex]].getStepLength()/3) && (globalObj->chainModeCountSwitch == 1)){
	    globalObj->chainModeCountSwitch = 0;
	  }*/
	    if (!(globalObj->chainModeCountSwitch)){
	    //  Serial.println("incrementing chain mode count on channel " + String(channel));
	      globalObj->chainModeCount[globalObj->chainModeIndex]++;
				CHAINLOGICBEGINNING: ;
	      if(globalObj->chainModeCount[globalObj->chainModeIndex] >= globalObj->chainPatternRepeatCount[globalObj->chainModeIndex]){
	      //  Serial.println("scheduling a pattern change");
	        globalObj->patternChangeTrigger = globalObj->chainModeMasterChannel[globalObj->chainModeIndex] + 1;
	        globalObj->chainModeIndex = (globalObj->chainModeIndex+1)%CHAIN_COUNT_MAX ;
	        if(globalObj->chainPatternRepeatCount[globalObj->chainModeIndex] == -1){ // chain stop
	          globalObj->chainModeActive = false;
	          globalObj->playing = false;
	          // Serial.println("Chain end detected");
	        } else if(globalObj->chainPatternRepeatCount[globalObj->chainModeIndex] == 0){ // repeat
	          globalObj->chainModeIndex = 0;
	          globalObj->chainModeCount[globalObj->chainModeIndex] = 0;
	          goto QUEUEPATTERN;
	        } else if(globalObj->chainPatternRepeatCount[globalObj->chainModeIndex] < -1){ //chain jump
	          globalObj->chainModeCount[globalObj->chainModeIndex]++;
	          if(globalObj->chainModeCount[globalObj->chainModeIndex] >= abs(globalObj->chainPatternRepeatCount[globalObj->chainModeIndex]+1) ){
	            globalObj->chainModeCount[globalObj->chainModeIndex] = 0;
	            globalObj->chainModeIndex = (globalObj->chainModeIndex+1)%CHAIN_COUNT_MAX ;
	            if(globalObj->chainPatternRepeatCount[globalObj->chainModeIndex] < -1){
	              goto CHAINLOGICBEGINNING;
	            } else  if(globalObj->chainPatternRepeatCount[globalObj->chainModeIndex] == -1){ // chain stop
	              globalObj->chainModeActive = false;
	              globalObj->playing = false;
								for(int channel =0; channel < SEQUENCECOUNT; channel++){
									sequenceArray[channel].clockReset(true);
								}
	            } else {
	              globalObj->patternChangeTrigger = globalObj->chainModeMasterChannel[globalObj->chainModeIndex] + 1;
	              goto QUEUEPATTERN;
	            }
	          } else {
	            globalObj->chainModeCount[globalObj->chainModeIndex]++;
	            globalObj->chainModeIndex = globalObj->chainPatternSelect[globalObj->chainModeIndex];
	            goto QUEUEPATTERN;
	          }
	        } else {
	          QUEUEPATTERN: ;
	          globalObj->chainModeCount[globalObj->chainModeIndex] = 0;
	          globalObj->queuePattern = globalObj->chainPatternSelect[globalObj->chainModeIndex];
						globalObj->patternChannelSelector = globalObj->convertBoolToByte(globalObj->chainChannelSelect[0][globalObj->chainModeIndex],
							globalObj->chainChannelSelect[1][globalObj->chainModeIndex],
							globalObj->chainChannelSelect[2][globalObj->chainModeIndex],
							globalObj->chainChannelSelect[3][globalObj->chainModeIndex]);
							// Serial.println(String(millis()) + "\tqueuing pattern " + String(globalObj->chainModeIndex));

	        }
	      }
	      globalObj->chainModeCountSwitch = 1;
	    }


	  }

		// if ((globalObj->patternChangeTrigger == globalObj->chainModeMasterChannel[globalObj->chainModeIndex] + 1)&&(globalObj->queuePattern != 255) ){
		if (globalObj->queuePattern != 255){
		//  Serial.println("changed sequence with ch" + String(channel));
			outputControl->flashMemoryLoadPattern(globalObj->queuePattern, globalObj->patternChannelSelector);
			 // get the pulses remaining from the previous chain mode index, since it has already been incremented
			globalObj->chainModeMasterPulseToGo = sequenceArray[globalObj->chainModeMasterChannel[globalObj->chainModeIndex-1]].pulsesRemaining;
			globalObj->patternLoadOperationInProgress = true;
			globalObj->waitingToResetAfterPatternLoad = true;
			globalObj->queuePattern = 255; //reset queue pattern so it doesn't get continously retriggered
			// activeStepReset = true;

			// Serial.println("Triggering pattern change index: " + String(globalObj->chainModeIndex) + "\tmaster channel: " + String(globalObj->chainModeMasterChannel[globalObj->chainModeIndex]) + "\tpulses2g: " + String(globalObj->chainModeMasterPulseToGo));
		}

	}
}

void MasterClock::checkGateClock(){
	bool clockPortVal;
	for (int i =0; i <5; i++){
		if( i == 4){
			clockPortVal = outputControl->readClockPort();
			if (clockPortVal == 1 && gatePrevState[i] == 0 ){
				gateTrig[i] = true;
			} else {
				gateTrig[i] = false;
			};
			gatePrevState[i] = clockPortVal;
		} else {
			if (globalObj->gateInputRaw[i] == 1 && gatePrevState[i] == 0 ){
				gateTrig[i] = true;
			} else {
				gateTrig[i] = false;
			};
			gatePrevState[i] = globalObj->gateInputRaw[i];
		}
	}
}

void MasterClock::externalClockTick(uint8_t gateNum){
	clearedToRunLoadOperation = true;

	checkGateClock();

	if (globalObj->playing){
		if (!globalObj->wasPlaying){
			for (int i=0; i< SEQUENCECOUNT; i++){
				outputControl->allNotesOff(i);
				//outputControl->clearVelocityOutput(i);
				sequenceArray[i].clockStart();
				//Serial.println("Starting sequence: " + String(i));
			}
		}
	}

		if (gateTrig[gateNum]){
			//Serial.print("PPQPULSE: ");
			globalObj->chainModeMasterPulseToGo--;
			if((globalObj->chainModeMasterPulseToGo <= 0) && globalObj->waitingToResetAfterPatternLoad){
				for (int i=0; i< SEQUENCECOUNT; i++){
					sequenceArray[i].clockReset(true);
				}
				globalObj->waitingToResetAfterPatternLoad = false;
			}
			for (int i=0; i< SEQUENCECOUNT; i++){
				sequenceArray[i].ppqPulse(4);
			}

		}

		if(globalObj->gateInputRaw[gateNum] == 1){
			outputControl->setClockOutput(HIGH);
		} else {
			outputControl->setClockOutput(LOW);
		}


	for (int i=0; i< SEQUENCECOUNT; i++){
		sequenceArray[i].runSequence();
	}
	displayRunSwitch = true;

}

void MasterClock::internalClockTick(){
 //digitalWriteFast(PIN_EXT_TX, HIGH);
 //Serial.println("begin internal clock tick");
        // int clock
	uint16_t clockCycles = ARM_DWT_CYCCNT/65536;

  if (globalObj->playing && !globalObj->wasPlaying){
		serialMidi->sendRealTime(midi::Start);

  	// if playing has just re-started, the master tempo timer and the master beat count must be reset
   // MIDI.send(Start, 0, 0, 1);  // MIDI.sendSongPosition(0);

    for (int i=0; i< SEQUENCECOUNT; i++){
			outputControl->allNotesOff(i);
			//outputControl->clearVelocityOutput(i);
    	sequenceArray[i].clockStart();
			lastClockValue = clockCycles;
			startingClockCount = clockCycles;
			masterClockCycleCount = 0;
    }
  //  Serial.println("Starting sequence - internal clock: ");

  }

	if ( pulseTrigger ){
		//Serial.println("Pulsetrigger " + String(pulseTimer));
		pulseTimer = 0;
		globalObj->chainModeMasterPulseToGo--;
		if((globalObj->chainModeMasterPulseToGo <= 0) && globalObj->waitingToResetAfterPatternLoad){
			for (int i=0; i< SEQUENCECOUNT; i++){
				sequenceArray[i].clockReset(true);
			}
			globalObj->waitingToResetAfterPatternLoad = false;
			} else {
			for (int i=0; i< SEQUENCECOUNT; i++){
				sequenceArray[i].ppqPulse(INTERNAL_PPQ_COUNT);
			}
		}
		pulseTrigger = 0;
  }

  for (int i=0; i< SEQUENCECOUNT; i++){
    sequenceArray[i].runSequence();
  }


//Serial.println("end internal clock tick");

  //digitalWriteFast(PIN_EXT_TX, LOW);
}

void MasterClock::midiClockTick(){
  // ext clock sync
	clearedToRunLoadOperation = true;

	  for (int i=0; i< SEQUENCECOUNT; i++){
			sequenceArray[i].runSequence();
	  }
}
