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

void MasterClock::changeTempo(uint32_t newTempoX100){
	tempoX100 = newTempoX100;
	beatLength = 58962000/(tempoX100/100);

	//uint32_t clockPeriod = (58962000/(tempoX100/100) )/(INTERNAL_PPQ_COUNT);
	uint32_t clockPeriod = (60000000/(tempoX100/100) )/(INTERNAL_PPQ_COUNT);

	clockCounter = clockCounter % (clockPeriod/kMasterClockInterval);
	totalClockCount = 0;
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
	digitalWriteFast(PIN_EXT_TX, HIGH);

	uint32_t clockPeriod = (60000000/(tempoX100/100) )/(INTERNAL_PPQ_COUNT);
	//uint32_t clockPeriod = (58962000/(tempoX100/100) )/(INTERNAL_PPQ_COUNT);
	uint32_t clockCycles = ARM_DWT_CYCCNT;

	if (globalObj->clockMode == INTERNAL_CLOCK){
/*
		clockCycles
		masterClockCycleCount
		startingClockCount
		kMasterClockInterval

		when clock is started,
			startingClockCount is set to clockCycles
			Time since Clock start: clockCycles-startingClockCount
			if (timeSinceClockStart / kMasterClockInterval > masterClockCycleCount ){
				run internal clock loop
				masterClockCycleCount++
			}


		*/
  /*
		uint8_t clocksToAdd = ((clockCycles-startingClockCount) / (kMasterClockInterval*120))-masterClockCycleCount ;
		if (clockCycles-startingClockCount > masterClockCycleCount*kMasterClockInterval*120 ){
			for (int i = 0; i < SEQUENCECOUNT; i++ ){
				sequenceArray[i].masterClockPulse(clocksToAdd);
			}
			globalObj->tempoMasterClkCounter += clocksToAdd;
			clockCounter += clocksToAdd;
			lfoClockCounter += clocksToAdd;
			masterClockCycleCount += clocksToAdd;
		}

		// if(clockCycles > lastClockValue + (120000000)/(96*4)){
		// 	lastClockValue = clockCycles;
		// 	pulseTrigger = 1;
		// }

		// if(masterDebugTimer > 100000 ){
			// masterDebugTimer = 0;
			// Serial.println("Pulse! start: " + String(lastClockValue) + "\tdiff: " + String((unsigned long)clockCycles-(unsigned long)lastClockValue) + "\tcmpval: " +String((unsigned long)masterClockCycleCount * kMasterClockInterval*120) + "\tclockcycles: " + String(clockCycles) + "\tmasterClockCycleCount: " + String(masterClockCycleCount) + "\tclocksToAdd: " + String(clocksToAdd) );
		// }

	//	if(clocksToAdd > 2){
			//Serial.println("additional Clocks!: " + String(startingClockCount) + "\tdiff: " + String((unsigned long)clockCycles-(unsigned long)startingClockCount) + "\tcmpval: " +String((unsigned long)masterClockCycleCount * kMasterClockInterval*120) + "\tclockcycles: " + String(clockCycles) + "\tmasterClockCycleCount: " + String(masterClockCycleCount) + "\tclocksToAdd: " + String(clocksToAdd) );
	//	}

	//if( clockCycles > 4294967295/10 ){
	//	Serial.println("Resetting Cycle Counter");
	//	CPU_RESET_CYCLECOUNTER_MSTR;
	//	startingClockCount = 0;
 	//};

  */

	///*
		if ((int)masterLoopTimer > 3 * kMasterClockInterval /2){
//			if ((int)masterLoopTimer > 2 * kMasterClockInterval-100){
			// a subroutine to add clock counts when the masterLoopTimer was skipped
			// maybe should be 2xmasterclockINterval instead?
			digitalWriteFast(PIN_EXT_AD_4, HIGH);

			uint8_t countToAdd = masterLoopTimer / kMasterClockInterval;
			clockCounter = clockCounter + countToAdd;
			globalObj->tempoMasterClkCounter += countToAdd;
			lfoClockCounter += countToAdd;
			//xSerial.println("Adding " + String(countToAdd) + " counts to the clock: " + String(masterLoopTimer) + "\tinterval: " + String(kMasterClockInterval) + "\tclockCounter: " + String(clockCounter));

			for (int i = 0; i < SEQUENCECOUNT; i++ ){
				sequenceArray[i].masterClockPulse(countToAdd);
			}
			digitalWriteFast(PIN_EXT_AD_4, LOW);

		} else {
			for (int i = 0; i < SEQUENCECOUNT; i++ ){
				sequenceArray[i].masterClockPulse(1);
			}
			globalObj->tempoMasterClkCounter++;
			clockCounter++;
			lfoClockCounter++;
		}
		//	*/

		// Make sure the LEDs do not refresh right before the clock needs to be triggered.
		if (extClockCounter > EXTCLOCKDIV - 1  && clockCounter * kMasterClockInterval>  clockPeriod - 3000){
			digitalWriteFast(PIN_EXT_AD_2, LOW);
			//displayRunSwitch = false;
		} else {
      displayRunSwitch = true;
    }

		if (clockCounter * kMasterClockInterval - totalClockCount*clockPeriod >  clockPeriod){
			// THIS IS WHERE THE CLOCK PROBLEM IS  --- was? think i fixed it.
 			// IT REDUCES EACH CLOCK COUNT TO A MULTIPLE OF CLOCK INTERVAL,
			// SO EACH CLOCK COUNT ADDS AN OFFSET, WHICH ACCUMULATES.
			extClockCounter++;

			if( extClockCounter >= EXTCLOCKDIV && playing){
				outputControl->setClockOutput(HIGH);
				digitalWriteFast(PIN_EXT_AD_3, HIGH);
				serialMidi->sendRealTime(midi::Clock);
				extClockCounter = 0;
				//Serial.println("Clock Fire debugTimer: " + String(masterDebugTimer) + "\tclockPeriod: " + String(clockPeriod) + "\tclockCounter: " + String(clockCounter) + "\tinterval:" + String(kMasterClockInterval) + "\ttotalTimer: " + String(clockCounter * kMasterClockInterval) + "\ttotalClockCount: " + String(totalClockCount));
				//masterDebugTimer = 0;
			  digitalWriteFast(PIN_EXT_AD_3, LOW);
			}
			//
			totalClockCount++;
			if(totalClockCount > INTERNAL_PPQ_COUNT){
				clockCounter = clockCounter % (clockPeriod/kMasterClockInterval);
				totalClockCount = 0;
			}
			pulseTrigger = 1;
		}

	} else if(globalObj->clockMode == EXTERNAL_MIDI_CLOCK){
    if ((int)masterLoopTimer > kMasterClockInterval + 100){
      uint32_t countToAdd = (int)masterLoopTimer / kMasterClockInterval;
      lfoClockCounter += countToAdd;
    } else {
      lfoClockCounter++;
    }
		for (int i = 0; i < SEQUENCECOUNT; i++ ){
			sequenceArray[i].masterClockPulse(1);
		}
  }else if(globalObj->clockMode == EXTERNAL_CLOCK_GATE_0){
    if ((int)masterLoopTimer > kMasterClockInterval + 100){
      uint32_t countToAdd = (int)masterLoopTimer / kMasterClockInterval;
      lfoClockCounter += countToAdd;
    } else {
      lfoClockCounter++;
    }
		for (int i = 0; i < SEQUENCECOUNT; i++ ){
			sequenceArray[i].masterClockPulse(1);
		}
	}
	masterLoopTimer = 0;

	if ((extClockCounter >= EXTCLOCKDIV / 2) && outputControl->clockValue) {
			outputControl->setClockOutput(LOW);
			displayRunSwitch = true;
			digitalWriteFast(PIN_EXT_AD_2, HIGH);
	}

	// digitalWriteFast(PIN_EXT_AD_2, LOW);
  // masterLooptimeAvg
	digitalWriteFast(PIN_EXT_TX, LOW);

};

void MasterClock::sequencerFunc(void){
//	digitalWriteFast(PIN_EXT_AD_2, HIGH);

	outputControl->inputRead();

	if(currentMenu == CALIBRATION_MENU){
		playing = 0;
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


	switch(globalObj->clockMode){
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
		case EXTERNAL_CLOCK_BIDIRECTIONAL_INPUT:
			externalClockTick(4);
			break;
  }
	if(lfoTimer > 2){
		for (int i=0; i< SEQUENCECOUNT; i++){
			outputControl->cv2update(i, sequenceArray[i].currentFrame, sequenceArray[i].getStepLength(), sequenceArray[i].muteCV2 );
		}
		lfoTimer = 0;
	}
  wasPlaying = playing;
//	digitalWriteFast(PIN_EXT_AD_2, LOW);

}

bool MasterClock::gateTrigger(uint8_t gateNum){
}

void MasterClock::checkGateClock(){
	for (int i =0; i <9; i++){
		if (globalObj->gateInputRaw[i] == 1 && gatePrevState[i] == 0 ){
			gateTrig[i] = true;
		} else {
			gateTrig[i] = false;
		};
		gatePrevState[i] = globalObj->gateInputRaw[i];
	}
}

void MasterClock::externalClockTick(uint8_t gateNum){
	checkGateClock();

	if (playing){
		if (!wasPlaying){
			for (int i=0; i< SEQUENCECOUNT; i++){
				outputControl->allNotesOff(i);
				sequenceArray[i].clockStart();
				//Serial.println("Starting sequence: " + String(i));
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
	displayRunSwitch = true;

}

void MasterClock::internalClockTick(){
 //digitalWriteFast(PIN_EXT_TX, HIGH);
 //Serial.println("begin internal clock tick");
        // int clock
	unsigned long clockCycles = ARM_DWT_CYCCNT;

  if (playing && !wasPlaying){
		serialMidi->sendRealTime(midi::Start);

        // if playing has just re-started, the master tempo timer and the master beat count must be reset
   // MIDI.send(Start, 0, 0, 1);  // MIDI.sendSongPosition(0);
    masterPulseCount = 0;

    for (int i=0; i< SEQUENCECOUNT; i++){
			outputControl->allNotesOff(i);
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
		for (int i=0; i< SEQUENCECOUNT; i++){
			sequenceArray[i].ppqPulse(INTERNAL_PPQ_COUNT);
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
	  for (int i=0; i< SEQUENCECOUNT; i++){
			sequenceArray[i].runSequence();
	  }
}
