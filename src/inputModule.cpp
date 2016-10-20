#include <Arduino.h>
#include "InputModule.h"

Zetaohm_MAX7301 max7301();

InputModule::InputModule():knob(ENCODER1LEFTPIN, ENCODER1RIGHTPIN)
{
// Encoder lib has no default constructor, so need to use initializer list instead
// from https://forum.pjrc.com/threads/25082-How-do-I-include-a-library-in-a-library
knobRead = 0;;
knobBuffer = 0;;
knobPrevious = 0;;
knobChange = 0;;

};

void InputModule::initialize(OutputController* outputControl, Zetaohm_MAX7301* midplaneGPIO, Zetaohm_MAX7301* backplaneGPIO, FlashMemory* saveFile, Sequencer *sequenceArray, MasterClock* clockMaster){
  Serial.println("button setup start");

  this->saveFile = saveFile;
  this->sequenceArray = sequenceArray;
  this->outputControl = outputControl;
  this->clockMaster= clockMaster;
  this->midplaneGPIO= midplaneGPIO;
  this->backplaneGPIO= backplaneGPIO;

  midplaneGPIO->begin(MIDPLANE_MAX7301_CS_PIN);

  midplaneGPIO->initPort(0, 22, INPUT_PULLUP);   // SW_00
  midplaneGPIO->initPort(1, 21, INPUT_PULLUP);    // SW_01
  midplaneGPIO->initPort(2, 6, INPUT_PULLUP);    // SW_02
  midplaneGPIO->initPort(3, 9, INPUT_PULLUP);   // SW_03
  midplaneGPIO->initPort(4, 23, INPUT_PULLUP);   // SW_04
  midplaneGPIO->initPort(5, 20, INPUT_PULLUP);   // SW_05
  midplaneGPIO->initPort(6, 11, INPUT_PULLUP);    // SW_06
  midplaneGPIO->initPort(7, 7, INPUT_PULLUP);    // SW_07
  midplaneGPIO->initPort(8, 24, INPUT_PULLUP);   // SW_08
  midplaneGPIO->initPort(9, 19, INPUT_PULLUP);   // SW_09
  midplaneGPIO->initPort(10, 14, INPUT_PULLUP);   // SW_10
  midplaneGPIO->initPort(11, 13, INPUT_PULLUP);  // SW_11
  midplaneGPIO->initPort(12, 3, INPUT_PULLUP);  // SW_12
  midplaneGPIO->initPort(13, 18, INPUT_PULLUP);  // SW_13
  midplaneGPIO->initPort(14, 17, INPUT_PULLUP);  // SW_14
  midplaneGPIO->initPort(15, 16, INPUT_PULLUP);   // SW_15
  midplaneGPIO->initPort(16, 0, INPUT_PULLUP);   // SW_PLAY
  midplaneGPIO->initPort(17, 27, INPUT_PULLUP);   // SW_PAUSE
  midplaneGPIO->initPort(18, 4, INPUT_PULLUP);   // SW_STOP
  midplaneGPIO->initPort(19, 1, INPUT_PULLUP);  // SW_M0
  midplaneGPIO->initPort(20, 26, INPUT_PULLUP);  // SW_M1
  midplaneGPIO->initPort(21, 2, INPUT_PULLUP);  // SW_M2
  midplaneGPIO->initPort(22, 25, INPUT_PULLUP);  // SW_M3
  midplaneGPIO->initPort(23, 5, INPUT_PULLUP);   // SW_PGDN
  midplaneGPIO->initPort(24, 8, INPUT_PULLUP);  // SW_PGUP
  midplaneGPIO->initPort(25, 10, INPUT_PULLUP);  // SW_PATTERN
  midplaneGPIO->initPort(26, 12, INPUT_PULLUP);  // SW_MENU
  midplaneGPIO->initPort(27, 15, INPUT_PULLUP);  // SW_SHIFT

  midplaneGPIO->updateGpioPinModes();

  Serial.println("button setup complete");

}

void InputModule::loop(uint16_t frequency){
  if (inputTimer > frequency){
    inputTimer = 0;
    knobPrevious = knobRead;
    knobRead = knob.read()/-2  ;
    knobChange = knobRead - knobPrevious;
    midplaneGPIO->update();

    //we always want the alt (non matrix) buttons to behave the same way
    altButtonHandler();

    // now to handle the rest of the buttons.
    switch (currentState) {
      case STEP_DISPLAY:
        channelPitchModeInputHandler();
      break;
      case SEQUENCE_MENU:
        sequenceMenuHandler();
      break;

      case GLOBAL_MENU:
        globalMenuHandler();
      break;

      case CHANNEL_PITCH_MODE:
        channelPitchModeInputHandler();
      break;

      case CHANNEL_GATE_MODE:
        channelGateModeInputHandler();
      break;

      case CHANNEL_ENVELOPE_MODE:
        channelEnvelopeModeInputHandler();
      break;

      case CHANNEL_STEP_MODE:
        channelStepModeInputHandler();
      break;

      case PATTERN_SELECT:
      patternSelectHandler();
      break;

      case TEMPO_MENU:
        tempoMenuHandler();
      break;

      case TIMING_MENU:
      timingMenuInputHandler();
      break;

      case DEBUG_SCREEN:
      debugScreenInputHandler();
      break;

      case CALIBRATION_MENU:
        calibrationMenuHandler();
      break;
    }
  }

}

// STATE VARIABLE INPUT HANDLERS

void InputModule::patternSelectHandler(){
  for (int i=0; i < 16; i++){
    if (midplaneGPIO->fell(i)){
      saveFile->changePattern(i, patternChannelSelector,  true, true);

      delay(10);
      changeState(CHANNEL_PITCH_MODE);
    }
  }
}

void InputModule::tempoMenuHandler(){

  if(knobChange){
    switch (stepMode){
      case STEPMODE_TEMPO:
        if (tempoX100 > 100100) {
          tempoX100 = 100100;
        }
        tempoX100 = positive_modulo(tempoX100 + knobChange*100, 100100 );
        if(tempoX100 == 0){
          tempoX100 = 100;
        }
        clockMaster->changeTempo(tempoX100);
      break;

      case STEPMODE_EXTCLOCK:
        if (knobChange > 0){
          extClock = 1;
        } else {
          extClock = 0;
        }
      break;
    }
  }

}

void InputModule::sequenceMenuHandler(){
  switch(stepMode){
    case STEPMODE_QUANTIZEKEY:
      sequenceArray[selectedChannel].quantizeKey = positive_modulo(sequenceArray[selectedChannel].quantizeKey + knobChange, 12);
    break;
    case STEPMODE_QUANTIZESCALE:
      sequenceArray[selectedChannel].quantizeScale = positive_modulo(sequenceArray[selectedChannel].quantizeScale + knobChange, 15);
    break;


  }

}

void InputModule::globalMenuHandler(){
  if (midplaneGPIO->fell(0)){
    sequenceArray[selectedChannel].initNewSequence(sequenceArray[selectedChannel].patternIndex, selectedChannel);
      changeState(CHANNEL_PITCH_MODE);

  } else if (midplaneGPIO->fell(4)){
    for (int i=0; i < 4; i++){
      sequenceArray[i].initNewSequence(sequenceArray[i].patternIndex, i);
    }
    changeState(CHANNEL_PITCH_MODE);

  } else if (midplaneGPIO->fell(8)){
    saveFile->deleteSaveFile();
    changeState(CHANNEL_PITCH_MODE);

  } else if (midplaneGPIO->fell(12)){
  /*  if (sequenceArray[selectedChannel].instType == 0){
      sequenceArray[selectedChannel].setInstType(1);
      //sequenceArray[selectedChannel].stepCount = 16;
      //sequenceArray[selectedChannel].beatCount = 16;
      life.genGrid(micros());
    } else {
      sequenceArray[selectedChannel].setInstType(0);
    }
    */
    changeState(CHANNEL_PITCH_MODE);
  } else if (midplaneGPIO->fell(3)){
    extClock = !extClock;
        changeState(CHANNEL_PITCH_MODE);
  }
}

void InputModule::channelButtonShiftHandler(uint8_t channel){
  if (selectedChannel != channel){
    selectedChannel = channel;
  }
  changeState(CHANNEL_TUNER_MODE);
};

void InputModule::channelButtonHandler(uint8_t channel){
  uint8_t previous = patternChannelSelector;

  resetKnobValues();
  if (currentState != PATTERN_SELECT){
    if (selectedChannel != channel){
      selectedChannel = channel;
      return;
    }
  }

  switch (currentState){
    case PATTERN_SELECT:
      patternChannelSelector =  patternChannelSelector ^ (1 << channel);
      if (patternChannelSelector == 0) {
        patternChannelSelector = previous;
      }
    break;

    case CHANNEL_PITCH_MODE:
      changeState(CHANNEL_GATE_MODE);
    break;

    case CHANNEL_GATE_MODE:
      changeState(CHANNEL_PITCH_MODE);
    break;

    case CHANNEL_ENVELOPE_MODE:
      changeState(CHANNEL_STEP_MODE);
    break;

    case CHANNEL_STEP_MODE:
      changeState(CHANNEL_PITCH_MODE);
    break;


    default:
      changeState(CHANNEL_PITCH_MODE);
    break;
  }
}

void InputModule::altButtonHandler(){
  for (int i=16; i <28; i++){
    if (midplaneGPIO->fell(i) ){
      switch (i){
        // left row bottom up
        case SW_M0:
          if (midplaneGPIO->pressed(SW_SHIFT)){
            channelButtonShiftHandler(0);
          } else {
            channelButtonHandler(0);
          }
        break;

        case SW_M1:
          if (midplaneGPIO->pressed(SW_SHIFT)){
            channelButtonShiftHandler(1);
          } else {
            channelButtonHandler(1);
          }
        break;

        case SW_M2:
          if (midplaneGPIO->pressed(SW_SHIFT)){
            channelButtonShiftHandler(2);
          } else {
            channelButtonHandler(2);
          }
        break;

        case SW_M3:
          if (midplaneGPIO->pressed(SW_SHIFT)){
            channelButtonShiftHandler(3);
          } else {
            channelButtonHandler(3);
          }
        break;

        case SW_PATTERN:
          if(currentState == PATTERN_SELECT){
            changeState(CHANNEL_PITCH_MODE);
          } else {
            changeState(PATTERN_SELECT);
          }
        break;

        case SW_MENU: //switch M3 toggles the sequence menu
         if (midplaneGPIO->pressed(SW_SHIFT)){
           changeState(GLOBAL_MENU);
         } else {
           if (currentState == SEQUENCE_MENU){
             switch (stepMode){
              case STEPMODE_QUANTIZEKEY:
                stepMode = STEPMODE_QUANTIZESCALE;
              break;
              case STEPMODE_QUANTIZESCALE:
                stepMode = STEPMODE_QUANTIZEKEY;
              break;
              default:
                stepMode = STEPMODE_QUANTIZEKEY;
              break;
             }
           } else {
             stepMode = STEPMODE_QUANTIZEKEY;
             changeState(SEQUENCE_MENU);
           }
         }
        break;

        case SW_SHIFT:

        break;

        case SW_PGDN:
          if (stepMode == STEPMODE_TEMPO){
            stepMode = STEPMODE_EXTCLOCK;
            break;
          } else if (stepMode == STEPMODE_EXTCLOCK){
            stepMode = STEPMODE_TEMPO;
            break;
          }

          if (midplaneGPIO->pressed(SW_SHIFT)){
            changeState(TEMPO_MENU);
            stepMode = STEPMODE_TEMPO;
            knobBuffer = tempoX100 - knobRead;
          } else {
            notePage = positive_modulo(notePage - 1, 4);
          }
        break;

        case SW_PGUP:
          if (midplaneGPIO->pressed(SW_SHIFT)){
            //changeState(GLOBAL_MENU);
          } else {
            notePage = positive_modulo(notePage + 1, 4);
          }

        break;

        case SW_STOP:
          if (!playing){ //if the sequence is already paused, stop kills all internal sound.
            for(uint8_t channel = 0; channel < sequenceCount; channel++){
              outputControl->allNotesOff(channel);
            }
          }
          playing = false;
          for(int s = 0; s < sequenceCount; s++){
            sequenceArray[s].activeStep = 0;
           // sam2695.allNotesOff(s);
          }
          break;

        // right two, bottom up
          case SW_PLAY:
          playing = !playing;
          break;

          case SW_REC:

          break;
        }
      }
    }
  }


  void InputModule::channelPitchModeInputHandler(){
  // selectedStep == getNote(i) means that the user pressed the button that is selected.
    uint8_t chrd;

    for (int i=0; i < 16; i++){
      if (midplaneGPIO->fell(i)){
        if(selectedStep == getNote(i)){
          // if currentstep is already selected, change stepMode
          stepMode = (stepMode + 1) % 10;

          switch( stepMode ){
            case STEPMODE_GATELENGTH:
              knobBuffer = sequenceArray[selectedChannel].stepData[getNote(i)].gateLength - knobRead;
              break;
            case STEPMODE_GATETYPE:
              knobBuffer = sequenceArray[selectedChannel].stepData[getNote(i)].gateType - knobRead;
              break;
            case STEPMODE_CHORD:
              knobBuffer = sequenceArray[selectedChannel].stepData[getNote(i)].chord - knobRead;
              break;
            case STEPMODE_GLIDE:
              knobBuffer = sequenceArray[selectedChannel].stepData[getNote(i)].glide - knobRead;
              break;
            case STEPMODE_PITCH1:
              knobBuffer = sequenceArray[selectedChannel].getStepPitch(selectedStep, 1) - knobRead;
              break;
            case STEPMODE_PITCH2:
              knobBuffer = sequenceArray[selectedChannel].getStepPitch(selectedStep, 2) - knobRead;
              break;
            case STEPMODE_PITCH3:
              knobBuffer = sequenceArray[selectedChannel].getStepPitch(selectedStep, 3) - knobRead;
              break;
            case STEPMODE_STEPCOUNT:
              knobBuffer = sequenceArray[selectedChannel].stepCount - knobRead;
              break;
            case STEPMODE_BEATCOUNT:
              knobBuffer =  sequenceArray[selectedChannel].beatCount - knobRead;
              break;
            case STEPMODE_ARPTYPE:
              knobBuffer = sequenceArray[selectedChannel].stepData[getNote(i)].arpType - knobRead;
              break;
            case STEPMODE_BEATDIV:
              knobBuffer = sequenceArray[selectedChannel].stepData[getNote(i)].arpSpeed - knobRead;
              break;
            case STEPMODE_ARPOCTAVE:
              knobBuffer = sequenceArray[selectedChannel].stepData[getNote(i)].arpOctave - knobRead;
              break;
          }
        } else {
          stepMode = 0;
          selectedStep = getNote(i);
          knobBuffer = sequenceArray[selectedChannel].getStepPitch(selectedStep, 0) - knobRead;
        }
      }
    }

    if (knobChange){
      //knobPrev = knobRead;

      if (midplaneGPIO->pressed(selectedStep%16) ){
        stepMode = (stepMode + knobChange) % 10;
      } else {

        switch (stepMode) {
          case STEPMODE_PITCH0:
        // just change the note
          if (knobRead + sequenceArray[selectedChannel].getStepPitch(selectedStep, 0) < 0){
            // you can turn off a note by turning the value to 0
            // turn off a note by setting gate type and pitch to 0
            sequenceArray[selectedChannel].stepData[selectedStep].gateType = GATETYPE_REST;
            sequenceArray[selectedChannel].setStepPitch(selectedStep, 0, 0);
            knob.write(4);
          } else {
            if(sequenceArray[selectedChannel].stepData[selectedStep].gateType == GATETYPE_REST){
              // if a note is not active, turn it on and give it a length.
              sequenceArray[selectedChannel].stepData[selectedStep].gateType = GATETYPE_STEP;
              sequenceArray[selectedChannel].stepData[selectedStep].gateLength = 1;
            }
            // and finally set the new step value!
            // monophonic so pitch[0] only
            sequenceArray[selectedChannel].setStepPitch(selectedStep, positive_modulo(sequenceArray[selectedChannel].getStepPitch(selectedStep, 0) + knobChange, 127), 0);
          }
          break;

          case STEPMODE_PITCH1:
            sequenceArray[selectedChannel].setStepPitch(selectedStep, positive_modulo(sequenceArray[selectedChannel].getStepPitch(selectedStep, 1) + knobChange, 1), 72);
          break;

          case STEPMODE_PITCH2:
            sequenceArray[selectedChannel].setStepPitch(selectedStep, positive_modulo(sequenceArray[selectedChannel].getStepPitch(selectedStep, 2) + knobChange, 2), 72);
          break;

          case STEPMODE_PITCH3:
            sequenceArray[selectedChannel].setStepPitch(selectedStep, positive_modulo(sequenceArray[selectedChannel].getStepPitch(selectedStep, 3) + knobChange, 3), 72);
          break;

          case STEPMODE_CHORD:
            chrd = positive_modulo(sequenceArray[selectedChannel].stepData[selectedStep].chord + knobChange, 27);
            sequenceArray[selectedChannel].stepData[selectedStep].chord = chrd;
            sequenceArray[selectedChannel].setStepPitch(selectedStep, chordArray[chrd][0],1);
            sequenceArray[selectedChannel].setStepPitch(selectedStep, chordArray[chrd][1],2);
            sequenceArray[selectedChannel].setStepPitch(selectedStep, chordArray[chrd][2],3);
          break;

          case STEPMODE_GATELENGTH:
      // change the gate type
          if ((sequenceArray[selectedChannel].stepData[selectedStep].gateLength == 0) && (knobChange < 0)  ) {
            sequenceArray[selectedChannel].stepData[selectedStep].gateType = GATETYPE_REST;
          } else if(knobChange > 0) {
            if (sequenceArray[selectedChannel].stepData[selectedStep].gateType == GATETYPE_REST){
              sequenceArray[selectedChannel].stepData[selectedStep].gateType = GATETYPE_STEP;
            }
          }

          if (sequenceArray[selectedChannel].stepData[selectedStep].gateType > 0){
            sequenceArray[selectedChannel].stepData[selectedStep].gateLength =  positive_modulo(sequenceArray[selectedChannel].stepData[selectedStep].gateLength + knobChange, 127);
          }

          break;

          case STEPMODE_STEPCOUNT:
            sequenceArray[selectedChannel].stepCount = positive_modulo(sequenceArray[selectedChannel].stepCount + knobChange, 64);

            if (sequenceArray[selectedChannel].stepCount == 0) {
              sequenceArray[selectedChannel].stepCount = 64;
            }
            break;

          case STEPMODE_BEATCOUNT:
            sequenceArray[selectedChannel].beatCount = positive_modulo(sequenceArray[selectedChannel].beatCount + knobChange, 129);
            if (sequenceArray[selectedChannel].beatCount < 1){
              sequenceArray[selectedChannel].beatCount = 1;
            }
            break;

          case STEPMODE_GATETYPE:
            sequenceArray[selectedChannel].stepData[selectedStep].gateType =  positive_modulo(sequenceArray[selectedChannel].stepData[selectedStep].gateType + knobChange, 4);
            break;

          case STEPMODE_GLIDE:
            sequenceArray[selectedChannel].stepData[selectedStep].glide =  positive_modulo(sequenceArray[selectedChannel].stepData[selectedStep].glide + knobChange, 128);
            break;

          case STEPMODE_ARPTYPE:
            sequenceArray[selectedChannel].stepData[selectedStep].arpType = positive_modulo(sequenceArray[selectedChannel].stepData[selectedStep].arpType + knobChange, 6);
          break;
        }
      }
    }
};

void InputModule::channelGateModeInputHandler(){

  for (int i=0; i < 16; i++){
    if (midplaneGPIO->fell(i)){

      if (selectedStep == getNote(i)){
        switch(stepMode){
          case STEPMODE_GATETYPE:
            stepMode = STEPMODE_ARPTYPE;
            knobBuffer = sequenceArray[selectedChannel].stepData[getNote(i)].arpType - knobRead;
          break;

          case STEPMODE_ARPTYPE:
            stepMode = STEPMODE_BEATDIV;
            knobBuffer = sequenceArray[selectedChannel].stepData[getNote(i)].arpSpeed - knobRead;
          break;

          case STEPMODE_BEATDIV:
            stepMode = STEPMODE_ARPOCTAVE;
            knobBuffer = sequenceArray[selectedChannel].stepData[getNote(i)].arpOctave - knobRead;
          break;

          case STEPMODE_ARPOCTAVE:
            stepMode = STEPMODE_ARPCOUNT;
            knobBuffer = sequenceArray[selectedChannel].stepData[getNote(i)].arpCount - knobRead;
          break;

          case STEPMODE_ARPCOUNT:
            stepMode = STEPMODE_GATETYPE;
            knobBuffer = sequenceArray[selectedChannel].stepData[getNote(i)].gateType - knobRead;
          break;

          default:
            stepMode = STEPMODE_GATETYPE;
            knobBuffer = sequenceArray[selectedChannel].stepData[getNote(i)].gateType - knobRead;
          break;

        }
      } else {
        stepMode = STEPMODE_GATETYPE;
        selectedStep = getNote(i);
        knobBuffer = sequenceArray[selectedChannel].stepData[getNote(i)].gateType - knobRead;
      }
    }
  }

  if (knobChange){
    //knobPrev = knobRead;
    switch (stepMode) {
      case STEPMODE_GATETYPE:
      sequenceArray[selectedChannel].stepData[selectedStep].gateType =  positive_modulo(sequenceArray[selectedChannel].stepData[selectedStep].gateType + knobChange, 4);
      break;
      case STEPMODE_ARPTYPE:
      sequenceArray[selectedChannel].stepData[selectedStep].arpType=  positive_modulo(sequenceArray[selectedChannel].stepData[selectedStep].arpType + knobChange, 6);
      break;
      case STEPMODE_BEATDIV:
      sequenceArray[selectedChannel].stepData[selectedStep].arpSpeed=  positive_modulo(sequenceArray[selectedChannel].stepData[selectedStep].arpSpeed + knobChange, 129) ;
      break;
      case STEPMODE_ARPOCTAVE:
      sequenceArray[selectedChannel].stepData[selectedStep].arpOctave=  positive_modulo(sequenceArray[selectedChannel].stepData[selectedStep].arpOctave + knobChange, 5);
      break;
      case STEPMODE_ARPCOUNT:
      sequenceArray[selectedChannel].stepData[selectedStep].arpCount=  positive_modulo(sequenceArray[selectedChannel].stepData[selectedStep].arpCount + knobChange, 129);
      if (sequenceArray[selectedChannel].stepData[selectedStep].arpCount == 0){
        sequenceArray[selectedChannel].stepData[selectedStep].arpCount = 1;
      }
      break;
    }
  }
};

void InputModule::channelEnvelopeModeInputHandler(){

};

void InputModule::channelStepModeInputHandler(){

};


void InputModule::timingMenuInputHandler() {
  /*
  for (int i=0; i < 16; i++){
    if (midplaneGPIO->fell(i)){
      menuSelector = i;
    }
  }
  switch(menuSelector){
    case 0:
    sequenceArray[selectedChannel].stepCount = positive_modulo(sequenceArray[selectedChannel].stepCount + knobChange, 129);
    break;
    case 4:
    sequenceArray[selectedChannel].beatCount = positive_modulo(sequenceArray[selectedChannel].beatCount + knobChange, 129);
    break;
  }
  */
}


void InputModule::debugScreenInputHandler(){
  if(midplaneGPIO->pressed(SW_MENU)){
    //voltManual = positive_modulo(voltManual + 10*knobChange, 65535);
  } else {
    //voltManual = positive_modulo(voltManual + knobChange, 65535);
  }
}

void InputModule::resetKnobValues(){
	knobRead = 0;
	knob.write(0);
	//Serial.println("resetting knob: " + String(knob.read()));
};


// this subroutine initializes the new state.
void InputModule::changeState(uint8_t targetState){
  Serial.println("State change - current: " + String(currentState) + "\ttarget: " + String(targetState));

  this->resetKnobValues();

  currentState = targetState;

  Serial.println("   - state change complete");
}

void InputModule::calibrationMenuHandler(){
  uint8_t multiplier = 100;
  if (midplaneGPIO->pressed(SW_REC)){
    multiplier = 10;
  }
  if (knobChange){
    calibrationBuffer += knobChange * multiplier;
    outputControl->calibrationRoutine();
  }

}
