#include <Arduino.h>
#include "InputModule.h"

Zetaohm_MAX7301 max7301();

InputModule::InputModule():knob(ENCODER1LEFTPIN, ENCODER1RIGHTPIN)
{
// Encoder lib has no default constructor, so need to use initializer list instead
// from https://forum.pjrc.com/threads/25082-How-do-I-include-a-library-in-a-library

//pinMode(ENCODER1LEFTPIN, INPUT);
//pinMode(ENCODER1RIGHTPIN, INPUT);

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
  midplaneGPIO->update();

  if( midplaneGPIO->pressed(SW_PLAY)&&
  midplaneGPIO->pressed(SW_REC)&&
  midplaneGPIO->pressed(SW_STOP) ){
    eraseAllFlag = true;
  }

}

void InputModule::loop(uint16_t frequency){
  if (inputTimer > frequency){
    inputTimer = 0;
    knobPrevious = knobRead;
    knobRead = knob.read()/-4  ;
    knobChange = knobRead - knobPrevious;
    midplaneGPIO->update();

    if (midplaneGPIO->pressed(SW_SHIFT) && midplaneGPIO->pressed(SW_PLAY) ){
      changeState(STATE_CALIBRATION);
    }

    if (midplaneGPIO->pressed(SW_SHIFT) && midplaneGPIO->pressed(SW_REC) ){
      changeState(STATE_INPUTDEBUG);
    }
    //we always want the alt (non matrix) buttons to behave the same way
    altButtonHandler();

    // now to handle the rest of the buttons.
    switch (currentMenu) {
      case PITCH_GATE_MENU:
      case ARPEGGIO_MENU:
      case VELOCITY_MENU:

        channelPitchModeInputHandler();
      break;
      case SEQUENCE_MENU:
        sequenceMenuHandler();
      break;

      case GLOBAL_MENU:
        globalMenuHandler();
      break;

      case CHANNEL_INPUT_MODE:
        channelInputInputHandler();
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

void InputModule::changeState(uint8_t targetState){
  Serial.println("change state: " + String(targetState));
  stepMode = targetState;
  switch (stepMode) {
    case STATE_PITCH0:
    case STATE_GATELENGTH:
    case STATE_GLIDE:
    case STATE_GATETYPE:
      currentMenu = PITCH_GATE_MENU;
      break;
    case STATE_CHORD:
    case STATE_ARPTYPE:
    case STATE_ARPSPEEDNUM:
    case STATE_ARPSPEEDDEN:
    case STATE_ARPOCTAVE:
      currentMenu = ARPEGGIO_MENU;
      break;
    case STATE_VELOCITY:
    case STATE_VELOCITYTYPE:
    case STATE_LFOSPEED:
      currentMenu = VELOCITY_MENU;
      break;
    case STATE_STEPCOUNT:
    case STATE_BEATCOUNT:
    case STATE_QUANTIZEKEY:
    case STATE_QUANTIZESCALE:
      currentMenu = SEQUENCE_MENU;
      break;
    case STATE_PATTERNSELECT:
      currentMenu = PATTERN_SELECT;
      break;
    case STATE_CALIBRATION:
      currentMenu = CALIBRATION_MENU;
      break;
    case STATE_EXTCLOCK:
    case STATE_TEMPO:
    case STATE_RESETINPUT:
    case STATE_YAXISINPUT:
      currentMenu = TEMPO_MENU;
      break;
    case STATE_INPUTDEBUG:
      currentMenu = INPUT_DEBUG_MENU;
      break;
    case STATE_GLOBAL:
      currentMenu = GLOBAL_MENU;
      break;
    default:
      Serial.println("This state has no menu selection! " + String(targetState));
    break;
  }

  this->resetKnobValues();
}

// STATE VARIABLE INPUT HANDLERS

void InputModule::patternSelectHandler(){

  for (int i=0; i < 16; i++){
    if (midplaneGPIO->fell(i)){
      saveFile->changePattern(i, patternChannelSelector,  true, true);

      //delay(10);
      changeState(STATE_PITCH0);
    }
  }
//  changeState(STATE_PITCH0);

}

void InputModule::tempoMenuHandler(){

  if(knobChange){
    if(backplaneGPIO->pressed(22)){
      changeState(min_max_cycle(stepMode+knobChange, STATE_TEMPO , STATE_YAXISINPUT));
    } else {
      switch (stepMode){
        case STATE_TEMPO:
          if (tempoX100 > 100100) {
            tempoX100 = 100100;
          }
          tempoX100 = positive_modulo(tempoX100 + knobChange*100, 100100 );
          if(tempoX100 == 0){
            tempoX100 = 100;
          }
          clockMaster->changeTempo(tempoX100);
        break;

        case STATE_EXTCLOCK:
          clockMode = positive_modulo(clockMode + knobChange, 6);
        break;

        case STATE_RESETINPUT:
          sequenceArray[selectedChannel].gpio_reset = positive_modulo(sequenceArray[selectedChannel].gpio_reset + knobChange, 5);
        break;

        case STATE_YAXISINPUT:
          sequenceArray[selectedChannel].gpio_yaxis = positive_modulo(sequenceArray[selectedChannel].gpio_yaxis + knobChange, 5);
        break;

      }
    }

  }

}

void InputModule::sequenceMenuHandler(){
  uint8_t newBeatDiv;
  if (midplaneGPIO->fell(SW_MENU)){
    changeState(STATE_PITCH0);
  }
  if(knobChange){
    if ( backplaneGPIO->pressed(22) ) {// Encoder Switch
      changeState(min_max_cycle(stepMode + knobChange,  STATE_STEPCOUNT,  STATE_QUANTIZESCALE));
    } else {

      switch(stepMode){
        case STATE_STEPCOUNT:
          sequenceArray[selectedChannel].stepCount = positive_modulo(sequenceArray[selectedChannel].stepCount + knobChange, 64);

          if (sequenceArray[selectedChannel].stepCount == 0) {
            sequenceArray[selectedChannel].stepCount = 64;
          }
          break;
        case STATE_BEATCOUNT:
           newBeatDiv = min_max(sequenceArray[selectedChannel].stepData[selectedStep].beatDiv + knobChange,-16, 16);

          for (int i =0; i<MAX_STEPS_PER_SEQUENCE; i++ ){
              sequenceArray[selectedChannel].stepData[i].beatDiv = newBeatDiv;
          }
          break;

        case STATE_QUANTIZEKEY:
          sequenceArray[selectedChannel].quantizeKey = positive_modulo(sequenceArray[selectedChannel].quantizeKey + knobChange, 12);
          break;
        case STATE_QUANTIZESCALE:
          sequenceArray[selectedChannel].quantizeScale = positive_modulo(sequenceArray[selectedChannel].quantizeScale + knobChange, 15);
          break;
        case STATE_RESETINPUT:
          sequenceArray[selectedChannel].gpio_reset = positive_modulo(sequenceArray[selectedChannel].gpio_reset + knobChange, 5);
          break;


      }
    }

  }

}

void InputModule::globalMenuHandler(){
  if (midplaneGPIO->fell(0)){
    sequenceArray[selectedChannel].initNewSequence(currentPattern, selectedChannel);
      changeState(STATE_PITCH0);

  } else if (midplaneGPIO->fell(4)){
    for (int i=0; i < SEQUENCECOUNT; i++){
      sequenceArray[i].initNewSequence(currentPattern, i);
    }
    changeState(STATE_PITCH0);

  } else if (midplaneGPIO->fell(8)){
    Serial.println("DELETING ALL SAVE FILES");
  //  saveFile->listFiles();
    delay(1000);
  //  saveFile->wipeEEPROM();
    saveFile->initializeCache();
    saveFile->eraseSaveFile();

    for(int pattern=0; pattern < 16; pattern++){
      Serial.println("***----###$$$###---*** *^~^* SAVING PATTERN " + String(pattern) + " TO CACHE *^~^* ***----###$$$###---***");

      for (int channel=0; channel < SEQUENCECOUNT; channel++){
        sequenceArray[channel].initNewSequence(pattern, channel);
      }
      Serial.println("Patterns initialized");
      for (int channel=0; channel < SEQUENCECOUNT; channel++){
        saveFile->saveSequenceJSON(channel, pattern);
      }
      Serial.println("json sequence saved");
      while(saveFile->cacheWriteSwitch){
        saveFile->cacheWriteLoop();

      }
      Serial.println("***----###$$$###---*** *^~^* PATTERN SAVED " + String(pattern) + " TO CACHE *^~^* ***----###$$$###---***");
      delay(500);
    }

    while(saveFile->cacheWriteSwitch){
      saveFile->cacheWriteLoop();
    }
    delay(500);
    saveFile->loadPattern(0, 0b1111);
    saveFile->listFiles();
    changeState(STATE_PITCH0);

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
    changeState(STATE_PITCH0);
  }
}


void InputModule::channelInputInputHandler(){


}


void InputModule::channelButtonShiftHandler(uint8_t channel){
  if (selectedChannel != channel){
    selectedChannel = channel;
  }
  changeState(STATE_TUNER);
};

void InputModule::channelButtonHandler(uint8_t channel){
  uint8_t previous = patternChannelSelector;

  resetKnobValues();
  if (currentMenu != PATTERN_SELECT){
    if (selectedChannel != channel){
      selectedChannel = channel;
      return;
    }
  }
  changeState((stepMode+1)% STATE_LFOSPEED);
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
          if(currentMenu == PATTERN_SELECT){
            changeState(STATE_PITCH0);
          } else {
            changeState(STATE_PATTERNSELECT);
          }
        break;

        case SW_MENU: //switch M3 toggles the sequence menu
         if (midplaneGPIO->pressed(SW_SHIFT)){
           changeState(STATE_GLOBAL);
         } else {
           if (currentMenu == SEQUENCE_MENU){
            changeState(STATE_PITCH0);
           } else {
             changeState(STATE_STEPCOUNT);
           }
         }
        break;

        case SW_SHIFT:

        break;

        case SW_PGDN:

          if (currentMenu == TEMPO_MENU){
            changeState(min_max_cycle(stepMode+1, STATE_TEMPO , STATE_RESETINPUT));
          }

          if (midplaneGPIO->pressed(SW_SHIFT)){
            changeState(STATE_TEMPO);
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
            for(uint8_t channel = 0; channel < SEQUENCECOUNT; channel++){
              outputControl->allNotesOff(channel);
            }
          }
          playing = false;

          for(int s = 0; s < SEQUENCECOUNT; s++){
            sequenceArray[s].clockReset();
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
        //  changeState(STATE_PITCH0);

        selectedStep = getNote(i);
        knobBuffer = sequenceArray[selectedChannel].getStepPitch(selectedStep, 0) - knobRead;

        if(lastSelectedStep == selectedStep && selectedStepTimer < DOUBLECLICKMS){
          sequenceArray[selectedChannel].stepData[selectedStep].gateType = GATETYPE_REST;
        } else {
          if (sequenceArray[selectedChannel].stepData[selectedStep].gateType == GATETYPE_REST){
            sequenceArray[selectedChannel].stepData[selectedStep].gateType = GATETYPE_STEP ;
          }
//            sequenceArray[selectedChannel].stepData[selectedStep].gateLength = 1;
        }

        lastSelectedStep = selectedStep;
        selectedStepTimer = 0;



      }
    }

    if (knobChange){
      //knobPrev = knobRead;

//      if (midplaneGPIO->pressed(selectedStep%16) ){
      backplaneGPIO->update();
      if ( midplaneGPIO->pressed(SW_M0) || midplaneGPIO->pressed(SW_M1) || midplaneGPIO->pressed(SW_M2) || midplaneGPIO->pressed(SW_M3) || backplaneGPIO->pressed(22) ) {// Encoder Switch
        changeState(positive_modulo(stepMode + knobChange, STATE_LFOSPEED));
      } else {

        switch (stepMode) {
          case STATE_PITCH0:
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
            sequenceArray[selectedChannel].setStepPitch(selectedStep, min_max(sequenceArray[selectedChannel].getStepPitch(selectedStep, 0) + knobChange, 0,127), 0);
          }
          break;

          case STATE_PITCH1:
            sequenceArray[selectedChannel].setStepPitch(selectedStep, positive_modulo(sequenceArray[selectedChannel].getStepPitch(selectedStep, 1) + knobChange, 1), 72);
          break;

          case STATE_PITCH2:
            sequenceArray[selectedChannel].setStepPitch(selectedStep, positive_modulo(sequenceArray[selectedChannel].getStepPitch(selectedStep, 2) + knobChange, 2), 72);
          break;

          case STATE_PITCH3:
            sequenceArray[selectedChannel].setStepPitch(selectedStep, positive_modulo(sequenceArray[selectedChannel].getStepPitch(selectedStep, 3) + knobChange, 3), 72);
          break;

          case STATE_CHORD:
            chrd = positive_modulo(sequenceArray[selectedChannel].stepData[selectedStep].chord + knobChange, 27);
            sequenceArray[selectedChannel].stepData[selectedStep].chord = chrd;
            sequenceArray[selectedChannel].setStepPitch(selectedStep, chordArray[chrd][0],1);
            sequenceArray[selectedChannel].setStepPitch(selectedStep, chordArray[chrd][1],2);
            sequenceArray[selectedChannel].setStepPitch(selectedStep, chordArray[chrd][2],3);
          break;

          case STATE_GATELENGTH:
      // change the gate type
              if ((sequenceArray[selectedChannel].stepData[selectedStep].gateLength == 0) && (knobChange < 0)  ) {
                sequenceArray[selectedChannel].stepData[selectedStep].gateType = GATETYPE_REST;
              } else if(knobChange > 0) {
                if (sequenceArray[selectedChannel].stepData[selectedStep].gateType == GATETYPE_REST){
                  sequenceArray[selectedChannel].stepData[selectedStep].gateType = GATETYPE_STEP;
                }
              }
              if (sequenceArray[selectedChannel].stepData[selectedStep].gateType > 0){
                sequenceArray[selectedChannel].stepData[selectedStep].gateLength =  min_max(sequenceArray[selectedChannel].stepData[selectedStep].gateLength + knobChange, 0, 255);
              }

          break;



          case STATE_GATETYPE:
            sequenceArray[selectedChannel].stepData[selectedStep].gateType =  min_max(sequenceArray[selectedChannel].stepData[selectedStep].gateType + knobChange, 0, 3);
            break;

          case STATE_GLIDE:
            sequenceArray[selectedChannel].stepData[selectedStep].glide =  min_max(sequenceArray[selectedChannel].stepData[selectedStep].glide + knobChange, 0, 128);
            break;

          case STATE_ARPTYPE:
            sequenceArray[selectedChannel].stepData[selectedStep].arpType = min_max(sequenceArray[selectedChannel].stepData[selectedStep].arpType + knobChange, 0, 5);
          break;

          case STATE_ARPSPEEDNUM:
          sequenceArray[selectedChannel].stepData[selectedStep].arpSpdNum = min_max(sequenceArray[selectedChannel].stepData[selectedStep].arpSpdNum + knobChange, 1, 16) ;
          break;
          case STATE_ARPSPEEDDEN:
          sequenceArray[selectedChannel].stepData[selectedStep].arpSpdDen = min_max(sequenceArray[selectedChannel].stepData[selectedStep].arpSpdDen + knobChange, 1, 64);
          break;

          case STATE_ARPOCTAVE:
          sequenceArray[selectedChannel].stepData[selectedStep].arpOctave=  min_max(sequenceArray[selectedChannel].stepData[selectedStep].arpOctave + knobChange, 1, 5);
          break;

          case STATE_VELOCITY:
          sequenceArray[selectedChannel].stepData[selectedStep].velocity =  positive_modulo(sequenceArray[selectedChannel].stepData[selectedStep].velocity + knobChange, 127);
          break;
          case STATE_VELOCITYTYPE:
          sequenceArray[selectedChannel].stepData[selectedStep].velocityType = min_max(sequenceArray[selectedChannel].stepData[selectedStep].velocityType + knobChange, 0, 4);
          break;
          case STATE_LFOSPEED:
          sequenceArray[selectedChannel].stepData[selectedStep].lfoSpeed = min_max(sequenceArray[selectedChannel].stepData[selectedStep].lfoSpeed + knobChange, 1, 255);
          break;

        }
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



void InputModule::calibrationMenuHandler(){
  uint8_t multiplier = 100;
  playing = 0;
  if (midplaneGPIO->pressed(SW_REC)){
    multiplier = 10;
  }
  if (knobChange){
    calibrationBuffer += knobChange * multiplier;
    outputControl->calibrationRoutine();
  }

}
