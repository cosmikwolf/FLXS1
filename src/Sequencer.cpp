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
    ppqPulseIndex = 0;
    clockSinceLastPulse = 0;
    getActiveStep(getCurrentFrame());
  }

  for (int stepNum = 0; stepNum < stepCount; stepNum++){
    if(stepNum == activeStep){
      break;
    }
    stepData[stepNum].noteStatus = AWAITING_TRIGGER;
    stepData[stepNum].arpStatus = 0;
  }

  calculateStepTimers();
  //lastStepOffset = 0;

//  firstPulse = 1;
  if(channel ==0 ){
    Serial.println("Ch " + String(channel) + " reset " + "\tcurrentFrame: " + String(getCurrentFrame()));
  }

}

void Sequencer::clockStart(elapsedMicros startTime){
	pulseTimer = 0;
};

void Sequencer::masterClockPulse(){
  if (!playing){
    return;
  }

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
  if (!playing){
    return;
  }

  ppqPulseIndex++;
  ppqPulseIndex = ppqPulseIndex % (pulsesPerBeat*stepCount* clockDivisionNum() / clockDivisionDen() );

	avgClocksPerPulse = clockSinceLastPulse ;// ( clockSinceLastPulse + 2 * avgClocksPerPulse ) / 3;
  clockSinceLastPulse = 0;
  framesPerPulse = FRAMES_PER_BEAT / pulsesPerBeat;

  for (int stepNum = 1; stepNum < pulsesPerBeat; stepNum++){
    if ( ((stepNum*pulsesPerBeat) % ( clockDivisionNum() / clockDivisionDen())) == 0 ){
      syncStep = stepNum;
      break;
    }
  }


/*
  if (pulsesSinceResync >= syncStep * pulsesPerBeat / clockDivision){
    pulsesSinceResync = 0;
    resyncSinceReset++;
  //  resyncSinceReset %=
    this->clockReset(false);
    if(channel==0){
      Serial.println("resync: syncStep: " + String(syncStep) + "\tpulsesPerBeat: " + String(pulsesPerBeat) + "\tClockdivision: " + String(clockDivision) + "\tbeatcount: " + String(beatCount) + "\ttotalBeats: " + String( ((stepCount-1)/clockDivision))  + "\tavgClkPP: " + String(avgClocksPerPulse));
    }
  }
*/

// reset has to happen on syncStep. Does not have to happen every iteration.
// if each sequence runs on ppq index, and then resets on a syncstep that also happens to
// be the activeStep == 0, ppq index could run to a very large value and wont cause problems for very long running sequences.
};

uint32_t Sequencer::getCurrentFrame(){
  uint32_t workingCpp;
  // this prevents clocksSinceLastPulse from exceeding avgClocksPerPulse, which results in the currentFrame going backwards
  if(clockSinceLastPulse > avgClocksPerPulse){
    workingCpp = avgClocksPerPulse;
  } else {
    workingCpp = clockSinceLastPulse;
  }
  return (ppqPulseIndex * framesPerPulse % framesPerSequence() ) + (framesPerPulse * workingCpp / avgClocksPerPulse);
}

void Sequencer::getActiveStep(uint32_t frame){
  // getCurrentFrame() / getStepLength()  <-- steps since last beat

  activeStep = getCurrentFrame() / getStepLength();

  if( lastActiveStep > activeStep ){
  //  if (channel ==0)Serial.println("activestep changed  old: " + String(lastActiveStep) + "\tnew: " + String(activeStep) + "\tppqIndex: " + String(ppqPulseIndex) + "\tpulseFrames: " + String(ppqPulseIndex * framesPerPulse) + "\tFPP: " + String(framesPerPulse) + "\tframesfromclocks: " + String(framesPerPulse * clockSinceLastPulse / avgClocksPerPulse) + "\tcurrentFrame: " + String(getCurrentFrame()) + "\tclocksSincePulse: " + String(clockSinceLastPulse) + "\tavgCpp:" + String(avgClocksPerPulse));
  }

  lastActiveStep = activeStep;
}

uint32_t Sequencer::framesPerSequence(){
  return getStepLength()*stepCount;
}

uint32_t Sequencer::getStepLength(){
  return FRAMES_PER_BEAT * clockDivisionNum() / clockDivisionDen();

		// if (clockDivision > 0){
		// 	//return beatLength/stepData[stepNum].beatDiv;
		// 	return FRAMES_PER_BEAT / clockDivision;
		// } else { // negative values of beatDiv allow for whole multiples of beatLength
		// 	return FRAMES_PER_BEAT*(abs(clockDivision)+2);
		// //	return beatLength*(abs(stepData[stepNum].beatDiv)+2);
		// }
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
  //incrementActiveStep(currentFrameVar);

  if (stepData[activeStep].noteStatus == AWAITING_TRIGGER){
    if (stepData[activeStep].gateType != GATETYPE_REST){

      stepData[activeStep].arpStatus = 0;

      arpTypeModulated[activeStep] = min_max(stepData[activeStep].arpType + cvInputMapped[cv_arptypemod]/20, 0, 5);

      arpOctaveModulated[activeStep] = min_max(stepData[activeStep].arpOctave + cvInputMapped[cv_arpoctmod]/20, 1, 5);
      arpSpeedModulation[activeStep] = cvInputMapped[cv_arpspdmod]/15;

      noteTrigger(activeStep, stepData[activeStep].gateTrig(), arpTypeModulated[activeStep], arpOctaveModulated[activeStep] );
      stepData[activeStep].noteStatus = CURRENTLY_PLAYING;
      //Serial.println("Triggering Step: " + String(activeStep) + "\tppqPulseIndex: " + String(ppqPulseIndex ) + "\tppqModulo: "+String(pulsesPerBeat*stepCount/clockDivision)  + "\tppB: " + String(pulsesPerBeat) + "\tstepCount: " + String(stepCount) + "\tclockdiv:" + String(clockDivision));
      //stepData[activeStep].offset = currentFrameVar;
    }
  }

	for (int stepNum = 0; stepNum < stepCount; stepNum++){
	// iterate through all steps to determine if they need to have action taken.

		if (stepData[stepNum].gateType == GATETYPE_REST){
			continue;
		}

    switch (stepData[stepNum].noteStatus){
      case NOTE_HAS_BEEN_PLAYED_THIS_ITERATION:
        if(stepNum != activeStep){
          //Serial.println("Resetting step: " + String(stepNum) + "\tactiveStep: " + String(activeStep)) ;
          stepData[stepNum].noteStatus = AWAITING_TRIGGER;
          stepData[stepNum].arpStatus = 0;
        }
  			continue;
      break;
      case CURRENTLY_PLAYING:
        if(stepData[stepNum].framesRemaining < stepData[stepNum].arpLastFrame){
          noteShutOff(stepNum, stepData[stepNum].gateOff());
          //Serial.println("note shut off");
          stepData[stepNum].noteStatus = BETWEEN_APEGGIATIONS;
          if ( stepData[stepNum].arpStatus > getArpCount(stepNum) ){
            stepData[stepNum].noteStatus = NOTE_HAS_BEEN_PLAYED_THIS_ITERATION;
          }
        }

      break;

      case BETWEEN_APEGGIATIONS:
        // Arpeggio retrigger
        if ( stepData[stepNum].framesRemaining <= 0 ) {
            noteTrigger(stepNum, stepData[stepNum].gateTrig(), arpTypeModulated[stepNum], arpOctaveModulated[stepNum] );
            stepData[stepNum].noteStatus = CURRENTLY_PLAYING;
        }
      break;
    }
	}
}
