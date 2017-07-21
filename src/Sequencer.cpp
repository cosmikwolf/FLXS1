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
  ppqPulseIndex = 0;

  if (activeStepReset){
    swingSwitch = false;
    firstPulse = true;

//    ppqPulseIndex = -1;
  }

  clockSinceLastPulse = 0;
  getCurrentFrame();

  for (int stepNum = 0; stepNum < (firstStep + stepCount); stepNum++){
    if(stepNum == activeStep){
  //    break;
    }
    stepData[stepNum].noteStatus = AWAITING_TRIGGER;
    stepData[stepNum].arpStatus = 0;
  }

  //calculateStepTimers();
  //lastStepOffset = 0;
  //firstPulse = 1;
  if(channel ==0 ){
   Serial.println("Ch " + String(channel) + " reset " + "\tcurrentFrame: " + String(getCurrentFrame()));
  }

  if((stepCount) %2 ){
    swingSwitch = !swingSwitch;
  }
}

void Sequencer::clockStart(){
	pulseTimer = 0;
  swingSwitch = 0;
};

void Sequencer::masterClockPulse(){
  if (!playing){
    //return;
  }

  clockSinceLastPulse++;
  for (int stepNum=0; stepNum < firstStep + stepCount; stepNum++){
  //  if (stepData[stepNum].framesRemaining > 0){
     stepData[stepNum].framesRemaining -= (framesPerPulse/avgClocksPerPulse);  // for arp with no swing
      // if(swingSwitch + activeStep % 2){
      //   stepData[stepNum].framesRemaining -= ((100+swingX100)*framesPerPulse/avgClocksPerPulse)/100 ;
      // }else{
      //   stepData[stepNum].framesRemaining -= ((100-swingX100)*framesPerPulse/avgClocksPerPulse)/100 ;
      // }
  //  }
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


  if (firstPulse == false){
    avgClocksPerPulse = clockSinceLastPulse ;// ( clockSinceLastPulse + 2 * avgClocksPerPulse ) / 3;
    ppqPulseIndex++;
  }  else {
    if(globalObj->clockMode != EXTERNAL_MIDI_CLOCK){
      ppqPulseIndex++; // this may need to be commented to get midi to work
    }

  }

  firstPulse = false;
  clockSinceLastPulse = 0;

  if (ppqPulseIndex > pulsesPerBeat*stepCount* clockDivisionNum() / clockDivisionDen() ){
    //this->clockReset(false);
  }
  //ppqPulseIndex = ppqPulseIndex % (pulsesPerBeat*stepCount* clockDivisionNum() / clockDivisionDen() );

  framesPerPulse = FRAMES_PER_BEAT / pulsesPerBeat;
};

uint32_t Sequencer::getCurrentFrame(){
  int32_t clockCount = 0;
  uint32_t currentFrame = 0 ;
  uint8_t lastActiveStep;
  // this prevents clocksSinceLastPulse from exceeding avgClocksPerPulse, which results in the currentFrame going backwards
  if(clockSinceLastPulse >= avgClocksPerPulse){
    clockCount = avgClocksPerPulse - 1;
  } else {
    clockCount = clockSinceLastPulse;
  }

  //return ((ppqPulseIndex * framesPerPulse % framesPerSequence() ) + (framesPerPulse * workingCpp / avgClocksPerPulse) + (firstStep *  getStepLength()))% (MAX_STEPS_PER_SEQUENCE * getStepLength());

  currentFrame = (ppqPulseIndex * framesPerPulse % framesPerSequence() ) + (framesPerPulse * clockCount / avgClocksPerPulse);
  lastActiveStep = activeStep;




  //  activeStep = isFrameSwinging(currentFrame);
  uint32_t framesSinceLastStep = 0;
  uint8_t preSwingActivestep = 0;

  framesSinceLastStep  =  currentFrame % getStepLength() ;

  if (playDirection == PLAY_REVERSE) {
    preSwingActivestep = firstStep + stepCount - currentFrame / getStepLength();

    activeStep = min_max(preSwingActivestep, firstStep, MAX_STEPS_PER_SEQUENCE - 1);
  //  activeStep = firstStep + stepCount - max(lastStepFrame,(currentFrame-swingOffset )) / getStepLength() - 1 ;
  } else if (playDirection == PLAY_FORWARD) {
    preSwingActivestep = firstStep + currentFrame / getStepLength();

    if( (swingX100 > 50 )
      &( (swingSwitch + preSwingActivestep) % 2)
      & (currentFrame%getStepLength() <= getStepLength() * (2*swingX100-100)/100 )){
      // if the current frame liees when the activeStep has advanced according to master clock,
      // but before the point where it should retrigger, keep activestep 1 step behind
      // swing step has not begun yet.
    //  activeStep = min_max(preSwingActivestep - 1, firstStep, MAX_STEPS_PER_SEQUENCE - 1);
      activeStep = preSwingActivestep - 1;
      swinging = false;
    } else if(   (swingX100 < 50 )
              & !((swingSwitch + preSwingActivestep) % 2)
              & (currentFrame%getStepLength() >= getStepLength() * (2*swingX100)/100 ) ){
          // activeStep = min_max(preSwingActivestep + 1, firstStep, MAX_STEPS_PER_SEQUENCE - 1 );
      activeStep = preSwingActivestep + 1;
      swinging = true;
    } else {
      //activeStep = min_max(preSwingActivestep, firstStep, MAX_STEPS_PER_SEQUENCE - 1);
      activeStep = preSwingActivestep;
      if (swingCount % 2){
        swinging = true;
      } else {
        swinging = false;
      }
    }

    activeStep = min_max_cycle(activeStep, firstStep, firstStep+stepCount-1);
  };


    if (channel == 1 && millis() % 20 == 0){
    // Serial.println(String(millis()) + "\tSwing: "+ String(swingSwitch) + " activeStep "  + String( activeStep ) + "\tstepLength: " + String(getStepLength()) + "\tswingX100: " + String(swingX100) + "\tcurrentFrame:"  + String(currentFrame) + "\tframesSinceLastStep:" + String(framesSinceLastStep) );
    }

    if ( activeStep != lastActiveStep ){
      swingCount += 1;
      //swingSwitch = !swingSwitch;
    //  if(channel == 0) Serial.println(String(millis()) + "\tSwing: "+ String(swingSwitch) + " activeStep "  + String( activeStep ) + "\tpreSwingActivestep "  + String( preSwingActivestep ) + "\tlastActiveStep: " + String(lastActiveStep) + "\tstepLength: " + String(getStepLength()) + "\tswingX100: " + String(swingX100) + "\tcurrentFrame:"  + String(currentFrame) + "\tframesSinceLastStep:" + String(framesSinceLastStep) + "\tfirstStep: " + String(firstStep) + "\tstepCount: " + String(stepCount));

    }
  if (currentFrame > framesPerSequence() ){
    this->clockReset(false);
   //if (channel == 1){ Serial.println("CLOCK RESET!"); };
  }

  return currentFrame;

}

bool Sequencer::isFrameSwinging(uint32_t frame){
  uint8_t preSwingActivestep = 0;
  bool swingTemp = 0;

  if (playDirection == PLAY_REVERSE) {
    preSwingActivestep = firstStep + stepCount - frame / getStepLength();

    //activeStep = preSwingActivestep;
  //  activeStep = firstStep + stepCount - max(lastStepFrame,(frame -swingOffset )) / getStepLength() - 1 ;
  } else if (playDirection == PLAY_FORWARD) {
    preSwingActivestep = firstStep + frame / getStepLength();
    if((swingSwitch + preSwingActivestep % 2) & (frame%getStepLength() < (getStepLength() * swingX100)/100 )){
      // if the current frame liees when the activeStep has advanced according to master clock,
      // but before the point where it should retrigger, keep activestep 1 step behind
      // swing step has not begun yet.
      swingTemp = false;
    }else if(swingSwitch + preSwingActivestep % 2){      // is a swing step
      swingTemp = true;
    } else { // is not a swing step
      swingTemp = false;
    }
  };

  return swingTemp;
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

// uint32_t Sequencer::calculateStepTimers(){
// 	//stepLength = beatLength*beatCount/stepCount;
// 	uint32_t accumulatedOffset = 0;
// 	//beatOffset
// 	//stepLength = beatLength/stepDivider*stepCount;
// 	for (int stepNum = activeStep; stepNum < firstStep + stepCount + activeStep-1; stepNum++){
//   //  stepData[stepNum % (firstStep + stepCount)].offset = accumulatedOffset;
//     accumulatedOffset += getStepLength();
// 	}
// 	return accumulatedOffset;
// }


void Sequencer::sequenceModeStandardStep(){
  uint32_t currentFrameVar = 0;
  //incrementActiveStep(currentFrameVar);
  if (mute || !playing){
    return;
  }
  currentFrameVar = getCurrentFrame();

  if (stepData[activeStep].noteStatus == AWAITING_TRIGGER){
    if (stepData[activeStep].gateType != GATETYPE_REST){

      stepData[activeStep].arpStatus = 0;
      arpTypeModulated[activeStep] = min_max(stepData[activeStep].arpType + outputControl->cvInputCheck(cv_arptypemod)/20, 0, 5);
      arpOctaveModulated[activeStep] = min_max(stepData[activeStep].arpOctave + outputControl->cvInputCheck(cv_arpoctmod)/20, 1, 5);
      arpSpeedModulation[activeStep] = outputControl->cvInputCheck(cv_arpspdmod)/15;
      noteTrigger(activeStep, stepData[activeStep].gateTrig(), arpTypeModulated[activeStep], arpOctaveModulated[activeStep] );
      stepData[activeStep].noteStatus = CURRENTLY_PLAYING;
      //Serial.println("Triggering Step: " + String(activeStep) + "\tppqPulseIndex: " + String(ppqPulseIndex ) + "\tppqModulo: "+String(pulsesPerBeat*stepCount/clockDivision)  + "\tppB: " + String(pulsesPerBeat) + "\tstepCount: " + String(stepCount) + "\tclockdiv:" + String(clockDivision) + "\tnoteStatus: "+ String(stepData[activeStep].noteStatus));
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
          //Serial.println("note shut off: " + String(stepNum));
          stepData[stepNum].noteStatus = BETWEEN_APEGGIATIONS;
          if ( stepData[stepNum].arpStatus > getArpCount(stepNum) ){
            stepData[stepNum].noteStatus = NOTE_HAS_BEEN_PLAYED_THIS_ITERATION;
          }
        }
      break;

      case BETWEEN_APEGGIATIONS:
        // Arpeggio retrigger
        if(currentFrameVar > getArpStartFrame(stepNum, stepData[stepNum].arpStatus) ){
        //if ( getFramesRemaining(stepNum) <= 0 ) {
          // Serial.println("arpRetrigger: "  + String(stepNum));
           noteTrigger(stepNum, stepData[stepNum].gateTrig(), arpTypeModulated[stepNum], arpOctaveModulated[stepNum] );
           stepData[stepNum].noteStatus = CURRENTLY_PLAYING;
        }
      break;
    }
	}
}
