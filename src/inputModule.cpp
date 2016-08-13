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
  midplaneGPIO->initPort(25, 10, INPUT_PULLUP);  // SW_MENU
  midplaneGPIO->initPort(26, 12, INPUT_PULLUP);  // SW_ALT
  midplaneGPIO->initPort(27, 15, INPUT_PULLUP);  // SW_SPARE

  midplaneGPIO->updateGpioPinModes();

  Serial.println("button setup complete");

}

void InputModule::loop(uint16_t frequency){
  if (inputTimer > frequency){
    inputTimer = 0;
    knobPrevious = knobRead;
    knobRead = knob.read()/-4  ;
    knobChange = knobRead - knobPrevious;

    midplaneGPIO->update();


    //we always want the alt (non matrix) buttons to behave the same way
    altButtonHandler();

    // now to handle the rest of the buttons.
    switch (currentState) {
      case STEP_DISPLAY:
      stepModeMatrixHandler();
      break;
      case SEQUENCE_MENU:
      sequencerMenuHandler();
      break;

      case CHANNEL_MENU:
      channelMenuHandler();
      break;

      case PATTERN_SELECT:
      patternSelectHandler();
      break;

      case INSTRUMENT_MENU:
      instrumentSelectInputHandler();
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
      changeState(STEP_DISPLAY);
    }
  }
}

void InputModule::channelMenuHandler(){
  if (midplaneGPIO->fell(0)){
    sequenceArray[selectedChannel].initNewSequence(sequenceArray[selectedChannel].patternIndex, selectedChannel);
      changeState(STEP_DISPLAY);

  } else if (midplaneGPIO->fell(4)){
    for (int i=0; i < 4; i++){
      sequenceArray[i].initNewSequence(sequenceArray[i].patternIndex, i);
    }
      changeState(STEP_DISPLAY);

  } else if (midplaneGPIO->fell(8)){
    saveFile->deleteSaveFile();
    changeState(STEP_DISPLAY);

  } else if (midplaneGPIO->fell(12)){
    if (sequenceArray[selectedChannel].instType == 0){
      sequenceArray[selectedChannel].setInstType(1);
      //sequenceArray[selectedChannel].stepCount = 16;
      //sequenceArray[selectedChannel].beatCount = 16;
      life.genGrid(micros());
    } else {
      sequenceArray[selectedChannel].setInstType(0);
    }
    changeState(STEP_DISPLAY);
  } else if (midplaneGPIO->fell(3)){
    extClock = !extClock;
        changeState(STEP_DISPLAY);

  }
}

void InputModule::channelButtonHandler(uint8_t channel){
  uint8_t previous = patternChannelSelector;
  if (currentState == PATTERN_SELECT) {
    patternChannelSelector =  patternChannelSelector ^ (1 << channel);
    if (patternChannelSelector == 0) {
      patternChannelSelector = previous;
    }
  } else {
    selectedChannel = channel;
    changeState(STEP_DISPLAY);
    resetKnobValues();

  }
}

void InputModule::altButtonHandler(){
  for (int i=16; i <28; i++){
    if (midplaneGPIO->fell(i) ){
      switch (i){
        // left row bottom up
        case SW_M0:
          channelButtonHandler(0);
        break;

        case SW_M1:
          channelButtonHandler(1);
        break;

        case SW_M2:
          channelButtonHandler(2);
        break;

        case SW_M3:
          channelButtonHandler(3);
        break;

        case SW_MENU:
          if(currentState == PATTERN_SELECT){
            changeState(STEP_DISPLAY);
          } else {
            changeState(PATTERN_SELECT);
          }
        break;

        case SW_ALT: //switch M3 toggles the sequence menu

          if (stepMode < 3 || stepMode > 7) {
            stepMode = 3;
          } else {
            stepMode++;
          }

          if( currentState != STEP_DISPLAY){
            changeState(STEP_DISPLAY);
          }

        break;

        case SW_SPARE:
          changeState(CHANNEL_MENU);
        break;
        case SW_PGDN:
          notePage = positive_modulo(notePage - 1, 4);
        break;

        case SW_PGUP:
          notePage = positive_modulo(notePage + 1, 4);
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
            changeState(CALIBRATION_MENU);
          break;
        }
      }
    }
  }


  void InputModule::stepModeMatrixHandler(){
  //  saveTimer = 0;
  // selectedStep == getNote(i) means that the user pressed the button that is selected.
    uint8_t instrumentSelectValue;

    for (int i=0; i < 16; i++){
      if (midplaneGPIO->fell(i)){
        if(selectedStep == getNote(i) && stepMode == STEPMODE_PITCH0){
          stepMode = STEPMODE_GATELENGTH; // change the step length
          knobBuffer = sequenceArray[selectedChannel].stepData[getNote(i)].gateLength - knobRead;
         // stepModeBuffer = sequenceArray[selectedChannel].stepData[i].gateLength;
    //    } else if (selectedStep == i && stepMode != 0){
    //      stepMode = positive_modulo(stepMode + 1, 3); // change the step length
    //   //   knob1.write(0);
    //   //   stepModeBuffer = sequenceArray[selectedChannel].stepData[i].gateType;
    //      knobBuffer = sequenceArray[selectedChannel].stepData[i].gateType - knobRead;
        } else if (selectedStep == getNote(i) && stepMode == STEPMODE_GATELENGTH){
          stepMode = STEPMODE_VELOCITY;
          knobBuffer = sequenceArray[selectedChannel].stepData[getNote(i)].velocity - knobRead;
        } else if (selectedStep == getNote(i) && stepMode == STEPMODE_VELOCITY){
          stepMode = STEPMODE_GATETYPE;
          knobBuffer = sequenceArray[selectedChannel].stepData[getNote(i)].gateType - knobRead;
        } else if (selectedStep == getNote(i) && stepMode == STEPMODE_GATETYPE) {
          stepMode = STEPMODE_GLIDE;
          selectedStep = getNote(i);
          knobBuffer = sequenceArray[selectedChannel].stepData[getNote(i)].glide - knobRead;
        } else if (selectedStep == getNote(i) && stepMode == STEPMODE_GLIDE) {
          stepMode = STEPMODE_PITCH1;
          selectedStep = getNote(i);
          knobBuffer = sequenceArray[selectedChannel].getStepPitch(selectedStep, 1) - knobRead;
        } else if (selectedStep == getNote(i) && stepMode == STEPMODE_PITCH1) {
          stepMode = STEPMODE_PITCH2;
          selectedStep = getNote(i);
          knobBuffer = sequenceArray[selectedChannel].getStepPitch(selectedStep, 2) - knobRead;
        } else if (selectedStep == getNote(i) && stepMode == STEPMODE_PITCH2) {
          stepMode = STEPMODE_PITCH3;
          selectedStep = getNote(i);
          knobBuffer = sequenceArray[selectedChannel].getStepPitch(selectedStep, 3) - knobRead;
        } else {
          stepMode = STEPMODE_PITCH0;
          selectedStep = getNote(i);
          knobBuffer = sequenceArray[selectedChannel].getStepPitch(selectedStep, 0) - knobRead;
        }
      }
    }


  //  if (knobRead != knobPrev) {
    if (knobChange){
      //knobPrev = knobRead;
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
          sequenceArray[selectedChannel].setStepPitch(selectedStep, sequenceArray[selectedChannel].getStepPitch(selectedStep, 1) + knobChange, 1);
        break;
        case STEPMODE_PITCH2:
          sequenceArray[selectedChannel].setStepPitch(selectedStep, sequenceArray[selectedChannel].getStepPitch(selectedStep, 2) + knobChange, 2);
        break;
        case STEPMODE_PITCH3:
          sequenceArray[selectedChannel].setStepPitch(selectedStep, sequenceArray[selectedChannel].getStepPitch(selectedStep, 3) + knobChange, 3);
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

        case STEPMODE_VELOCITY:
      // change length of gate
        sequenceArray[selectedChannel].stepData[selectedStep].velocity = positive_modulo(sequenceArray[selectedChannel].stepData[selectedStep].velocity + knobChange, 128 );
        break;

        case STEPMODE_TEMPO:
        if (tempoX100 > 100100) {
          tempoX100 = 100100;
        }
        tempoX100 = positive_modulo(tempoX100 + knobChange*100, 100100 );
        if(tempoX100 == 0){
          tempoX100 = 100;
        }
      /* THIS CAUSES A CRAZY COMPILATION ERROR! :
      /Users/tenkai/.platformio/packages/toolchain-gccarmnoneeabi/bin/../lib/gcc/arm-none-eabi/4.8.4/../../../../arm-none-eabi/lib/armv7e-m/libc_s.a(lib_a-timesr.o): In funct
ion `_times_r':
timesr.c:(.text._times_r+0x2): undefined reference to `_times'
*/
       clockMaster->changeTempo(tempoX100);
        break;

        case 4:
        instrumentSelectValue = positive_modulo(sequenceArray[selectedChannel].instrument + knobChange,128);
        if (sequenceArray[selectedChannel].instrument != instrumentSelectValue){
          sequenceArray[selectedChannel].instrument = instrumentSelectValue;
          outputControl->samCommand(PROGRAM_CHANGE, selectedChannel, sequenceArray[selectedChannel].instrument);
        }
        break;

        case 5:
        instrumentSelectValue = positive_modulo(sequenceArray[selectedChannel].volume + knobChange, 128);
        if (sequenceArray[selectedChannel].volume != instrumentSelectValue ){
          sequenceArray[selectedChannel].volume = instrumentSelectValue;
          outputControl->samCommand(SET_CHANNEL_VOLUME, selectedChannel, sequenceArray[selectedChannel].volume);
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
          break;

        case STEPMODE_GATETYPE:
          sequenceArray[selectedChannel].stepData[selectedStep].gateType =  positive_modulo(sequenceArray[selectedChannel].stepData[selectedStep].gateType + knobChange, 64);
          break;

        case STEPMODE_GLIDE:
          sequenceArray[selectedChannel].stepData[selectedStep].glide =  positive_modulo(sequenceArray[selectedChannel].stepData[selectedStep].glide + knobChange, 128);
          break;
      }
    }
};




  void InputModule::sequencerMenuHandler(){
    for (int i=0; i < 16; i++){
      if (midplaneGPIO->fell(i)){
        switch (i) {
          case 0:
          changeState(INSTRUMENT_MENU);
          break;
          case 1:
          changeState(TIMING_MENU);
          break;

        }
      }
    }
  }

  void InputModule::instrumentSelectInputHandler() {

    for (int i=0; i < 16; i++){
      if (midplaneGPIO->fell(i)){
        menuSelector = i;
      }
    }
    uint8_t instrumentSelectValue;

    switch(menuSelector){
      case 0:
     //changing the instrument every loop causes weird problems with playback during instrument selection
      instrumentSelectValue = positive_modulo(sequenceArray[selectedChannel].instrument + knobChange,128);
      if (sequenceArray[selectedChannel].instrument != instrumentSelectValue){
        sequenceArray[selectedChannel].instrument = instrumentSelectValue;
        outputControl->samCommand(PROGRAM_CHANGE, selectedChannel, sequenceArray[selectedChannel].instrument);
      }
      break;
      case 4:
      instrumentSelectValue = positive_modulo(sequenceArray[selectedChannel].volume + knobChange, 128);
      if (sequenceArray[selectedChannel].volume != instrumentSelectValue ){
        sequenceArray[selectedChannel].volume = instrumentSelectValue;
        outputControl->samCommand(SET_CHANNEL_VOLUME, selectedChannel, sequenceArray[selectedChannel].volume);
      }
      break;
      case 8:
      instrumentSelectValue = positive_modulo(sequenceArray[selectedChannel].bank + knobChange, 128) ;
      if (sequenceArray[selectedChannel].bank != instrumentSelectValue ){
        sequenceArray[selectedChannel].bank = instrumentSelectValue;
        outputControl->samCommand(SET_CHANNEL_VOLUME, selectedChannel, sequenceArray[selectedChannel].bank);
      }
      break;
    }

  }

void InputModule::timingMenuInputHandler() {
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
}


void InputModule::debugScreenInputHandler(){
  if(midplaneGPIO->pressed(SW_ALT)){
//      voltManual = positive_modulo(voltManual + 10*knobChange, 65535);

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
