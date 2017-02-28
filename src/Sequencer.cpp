 #include "Arduino.h"
#include "Sequencer.h"

// noteStatus indicates the status of the next note
// 0 indicates not playing, not queued
// 1 indicates the note is currently playing
// 2 indicates the note is currently queued.
// 3 indicates that the note is currently playing and currently queued
// 4 indicates that the note has been played this iteration
// stepData[activeStep].noteStatus = stepData[activeStep].pitch;


void Sequencer::clockReset(bool activeStepReset){
  if (activeStepReset){
    beatCount = 0;
  }

  for (int stepNum = 0; stepNum < stepCount; stepNum++){
    if(stepNum == activeStep){
      break;
    }
    stepData[stepNum].noteStatus = AWAITING_TRIGGER;
    stepData[stepNum].arpStatus = 0;
  }

  calculateStepTimers();
  lastStepOffset = 0;
  ppqPulseIndex = 0;

//  firstPulse = 1;
  if(channel ==0 ){
    Serial.println("Ch " + String(channel) + " reset " + "\tcurrentFrame: " + String(getCurrentFrame()));
  }

}

void Sequencer::clockStart(elapsedMicros startTime){
	pulseTimer = 0;
};

void Sequencer::masterClockPulse(){
  clockSinceLastPulse++;
  for (int stepNum=0; stepNum<stepCount; stepNum++){
    if (stepData[stepNum].framesRemaining > 0){
      stepData[stepNum].framesRemaining -= (framesPerPulse / avgClocksPerPulse);
    }
  }
}

void Sequencer::runSequence(){
	if(playing){
    sequenceModeStandardStep();
  }
}

void Sequencer::ppqPulse(uint8_t pulsesPerBeat){
  this->pulsesPerBeat = pulsesPerBeat;
  ppqPulseIndex++;
  pulsesSinceResync++;
	avgClocksPerPulse = ( clockSinceLastPulse + 2 * avgClocksPerPulse ) / 3;
  clockSinceLastPulse = 0;
  framesPerPulse = FRAMES_PER_BEAT / pulsesPerBeat;

  for (int stepNum=0; stepNum<stepCount; stepNum++){
    if (stepData[stepNum].framesRemaining > 0){
    //  stepData[stepNum].framesRemaining -= framesPerPulse;
    }
  }

  if (!playing){
    return;
  }

  for (int stepNum = 1; stepNum < pulsesPerBeat; stepNum++){
    if ( ((stepNum*pulsesPerBeat) % clockDivision) == 0 ){
      syncStep = stepNum;
      break;
    }
  }
  if (pulsesSinceResync >= syncStep * pulsesPerBeat){
    pulsesSinceResync = 0;
    stepsSinceResync = 0;
    beatCount++;
    beatCount = beatCount % ((stepCount)/clockDivision);
    this->clockReset(false);
    if(channel==0){
      Serial.println("resync: syncStep: " + String(syncStep) + "\tpulsesPerBeat: " + String(pulsesPerBeat) + "\tClockdivision: " + String(clockDivision) + "\tbeatcount: " + String(beatCount) + "\ttotalBeats: " + String( ((stepCount-1)/clockDivision))  + "\tavgClkPP: " + String(avgClocksPerPulse));
    }
  }

};

uint32_t Sequencer::getCurrentFrame(){
  uint32_t currentFrame = pulsesSinceResync * framesPerPulse + framesPerPulse * clockSinceLastPulse / avgClocksPerPulse;
	return currentFrame;
}

void Sequencer::getActiveStep(uint32_t frame){
  activeStep = getCurrentFrame() / getStepLength() + beatCount * clockDivision ;
}

uint32_t Sequencer::getStepLength(){
		if (clockDivision > 0){
			//return beatLength/stepData[stepNum].beatDiv;
			return FRAMES_PER_BEAT / clockDivision;
		} else { // negative values of beatDiv allow for whole multiples of beatLength
			return FRAMES_PER_BEAT*(abs(clockDivision)+2);
		//	return beatLength*(abs(stepData[stepNum].beatDiv)+2);
		}
}

uint32_t Sequencer::calculateStepTimers(){
	//stepLength = beatLength*beatCount/stepCount;
	uint32_t accumulatedOffset = 0;
	//beatOffset
	//stepLength = beatLength/stepDivider*stepCount;
	for (int stepNum = activeStep; stepNum < stepCount + activeStep-1; stepNum++){
    stepData[stepNum % stepCount].offset = accumulatedOffset;
    accumulatedOffset += getStepLength();
	}
	return accumulatedOffset;
}


void Sequencer::sequenceModeStandardStep(){
  uint32_t currentFrameVar = getCurrentFrame();
  getActiveStep(currentFrameVar);

  if (stepData[activeStep].noteStatus == AWAITING_TRIGGER){
    if (stepData[activeStep].gateType != GATETYPE_REST){
      noteTrigger(activeStep, stepData[activeStep].gateTrig());
      stepData[activeStep].noteStatus = CURRENTLY_PLAYING;
      //stepData[activeStep].offset = currentFrameVar;
    }
  }

	for (int stepNum = 0; stepNum < activeStep + 1; stepNum++){
	// iterate through all steps to determine if they need to have action taken.
		if (stepData[stepNum].noteStatus == NOTE_HAS_BEEN_PLAYED_THIS_ITERATION){
			continue;
		}

		if (stepData[stepNum].gateType == GATETYPE_REST){
			continue;
		}

		uint32_t stepOffTime = (stepData[stepNum].gateLength+1)*getStepLength()/4;
		uint32_t trigLength;

		if (stepData[stepNum].arpType != ARPTYPE_OFF ){
			trigLength = stepData[stepNum].arpSpdNum*getStepLength()/stepData[stepNum].arpSpdDen;
		} else {
			trigLength = stepOffTime;
		};

		if (stepData[stepNum].noteStatus == CURRENTLY_PLAYING){
    //  if ( currentFrameVar > (stepData[stepNum].offset + stepData[stepNum].arpStatus * trigLength  - trigLength/2 ) ) {
      if ( stepData[stepNum].framesRemaining <= 0 ) {
  				noteShutOff(stepNum, stepData[stepNum].gateOff());
				if ( stepData[stepNum].arpStatus > getArpCount(stepNum) ){
					stepData[stepNum].noteStatus = NOTE_HAS_BEEN_PLAYED_THIS_ITERATION;
				} else {
					stepData[stepNum].noteStatus = AWAITING_TRIGGER;
				}
			}
		}
	}
}
