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

	for (int i = 0; i < SEQUENCECOUNT; i++ ){
    sequenceArray[i].masterClockPulse();
  }

	uint32_t clockPeriod = (60000000/(tempoX100/100) )/(INTERNAL_PPQ_COUNT);
	if (clockMode == INTERNAL_CLOCK){
		if ((int)masterLoopTimer > kMasterClockInterval + 100){
			uint32_t countToAdd = (int)masterLoopTimer / kMasterClockInterval;
			clockCounter = clockCounter + countToAdd;
			lfoClockCounter += countToAdd;
			//Serial.println("Adding " + String((int)masterLoopTimer / kMasterClockInterval) + " counts to the clock: " + String((int)masterLoopTimer) );
		//	for(int i=0;  i< countToAdd; i++){
		//		digitalWriteFast(PIN_EXT_AD_2, LOW);delayMicroseconds(10);
		//		digitalWriteFast(PIN_EXT_AD_2, HIGH);delayMicroseconds(10);
		//	}
		} else {
			clockCounter++;
			lfoClockCounter++;
		}

		// Make sure the LEDs do not refresh right before the clock needs to be triggered.
		if (extClockCounter > EXTCLOCKDIV - 1  && clockCounter * kMasterClockInterval>  clockPeriod - 3000){
			digitalWriteFast(PIN_EXT_AD_2, LOW);
			ledRunSwitch = false;
		}

		if (clockCounter * kMasterClockInterval >  clockPeriod){
			extClockCounter++;

			if( extClockCounter >= EXTCLOCKDIV ){
				outputControl->setClockOutput(HIGH);
				extClockCounter = 0;
				//Serial.println("Clock Fire debugTimer: " + String(masterDebugTimer) + "\tclockPeriod: " + String(clockPeriod) + "\tclockCounter: " + String(clockCounter) + "\tinterval:" + String(kMasterClockInterval) + "\ttotalTimer: " + String(clockCounter * kMasterClockInterval));
				masterDebugTimer = 0;
			}
			clockCounter = 0;
			pulseTrigger = 1;
		}
	} else if(clockMode == EXTERNAL_MIDI_CLOCK){
    if ((int)masterLoopTimer > kMasterClockInterval + 100){
      uint32_t countToAdd = (int)masterLoopTimer / kMasterClockInterval;
      lfoClockCounter += countToAdd;
    } else {
      lfoClockCounter++;
    }

  }
	masterLoopTimer = 0;

	if ((clockCounter * kMasterClockInterval >  clockPeriod/2 ) && outputControl->clockValue) {
		outputControl->setClockOutput(LOW);
		ledRunSwitch = true;
		digitalWriteFast(PIN_EXT_AD_2, HIGH);
	}

	// digitalWriteFast(PIN_EXT_AD_2, LOW);
  // masterLooptimeAvg

};

void MasterClock::sequencerFunc(void){
//	digitalWriteFast(PIN_EXT_AD_2, HIGH);

	outputControl->inputRead();

	if(currentMenu == CALIBRATION_MENU){
		playing = 0;
		outputControl->dacTestLoop();
		return;
	}

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
		case EXTERNAL_CLOCK_BIDIRECTIONAL_INPUT:
			externalClockTick(4);
			break;
  }

	if(lfoTimer > 2){
		for (int i=0; i< SEQUENCECOUNT; i++){
			outputControl->lfoUpdate(i);
		}
		lfoTimer = 0;
	}

  wasPlaying = playing;

//	digitalWriteFast(PIN_EXT_AD_2, LOW);

}

bool MasterClock::gateTrigger(uint8_t gateNum){
}

void MasterClock::checkGateClock(){
	for (int i =0; i <4; i++){
		if (gateInputRaw[i] == 1 && gatePrevState[i] == 0 ){
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
		if (!wasPlaying){
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

  if (playing && !wasPlaying){
        // if playing has just re-started, the master tempo timer and the master beat count must be reset
   // MIDI.send(Start, 0, 0, 1);  // MIDI.sendSongPosition(0);
    masterPulseCount = 0;
    startTime = 0;

    for (int i=0; i< SEQUENCECOUNT; i++){
			outputControl->allNotesOff(i);
    	sequenceArray[i].clockStart(startTime);
    }

    Serial.println("Starting sequence - internal clock: ");

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


	debug("end internal clock tick");

  //digitalWriteFast(DEBUG_PIN, LOW);
}

void MasterClock::midiClockTick(){
  // ext clock sync
	  for (int i=0; i< SEQUENCECOUNT; i++){
			sequenceArray[i].runSequence();
	  }
}
