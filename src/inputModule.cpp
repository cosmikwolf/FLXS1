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

void InputModule::initialize(OutputController* outputControl, FlashMemory* saveFile, Sequencer (*sequenceArray)[4]){
  Serial.println("button setup start");

  this->saveFile = saveFile;
  this->sequenceArray = sequenceArray;
  this->outputControl = outputControl;

  max7301.begin(5);                      delay(1);
  max7301.gpioPinMode(INPUT_PULLUP);     delay(1);
  max7301.init(0, 14);   // SW_00
  max7301.init(1, 7);    // SW_01
  max7301.init(2, 5);    // SW_02
  max7301.init(3, 26);   // SW_03
  max7301.init(4, 15);   // SW_04
  max7301.init(5, 11);   // SW_05
  max7301.init(6, 9);    // SW_06
  max7301.init(7, 1);    // SW_07
  max7301.init(8, 16);   // SW_08
  max7301.init(9, 12);   // SW_09
  max7301.init(10, 6);   // SW_10
  max7301.init(11, 27);  // SW_11
  max7301.init(12, 17);  // SW_12
  max7301.init(13, 13);  // SW_13
  max7301.init(14, 10);  // SW_14
  max7301.init(15, 0);   // SW_15
  max7301.init(16, 8);   // SW_PLAY
  max7301.init(17, 4);   // SW_PAUSE
  max7301.init(18, 2);   // SW_STOP
  max7301.init(19, 21);  // SW_M0
  max7301.init(20, 20);  // SW_M1
  max7301.init(21, 19);  // SW_M2
  max7301.init(22, 18);  // SW_M3
  max7301.init(23, 3);   // SW_PGDN
  max7301.init(24, 25);  // SW_PGUP
  max7301.init(25, 24);  // SW_MENU
  max7301.init(26, 23);  // SW_ALT
  max7301.init(27, 22);  // SW_SPARE

  Serial.println("button setup complete");

}

void InputModule::buttonLoop(){


  knobPrevious = knobRead;
  knobRead = knob.read()/-4  ;
  knobChange = knobRead - knobPrevious;
  if (knobChange != 0){
    Serial.println("Knobchange: " + String(knobChange));
    delay(1500);
  }

  max7301.update();

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
  }

}

// STATE VARIABLE INPUT HANDLERS

void InputModule::patternSelectHandler(){
  for (int i=0; i < 16; i++){
    if (max7301.fell(i)){
      saveFile->changePattern(i, patternChannelSelector,  true, true);

      delay(10);
      changeState(STEP_DISPLAY);
    }
  }
}

void InputModule::channelMenuHandler(){
  if (max7301.fell(0)){
    sequenceArray[selectedChannel]->initNewSequence(sequenceArray[selectedChannel]->patternIndex, selectedChannel);
      changeState(STEP_DISPLAY);

  } else if (max7301.fell(4)){
    for (int i=0; i < 4; i++){
      sequenceArray[i]->initNewSequence(sequenceArray[i]->patternIndex, i);
    }
      changeState(STEP_DISPLAY);

  } else if (max7301.fell(8)){
    saveFile->deleteSaveFile();
    changeState(STEP_DISPLAY);

  } else if (max7301.fell(12)){
    if (sequenceArray[selectedChannel]->instType == 0){
      sequenceArray[selectedChannel]->setInstType(1);
      //sequenceArray[selectedChannel]->stepCount = 16;
      //sequenceArray[selectedChannel]->beatCount = 16;
      life.genGrid(micros());
    } else {
      sequenceArray[selectedChannel]->setInstType(0);
    }
    changeState(STEP_DISPLAY);
  } else if (max7301.fell(3)){
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
    if (max7301.fell(i) ){
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
            sequenceArray[s]->activeStep = 0;
           // sam2695.allNotesOff(s);
          }
          break;

        // right two, bottom up
          case SW_PLAY:
          playing = !playing;
          break;
        }
      }
    }
  }


  void InputModule::stepModeMatrixHandler(){
  //  saveTimer = 0;
    uint8_t instrumentSelectValue;

    for (int i=0; i < 16; i++){
      if (max7301.fell(i)){
        if(selectedStep == getNote(i) && stepMode == 0){
          stepMode = 1; // change the step length
          knobBuffer = sequenceArray[selectedChannel]->stepData[getNote(i)].gateLength - knobRead;
         // stepModeBuffer = sequenceArray[selectedChannel]->stepData[i].gateLength;
    //    } else if (selectedStep == i && stepMode != 0){
    //      stepMode = positive_modulo(stepMode + 1, 3); // change the step length
    //   //   knob1.write(0);
    //   //   stepModeBuffer = sequenceArray[selectedChannel]->stepData[i].gateType;
    //      knobBuffer = sequenceArray[selectedChannel]->stepData[i].gateType - knobRead;
        } else if (selectedStep == getNote(i) && stepMode == 1){
          stepMode = 2;
          knobBuffer = sequenceArray[selectedChannel]->stepData[getNote(i)].velocity - knobRead;
        }else {
          stepMode = 0;
          selectedStep = getNote(i);
          knobBuffer = sequenceArray[selectedChannel]->getStepPitch(selectedStep) - knobRead;
        }
      }
    }


  //  if (knobRead != knobPrev) {
    if (knobChange){
      //knobPrev = knobRead;
      switch (stepMode) {
        case 0:
      // just change the note
        if (knobRead + sequenceArray[selectedChannel]->getStepPitch(selectedStep) < 0){
          // you can turn off a note by turning the value to 0
          // turn off a note by setting gate type and pitch to 0
          sequenceArray[selectedChannel]->stepData[selectedStep].gateType = 0;
          sequenceArray[selectedChannel]->setStepPitch(selectedStep, 0);
          knob.write(4);
        } else {
          if(sequenceArray[selectedChannel]->stepData[selectedStep].gateType == 0){
            // if a note is not active, turn it on and give it a length.
            sequenceArray[selectedChannel]->stepData[selectedStep].gateType = 1;
            sequenceArray[selectedChannel]->stepData[selectedStep].gateLength = 1;
          }
          // and finally set the new step value!
          sequenceArray[selectedChannel]->setStepPitch(selectedStep, positive_modulo(sequenceArray[selectedChannel]->getStepPitch(selectedStep) + knobChange, 127));

        }
        break;

        case 1:
    // change the gate type
        if ((sequenceArray[selectedChannel]->stepData[selectedStep].gateLength == 0) && (knobChange < 0)  ) {
          sequenceArray[selectedChannel]->stepData[selectedStep].gateType = 0;
        } else if(knobChange > 0) {
          sequenceArray[selectedChannel]->stepData[selectedStep].gateType = 1;
        }

        if (sequenceArray[selectedChannel]->stepData[selectedStep].gateType > 0){
          sequenceArray[selectedChannel]->stepData[selectedStep].gateLength =  positive_modulo(sequenceArray[selectedChannel]->stepData[selectedStep].gateLength + knobChange, 127);
        }

        break;

        case 2:
      // change length of gate
        sequenceArray[selectedChannel]->stepData[selectedStep].velocity = positive_modulo(sequenceArray[selectedChannel]->stepData[selectedStep].velocity + knobChange, 128 );
        break;

        case 3:
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
      // --->  clock.changeTempo(tempoX100);  <--
        break;

        case 4:
        instrumentSelectValue = positive_modulo(sequenceArray[selectedChannel]->instrument + knobChange,128);
        if (sequenceArray[selectedChannel]->instrument != instrumentSelectValue){
          sequenceArray[selectedChannel]->instrument = instrumentSelectValue;
          outputControl->samCommand(PROGRAM_CHANGE, selectedChannel, sequenceArray[selectedChannel]->instrument);
        }
        break;

        case 5:
        instrumentSelectValue = positive_modulo(sequenceArray[selectedChannel]->volume + knobChange, 128);
        if (sequenceArray[selectedChannel]->volume != instrumentSelectValue ){
          sequenceArray[selectedChannel]->volume = instrumentSelectValue;
          outputControl->samCommand(SET_CHANNEL_VOLUME, selectedChannel, sequenceArray[selectedChannel]->volume);
        }
        break;

        case 6:
        sequenceArray[selectedChannel]->stepCount = positive_modulo(sequenceArray[selectedChannel]->stepCount + knobChange, 129);
        break;

        case 7:
        sequenceArray[selectedChannel]->beatCount = positive_modulo(sequenceArray[selectedChannel]->beatCount + knobChange, 129);
        break;

      }
    }
};




  void InputModule::sequencerMenuHandler(){
    for (int i=0; i < 16; i++){
      if (max7301.fell(i)){
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
      if (max7301.fell(i)){
        menuSelector = i;
      }
    }
    uint8_t instrumentSelectValue;

    switch(menuSelector){
      case 0:
     //changing the instrument every loop causes weird problems with playback during instrument selection
      instrumentSelectValue = positive_modulo(sequenceArray[selectedChannel]->instrument + knobChange,128);
      if (sequenceArray[selectedChannel]->instrument != instrumentSelectValue){
        sequenceArray[selectedChannel]->instrument = instrumentSelectValue;
        outputControl->samCommand(PROGRAM_CHANGE, selectedChannel, sequenceArray[selectedChannel]->instrument);
      }
      break;
      case 4:
      instrumentSelectValue = positive_modulo(sequenceArray[selectedChannel]->volume + knobChange, 128);
      if (sequenceArray[selectedChannel]->volume != instrumentSelectValue ){
        sequenceArray[selectedChannel]->volume = instrumentSelectValue;
        outputControl->samCommand(SET_CHANNEL_VOLUME, selectedChannel, sequenceArray[selectedChannel]->volume);
      }
      break;
      case 8:
      instrumentSelectValue = positive_modulo(sequenceArray[selectedChannel]->bank + knobChange, 128) ;
      if (sequenceArray[selectedChannel]->bank != instrumentSelectValue ){
        sequenceArray[selectedChannel]->bank = instrumentSelectValue;
        outputControl->samCommand(SET_CHANNEL_VOLUME, selectedChannel, sequenceArray[selectedChannel]->bank);
      }
      break;
    }

  }

  void InputModule::timingMenuInputHandler() {
    for (int i=0; i < 16; i++){
      if (max7301.fell(i)){
        menuSelector = i;
      }
    }
    switch(menuSelector){
      case 0:
      sequenceArray[selectedChannel]->stepCount = positive_modulo(sequenceArray[selectedChannel]->stepCount + knobChange, 129);
      break;
      case 4:
      sequenceArray[selectedChannel]->beatCount = positive_modulo(sequenceArray[selectedChannel]->beatCount + knobChange, 129);
      break;
    }
  }


  void InputModule::debugScreenInputHandler(){
    if(max7301.pressed(SW_ALT)){
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
