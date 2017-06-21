#include "Arduino.h"
#include "Sequencer.h"

#define min(X, Y)  ((X) < (Y) ? (X) : (Y))
#define max(X, Y)  ((X) > (Y) ? (X) : (Y))

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
    getCurrentFrame();
  }

  for (int stepNum = 0; stepNum < (firstStep + stepCount); stepNum++){
    if(stepNum == activeStep){
  //    break;
    }
    stepData[stepNum].noteStatus = AWAITING_TRIGGER;
    stepData[stepNum].arpStatus = 0;
  }

  calculateStepTimers();
  //lastStepOffset = 0;
  //firstPulse = 1;
  if(channel ==0 ){
   //Serial.println("Ch " + String(channel) + " reset " + "\tcurrentFrame: " + String(getCurrentFrame()));
  }

  if((stepCount) %2 ){
    swingSwitch = !swingSwitch;
  }
  justReset = true;
}

void Sequencer::clockStart(elapsedMicros startTime){
	pulseTimer = 0;
  swingSwitch = 0;
};

void Sequencer::masterClockPulse(){
  if (!playing){
    //return;
  }

  clockSinceLastPulse++;
  for (int stepNum=0; stepNum < firstStep + stepCount; stepNum++){
    if (stepData[stepNum].framesRemaining > 0){
    //  stepData[stepNum].framesRemaining -= (framesPerPulse/avgClocksPerPulse);  // for arp with no swing
      if(swingSwitch + activeStep % 2){
        stepData[stepNum].framesRemaining -= ((100+swingX100)*framesPerPulse/avgClocksPerPulse)/100 ;
      }else{
        stepData[stepNum].framesRemaining -= ((100-swingX100)*framesPerPulse/avgClocksPerPulse)/100 ;
      }
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
  if (ppqPulseIndex > pulsesPerBeat*stepCount* clockDivisionNum() / clockDivisionDen() ){
    this->clockReset(true);
    ppqPulseIndex = 0;
  }
  //ppqPulseIndex = ppqPulseIndex % (pulsesPerBeat*stepCount* clockDivisionNum() / clockDivisionDen() );

	avgClocksPerPulse = clockSinceLastPulse ;// ( clockSinceLastPulse + 2 * avgClocksPerPulse ) / 3;
  clockSinceLastPulse = 0;
  framesPerPulse = FRAMES_PER_BEAT / pulsesPerBeat;
};

uint32_t Sequencer::getCurrentFrame(){
  int32_t clockCount = 0;
  int32_t swingFrames = 0;
  int32_t swingOffset = 0;
  int32_t lastStepFrame = 0;
  uint32_t currentFrame = 0 ;
  uint32_t framesSinceLastStep = 0;
  uint8_t preSwingActivestep = 0;
  uint8_t lastActiveStep;
  // this prevents clocksSinceLastPulse from exceeding avgClocksPerPulse, which results in the currentFrame going backwards
  if(clockSinceLastPulse > avgClocksPerPulse){
    clockCount = avgClocksPerPulse;
  } else {
    clockCount = clockSinceLastPulse;
  }

  //return ((ppqPulseIndex * framesPerPulse % framesPerSequence() ) + (framesPerPulse * workingCpp / avgClocksPerPulse) + (firstStep * getStepLength()))% (MAX_STEPS_PER_SEQUENCE * getStepLength());

  currentFrame = (ppqPulseIndex * framesPerPulse % framesPerSequence() ) + (framesPerPulse * clockCount / avgClocksPerPulse);
  framesSinceLastStep  =  currentFrame % getStepLength();

    lastActiveStep = activeStep;

    if (swingCount % 2){
      swingOffset = (getStepLength() * swingX100) / 100;
    } else {
      swingOffset = 0;
    }

    framesSinceLastStep  =  currentFrame % getStepLength() ;
    lastStepFrame = currentFrame - framesSinceLastStep ;

    swingFrames = (getStepLength() * swingX100) / 100;

    if (currentFrame > framesPerSequence() ){
      this->clockReset(true);
     //if (channel == 1){ Serial.println("CLOCK RESET!"); };
    }

    for (int i=firstStep; i< firstStep + stepCount; i++ ){

    }
    // swingStepStartTime = preSwingActivestep * getStepLength() + swingFrames;
    // swingStepEndTime   = (preSwingActivestep + 1) * getStepLength();
    //
    // normalStepStartTime = preSwingActivestep * getStepLength()
    // normalStepEndTime   = (preSwingActivestep + 1) * getStepLength() + swingFrames;

    if (playDirection == PLAY_REVERSE) {
      preSwingActivestep = firstStep + stepCount - currentFrame / getStepLength();
      activeStep = firstStep + stepCount - max(lastStepFrame,(currentFrame-swingOffset )) / getStepLength() - 1 ;
      // if ((activeStep > lastActiveStep) & justReset){
      //   activeStep = lastActiveStep;
      // }
    } else if (playDirection == PLAY_FORWARD) {
      //activeStep = firstStep + (currentFrame-swingOffset) / getStepLength();

      //activeStep = firstStep + lastStepFrame / getStepLength() - 1 + (getStepLength()+framesSinceLastStep) / (swingOffset + getStepLength());
      preSwingActivestep = firstStep + currentFrame / getStepLength();

      if((swingSwitch + preSwingActivestep % 2) & (currentFrame%getStepLength() < getStepLength() * swingX100/100 )){
          activeStep = preSwingActivestep - 1;
      }else {
        activeStep = preSwingActivestep;
      }
    };

    if (channel == 1 && millis() % 20 == 0){
  //   Serial.println(String(millis()) + "\tSwing: "+ String(swingSwitch) + " activeStep "  + String( activeStep ) + "\tstepLength: " + String(getStepLength()) + "\tswingX100: " + String(swingX100) + "\tcurrentFrame:"  + String(currentFrame) + "\tswingOffset: " + String(swingOffset) + "\tframesSinceLastStep:" + String(framesSinceLastStep) + "\tLastStepFrame: " + String(lastStepFrame));
    }

    if ( activeStep != lastActiveStep ){
      swingCount += 1;
      //swingSwitch = !swingSwitch;
  //    if(channel ==1) Serial.println("Activestep Changed: " + String(activeStep));
    }

    justReset = false;
  return currentFrame;

}

void Sequencer::getActiveStep(uint32_t frame){
  // getCurrentFrame() / getStepLength()  <-- steps since last beat
  // if (playDirection == PLAY_REVERSE) {
  //   activeStep = firstStep + stepCount - getCurrentFrame() / getStepLength() - 1;
  // } else if (playDirection == PLAY_FORWARD) {
  //   activeStep = firstStep + getCurrentFrame() / getStepLength();
  // };

//
// NUM  activeStep       REVERSED
// 0    12                 14
// 1    13                 13
// 2    14                 12
// NUM = ACTIVESTEP-FIRSTSTEP
//
// FIRSTSTEP + STEPCOUNT - (ACTIVESTEP - FIRSTSTEP) - 1
// 2 * FIRSTSTEP + STEPCOUNT - ACTIVESTEP - 1
// 12 + 3 - (12 - 12) - 1 = 14
//
// 12 + 3 - 0 - 1  = 14
// 12 + 3 - 1 - 1  = 13
//
//
// firstStep
// 12            13
//
// stepCount
// 3

  //if( lastActiveStep > activeStep ){
  //  if (channel ==0)Serial.println("activestep changed  old: " + String(lastActiveStep) + "\tnew: " + String(activeStep) + "\tppqIndex: " + String(ppqPulseIndex) + "\tpulseFrames: " + String(ppqPulseIndex * framesPerPulse) + "\tFPP: " + String(framesPerPulse) + "\tframesfromclocks: " + String(framesPerPulse * clockSinceLastPulse / avgClocksPerPulse) + "\tcurrentFrame: " + String(getCurrentFrame()) + "\tclocksSincePulse: " + String(clockSinceLastPulse) + "\tavgCpp:" + String(avgClocksPerPulse));
  //}

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

int Sequencer::getActivePage(){
  return activeStep/16;
}

uint32_t Sequencer::calculateStepTimers(){
	//stepLength = beatLength*beatCount/stepCount;
	uint32_t accumulatedOffset = 0;
	//beatOffset
	//stepLength = beatLength/stepDivider*stepCount;
	for (int stepNum = activeStep; stepNum < firstStep + stepCount + activeStep-1; stepNum++){
    stepData[stepNum % (firstStep + stepCount)].offset = accumulatedOffset;
    accumulatedOffset += getStepLength();
	}
	return accumulatedOffset;
}


void Sequencer::sequenceModeStandardStep(){
  uint32_t currentFrameVar = getCurrentFrame();
  //incrementActiveStep(currentFrameVar);
  if (mute || !playing){
    return;
  }

  if (stepData[activeStep].noteStatus == AWAITING_TRIGGER){
    if (stepData[activeStep].gateType != GATETYPE_REST){

      stepData[activeStep].arpStatus = 0;
      arpTypeModulated[activeStep] = min_max(stepData[activeStep].arpType + outputControl->cvInputCheck(cv_arptypemod)/20, 0, 5);
      arpOctaveModulated[activeStep] = min_max(stepData[activeStep].arpOctave + outputControl->cvInputCheck(cv_arpoctmod)/20, 1, 5);
      arpSpeedModulation[activeStep] = outputControl->cvInputCheck(cv_arpspdmod)/15;
      noteTrigger(activeStep, stepData[activeStep].gateTrig(), arpTypeModulated[activeStep], arpOctaveModulated[activeStep] );
      stepData[activeStep].noteStatus = CURRENTLY_PLAYING;
      Serial.println("Triggering Step: " + String(activeStep) + "\tppqPulseIndex: " + String(ppqPulseIndex ) + "\tppqModulo: "+String(pulsesPerBeat*stepCount/clockDivision)  + "\tppB: " + String(pulsesPerBeat) + "\tstepCount: " + String(stepCount) + "\tclockdiv:" + String(clockDivision) + "\tnoteStatus: "+ String(stepData[activeStep].noteStatus));
      //stepData[activeStep].offset = currentFrameVar;
    }
  }

	for (int stepNum = 0; stepNum <= (firstStep + stepCount); stepNum++){
	// iterate through all steps to determine if they need to have action taken.

		if (stepData[stepNum].gateType == GATETYPE_REST){
			continue;
		}
    //Serial.println("noteStatus: " + String(stepData[stepNum].noteStatus));

    switch (stepData[stepNum].noteStatus){
      case NOTE_HAS_BEEN_PLAYED_THIS_ITERATION:
        if(stepNum != activeStep || ( stepCount == 1 && currentFrameVar < getStepLength()/4 ) ){
          //Serial.println("Resetting step: " + String(stepNum) + "\tactiveStep: " + String(activeStep)) ;
          stepData[stepNum].noteStatus = AWAITING_TRIGGER;
          stepData[stepNum].arpStatus = 0;
        }
  			continue;
      break;
      case CURRENTLY_PLAYING:
        if(getFramesRemaining(stepNum) < stepData[stepNum].arpLastFrame){
          noteShutOff(stepNum, stepData[stepNum].gateOff());
          Serial.println("note shut off: " + String(stepNum));
          stepData[stepNum].noteStatus = BETWEEN_APEGGIATIONS;
          if ( stepData[stepNum].arpStatus > getArpCount(stepNum) ){
            stepData[stepNum].noteStatus = NOTE_HAS_BEEN_PLAYED_THIS_ITERATION;
          }
        }
      break;

      case BETWEEN_APEGGIATIONS:
        // Arpeggio retrigger

        if ( getFramesRemaining(stepNum) <= 0 ) {
           Serial.println("arpRetrigger: "  + String(stepNum));
           noteTrigger(stepNum, stepData[stepNum].gateTrig(), arpTypeModulated[stepNum], arpOctaveModulated[stepNum] );
           stepData[stepNum].noteStatus = CURRENTLY_PLAYING;
        }
      break;
    }
	}
}
