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
#define FRAMES_PER_BEAT  1048576
//Sequencer::Sequencer() {
//};

//void Sequencer::initialize(uint8_t ch, uint8_t stepCount, uint8_t beatCount, uint32_t tempoX100){
	void Sequencer::initialize(uint8_t ch, uint8_t stepCount, uint8_t beatCount, uint32_t tempoX100, OutputController* outputControl){
	// initialization routine that runs during setup
	Serial.println("Initializing Sequencer Object");
	this->channel = ch;
	this->pattern = 0;
	this->stepCount = stepCount;
	this->beatCount = beatCount;
	this->tempoX100 = tempoX100;
	this->sequenceTimer = 0;
	this->beatLength = 60000000/(tempoX100/100);
	this->calculateStepTimers();
	this->monophonic = true;
	this->outputControl = outputControl;
	//this->initNewSequence(pattern, ch);
};


void Sequencer::initNewSequence(uint8_t pattern, uint8_t ch){
	Serial.println("*&*&*&*& Initializing pattern: " + String(pattern) + " channel: " + String(ch));

	this->stepCount = 16;
	this->beatCount = 4;
	this->quantizeKey = 0;
	this->quantizeScale = 0;
	this->pattern = pattern;
	this->channel = ch;
	this->avgPulseLength = 20000;
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
	Serial.println("starttime: " + String(startTime));
};

void Sequencer::beatPulse(uint32_t beatLength){

};

void Sequencer::runSequence(){
//	clearNoteData(noteData);
	incrementActiveStep();
	sequenceModeStandardStep();
}

void Sequencer::ppqPulse(uint8_t maxPulseCount){
	this->maxPulseCount = maxPulseCount;
	framesPerPulse = FRAMES_PER_BEAT / maxPulseCount;
	ppqPulseIndex = (ppqPulseIndex + 1) % maxPulseCount;

	if (pulseTimer > 50 * avgPulseLength){
		//pulse has been stopped. simply reset pulseTimer. do not modify avgPulseLength
	//	ppqPulseIndex = 0;
	} else {
	//	avgPulseLength = (avgPulseLength * 2 + (int)pulseTimer ) / 3;
	}
	avgPulseLength = (avgPulseLength * 2 + (int)pulseTimer ) / 3;

	if (ppqPulseIndex == 0){
		beatsSinceZero++;
		beatsSinceZero = beatsSinceZero % stepCount; //resync at the zero point, which is stepCounts beats from the start.
		if (beatsSinceZero == 0){
			zeroSequenceCount = 0;
		}
	}

	if(firstBeat){
		zeroSequenceCount = 0;
		activeStep = 0;
		zeroBeat = 0;
		firstBeat = false;
		ppqPulseIndex = 0;
		beatsSinceZero = 0;
		for (int stepNum = 0; stepNum < stepCount; stepNum++){
			stepData[stepNum].noteStatus = NOTPLAYING_NOTQUEUED;
			stepData[stepNum].arpStatus = 0;
		}
		//		Serial.println("FIRST PPQ PULSE-  CH: " + String(channel) + "\tavgPulseLength: " + String(avgPulseLength) + "\tpulseTimer:  " + String(pulseTimer));
	}
	//Serial.println("another PPQ PULSE-  CH: " + String(channel) + "\tavgPulseLength: " + String(avgPulseLength) + "\tpulseTimer:  " + String(pulseTimer));
	pulseTimer = 0;

//	calculateStepTimers();

	if(channel == 0 && ppqPulseIndex == 0){
		Serial.println("activestep: " + String(activeStep) + "\tzeroBeat: " + String(zeroBeat) + "\t|CF: " +String(getCurrentFrame()) + "\toffset: " + String(stepData[activeStep].offset) + "\tavgPulseLength: " + String(avgPulseLength) + "\tppqPulseIndex: " + String(ppqPulseIndex) + "\tbeatsSinceZero: " + String(beatsSinceZero) + "\tzeroSequenceCount" + String(zeroSequenceCount));
	}

};

uint32_t Sequencer::getCurrentFrame(){
  uint32_t currentFrame = beatsSinceZero*FRAMES_PER_BEAT + ppqPulseIndex * framesPerPulse + (pulseTimer * framesPerPulse)/avgPulseLength;
	uint32_t sequenceLength = calculateStepTimers();
	if (currentFrame > sequenceLength * zeroSequenceCount){
		if(channel ==1){
			Serial.println("RESETTING ACTIVESTEP CF: " + String(currentFrame)  + "\tSL:" + String(sequenceLength));
		}
		zeroSequenceCount++;
		activeStep = 0;
		for (int stepNum = 0; stepNum < stepCount; stepNum++){
			if (stepData[stepNum].noteStatus == NOTE_HAS_BEEN_PLAYED_THIS_ITERATION){
				stepData[stepNum].noteStatus = NOTPLAYING_NOTQUEUED;
				stepData[stepNum].arpStatus = 0;
			}
		}
	}
	currentFrame = currentFrame % sequenceLength;
	return currentFrame;
}

uint32_t Sequencer::getStepLength(uint8_t stepNum){
		if (stepData[stepNum].beatDiv > 0){
			//return beatLength/stepData[stepNum].beatDiv;
			return FRAMES_PER_BEAT / stepData[stepNum].beatDiv;
		} else { // negative values of beatDiv allow for whole multiples of beatLength
			return FRAMES_PER_BEAT*(abs(stepData[stepNum].beatDiv)+2);
		//	return beatLength*(abs(stepData[stepNum].beatDiv)+2);
		}
}

uint32_t Sequencer::calculateStepTimers(){
	//stepLength = beatLength*beatCount/stepCount;
	uint32_t accumulatedOffset = 0;
	//beatOffset
	//stepLength = beatLength/stepDivider*stepCount;
	for (int stepNum = 0; stepNum < stepCount; stepNum++){
		stepData[stepNum].offset = accumulatedOffset;
		accumulatedOffset += getStepLength(stepNum);
	}
	return accumulatedOffset;
}

void Sequencer::incrementActiveStep(){
	calculateStepTimers();

	if( getCurrentFrame() > stepData[activeStep].offset + getStepLength(activeStep)){
		activeStep++;
		zeroBeat++;
		zeroBeat = zeroBeat % stepData[0].beatDiv;

		if(channel ==0 ){
		//	Serial.println(" AS++" + String(activeStep) + "\tCF:" + String(getCurrentFrame())+"\toffset:" + String(stepData[activeStep].offset) + "\tSL:" + String(getStepLength(activeStep)) + "\tZB:" + String(zeroBeat) + "\tBSZ:" + String(beatsSinceZero) + "\tcalcStepTime: " + String(calculateStepTimers()));
		}

//		if (activeStep >= stepCount) {
		//	if (zeroBeat == 0){
		//		beatPulseResyncFlag = true;
		//		beatsSinceZero = 0;
		//	}
	//		activeStep = 0;

	//	}

	}
}


void Sequencer::sequenceModeStandardStep(){
	// sequenceModeStandardStep determines if any notes should be triggered this loop.
	// This means that this loop is responsible for all timing calculations and triggering notes
	for (int stepNum = 0; stepNum < activeStep + 1; stepNum++){
	// iterate through all steps to determine if they need to have action taken.
		if (stepData[stepNum].noteStatus == NOTE_HAS_BEEN_PLAYED_THIS_ITERATION){
		//	Serial.println("------- has been played: ActiveStep: " + String(activeStep) + "\tstepNum: " + String(stepNum));
			continue;
		}

		if (stepData[stepNum].gateType == GATETYPE_REST){
		//	outputControl->setGateOutputDebug(3,HIGH);
		//	outputControl->setGateOutputDebug(3,LOW);
			continue;
		}

		uint32_t stepOffTime = (stepData[stepNum].gateLength+1)*getStepLength(stepNum)/4;
		uint32_t trigLength;

		if (stepData[stepNum].noteStatus == NOTPLAYING_NOTQUEUED){
				stepData[stepNum].arpStatus = 0;
		}

		if (stepData[stepNum].arpType != ARPTYPE_OFF ){
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
			if ((int)stepData[stepNum].stepTimer < stepOffTime - trigLength){
				gateOff = false;
			} else {
				gateOff = true;
			}
		}

		if (stepData[stepNum].noteStatus == CURRENTLY_PLAYING){
    	if ( getCurrentFrame() > (stepData[stepNum].arpStatus * trigLength - trigLength/10) ) {
				noteShutOff(stepNum, gateOff);
			}
		}

		if ( getCurrentFrame() > (stepData[stepNum].offset + stepData[stepNum].arpStatus*trigLength) ) {
		  if ( getCurrentFrame() < (stepData[stepNum].offset + stepOffTime - 500  )) {
				noteTrigger(stepNum, gateTrig);
			}
		}

	}
}

void Sequencer::noteTrigger(uint8_t stepNum, bool gateTrig){
	// sets pitch of notes to be played.
	//noteData->noteOn = true;
	//noteData->channel = channel;
	//noteData->noteOnStep = stepNum;
	//noteData->offset = stepData[stepNum].offset;
	if (stepData[stepNum].arpStatus == 0){
		//only reset the step timer if it is the first trigger of the arpeggio
		stepData[stepNum].stepTimer = stepData[stepNum].stepTimer - stepData[stepNum].stepTimer;
		debugTimer2 = 0;
	}

	if ( stepData[stepNum].noteStatus == NOTE_HAS_BEEN_PLAYED_THIS_ITERATION ) {
		Serial.println("Skipping trig: " + String(stepNum));
		return;
	} else {
		//Serial.println("trig: " + String(stepNum));
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


	//Serial.println("noteOn"); delay(10);
	outputControl->noteOn(channel,stepData[stepNum].notePlaying,stepData[stepNum].velocity,stepData[stepNum].velocityType, stepData[stepNum].lfoSpeed, stepData[stepNum].glide, gateTrig );
	//Serial.println("noteOn complete"); delay(10);

/*
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
 */

	stepData[stepNum].arpStatus++;


}

uint8_t Sequencer::getArpCount(uint8_t stepNum){
	uint8_t arpCount;

	if (stepData[stepNum].arpType == ARPTYPE_OFF) {
		arpCount = 0;
	} else {
		arpCount = (stepData[stepNum].gateLength *  stepData[stepNum].arpSpdDen / stepData[stepNum].arpSpdNum )/4 ;
	}

	return arpCount;
}

/*void Sequencer::clearNoteData(NoteDatum *noteData){
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
}*/

void Sequencer::noteShutOff(uint8_t stepNum, bool gateOff){
	//shut off any other notes that might still be playing.

		if( stepData[stepNum].noteStatus == CURRENTLY_PLAYING ){
	//		noteData->noteOff = true;
	//		noteData->channel = channel;
	//		noteData->noteOffStep = stepNum;
	//		noteData->noteGateOffArray[stepNum] = gateOff;


			outputControl->noteOff(channel, stepData[stepNum].notePlaying, gateOff );
/*
			for (int f=0; f<stepCount; f++){
				if (noteData->noteOffArray[f] == NULL){
					noteData->noteOffArray[f] = stepData[stepNum].notePlaying;
					break;
				}
			}
*/
			if ( stepData[stepNum].arpStatus > getArpCount(stepNum) && stepData[stepNum].noteStatus != NOTE_HAS_BEEN_PLAYED_THIS_ITERATION ){
				stepData[stepNum].noteStatus = NOTE_HAS_BEEN_PLAYED_THIS_ITERATION;
			//	Serial.println(String(stepNum) + " has been played arpCount: " + String(getArpCount(stepNum)) + "\tarpStatus: " + String(stepData[stepNum].arpStatus) );
			} else {
				//Serial.println(String(stepNum) + " did not reset:  arpCount: " + String(getArpCount(stepNum)) + "\tarpStatus: " + String(stepData[stepNum].arpStatus) );
			}
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
