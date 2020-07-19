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

bool Sequencer::we_should_trigger_another_arpeggio(uint8_t stepNum){
  uint32_t gate_frames_per_gateLength = getStepLength() / 4;
  uint32_t step_length_in_frames = gate_frames_per_gateLength * stepData[stepNum].gateLength;

  // add another step, since we are seeing if triggering another arpeggio would end out of step bounds
  #warning this is where we need to add arp speed input modulation compensation for arpspd
  step_length_in_frames += gate_frames_per_gateLength * stepData[stepNum].arpSpdNum / stepData[stepNum].arpSpdDen;
  
  if (currentFrame  < stepData[stepNum].stepStartFrame + step_length_in_frames ) {
    return 1;
  } else {
    return 0;
  }
}

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
    if ((stepData[stepNum].noteStatus == CURRENTLY_PLAYING) || (stepData[stepNum].noteStatus == BETWEEN_APEGGIATIONS))
    {
      if (framesToSubtract >= stepData[stepNum].framesRemaining)
      {
        stepData[stepNum].framesRemaining = 0;
        if(stepData[stepNum].noteStatus == BETWEEN_APEGGIATIONS){       
          if ( this->we_should_trigger_another_arpeggio(stepNum) ){
            stepData[stepNum].noteStatus = CURRENTLY_QUEUED;
          } else {
            stepData[stepNum].noteStatus = NOTE_HAS_BEEN_PLAYED_THIS_ITERATION;
          }
        }
      }
      else
      {
        stepData[stepNum].framesRemaining -= framesToSubtract;
      }
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

  framesPerPulse = FRAMES_PER_BEAT / pulsesPerBeat;
  pulsesRemaining = (framesPerSequence() - currentFrame) / framesPerPulse;

  lastPulseClockCount = clockCycles;
};

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
  uint32_t frames_per_sequence = getStepLength() * stepCount;
  return frames_per_sequence;
}

uint32_t Sequencer::getStepLength()
{
  uint32_t step_length = 0;
  step_length = FRAMES_PER_BEAT;
  step_length *= clockDivisionNum();
  step_length /= clockDivisionDen();

  return step_length;
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

  if (lastActiveStep != activeStep)
  {
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
    // if (currentFrame > stepData[stepNum].stepStartFrame + getStepLength() * (stepData[stepNum].gateLength+1) / 4)
    // { // check and see if the step is over, and if it should be killed. 
    //   if (stepData[stepNum].noteStatus == CURRENTLY_PLAYING)
    //   {
    //     noteShutOff(stepNum, stepData[stepNum].gateOff());
    //   }
    //   stepData[stepNum].noteStatus = NOTE_HAS_BEEN_PLAYED_THIS_ITERATION;
    // }

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
      if (stepData[stepNum].framesRemaining <= stepData[stepNum].arpLastFrame)
      {
        Serial.println("Shutoff");
        noteShutOff(stepNum, stepData[stepNum].gateOff());
        // stepData[stepNum].arpLastFrame += currentFrame;
        if (stepData[stepNum].arpType == ARPTYPE_OFF) {
          stepData[stepNum].noteStatus = NOTE_HAS_BEEN_PLAYED_THIS_ITERATION;
        } else {
          // if (currentFrame > stepData[stepNum].stepStartFrame + getStepLength() * (2*stepData[stepNum].gateLength+1) / 8 )  {
          //   //this might fall right on the borderline, so sometimes one additional note gets played, so I had to add a bit of padding to this equation
          //   stepData[stepNum].noteStatus = NOTE_HAS_BEEN_PLAYED_THIS_ITERATION;
          // } else {
          stepData[stepNum].noteStatus = BETWEEN_APEGGIATIONS;
          // }
        }
// #warning this line is not correct, get arp count doesnt work if there is cv input on arp speed
        // if (stepData[stepNum].arpStatus > getArpCount(stepNum))
        // if (stepData[stepNum].framesRemaining == 0)
      }
      break;

    case CURRENTLY_QUEUED: 
      noteTrigger(stepNum, stepData[stepNum].gateTrig(), arpTypeModulated[stepNum], arpOctaveModulated[stepNum]);
      stepData[stepNum].noteStatus = CURRENTLY_PLAYING;
      break;

    case BETWEEN_APEGGIATIONS:
    
      // Arpeggio retrigger
      if (stepData[stepNum].framesRemaining <= 0){
        stepData[stepNum].noteStatus = NOTE_HAS_BEEN_PLAYED_THIS_ITERATION;
      }
      // {
      // if ( currentFrame > stepData[stepNum].arpLastFrame ) {
      // }
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
}
