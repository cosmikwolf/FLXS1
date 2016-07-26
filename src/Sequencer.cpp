#include "Arduino.h"
#include "Sequencer.h"

// noteStatus indicates the status of the next note
// 0 indicates not playing, not queued
// 1 indicates the note is currently playing
// 2 indicates the note is currently queued.
// 3 indicates that the note is currently playing and currently queued
// 4 indicates that the note has been played this iteration
// stepData[activeStep].noteStatus = stepData[activeStep].pitch;


Sequencer sequence[4];


Sequencer::Sequencer() {

};

void Sequencer::initialize(uint8_t ch, uint8_t stepCount, uint8_t beatCount, uint32_t tempoX100){
	// initialization routine that runs during setup
	Serial.println("Initializing Sequencer Object");
	this->channel = ch;
	this->stepCount = stepCount;
	this->beatCount = beatCount;
	this->tempoX100 = tempoX100;
	this->sequenceTimer = 0;
	this->instrument = 38;
	this->instType = 0;
	this->volume = 100;
	//for (int i=0; i < stepCount; i++){
	// stepData[i].gateLength = 1;
	//  stepData[i].velocity = 127;
	// stepData[i].pitch = 24;
	//};

	for (int i=0; i<MAX_STEPS_PER_SEQUENCE; i++){
		stepUtil[i].beat = 0;
		stepUtil[i].offset = 0;
		stepUtil[i].noteStatus = NOTPLAYING_NOTQUEUED;
		stepUtil[i].notePlaying = 0;
		stepUtil[i].lengthMcs = 0;
		stepUtil[i].noteTimerMcs = 0;
		stepUtil[i].stepTimer = 0;
	}



	this->beatLength = 60000000/(tempoX100/100);
	this->calculateStepTimers();
	this->monophonic = true;
};


void Sequencer::initNewSequence(uint8_t index, uint8_t ch){
	this->stepCount = stepCount;
	this->beatCount = 4;
	this->quantizeKey = 1;
	this->instrument = 0;
	this->volume = 100;
	this->bank = 0;
	this->patternIndex = index;
	this->channel = ch;
	this->instType = 2; //initialized regular instrument
	this->instType = 0;

	for(int n=0; n < MAX_STEPS_PER_SEQUENCE; n++){
		for (int i=0; i<4; i++){
			this->stepData[n].pitch[i]   = 24;
		}
		this->stepData[n].gateLength = 1;
		this->stepData[n].gateType   = 0;
		this->stepData[n].velocity   = 67;
		this->stepData[n].glide      = 0;
	}
};

void Sequencer::setInstType(uint8_t type){
	this->instType = type;
}

void Sequencer::setTempo(uint32_t tempoX100){
	this->tempoX100 = tempoX100;
	beatLength = 60000000/(tempoX100/100);
	calculateStepTimers();
}

void Sequencer::setStepPitch(uint8_t step, uint8_t pitch, uint8_t index){
	stepData[step].pitch[0] = pitch;
};

void Sequencer::setGateLength(uint8_t step, uint8_t length){
	stepData[step].gateLength = length;
	calculateStepTimers();
};

void Sequencer::setStepCount(uint8_t stepCountNew){
	stepCount = stepCountNew;
	calculateStepTimers();
};

void Sequencer::setBeatCount(uint16_t beatCountNew){
	beatCount = beatCountNew;
	calculateStepTimers();
};

void Sequencer::setGateType(uint8_t step, uint8_t gate){
	stepData[step].gateType = gate;
}

void Sequencer::setStepVelocity(uint8_t step, uint8_t velocity){
	stepData[step].velocity = velocity;
};

void Sequencer::setStepGlide(uint8_t step, uint8_t glideTime){
	stepData[step].glide = glideTime;
}

void Sequencer::calculateStepTimers(){
	uint32_t noteTimerMcsCounter = 0;
	stepLength = beatLength*beatCount/stepCount;

	// Serial.println(" stepCount: " + String(stepCount) + " stepLength: " + String(stepLength) + " beatLength: " + String(beatLength) + " tempo: " + String(tempo));
	for (int stepNum = 0; stepNum < stepCount; stepNum++){

		stepUtil[stepNum].noteTimerMcs = (stepData[stepNum].gateLength*stepLength);
		stepUtil[stepNum].beat = floor(noteTimerMcsCounter / beatLength);
		stepUtil[stepNum].offset = stepNum*stepLength;
		noteTimerMcsCounter = noteTimerMcsCounter + stepUtil[stepNum].noteTimerMcs;
		/*
		Serial.println( String(channel) + " " + String(stepNum) + " " +
		"ntm: " + String(stepData[stepNum].noteTimerMcs) +
		"\tbt: " + String(stepData[stepNum].beat) +
		"\toff: " + String(stepData[stepNum].offset)  +
		"\tgaL: " + String(stepData[stepNum].gateLength) +
		"\tgaT: " + String(stepData[stepNum].gateType) +
		"\tptch: " + String(stepData[stepNum].pitch) +
		"\tOC: " + String(noteTimerMcsCounter)
	);
	*/

}



}


void Sequencer::clockStart(elapsedMicros startTime){
	firstBeat = true;
	sequenceTimer = startTime;
};

void Sequencer::beatPulse(uint32_t beatLength, GameOfLife *life){
	// this is sent every 24 pulses received from midi clock
	// and also when a play or continue command is received.

	this->beatLength = beatLength;
	calculateStepTimers();
	beatTimer = 0;
	tempoPulse = true;

	if(firstBeat){
		activeStep = 0;
		beatTracker = 0;
		firstBeat = false;
	} else {
		//beatTracker= positive_modulo(beatTracker + 1, beatCount);
		beatTracker = (beatTracker + 1) % beatCount;
	}

	if (beatTracker == 0) {
		for(int i = 0; i < stepCount; i++){
			// reset the note status for notes that have been played.
			// leave notes that have not been turned off yet.
			//  if (stepData[i].noteStatus == NOTE_HAS_BEEN_PLAYED_THIS_ITERATION){
			//    stepData[i].noteStatus = 0;
			//  }
		}
		activeStep = 0;
		if(channel == 1){
			// Serial.println("Resetting Sequence Timer " + String(sequenceTimer));
		}
		sequenceTimer = 0;
		// We are resetting the note status when the sequenceTimer is no longer larger than
		// the offset value. This means that runSequence needs to run at least once before
		// it can reset the note statuses. For the first note, that means it wont ever reset
		// so we must reset it manually here.
		stepUtil[0].noteStatus = NOTPLAYING_NOTQUEUED;
	}

	if(instType == 1 && channel == 0){

		life->lifeIterate(life->grid);

	}

};

void Sequencer::runSequence(NoteDatum *noteData, GameOfLife *life){

	clearNoteData(noteData);
	incrementActiveStep();

	switch (instType){
		case 1:
		// GAME OF LIFE MODE
		sequenceModeGameOfLife(noteData, life);
		break;
		case 0:
		// NORMAL STEP MODE
		sequenceModeStandardStep(noteData);
		break;
	}
}

void Sequencer::incrementActiveStep(){
	// increment active step taking avg jitter into account
	int32_t sequenceTimerInt = sequenceTimer;

	int sequenceAvgJitter = 0;
	for(int i = 0; i < 3; i++){
		sequenceAvgJitter += sequenceJitter[i];
	}
	sequenceAvgJitter = sequenceAvgJitter / 3;

	if(sequenceTimerInt > (activeStep+1)*stepLength - sequenceAvgJitter/2 ){
		for(int i = 3; i > 1; i--){
			sequenceJitter[i] = sequenceJitter[i-1];
		}
		sequenceJitter[0] = sequenceTimerInt - (activeStep+1)*stepLength;

		if (activeStep < stepCount -1 ){
			activeStep++;
			if (instType == 1){
				stepUtil[activeStep].stepTimer = 0;
				lifeCellToPlay = 0;
				lifeCellsPlayed = 0;
			}
		}
	}
}

void Sequencer::clearNoteData(NoteDatum *noteData){
	noteData->noteOff = false;
	noteData->noteOn = false;

	for(int i = 0; i < stepCount; i++){
		noteData->noteOnArray[i] = NULL;
		noteData->noteVelArray[i] = NULL;
		noteData->noteOffArray[i] = NULL;
	}

	noteData->channel = 0;
	noteData->noteOnStep = 0;
	noteData->noteOffStep = 0;
}

void Sequencer::sequenceModeStandardStep(NoteDatum *noteData){
	for (int stepNum = 0; stepNum < stepCount; stepNum++){
		if (stepData[stepNum].gateType > GATETYPE_REST){

			int arpCount = stepData[stepNum].gateType;
			int arpLength = stepUtil[stepNum].noteTimerMcs / arpCount;

			if (sequenceTimer >= stepUtil[stepNum].offset + stepUtil[stepNum].noteTimerMcs) {
				stepUtil[stepNum].arpStatus = 0;
			}

			if (stepUtil[stepNum].stepTimer > arpLength - 1500 ) {
        // 1500 is a 1.5ms buffer for each note to make sure each note is shut off in time.
				// note shut off if no new notes are being triggered
				noteShutOff(noteData, stepNum);
			}

			// set notes to be played
			// HERE IS WHERE I NEED TO FIGURE OUT ARP LOGIC!
			// if ( sequenceTimer >= (stepUtil[stepNum].offset + stepUtil[stepNum].arpStatus*arpLength) &&
			//     sequenceTimer < (stepUtil[stepNum].offset + stepUtil[stepNum].noteTimerMcs) ) {

      if ( sequenceTimer > stepUtil[stepNum].offset + stepUtil[stepNum].arpStatus*arpLength
        && sequenceTimer < (stepUtil[stepNum].offset + stepUtil[stepNum].noteTimerMcs)){
					//ensure all notes playing are ended before a new trigger.
					//noteShutOff(noteData, stepNum);
					noteTrigger(noteData, stepNum, 0);

					Serial.println("arplength: " + String(arpLength) + " arpcount: " + String(arpCount) + " arpStatus: " + String(stepUtil[stepNum].arpStatus) + " offset: " + String(stepUtil[stepNum].offset) + " timer:" + String(sequenceTimer) + " noteTimer: " + String(stepUtil[stepNum].noteTimerMcs) + " stepTimer: " + String(stepUtil[stepNum].stepTimer));
					Serial.println("Note on array:");
					for (int i=0; i< MAX_STEPS_PER_SEQUENCE; i++) {
						Serial.print(noteData->noteOnArray[i]);
						Serial.print(" ");
					}
					Serial.println("");

          stepUtil[stepNum].arpStatus++;
			}
		}
	}
}

void Sequencer::noteShutOff(NoteDatum *noteData, uint8_t stepNum){
	//shut off any other notes that might still be playing.

		int arpCount = stepData[stepNum].gateType;
		int arpLength = stepUtil[stepNum].noteTimerMcs / arpCount;

		if( stepUtil[stepNum].noteStatus == CURRENTLY_PLAYING ){
			noteData->noteOff = true;
			noteData->channel = channel;
			noteData->noteOffStep = stepNum;
			for (int f=0; f<stepCount; f++){
				if (noteData->noteOffArray[f] == NULL){
					noteData->noteOffArray[f] = stepUtil[stepNum].notePlaying;
					break;
				}
			}
      /*
			Serial.println("arplength: " + String(arpLength) + " arpcount: " + String(arpCount) + " arpStatus: " + String(stepUtil[stepNum].arpStatus) + " offset: " + String(stepUtil[stepNum].offset) + " timer:" + String(sequenceTimer) + " noteTimer: " + String(stepUtil[stepNum].noteTimerMcs) + " stepTimer: " + String(stepUtil[stepNum].stepTimer));

			Serial.println("Note off array:");
			for (int i=0; i< MAX_STEPS_PER_SEQUENCE; i++) {
				Serial.print(noteData->noteOffArray[i]);
				Serial.print(" ");
			}
			Serial.println("");
*/
			stepUtil[stepNum].noteStatus = NOTPLAYING_NOTQUEUED;
		}

}

void Sequencer::noteTrigger(NoteDatum *noteData, uint8_t stepNum, uint8_t index){
	noteData->noteOn = true;
	noteData->channel = channel;
	noteData->noteOnStep = stepNum;
	//noteData->sequenceTime = sequenceTimer;
	//noteData->offset = stepUtil[stepNum].offset;
	stepUtil[stepNum].stepTimer = 0;
	stepUtil[stepNum].noteStatus = CURRENTLY_PLAYING;

	if (quantizeKey == 1){
		stepUtil[stepNum].notePlaying = quantizePitch(stepData[stepNum].pitch[index], aminor, 1);
	} else {
		stepUtil[stepNum].notePlaying = stepData[stepNum].pitch[index];
	}

	for (int i=0; i< stepCount; i++){
		// since there could be up to stepCount steps being triggered in a single noteOnArray,
		// need to find the first NULL entry. After setting it, break.
		if (noteData->noteOnArray[i] == NULL){
			noteData->noteOnArray[i] = stepUtil[stepNum].notePlaying;
			noteData->noteVelArray[i] = stepData[stepNum].velocity;
			break;
		}
	}
}



void Sequencer::sequenceModeGameOfLife(NoteDatum *noteData, GameOfLife *life){
	// GAME OF LIFE SEQUENCING MODE

	int activeCellCount = 0;
	int activeCellValues[16];
	int activeRowValues[16];

	for(int row=0; row < LIFELINES; row++){
		if (life->grid[row][activeStep] > 0 ){
			activeCellValues[activeCellCount] = life->grid[row][activeStep];
			activeRowValues[activeCellCount] = row;
			activeCellCount++;
		};
	}

	//if the step just began, trigger the first note
	//if the step has passed a percentage, trigger the next note:
	//percentage is cells played divided by represented by number of active cells
	//cells played can be calculated as how many cells should have been played so far.
	//divide the step length by the number of active cells.
	//current step time index is needed. individual StepTimer can be used.
	// stepUtil[activeStep].stepTimer is reset in the activeStep increment section above
	// to determine which cell should be played, we need to know what the time index is, and when each cell index should be played.
	// activeCellValues[0] should be played at stepTimer= 0 and then stepTimer= cellPlayLength; up to stepTimer
	// so the timing of this runSequence could be off by up to 1ms sometimes... perhaps we need to keep track of what has already played.
	// lifeCellToPlay will keep track of the next cell to play


	int cellPlayLength = stepLength/activeCellCount;


	for(int d=0; d<16; d++){
		if(stepUtil[activeStep].stepTimer > d * cellPlayLength ){
			lifeCellToPlay = d;
		}
	}


	if ( (stepUtil[activeStep].stepTimer > (lifeCellToPlay * cellPlayLength) )
	&& activeCellCount > 0
	&& lifeCellsPlayed == lifeCellToPlay) {

		// Serial.println("activeCellCount: " + String(activeCellCount));
		for (int stepNum = 0; stepNum < stepCount; stepNum++){
			if(stepUtil[stepNum].noteStatus == CURRENTLY_PLAYING){
				noteData->noteOff = true;
				noteData->channel = channel;
				noteData->noteOffStep = stepNum;
				for (int f=0; f<stepCount; f++){
					if (noteData->noteOffArray[f] == NULL){
						noteData->noteOffArray[f] = stepUtil[stepNum].notePlaying;
						break;
					}
				}
				stepUtil[stepNum].noteStatus = 0;
			}
		}


		noteData->noteOn = true;
		noteData->channel = channel;
		noteData->noteOnStep = lifeCellToPlay;

		//noteData->triggerTime = micros();
		//noteData->sequenceTime = sequenceTimer;

		if (quantizeKey == 1){
			stepUtil[lifeCellToPlay].notePlaying = quantizePitch(activeCellValues[lifeCellToPlay], aminor, 1);
			// Serial.println("quantized note: " + String(stepData[stepNum].pitch) + " -> " + String(stepUtil[stepNum].notePlaying));
		} else {
			stepUtil[lifeCellToPlay].notePlaying = activeCellValues[lifeCellToPlay];
		}
		stepUtil[lifeCellToPlay].noteStatus = 1;
		noteData->noteOnArray[lifeCellToPlay] = stepUtil[lifeCellToPlay].notePlaying;
		noteData->noteVelArray[lifeCellToPlay] = stepData[lifeCellToPlay].velocity;
		lifeCellsPlayed++;
	}

}

uint8_t Sequencer::quantizePitch(uint8_t note, uint32_t scale, bool direction){
	uint8_t count = 0;
	while ( (0b100000000000 >> (note % 12) ) & ~scale ) {
		if (direction){
			note += 1;
		} else {
			note -= 1;
		}
		count += 1;
		if (count > 12) {
			break; // emergency break if while loop goes OOC
		}
	}
	return note;
}

uint8_t Sequencer::getStepPitch(uint8_t step, uint8_t index){
	return stepData[step].pitch[index];
};

int Sequencer::positive_modulo(int i, int n) {
	return (i % n + n) % n;
}
