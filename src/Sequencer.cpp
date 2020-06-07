#include "Arduino.h"
#include "Sequencer.h"

#define min(X, Y) ((X) < (Y) ? (X) : (Y))
#define max(X, Y) ((X) > (Y) ? (X) : (Y))

// noteStatus indicates the status of the next note
// 0 indicates not playing, not queued
// 1 indicates the note is currently playing
// 2 indicates the note is currently queued.
// 3 indicates that the note is currently playing and currently queued
// 4 indicates that the note has been played this iteration
// stepData[activeStep].noteStatus = stepData[activeStep].pitch;

void Sequencer::clockReset(bool activeStepReset)
{
  globalObj->channelResetSwich[channel] = true;

  if (globalObj->chainModeMasterChannel[globalObj->chainModeIndex] == channel)
  {
    globalObj->chainModeCountSwitch = 0;
  }
  /*
  if( channel == globalObj->chainModeMasterChannel[globalObj->chainModeIndex]){
      if(globalObj->waitingToResetAfterPatternLoad){
        for(int resetChannel = 0; resetChannel < SEQUENCECOUNT; resetChannel++){
          if(globalObj->chainChannelSelect[resetChannel][globalObj->chainModeIndex] == 0){
            continue;
          }
          if(resetChannel == channel) continue;
        //  Serial.println(String(millis()) + "\tresetting all sequences after pattern change " + String(resetChannel), );
        //  outputControl->sequenceArrayResetSequence(resetChannel);
        }
        globalObj->waitingToResetAfterPatternLoad = false;
        activeStepReset = true;
      }
  }*/

  ppqPulseIndex = 0;

  if (activeStepReset)
  {
    swingSwitch = false;
    firstPulse = true;
    receivedFirstExtClock = false;
    lastPulseClockCount = ARM_DWT_CYCCNT;
    lastStepIndex = 0;
    activeStep = firstStep;
    pendulumSwitch = false;
  }

  this->getCurrentFrame();
  // calculateActiveStep();

  for (int stepNum = 0; stepNum < MAX_STEPS_PER_SEQUENCE; stepNum++)
  {
    if (stepNum == activeStep)
    {
      //break;
    }
    stepData[stepNum].noteStatus = AWAITING_TRIGGER;
    stepData[stepNum].arpStatus = 0;
    stepData[stepNum].framesRemaining = 0;
  }

  outputControl->allNotesOff(channel);

  //calculateStepTimers();
  //lastStepOffset = 0;
  //firstPulse = 1;
  if (channel == 0)
  {
    // Serial.println("Ch " + String(channel) + " reset " + "\tcurrentFrame: " + String(currentFrame) + "\tactiveStep: " + String(activeStep)
    //  + "\tppqPulseIndex: " + String(ppqPulseIndex) + "\tframesPerPulse: " + String(framesPerPulse) + "\tframesPerSeq: " + String(framesPerSequence())
    //  + "\tfpp: " + String(framesPerPulse) + "\tavgCPP: " + String(avgClocksPerPulse) + "\lastClockValue: " + String(lastPulseClockCount)
    // currentFrame = (ppqPulseIndex * framesPerPulse % framesPerSequence() ) + (framesPerPulse * clockCount / avgClocksPerPulse)
    // );
    // Serial.println("reset --------------------------------------------");
  }
}

void Sequencer::clockStart()
{
  swingSwitch = 0;
  lastPulseClockCount = ARM_DWT_CYCCNT;

  // if(channel == 0) Serial.println(
  //   "CLOCKSTART Swing: "+ String(swinging) + " activeStep "  + String( activeStep ) +
  //    + "\tstepLength: " + String(getStepLength()) + // "\tswingX100: " + String(swingX100)
  //    + "\tcurrentFrame:"  + String(currentFrame) + "\tppqPulseIndex:" + String(ppqPulseIndex) + "\tfpp: " + String(framesPerPulse)+  "\tavgClocksPerPulse: " + String(avgClocksPerPulse) );
};

void Sequencer::masterClockPulse()
{
  if (!globalObj->playing)
  {
    //return;
  }
  uint32_t clockCycles = ARM_DWT_CYCCNT;
  uint32_t framesToSubtract = ((long long)framesPerPulse * (clockCycles - lastMasterClockCycleCount)) / avgClocksPerPulse;

  for (int stepNum = 0; stepNum < firstStep + stepCount; stepNum++)
  {
    if (stepData[stepNum].noteStatus == CURRENTLY_PLAYING)
    {
      if (framesToSubtract >= stepData[stepNum].framesRemaining)
      {
        stepData[stepNum].framesRemaining = 0;
      }
      else
      {
        stepData[stepNum].framesRemaining -= framesToSubtract;
      }
      //  if(framesToSubtract > 0) {
      //    Serial.println(String(stepNum) + " f2s: "+ String(framesToSubtract) + "\tavgCPP: " + String(avgClocksPerPulse) + "\tfpp: " + String(framesPerPulse) +"\tclock: " + String(clockCycles));
      //  }
    }
  }
  lastMasterClockCycleCount = clockCycles;
}

void Sequencer::runSequence()
{

  if (globalObj->playing)
  {
    sequenceModeStandardStep();
  }
}

void Sequencer::ppqPulse(uint8_t pulsesPerBeat)
{
  uint32_t clockCycles = ARM_DWT_CYCCNT;
  this->pulsesPerBeat = pulsesPerBeat;

  if (globalObj->extClock())
  {
    receivedFirstExtClock = true;
  }

  if (firstPulse == false)
  {
    //avgClocksPerPulse = ((clockCycles-lastPulseClockCount)+2*avgClocksPerPulse)/3;
    avgClocksPerPulse = (clockCycles - lastPulseClockCount);
    if (channel == 1)
    {
      //    Serial.println("clockCyclesSinceLast: " + String((clockCycles-lastPulseClockCount)) ) ;
    }
    if (globalObj->playing)
    {
      ppqPulseIndex++;
      //  ppqPulseIndex = ppqPulseIndex % (pulsesPerBeat*stepCount*clockDivisionNum()/clockDivisionDen());
    }
  }

  if (globalObj->playing)
  {
    firstPulse = false;
  }

  framesPerPulse = this->framesPerBeat(globalObj->tempoX100) / pulsesPerBeat;
  pulsesRemaining = (framesPerSequence() - currentFrame) / framesPerPulse;

  lastPulseClockCount = clockCycles;
};

uint32_t Sequencer::framesPerBeat(int tempoX100)
{
  //return ((60*100*96000000)/globalObj->tempoX100);
  return FRAMES_PER_BEAT;
}

void Sequencer::setStepCount(uint8_t newStepCount)
{
  int8_t lastStepIndexDifference = lastStepIndex - activeStep;
  //lastStepIndex = activeStep;
  stepCount = newStepCount;
  ppqPulseIndex = ppqPulseIndex % (pulsesPerBeat * stepCount);
}

void Sequencer::updateClockDivision(int8_t clockDiv)
{
  //designed to make sure ppq is still in sync after clock div change
  uint8_t oldClockDivNum = clockDivisionNum();
  uint8_t oldClockDivDen = clockDivisionDen();
  uint32_t origCurrentFrame = currentFrame;
  uint32_t origFramesPerSequence = framesPerSequence();
  long long tempPulseIndex = ppqPulseIndex;

  this->clockDivision = clockDiv;

  //Serial.println("org ppq index: " + String(ppqPulseIndex) + "\torgNum: " + String(oldClockDivNum) + "\torgDen: " + String(oldClockDivDen) +"\tttlPulsOrg: " + String(pulsesPerBeat*stepCount*oldClockDivNum / oldClockDivDen)+"\tttlPulsNew: " + String(pulsesPerBeat*stepCount*clockDivisionNum() / clockDivisionDen()) + "\tcf: " + String(currentFrame) + "\tfps: " + String(framesPerSequence()));

  //recalculate the ppqPulseIndex based on the new total PpqPulseCount with the new clockDiv
  tempPulseIndex = pulsesPerBeat * stepCount * clockDivisionNum() / clockDivisionDen();
  tempPulseIndex *= origCurrentFrame;
  tempPulseIndex /= origFramesPerSequence;
  ppqPulseIndex = tempPulseIndex % (pulsesPerBeat * stepCount * clockDivisionNum() / clockDivisionDen());

  // tempPulseIndex *= 1024;
  // tempPulseIndex *= oldClockDivDen;
  // tempPulseIndex *= clockDivisionNum();
  // tempPulseIndex /= oldClockDivNum;
  // tempPulseIndex /= clockDivisionDen();
  // tempPulseIndex /= 1024;

  // tempPulseIndex *= currentFrame;
  // tempPulseIndex *= currentFrame;
  // tempPulseIndex /= currentFrame;
  // tempPulseIndex /= currentFrame;

  //  ppqPulseIndex = tempPulseIndex % (pulsesPerBeat*stepCount*clockDivisionNum()/clockDivisionDen());
  this->getCurrentFrame();
  // Serial.println("new ppq index: " + String(ppqPulseIndex) + "\tnewNum: " + String(clockDivisionNum()) + "\tnewDen: " + String(clockDivisionDen()) + "\torgCF: " + String(origCurrentFrame) + "\torgFPS: " + String(origFramesPerSequence) );
}

uint32_t Sequencer::getCurrentFrame()
{
  uint32_t clockCycles = ARM_DWT_CYCCNT;
  uint32_t unquantizedCF;
  int clockCount = 0;

BEGINCURRENTFRAME:

  // this prevents clocksSinceLastPulse from exceeding avgClocksPerPulse, which results in the currentFrame going backwards
  if (clockCycles - lastPulseClockCount >= avgClocksPerPulse)
  {
    clockCount = avgClocksPerPulse - 1;
    // Serial.println("clock counter exceeded avg clocks per pulse");
  }
  else
  {
    clockCount = clockCycles - lastPulseClockCount;
  }

  unquantizedCF = ((uint32_t)ppqPulseIndex * framesPerPulse) + (((long long)framesPerPulse * (long long)clockCount) / avgClocksPerPulse);

  currentFrame = unquantizedCF;
  return unquantizedCF;
}

void Sequencer::calculateActiveStep()
{
  lastActiveStep = activeStep;
  currentFrame = this->getCurrentFrame();

  if (currentFrame >= framesPerSequence())
  {
    this->clockReset(false);
    this->getCurrentFrame();
  }

  int16_t preSwingStepIndex = 0;
  int16_t postSwingStepIndex = 0;
  preSwingStepIndex = currentFrame / getStepLength();

  if ((swingX100 > 50) && (preSwingStepIndex % 2) && (currentFrame % getStepLength() <= getStepLength() * (2 * swingX100 - 100) / 100))
  {
    // if the current frame liees when the activeStep has advanced according to master clock,
    // but before the point where it should retrigger, keep activestep 1 step behind
    // swing step has not begun yet.
    postSwingStepIndex = preSwingStepIndex - 1;
    swinging = false;
  }
  else if ((swingX100 < 50) && (preSwingStepIndex % 2) && (currentFrame % getStepLength() >= getStepLength() * (2 * swingX100) / 100))
  {
    postSwingStepIndex = preSwingStepIndex + 1;
    swinging = true;
  }
  else
  {
    postSwingStepIndex = preSwingStepIndex;
  }

  if (postSwingStepIndex != lastStepIndex)
  {
    switch (playMode)
    {
    case PLAY_PENDULUM:
      if (activeStep == firstStep)
      {
        pendulumSwitch = true;
      }
      else if (activeStep == (firstStep + stepCount - 1))
      {
        pendulumSwitch = false;
      }
      if (pendulumSwitch)
      {
        goto FORWARD;
      }
      else
      {
        goto REVERSE;
      }
      break;
    case PLAY_FORWARD:
    FORWARD:
      activeStep = min_max_wrap(activeStep + 1, firstStep, stepCount, MAX_STEPS_PER_SEQUENCE);
      break;
    case PLAY_REVERSE:
    REVERSE:
      activeStep = min_max_wrap(activeStep - 1, (firstStep + stepCount - 1) % MAX_STEPS_PER_SEQUENCE, stepCount, MAX_STEPS_PER_SEQUENCE);
      break;
    case PLAY_RANDOM:
      activeStep = min_max_wrap(random(firstStep, firstStep + stepCount), (firstStep + stepCount - 1) % MAX_STEPS_PER_SEQUENCE, stepCount, MAX_STEPS_PER_SEQUENCE);
      break;
    }

    lastStepIndex = postSwingStepIndex;
  }
  activeStep = min_max(activeStep, firstStep, firstStep + stepCount);

  if (activeStep != lastActiveStep)
  {
    swingCount += 1;
  } 
}

bool Sequencer::isFrameSwinging(uint32_t frame)
{
  uint8_t preSwingActivestep = 0;
  bool swingTemp = 0;

  if (playMode == PLAY_REVERSE)
  {
    preSwingActivestep = firstStep + stepCount - frame / getStepLength();

    //activeStep = preSwingActivestep;
    //  activeStep = firstStep + stepCount - max(lastStepFrame,(frame -swingOffset )) / getStepLength() - 1 ;
  }
  else if (playMode == PLAY_FORWARD)
  {
    preSwingActivestep = firstStep + frame / getStepLength();
    if ((swingSwitch + preSwingActivestep % 2) & (frame % getStepLength() < (getStepLength() * swingX100) / 100))
    {
      // if the current frame liees when the activeStep has advanced according to master clock,
      // but before the point where it should retrigger, keep activestep 1 step behind
      // swing step has not begun yet.
      swingTemp = false;
    }
    else if (swingSwitch + preSwingActivestep % 2)
    { // is a swing step
      swingTemp = true;
    }
    else
    { // is not a swing step
      swingTemp = false;
    }
  };

  return swingTemp;
}

uint32_t Sequencer::framesPerSequence()
{
  uint32_t returnValue = getStepLength() * stepCount;
  return returnValue;
}

uint32_t Sequencer::getStepLength()
{
  uint32_t returnValue = framesPerBeat(globalObj->tempoX100) * clockDivisionNum();
  returnValue = returnValue / clockDivisionDen();

  return returnValue;

  // if (clockDivision > 0){
  // 	//return beatLength/stepData[stepNum].beatDiv;
  // 	return ARM_DWT_CYCCNT/65536 / clockDivision;
  // } else { // negative values of beatDiv allow for whole multiples of beatLength
  // 	return ARM_DWT_CYCCNT/65536*(abs(clockDivision)+2);
  // //	return beatLength*(abs(stepData[stepNum].beatDiv)+2);
  // }
}

int Sequencer::getActivePage()
{
  return activeStep / 16;
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

void Sequencer::sequenceModeStandardStep()
{
  //uint32_t currentFrame = 0;
  //incrementActiveStep(currentFrame);
  if (muteGate || !globalObj->playing)
  {
    return;
  }
  calculateActiveStep();

  if(lastActiveStep != activeStep){
    if (stepData[activeStep].noteStatus == CURRENTLY_PLAYING || stepData[activeStep].noteStatus == BETWEEN_APEGGIATIONS)
      {
        Serial.println("resetting arpeggio " + String(activeStep));
        noteShutOff(activeStep, stepData[activeStep].gateOff());
        stepData[activeStep].framesRemaining = 0;
        stepData[activeStep].noteStatus = AWAITING_TRIGGER;
        stepData[activeStep].arpStatus = 0;
      }
  }


  //if(stepData[activeStep].noteStatus == CURRENTLY_PLAYING

  for (int stepNum = 0; stepNum <= (firstStep + stepCount); stepNum++)
  {
    // iterate through all steps to determine if they need to have action taken.

    if (stepData[stepNum].gateType == GATETYPE_REST)
    {
      continue;
    }
    //Serial.println("noteStatus: " + String(stepData[stepNum].noteStatus));

    switch (stepData[stepNum].noteStatus)
    {
    case NOTE_HAS_BEEN_PLAYED_THIS_ITERATION:
      if (stepNum != activeStep || (stepCount == 1 && currentFrame < getStepLength() / 4))
      {
        //Serial.println("Resetting step: " + String(stepNum) + "\tactiveStep: " + String(activeStep)) ;
        stepData[stepNum].noteStatus = AWAITING_TRIGGER;
        stepData[stepNum].arpStatus = 0;
      }
      continue;
      break;
    case CURRENTLY_PLAYING:
      if (stepData[stepNum].framesRemaining < stepData[stepNum].arpLastFrame)
      {
        noteShutOff(stepNum, stepData[stepNum].gateOff());
        stepData[stepNum].noteStatus = BETWEEN_APEGGIATIONS;
        #warning this line is not correct, get arp count doesnt work if there is cv input on arp speed
        if (stepData[stepNum].arpStatus > getArpCount(stepNum))
        {
          stepData[stepNum].noteStatus = NOTE_HAS_BEEN_PLAYED_THIS_ITERATION;
        }
      }
      break;

    case BETWEEN_APEGGIATIONS:
      // Arpeggio retrigger
      if (currentFrame > stepData[stepNum].arpLastFrame)
      {
        //if ( getFramesRemaining(stepNum) <= 0 ) {
        noteTrigger(stepNum, stepData[stepNum].gateTrig(), arpTypeModulated[stepNum], arpOctaveModulated[stepNum]);
        stepData[stepNum].noteStatus = CURRENTLY_PLAYING;
      }
      break;
    }
  }
  // I might be able to combine the between arpeggiations part above
  // and the awaiting trigger part below, to just be triggered entirely by getArpStartFrame
  // 
  if (stepData[activeStep].noteStatus == AWAITING_TRIGGER)
  {
    if (stepData[activeStep].gateType != GATETYPE_REST)
    {
      
      stepData[activeStep].arpStatus = 0;
      arpTypeModulated[activeStep] = min_max(stepData[activeStep].arpType + outputControl->cvInputCheck(cv_arptypemod) / 20, 0, 5);
      arpOctaveModulated[activeStep] = min_max(stepData[activeStep].arpOctave + outputControl->cvInputCheck(cv_arpoctmod) / 20, 1, 5);
      noteTrigger(activeStep, stepData[activeStep].gateTrig(), arpTypeModulated[activeStep], arpOctaveModulated[activeStep]);
      stepData[activeStep].noteStatus = CURRENTLY_PLAYING;
      //Serial.println("Triggering Step: " + String(activeStep) + "\tppqPulseIndex: " + String(ppqPulseIndex ) + "\tppqModulo: "+String(pulsesPerBeat*stepCount/clockDivision)  + "\tppB: " + String(pulsesPerBeat) + "\tstepCount: " + String(stepCount) + "\tclockdiv:" + String(clockDivision) + "\tnoteStatus: "+ String(stepData[activeStep].noteStatus));
      //stepData[activeStep].offset = currentFrame;
    }
  }
  //return (((uint32_t)ppqPulseIndex * framesPerPulse) + (((long long)framesPerPulse * (long long)clockCount) / avgClocksPerPulse) ) % framesPerSequence() ;

  // if(previousActiveStepSeqMode != activeStep && channel == 0){
  //   Serial.println("as: " + String(activeStep) + "\tperc: " + String(currentFrame*100/framesPerSequence()) + "\tppqi: " + String(ppqPulseIndex) + "\tcf: " + String(currentFrame) + "\tsl: " + String(getStepLength()) + "\tfps: " + String(framesPerSequence()));
  //   for (int stepNum = 0; stepNum <= (firstStep + stepCount-1); stepNum++){
  //     if(stepData[stepNum].noteStatus == CURRENTLY_PLAYING){
  //       Serial.print("CURRENTLY_PLAYING ");
  //       Serial.println("\tstp: " + String(stepNum) + "\tFR " + String(stepData[stepNum].framesRemaining) + "\tarpS: " + String(getArpStartFrame(stepNum, stepData[stepNum].arpStatus)) + "\tarpLF: " + String(stepData[stepNum].arpLastFrame) );
  //     } else if(stepData[stepNum].noteStatus == BETWEEN_APEGGIATIONS ){
  //       Serial.print("BETWEEN_APEGGIATIONS");
  //       Serial.println("\tstp: " + String(stepNum) + "\tFR " + String(stepData[stepNum].framesRemaining) + "\tarpS: " + String(getArpStartFrame(stepNum, stepData[stepNum].arpStatus)) + "\tarpLF: " + String(stepData[stepNum].arpLastFrame) );
  //     } else {
  //       Serial.println("ns: " + String(stepData[stepNum].noteStatus));
  //     }
  //   }
  //   Serial.println(" ");
  // }
}
