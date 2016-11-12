#include "Arduino.h"
#include "Sequencer.h"

// noteStatus indicates the status of the next note
// 0 indicates not playing, not queued
// 1 indicates the note is currently playing
// 2 indicates the note is currently queued.
// 3 indicates that the note is currently playing and currently queued
// 4 indicates that the note has been played this iteration
// stepData[activeStep].noteStatus = stepData[activeStep].pitch;

#define NOTE_LENGTH_BUFFER 5000  // number of microseconds to end each gate early



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
	this->beatLength = 60000000/(tempoX100/100);
	this->calculateStepTimers();
	this->monophonic = true;
};


void Sequencer::initNewSequence(uint8_t index, uint8_t ch){
	this->stepCount = 16;
	this->beatCount = 4;
	this->quantizeKey = 0;
	this->quantizeScale = 0;
	this->patternIndex = index;
	this->channel = ch;

	for(int n=0; n < MAX_STEPS_PER_SEQUENCE; n++){
		this->stepData[n].pitch[0]   = 24;

		for (int i=1; i<4; i++){
			this->stepData[n].pitch[i] = 0;
		}
		this->stepData[n].chord	   		 =	 0;
		this->stepData[n].gateType		 =	 0;
		this->stepData[n].gateLength	 =	 1;
		this->stepData[n].arpType			 =	 0;
		this->stepData[n].arpOctave		 =   1;
		this->stepData[n].arpSpdNum		 =   1;
		this->stepData[n].arpSpdDen		 =   4;
		this->stepData[n].glide				 =   0;
		this->stepData[n].beatDiv			 =   4;
		this->stepData[n].velocity		 =  67;
		this->stepData[n].velocityType =   0;
		this->stepData[n].lfoSpeed		 =  16;
	//	this->stepData[n].test		     =  16;

		this->stepData[n].offset       =   0;
		this->stepData[n].noteStatus   =  NOTPLAYING_NOTQUEUED;
		this->stepData[n].notePlaying  =   0;
		this->stepData[n].stepTimer    =   0;
	}
};


void Sequencer::setTempo(uint32_t tempoX100){
	this->tempoX100 = tempoX100;
	beatLength = 60000000	/(tempoX100/100);
	calculateStepTimers();
}

void Sequencer::setStepPitch(uint8_t step, uint8_t pitch, uint8_t index){
	stepData[step].pitch[index] = pitch;
	Serial.println("step: " + String(step) + " pitch: " + String(pitch) + " index: " + String(index) + " set pitch: " + String(stepData[step].pitch[index]));
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
		firstBeat = false;
	}

};

void Sequencer::runSequence(NoteDatum *noteData, GameOfLife *life){
	calculateStepTimers();
	clearNoteData(noteData);
	incrementActiveStep();
	sequenceModeStandardStep(noteData);

}

uint32_t Sequencer::getStepLength(uint8_t stepNum){
		if (stepData[stepNum].beatDiv > 0){
			return beatLength/stepData[stepNum].beatDiv;
		} else { // negative values of beatDiv allow for whole multiples of beatLength
			return beatLength*(abs(stepData[stepNum].beatDiv)+2);
		}
}

void Sequencer::calculateStepTimers(){
	//stepLength = beatLength*beatCount/stepCount;
	uint32_t accumulatedOffset = 0;
	//beatOffset
	//stepLength = beatLength/stepDivider*stepCount;
	for (int stepNum = 0; stepNum < stepCount; stepNum++){
		stepData[stepNum].offset = accumulatedOffset;
		accumulatedOffset += getStepLength(stepNum);
	}
}

void Sequencer::incrementActiveStep(){
	uint32_t sequenceTimerInt = sequenceTimer;
	uint32_t activeStepEndTime = 0;

	//calculate if this step should be finished by now.
	for (int stepNum = 0; stepNum < (activeStep+1); stepNum++ ){
		activeStepEndTime += getStepLength(stepNum);
	}

	if(sequenceTimerInt > activeStepEndTime ){
		activeStep++;
		if (activeStep >= stepCount ) {
			activeStep = 0;
			sequenceTimer = 0;
			stepData[0].noteStatus = NOTPLAYING_NOTQUEUED;
		}
	}
	//Serial.println("ending incrementactivestep beatdiv:\t" + String(stepData[activeStep].beatDiv) + "\tendtime:\t" + String(activeStepEndTime) + "\tactivestep\t" + String(activeStep));

}


void Sequencer::sequenceModeStandardStep(NoteDatum *noteData){
	// sequenceModeStandardStep determines if any notes should be triggered this loop.
	// This means that this loop is responsible for all timing calculations and triggering notes
	for (int stepNum = 0; stepNum < stepCount; stepNum++){
	// iterate through all steps to determine if they need to have action taken.
		if (stepData[stepNum].gateType > GATETYPE_REST){
			// if the gateType is not rest, some action should be taken
			//uint32_t stepOffTime = (stepData[stepNum].gateLength+1)*beatLength/(stepData[stepNum].beatDiv*4);

			uint32_t stepOffTime = (stepData[stepNum].gateLength+1)*getStepLength(stepNum)/4;
			uint32_t trigLength;

			if (stepData[stepNum].stepTimer > stepOffTime && stepData[stepNum].arpStatus != 0) {
				stepData[stepNum].arpStatus = 0;
			}

			if (stepData[stepNum].arpType != 0 ){
				trigLength = stepData[stepNum].arpSpdNum*getStepLength(stepNum)/stepData[stepNum].arpSpdDen;
			} else {
				trigLength = stepOffTime;
			};

			bool gateTrig;
			bool gateOff;

			if (stepData[stepNum].gateType == 0) {
				//no gate
				gateTrig = false;
				gateOff = true;
			} else if (stepData[stepNum].gateType == 1){
				//gate is on / retrigger
				gateTrig = true;
				gateOff = true;
			} else if (stepData[stepNum].gateType == 2){
				if (stepData[stepNum].arpStatus == 0){
					gateTrig = true;
					gateOff = true;
				} else {
					gateTrig = false;
				}
			} else {
				gateTrig = true; // for hold. logic needs to be in note Off section
				if (stepData[stepNum].stepTimer < stepOffTime - trigLength){
					gateOff = false;
				} else {
					gateOff = true;
				}
			}

//			if (stepData[stepNum].stepTimer > stepData[stepNum].arpStatus * trigLength - 10000 ) {
      if (stepData[stepNum].stepTimer > stepData[stepNum].arpStatus * trigLength - trigLength/10  || stepData[stepNum].stepTimer > stepOffTime) {
  			// shut off notes that should stop playing.
				noteShutOff(noteData, stepNum, gateOff);
			}

			if ( sequenceTimer >= (stepData[stepNum].offset + stepData[stepNum].arpStatus*trigLength) ) {
			   if ( sequenceTimer < (stepData[stepNum].offset + stepOffTime - 500 )) {
					 // 1000us buffer is so that a note doesn't get retriggered after it gets shut off for the last time.
/*
				 sequenceTimer <-- starts at the beginning of the sequence
				 stepData[stepNum].offset <-- time index when step should start. offset from beginning
				 stepData[stepNum].arpStatus <-- whihc arpeggiation is being triggered
				 stepData[stepNum].arpLength <-- stepTime
				 stepData[stepNum].stepOffTime <-- stepTime
				 stepData[stepNum].arpSpeed <-- stepTime

				 if

				 sequenceTimer >= stepData[stepNum].offset <-- first of arp begins

				 */
				noteTrigger(noteData, stepNum, gateTrig);
				stepData[stepNum].arpStatus++;

		 /*
				Serial.println("First Condition Met - stepNum: " + String(stepNum)  +
					+ "\tarpStatus: " + String(stepData[stepNum].arpStatus)
					+ "\tsequenceTimer: " + String(sequenceTimer)
					+ "\tlt: " + String(stepData[stepNum].offset + stepData[stepNum].stepOffTime)
					+ "\tgteq: " + String(stepData[stepNum].offset + stepData[stepNum].arpStatus*stepData[stepNum].arpLength())
					+ "\toffset: " + String(stepData[stepNum].offset)
					+ "\tstepoff: " + String(stepData[stepNum].stepOffTime)
					+ "\tarpLength: " + String(stepData[stepNum].arpLength())
					+ "\tstpLength: " + String(stepLength)
					+ "\tgateLength: " + String(stepData[stepNum].gateLength)
					+ "\tbeatLength: " + String(beatLength)
					+ "\tbeatDiv:" + String(stepData[stepNum].beatDiv)
					+ "\tstepCount:" + String(stepCount)
					+ "\ttrigLength:" + String(trigLength)
				);
		 */

			}
		}
		}
	}
}

void Sequencer::noteTrigger(NoteDatum *noteData, uint8_t stepNum, bool gateTrig){
	// sets pitch of notes to be played.
	noteData->noteOn = true;
	noteData->channel = channel;
	noteData->noteOnStep = stepNum;
	noteData->offset = stepData[stepNum].offset;
	if (stepData[stepNum].arpStatus == 0){
		//only reset the step timer if it is the first trigger of the arpeggio
		stepData[stepNum].stepTimer = 0;
	}

	stepData[stepNum].noteStatus = CURRENTLY_PLAYING;

	//fill an array with the pitches of the step
	uint8_t pitchArray[22];
	pitchArray[0] = stepData[stepNum].pitch[0];
	pitchArray[1] = stepData[stepNum].pitch[0] + stepData[stepNum].pitch[1];
	pitchArray[2] = stepData[stepNum].pitch[0] + stepData[stepNum].pitch[2];
	pitchArray[3] = stepData[stepNum].pitch[0] + stepData[stepNum].pitch[3];

	//figure out how many steps are nil (255)
	uint8_t arpSteps = 4;
	for(int i=1; i<4; i++){
		if(stepData[stepNum].pitch[i] == 255){
			arpSteps = arpSteps - 1;
		}
	}

	//fill up the rest of the array with octave up pitches
	for (int i = arpSteps; i< 5 * arpSteps; i++){
		pitchArray[i] = pitchArray[i-arpSteps]+12;
	}

	if (stepData[stepNum].arpOctave > 0){
		arpSteps = stepData[stepNum].arpOctave * arpSteps;
	}

	uint8_t index;

	int8_t playPitch; 	//pitch that will be triggered in this loop


	switch (stepData[stepNum].arpType){
		case ARPTYPE_UP:
			index = stepData[stepNum].arpStatus % arpSteps;
			playPitch = pitchArray[index];
		break;

		case ARPTYPE_DN:
			index = arpSteps - stepData[stepNum].arpStatus % arpSteps;
			playPitch = pitchArray[index];
		break;

		case ARPTYPE_UPDN1:
			// repeating top and bottom note
			if (stepData[stepNum].arpStatus/arpSteps % 2){
				index = stepData[stepNum].arpStatus % arpSteps;
			} else {
				index = arpSteps - stepData[stepNum].arpStatus % arpSteps;
			}

			playPitch = pitchArray[index];
		break;

		case ARPTYPE_UPDN2: // no repeat of top and bottom note
	//arpsteps:           4
	//index:              0 1 2 3 2 1 0 1 2 3
	//arpStatus:          0 1 2 3 4 5 6 7 8 9
	//arpStatus/arpSteps: 0 0 0 0 1 1 1 1 2 2
	//arpstatus%arpSteps: 0 1 2 3 0 1 2 3 0 1
	//                    0 0 0 0 1 1 1
	//										0 0 0 1 1 1 0 0 0 1 1 1
	//up:                 0 1 2 3 0 1 2 3 0 1 2 3
	//down:      			  3 2 1 0 3 2 1 0 3 2

			if (stepData[stepNum].arpStatus/(arpSteps-1) % 2){
				index = stepData[stepNum].arpStatus % arpSteps;
			} else {
				index = arpSteps - stepData[stepNum].arpStatus % (arpSteps-1);
			}
			playPitch = pitchArray[index];
		break;

		case ARPTYPE_RNDM: // no repeat of top and bottom note
			index = random(0,arpSteps);
			playPitch = pitchArray[index];
		break;

		default:
			playPitch = pitchArray[0];
		break;
	}

	//if (quantizeKey == 1){
	//	stepData[stepNum].notePlaying = quantizePitch(playPitch, aminor, 1);
	if (quantizeScale > 0){
		stepData[stepNum].notePlaying = quantizePitch(playPitch, quantizeKey, quantizeScale, 1);
	} else {
		stepData[stepNum].notePlaying = playPitch;
	}



	for (int i=0; i< stepCount; i++){
		// since there could be up to stepCount steps being triggered in a single noteOnArray,
		// need to find the first NULL entry. After setting it, break.
		if (noteData->noteOnArray[i] == NULL){
			noteData->noteGateArray[i] = gateTrig;
			noteData->noteOnArray[i] = stepData[stepNum].notePlaying;
			noteData->noteVelArray[i] = stepData[stepNum].velocity;
			noteData->noteVelTypeArray[i] = stepData[stepNum].velocityType;
			noteData->noteLfoSpeed[i] = stepData[stepNum].lfoSpeed;
			noteData->noteGlideArray[i] = stepData[stepNum].glide;
			break;
		}
	}
}

void Sequencer::clearNoteData(NoteDatum *noteData){
	noteData->noteOff = false;
	noteData->noteOn = false;

	for(int i = 0; i < stepCount; i++){
		noteData->noteOnArray[i] = NULL;
		noteData->noteVelArray[i] = NULL;
		noteData->noteVelTypeArray[i] = NULL;
		noteData->noteLfoSpeed[i] = NULL;
		noteData->noteGlideArray[i] = NULL;
		noteData->noteOffArray[i] = NULL;
	}

	noteData->channel = 0;
	noteData->noteOnStep = 0;
	noteData->noteOffStep = 0;
}

void Sequencer::noteShutOff(NoteDatum *noteData, uint8_t stepNum, bool gateOff){
	//shut off any other notes that might still be playing.

		if( stepData[stepNum].noteStatus == CURRENTLY_PLAYING ){
			noteData->noteOff = true;
			noteData->channel = channel;
			noteData->noteOffStep = stepNum;
			noteData->noteGateOffArray[stepNum] = gateOff;


			for (int f=0; f<stepCount; f++){
				if (noteData->noteOffArray[f] == NULL){
					noteData->noteOffArray[f] = stepData[stepNum].notePlaying;
					break;
				}
			}

	//		Serial.print("-off- TRIG\t ");
	/*		Serial.println("stepData[stepNum].arpLength(): " + String(stepData[stepNum].arpLength()) + " arpcount: " + String(stepData[stepNum].arpCount()) + " arpStatus: " + String(stepData[stepNum].arpStatus) + " offset: " + String(stepData[stepNum].offset) + " timer:" + String(sequenceTimer) + " noteTimer: " + String(stepData[stepNum].stepOffTime) + " stepTimer: " + String(stepData[stepNum].stepTimer));
  */
  /*
			Serial.print("Notes off:\t");
			for (int i=0; i< MAX_STEPS_PER_SEQUENCE; i++) {
				if (noteData->noteOnArray[i] != 0){
					Serial.print(noteData->noteOffArray[i]);
					Serial.print(" ");
				}
			}
			Serial.println("");
  */
			stepData[stepNum].noteStatus = NOTPLAYING_NOTQUEUED;
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
	// stepData[activeStep].stepTimer is reset in the activeStep increment section above
	// to determine which cell should be played, we need to know what the time index is, and when each cell index should be played.
	// activeCellValues[0] should be played at stepTimer= 0 and then stepTimer= cellPlayLength; up to stepTimer
	// so the timing of this runSequence could be off by up to 1ms sometimes... perhaps we need to keep track of what has already played.
	// lifeCellToPlay will keep track of the next cell to play


	int cellPlayLength = stepLength/activeCellCount;


	for(int d=0; d<16; d++){
		if(stepData[activeStep].stepTimer > d * cellPlayLength ){
			lifeCellToPlay = d;
		}
	}


	if ( (stepData[activeStep].stepTimer > (lifeCellToPlay * cellPlayLength) )
	&& activeCellCount > 0
	&& lifeCellsPlayed == lifeCellToPlay) {

		// Serial.println("activeCellCount: " + String(activeCellCount));
		for (int stepNum = 0; stepNum < stepCount; stepNum++){
			if(stepData[stepNum].noteStatus == CURRENTLY_PLAYING){
				noteData->noteOff = true;
				noteData->channel = channel;
				noteData->noteOffStep = stepNum;
				for (int f=0; f<stepCount; f++){
					if (noteData->noteOffArray[f] == NULL){
						noteData->noteOffArray[f] = stepData[stepNum].notePlaying;
						break;
					}
				}
				stepData[stepNum].noteStatus = 0;
			}
		}

		noteData->noteOn = true;
		noteData->channel = channel;
		noteData->noteOnStep = lifeCellToPlay;

		//noteData->triggerTime = micros();
		//noteData->sequenceTime = sequenceTimer;

		if (quantizeScale > 0){
			stepData[lifeCellToPlay].notePlaying = quantizePitch(activeCellValues[lifeCellToPlay], quantizeKey, quantizeScale, 1);
			// Serial.println("quantized note: " + String(stepData[stepNum].pitch) + " -> " + String(stepData[stepNum].notePlaying));
		} else {
			stepData[lifeCellToPlay].notePlaying = activeCellValues[lifeCellToPlay];
		}

		stepData[lifeCellToPlay].noteStatus = 1;
		noteData->noteOnArray[lifeCellToPlay] = stepData[lifeCellToPlay].notePlaying;
		noteData->noteVelArray[lifeCellToPlay] = stepData[lifeCellToPlay].velocity;
		lifeCellsPlayed++;
	}

}

uint8_t Sequencer::quantizePitch(uint8_t note, uint8_t key, uint8_t scale, bool direction){

	uint8_t count = 0;
	uint16_t scaleExpanded;

	switch(scale){
		case 1: scaleExpanded = CHROMATIC         ; break;
		case 2: scaleExpanded = MAJOR             ; break;
		case 3: scaleExpanded = MINOR             ; break;
		case 4: scaleExpanded = MAJORMINOR        ; break;
		case 5: scaleExpanded = PENTATONIC_MAJOR  ; break;
		case 6: scaleExpanded = PENTATONIC_MINOR  ; break;
		case 7: scaleExpanded = PENTATONIC_BLUES  ; break;
		case 8: scaleExpanded = IONIAN            ; break;
		case 9: scaleExpanded = AEOLIAN           ; break;
		case 10: scaleExpanded = DORIAN           ; break;
		case 11: scaleExpanded = MIXOLYDIAN       ; break;
		case 12: scaleExpanded = PHRYGIAN         ; break;
		case 13: scaleExpanded = LYDIAN           ; break;
		case 14: scaleExpanded = LOCRIAN          ; break;
		default: scaleExpanded = CHROMATIC				; break;
	}

	//Serial.println("Original Scale:\t" + String(scaleExpanded, BIN) );
	for (int i=0; i< key; i++){
		//bitwise rotation - 11 bits rotate to the right. Do it once for each scale degree
		scaleExpanded = (scaleExpanded >> 1) | ((0b01 & scaleExpanded) << 11);
	}
	//Serial.println("Shifted to " + String(quantizeKey) + "\t" + String(scaleExpanded, BIN) );

	while ( (0b100000000000 >> (note % 12) ) & ~scaleExpanded ) {
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
