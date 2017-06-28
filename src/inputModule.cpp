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

void InputModule::initialize(OutputController* outputControl, Zetaohm_MAX7301* midplaneGPIO, Zetaohm_MAX7301* backplaneGPIO, FlashMemory* saveFile, Sequencer *sequenceArray, MasterClock* clockMaster, DisplayModule* display){
  Serial.println("button setup start");

  this->saveFile = saveFile;
  this->sequenceArray = sequenceArray;
  this->outputControl = outputControl;
  this->clockMaster= clockMaster;
  this->midplaneGPIO= midplaneGPIO;
  this->backplaneGPIO= backplaneGPIO;
  this->display = display;

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
  midplaneGPIO->initPort(19, 1, INPUT_PULLUP);  // SW_CH0
  midplaneGPIO->initPort(20, 26, INPUT_PULLUP);  // SW_CH1
  midplaneGPIO->initPort(21, 2, INPUT_PULLUP);  // SW_CH2
  midplaneGPIO->initPort(22, 25, INPUT_PULLUP);  // SW_CH3
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
    knobRead = -1 * knob.read()/2  ;
    knobChange = knobRead - knobPrevious;
    midplaneGPIO->update();

    if (midplaneGPIO->pressed(SW_SHIFT) && midplaneGPIO->pressed(SW_PLAY) ){
      changeState(STATE_CALIBRATION);
    }

    if (midplaneGPIO->pressed(SW_SHIFT) && midplaneGPIO->pressed(SW_REC) ){
      changeState(STATE_INPUTDEBUG);
    }
    if (knobChange){
      switch (currentMenu) {
        case SEQUENCE_MENU:
        case MOD_MENU_1:
        case MOD_MENU_2:
        case QUANTIZE_MENU:
          if(backplaneGPIO->pressed(22)){
            changeState(min_max_cycle(stepMode+knobChange, STATE_FIRSTSTEP , STATE_ARPINTMOD ));
          }
          break;
      }
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
      case MOD_MENU_1:
      case MOD_MENU_2:
      case QUANTIZE_MENU:
        sequenceMenuHandler();
      break;
      case INPUT_MENU:
        inputMenuHandler();
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

      case INPUT_DEBUG_MENU:
        if (knobChange){
          selectedText = positive_modulo(selectedText+knobChange, 5);
        }
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

      case MENU_MODAL:
        if (modalTimer > 1000){
          changeState(STATE_PITCH0);
        }
      break;

      case SAVE_MENU:
        saveMenuInputHandler();
      break;

    }
  }

}

void InputModule::changeState(uint8_t targetState){
//  Serial.println("change state: " + String(targetState));
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
    case STATE_FIRSTSTEP:
    case STATE_STEPCOUNT:
    case STATE_BEATCOUNT:
    case STATE_SKIPSTEPCOUNT:
    case STATE_SKIPSTEP:
    case STATE_SWING:
      currentMenu = SEQUENCE_MENU;
      break;
    case STATE_QUANTIZESCALE:
    case STATE_QUANTIZEKEY:
    case STATE_QUANTIZEMODE:
      currentMenu = QUANTIZE_MENU;
      break;
    case STATE_NOTEDISPLAY:
      currentMenu = NOTE_DISPLAY;
      break;
    case STATE_PATTERNSELECT:
      currentMenu = PATTERN_SELECT;
      break;
    case STATE_SAVE:
      currentMenu = SAVE_MENU;
      break;
    case STATE_CALIBRATION:
      currentMenu = CALIBRATION_MENU;
      break;
    case STATE_EXTCLOCK:
    case STATE_TEMPO:
    case STATE_RESETINPUT:
      currentMenu = TEMPO_MENU;
      break;
    case STATE_GATEMUTE:
    case STATE_RANDOMPITCH:
    case STATE_PITCHMOD:
    case STATE_GLIDEMOD:
    case STATE_GATEMOD:
      currentMenu = MOD_MENU_1;
      break;
    case STATE_ARPTYPEMOD:
    case STATE_ARPSPDMOD:
    case STATE_ARPOCTMOD:
    case STATE_ARPINTMOD:
      currentMenu = MOD_MENU_2;
      break;
    case STATE_INPUTDEBUG:
      currentMenu = INPUT_DEBUG_MENU;
      break;
    case STATE_GLOBAL:
      currentMenu = GLOBAL_MENU;
      break;
    case STATE_CALIB_INPUT0_OFFSET:
    case STATE_CALIB_INPUT0_LOW:
    case STATE_CALIB_INPUT0_HIGH:
    case STATE_CALIB_INPUT1_OFFSET:
    case STATE_CALIB_INPUT1_LOW:
    case STATE_CALIB_INPUT1_HIGH:
    case STATE_CALIB_INPUT2_OFFSET:
    case STATE_CALIB_INPUT2_LOW:
    case STATE_CALIB_INPUT2_HIGH:
    case STATE_CALIB_INPUT3_OFFSET:
    case STATE_CALIB_INPUT3_LOW:
    case STATE_CALIB_INPUT3_HIGH:
    case STATE_CALIB_OUTPUT0_LOW:
    case STATE_CALIB_OUTPUT0_HIGH:
    case STATE_CALIB_OUTPUT1_LOW:
    case STATE_CALIB_OUTPUT1_HIGH:
    case STATE_CALIB_OUTPUT2_LOW:
    case STATE_CALIB_OUTPUT2_HIGH:
    case STATE_CALIB_OUTPUT3_LOW:
    case STATE_CALIB_OUTPUT3_HIGH:
      currentMenu = CALIBRATION_MENU;
      break;
    case STATE_CALIBRATION_SAVE_MODAL:
      currentMenu = MENU_MODAL;
      break;
    case STATE_MULTISELECT:
      currentMenu = MENU_MULTISELECT;
      break;
    default:
      Serial.println("This state has no menu selection! " + String(targetState));
    break;
  }

  //this->resetKnobValues();
}

// STATE VARIABLE INPUT HANDLERS

void InputModule::patternSelectHandler(){

  for (int i=0; i < 16; i++){
    if (midplaneGPIO->fell(i)){
      saveFile->changePattern(i, patternChannelSelector, true);

      //delay(10);
      changeState(STATE_PITCH0);
    }
  }
//  changeState(STATE_PITCH0);

}

void InputModule::tempoMenuHandler(){
  for (int i=0; i < 16; i++){
    if (midplaneGPIO->fell(i)){
      changeState(STATE_PITCH0);
      selectedStep = getNote(i);
    }
  }

  if(knobChange){
    if(backplaneGPIO->pressed(SW_ENCODER_BACKPLANE)){
      changeState(min_max_cycle(stepMode+knobChange, STATE_TEMPO , STATE_RESETINPUT ));
    } else {
      switch (stepMode){
        case STATE_TEMPO:
        //  if (tempoX100 > 100200) {
        //    tempoX100 = 100200;
        //  }
          tempoX100 = min_max_cycle(tempoX100 + knobChange*100, 100, 100000 );
        //  if(tempoX100 == 0){
        //    tempoX100 = 100;
        //  }
          clockMaster->changeTempo(tempoX100);
        break;

        case STATE_EXTCLOCK:
          clockMode = positive_modulo(clockMode + knobChange, 7);
        break;

        case STATE_RESETINPUT:
          sequenceArray[selectedChannel].gpio_reset = min_max(sequenceArray[selectedChannel].gpio_reset + knobChange, 0,8);
        break;



      }
    }
  }
}

void InputModule::sequenceMenuHandler(){
  int8_t newBeatDiv;

  for (int i=0; i < 16; i++){
    if (midplaneGPIO->fell(i)){
      changeState(STATE_PITCH0);
      selectedStep = sequenceArray[selectedChannel].getActivePage()*16 + i;
      notePage =   sequenceArray[selectedChannel].getActivePage();
    }
  }
  if(knobChange){
    if ( backplaneGPIO->pressed(22) ) {// Encoder Switch
    //  changeState(min_max_cycle(stepMode + knobChange,  STATE_STEPCOUNT,  STATE_QUANTIZEMODE));
    } else {
      switch(stepMode){
        case STATE_FIRSTSTEP:
          sequenceArray[selectedChannel].firstStep = min_max(sequenceArray[selectedChannel].firstStep + knobChange, 0, 63);
          //sequenceArray[selectedChannel].firstStepChanged == TRUE;
          break;
        case STATE_STEPCOUNT:
          sequenceArray[selectedChannel].stepCount = min_max(sequenceArray[selectedChannel].stepCount + knobChange, 1, 64);

          // if (sequenceArray[selectedChannel].stepCount == 0) {
          //   sequenceArray[selectedChannel].stepCount = 64;
          // }
          break;
        case STATE_BEATCOUNT:
           newBeatDiv = min_max(sequenceArray[selectedChannel].clockDivision + knobChange, -16, 16);
           sequenceArray[selectedChannel].clockDivision = newBeatDiv;
        //   Serial.println("newBeatDiv: " + String(newBeatDiv));
          // Serial.println("clockDiv: " + String(sequenceArray[selectedChannel].clockDivision));

          break;

        case STATE_QUANTIZEKEY:
          sequenceArray[selectedChannel].quantizeKey = positive_modulo(sequenceArray[selectedChannel].quantizeKey + knobChange, 12);
          break;
        case STATE_QUANTIZEMODE:
          sequenceArray[selectedChannel].quantizeMode = positive_modulo(sequenceArray[selectedChannel].quantizeMode + knobChange, 19);
          break;
        case STATE_QUANTIZESCALE:
          sequenceArray[selectedChannel].quantizeScale = positive_modulo(sequenceArray[selectedChannel].quantizeScale + knobChange, 3);
          break;

        case STATE_RESETINPUT:
          sequenceArray[selectedChannel].gpio_reset = min_max_skip(sequenceArray[selectedChannel].gpio_reset, knobChange, -9, 8, selectedChannel+5);

          break;

// mod menu 1
        case STATE_GLIDEMOD:
          sequenceArray[selectedChannel].cv_glidemod = positive_modulo(sequenceArray[selectedChannel].cv_glidemod + knobChange, 12);
        break;

        case STATE_GATEMOD:
          sequenceArray[selectedChannel].cv_gatemod = positive_modulo(sequenceArray[selectedChannel].cv_gatemod + knobChange, 12);
        break;
        case STATE_GATEMUTE:
          sequenceArray[selectedChannel].gpio_gatemute = min_max_skip(sequenceArray[selectedChannel].gpio_gatemute, knobChange, -9, 8, selectedChannel+5);

        break;

        case STATE_RANDOMPITCH:
          sequenceArray[selectedChannel].gpio_randompitch = min_max_skip(sequenceArray[selectedChannel].gpio_randompitch, knobChange, -9, 8, selectedChannel+5);
        break;
        case STATE_RANDOMHIGH:
          sequenceArray[selectedChannel].randomHigh = min_max(sequenceArray[selectedChannel].randomHigh + knobChange, 0, 127);
        break;
        case STATE_RANDOMLOW:
        sequenceArray[selectedChannel].randomLow = min_max(sequenceArray[selectedChannel].randomLow + knobChange, 0, 127);
        break;

        case STATE_PITCHMOD:
          sequenceArray[selectedChannel].cv_pitchmod = positive_modulo(sequenceArray[selectedChannel].cv_pitchmod + knobChange, 13);
        break;
// mod menu 2
        case STATE_ARPTYPEMOD:
          sequenceArray[selectedChannel].cv_arptypemod = positive_modulo(sequenceArray[selectedChannel].cv_arptypemod + knobChange, 13);
        break;

        case STATE_ARPSPDMOD:
          sequenceArray[selectedChannel].cv_arpspdmod = positive_modulo(sequenceArray[selectedChannel].cv_arpspdmod + knobChange, 13);
        break;

        case STATE_ARPOCTMOD:
          sequenceArray[selectedChannel].cv_arpoctmod = positive_modulo(sequenceArray[selectedChannel].cv_arpoctmod + knobChange, 13);
        break;

        case STATE_ARPINTMOD:
          sequenceArray[selectedChannel].cv_arpintmod = positive_modulo(sequenceArray[selectedChannel].cv_arpintmod + knobChange, 13);
        break;

        case STATE_SKIPSTEP:
          sequenceArray[selectedChannel].gpio_skipstep = min_max_skip(sequenceArray[selectedChannel].gpio_skipstep, knobChange, -9, 8, selectedChannel+5);
        break;

        case STATE_SKIPSTEPCOUNT:
          sequenceArray[selectedChannel].skipStepCount = positive_modulo(sequenceArray[selectedChannel].skipStepCount + knobChange, 16);
        break;

        case STATE_SWING:
          sequenceArray[selectedChannel].swingX100 = min_max(sequenceArray[selectedChannel].swingX100 + knobChange, 1,99);
        break;




      }
    }

  }

}

void InputModule::inputMenuHandler(){

  if(knobChange){
    if ( backplaneGPIO->pressed(22) ) {// Encoder Switch
      changeState(min_max_cycle(stepMode+knobChange, STATE_STEPCOUNT , STATE_SKIPSTEP));
    } else {

      switch(stepMode){



      }
    }

  }

}

void InputModule::globalMenuHandler(){
  /*if (midplaneGPIO->fell(0)){
    Serial.println("Initializing sequence");
    sequenceArray[selectedChannel].initNewSequence(currentPattern, selectedChannel);
      changeState(STATE_PITCH0);

  } else if (midplaneGPIO->fell(4)){
    Serial.println("Initializing pattern");
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

  }
  */
  changeState(STATE_PITCH0);
}


void InputModule::channelInputInputHandler(){

}


void InputModule::channelButtonShiftHandler(uint8_t channel){
  if (selectedChannel != channel){
    selectedChannel = channel;
  }
  changeState(STATE_TUNER);
};

void InputModule::channelButtonShiftMenuHandler(uint8_t channel){
  selectedCalibrationChannel = channel;
  changeState(STATE_CALIB_INPUT0_OFFSET);
  Serial.println("CALIB MENU " + String(channel) + "\t" + String(CALIBRATION_MENU));
}

void InputModule::channelButtonChannelSelectorHandler(uint8_t channel){
  switch(channel){
    case 0:
      patternChannelSelector = patternChannelSelector ^ 0b0001;
    break;
    case 1:
      patternChannelSelector = patternChannelSelector ^ 0b0010;
    break;
    case 2:
      patternChannelSelector = patternChannelSelector ^ 0b0100;
    break;
    case 3:
      patternChannelSelector = patternChannelSelector ^ 0b1000;
    break;
  }
};

void InputModule::channelButtonHandler(uint8_t channel){
  //uint8_t previous = patternChannelSelector;

  resetKnobValues();
  if (selectedChannel != channel){
    selectedChannel = channel;
    return;
  }
  changeState(min_max_cycle(++stepMode,STATE_PITCH0, STATE_LFOSPEED));

}


void InputModule::altButtonHandler(){

  uint8_t channelButton;
  //chPressedSelector = 0;
  for (int i=16; i <28; i++){
    if (midplaneGPIO->fell(i) ){
      switch (i){
        // left row bottom up
        case SW_CH0:
          channelButton = 0;
          goto  SW_DEFAULT;
        case SW_CH1:
          channelButton = 1;
          goto  SW_DEFAULT;
        case SW_CH2:
          channelButton = 2;
          goto  SW_DEFAULT;
        case SW_CH3:
         channelButton = 3;

         SW_DEFAULT:

         switch(currentMenu){
           case PATTERN_SELECT:
           case SAVE_MENU:
            channelButtonChannelSelectorHandler(channelButton);
            break;
           case CALIBRATION_MENU:
            if (midplaneGPIO->pressed(SW_SHIFT)){
              calibrationSaveHandler();
            }
           break;
           default:

              if (midplaneGPIO->pressed(SW_CH0)){
                  chPressedSelector = chPressedSelector | 0b0001;
              } else {chPressedSelector = chPressedSelector & ~0b0001;}
              if (midplaneGPIO->pressed(SW_CH1)){
                  chPressedSelector = chPressedSelector | 0b0010;
              } else {chPressedSelector = chPressedSelector & ~0b0010;}
              if (midplaneGPIO->pressed(SW_CH2)){
                  chPressedSelector = chPressedSelector | 0b0100;
              } else {chPressedSelector = chPressedSelector & ~0b0100;}
              if (midplaneGPIO->pressed(SW_CH3)){
                  chPressedSelector = chPressedSelector | 0b1000;
              } else {chPressedSelector = chPressedSelector & ~0b1000;}


             if (midplaneGPIO->pressed(SW_SHIFT)){
               if(midplaneGPIO->pressed(SW_MENU)){
                 channelButtonShiftMenuHandler(channelButton);
               } else {
                 channelButtonShiftHandler(channelButton);
               }
             } else if (selectedChannel == channelButton ) {
               switch(currentMenu){
                 case SEQUENCE_MENU:
                 changeState(STATE_QUANTIZESCALE);
                 break;
                 case QUANTIZE_MENU:
                 changeState(STATE_GATEMOD);
                 break;
                 case MOD_MENU_1:
                 changeState(STATE_ARPTYPEMOD);
                 break;
                 case MOD_MENU_2:
                 changeState(STATE_FIRSTSTEP);
                 break;
                 default:
                 changeState(STATE_FIRSTSTEP);
               }
             } else {
              //Serial.println("going to channel button menu");
               channelButtonHandler(channelButton);
             }
         }
        break;

        case SW_REC:

        break;

        case SW_PATTERN:
          if (midplaneGPIO->pressed(SW_SHIFT)){
            for(int i=0; i<SEQUENCECOUNT; i++){
              //saveDestination[i] = sequenceArray[i].pattern;
              saveDestination[i] = currentPattern;
            }
            changeState(STATE_SAVE);
            break;
          }


          if(midplaneGPIO->pressed(SW_CH0)){
            if( sequenceArray[0].toggleMute() ){
              display->displayModal(750, MODAL_MUTE_CH1);
            } else {
              display->displayModal(750, MODAL_UNMUTE_CH1);
            }
          }
          if(midplaneGPIO->pressed(SW_CH1)){
             if( sequenceArray[1].toggleMute() ){
               display->displayModal(750, MODAL_MUTE_CH2);
             } else {
               display->displayModal(750, MODAL_UNMUTE_CH2);
             }
           }
          if(midplaneGPIO->pressed(SW_CH2)){
             if( sequenceArray[2].toggleMute() ){
               display->displayModal(750, MODAL_MUTE_CH3);
             } else {
               display->displayModal(750, MODAL_UNMUTE_CH3);
             }
           }
          if(midplaneGPIO->pressed(SW_CH3)){
             if( sequenceArray[3].toggleMute() ){
               display->displayModal(750, MODAL_MUTE_CH4);
             } else {
               display->displayModal(750, MODAL_UNMUTE_CH4);
             }
           }

          if(midplaneGPIO->pressed(SW_CH0) || midplaneGPIO->pressed(SW_CH1) || midplaneGPIO->pressed(SW_CH2) || midplaneGPIO->pressed(SW_CH3)) break;

          switch(currentMenu){
            case PATTERN_SELECT:
              changeState(STATE_PITCH0);
            break;
            case SAVE_MENU:
              saveFile->savePattern(patternChannelSelector, saveDestination);
              changeState(STATE_PITCH0);
              display->displayModal(750, MODAL_SAVE);

            break;
            default:
              changeState(STATE_PATTERNSELECT);
            break;
          }
        break;

        case SW_MENU: //switch M3 toggles the sequence menu
         if (midplaneGPIO->pressed(SW_SHIFT)){
           changeState(STATE_GLOBAL);
         } else {
          //  if (currentMenu == SEQUENCE_MENU || currentMenu == INPUT_MENU || currentMenu == MOD_MENU_1 || currentMenu == MOD_MENU_2){
          //    changeState(min_max_cycle(stepMode+1, STATE_STEPCOUNT , STATE_SKIPSTEP));
          //  } else {
          //    changeState(STATE_STEPCOUNT);
          //  }
          if (currentMenu == TEMPO_MENU){
            changeState(min_max_cycle(stepMode+1, STATE_TEMPO , STATE_SKIPSTEP));
          } else {
            changeState(STATE_TEMPO);
          }
         }
        break;

        case SW_SHIFT:
          if (currentMenu == SAVE_MENU){
            changeState(STATE_PITCH0);
          }
        break;

        case SW_PGDN:



          if (midplaneGPIO->pressed(SW_SHIFT)){
            changeState(STATE_MULTISELECT);
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
          chPressedSelector = 0;
          if (midplaneGPIO->pressed(SW_CH0)){
              chPressedSelector = chPressedSelector | 0b0001;
          } else {chPressedSelector = chPressedSelector & ~0b0001;}
          if (midplaneGPIO->pressed(SW_CH1)){
              chPressedSelector = chPressedSelector | 0b0010;
          } else {chPressedSelector = chPressedSelector & ~0b0010;}
          if (midplaneGPIO->pressed(SW_CH2)){
              chPressedSelector = chPressedSelector | 0b0100;
          } else {chPressedSelector = chPressedSelector & ~0b0100;}
          if (midplaneGPIO->pressed(SW_CH3)){
              chPressedSelector = chPressedSelector | 0b1000;
          } else {chPressedSelector = chPressedSelector & ~0b1000;}


          if (chPressedSelector && chRecEraseTimer > 750){
              chRecEraseTimer = 0;
              display->displayModal(750, MODAL_ERASEARMED, chPressedSelector);
          } else if (chPressedSelector && chRecEraseTimer < 750) {
              for(int i=0; i<4; i++){
                if ((0b001 << i) & chPressedSelector){
                  sequenceArray[i].initNewSequence(currentPattern, i);
                }
              }
              display->displayModal(750, MODAL_ERASED, chPressedSelector);
          } else {
            if (!playing){ //if the sequence is already paused, stop kills all internal sound.
              for(uint8_t channel = 0; channel < SEQUENCECOUNT; channel++){
                outputControl->allNotesOff(channel);
              }
            }
            playing = false;

            for(int s = 0; s < SEQUENCECOUNT; s++){
              sequenceArray[s].clockReset(true);
            }


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


  void InputModule::channelPitchModeInputHandler(){
  // selectedStep == getNote(i) means that the user pressed the button that is selected.
    uint8_t chrd;
    for (int i=0; i < 16; i++){
      if (midplaneGPIO->fell(i)){
        // select the step
        selectedStep = getNote(i);

        buttonMode = BUTTON_MODE_XOX;
        //  changeState(STATE_PITCH0);
        // i is the first step pressed
        // n is the second step pressed
        if (playing){
          //two butons are pressed simultaneously, loop between the buttons
          //play direction is dependent upon the order in which buttons are pressed.
          for (int n=0; n < 16; n++){
            if (i == n) continue;
            if (midplaneGPIO->pressed(n)){
              buttonMode = BUTTON_MODE_PLAYRANGE;
              if(lastSelectedStep == selectedStep && selectedStepTimer < DOUBLECLICKMS){
                sequenceArray[selectedChannel].setPlayRange(n,i);
                break;
              }
            }
          }

        }

        if(!playing){
          sequenceArray[selectedChannel].stoppedTrig(selectedStep, true, true);
        }


        //multi select logic - double tap and hold
        if(lastSelectedStep == selectedStep && selectedStepTimer < DOUBLECLICKMS){
          //enable multi select mode -
          multiSelectStep = selectedStep;
         //Serial.println("enable mutli select");
        }
        // record which was the last step, and time, for double press purposes


      } else if (midplaneGPIO->rose(i)){
        if (!playing){
          sequenceArray[selectedChannel].stoppedTrig(getNote(i), false, false);
        }

        if(buttonMode == BUTTON_MODE_XOX){
          selectedStep = getNote(i);
          if(lastSelectedStep == selectedStep && selectedStepTimer < DOUBLECLICKMS){
            sequenceArray[selectedChannel].stepData[selectedStep].gateType = GATETYPE_REST;
          } else {
            if (sequenceArray[selectedChannel].stepData[selectedStep].gateType == GATETYPE_REST){
              sequenceArray[selectedChannel].stepData[selectedStep].gateType = GATETYPE_STEP ;
            }
  //            sequenceArray[selectedChannel].stepData[selectedStep].gateLength = 1;
          }
        }

        lastSelectedStep = selectedStep;
        selectedStepTimer = 0;

        if (multiSelectStep == selectedStep){
          //disable multi select mode
         //Serial.println("disable mutli select");
          multiSelectStep = 255;
        }
      }
    }

    if (knobChange){
      //knobPrev = knobRead;

//      if (midplaneGPIO->pressed(selectedStep%16) ){
      backplaneGPIO->update();
      if ( midplaneGPIO->pressed(SW_CH0) || midplaneGPIO->pressed(SW_CH1) || midplaneGPIO->pressed(SW_CH2) || midplaneGPIO->pressed(SW_CH3) || backplaneGPIO->pressed(22) ) {// Encoder Switch
        changeState(positive_modulo(stepMode + knobChange, STATE_LFOSPEED+1));
      } else {

        switch (stepMode) {
          case STATE_PITCH0:
        // just change the note

          //  if(sequenceArray[selectedChannel].stepData[selectedStep].gateType == GATETYPE_REST){
          //    // if a note is not active, turn it on and give it a length.
          //    sequenceArray[selectedChannel].stepData[selectedStep].gateType = GATETYPE_STEP;
          //    sequenceArray[selectedChannel].stepData[selectedStep].gateLength = 1;
          //  }
            // and finally set the new step value!
            // monophonic so pitch[0] only
            sequenceArray[selectedChannel].setStepPitch(selectedStep, min_max_cycle(sequenceArray[selectedChannel].getStepPitch(selectedStep, 0) + knobChange, 0,120), 0);
            if(!playing){
              sequenceArray[selectedChannel].stoppedTrig(selectedStep, true, false);
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
              //  Serial.println("Setting Gatelength: " + String(sequenceArray[selectedChannel].stepData[selectedStep].gateLength));
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


void InputModule::saveMenuInputHandler(){
  uint8_t chButtonMask = 0;
  for (int i=0; i < 16; i++){
    if (midplaneGPIO->fell(i)){

      if(midplaneGPIO->pressed(SW_CH0)){
        chButtonMask |= 0b0001;
      }
      if(midplaneGPIO->pressed(SW_CH1)){
        chButtonMask |= 0b0010;
      }
      if(midplaneGPIO->pressed(SW_CH2)){
        chButtonMask |= 0b0100;
      }
      if(midplaneGPIO->pressed(SW_CH3)){
        chButtonMask |= 0b1000;
      }

      // if channel buttons are pressed, matrix buttons sets individual channel destinations
      if (chButtonMask & 0b0001) {
        saveDestination[0]=i;
      };
      if (chButtonMask & 0b0010) {
        saveDestination[1]=i;
      };
      if (chButtonMask & 0b0100) {
        saveDestination[2]=i;
      };
      if (chButtonMask & 0b1000) {
        saveDestination[3]=i;
      };
      if (chButtonMask == 0){
        //if no ch buttons are pressed, matrix buttons sets save destination for all channels
        saveDestination[0]=i;
        saveDestination[1]=i;
        saveDestination[2]=i;
        saveDestination[3]=i;
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
	knob.write(knob.read()%4);
  knobRead = -1 * knob.read()/2;
  knobPrevious = knobRead;
	//Serial.println("resetting knob: " + String(knob.read()));
};

void InputModule::calibrationSaveHandler(){
  modalTimer = 0;
  saveFile->saveCalibrationEEPROM();
  changeState(STATE_CALIBRATION_SAVE_MODAL);
}

void InputModule::calibrationMenuHandler(){
  uint8_t multiplier = 100;
  playing = 0;
  if (midplaneGPIO->pressed(SW_REC)){
    multiplier = 10;
  }
  if(knobChange){
    if(backplaneGPIO->pressed(SW_ENCODER_BACKPLANE)){
      changeState(min_max_cycle(stepMode+knobChange, STATE_CALIB_INPUT0_OFFSET , STATE_CALIB_OUTPUT7_TEST ));
    } else {
      switch (stepMode){
        case STATE_CALIB_INPUT0_LOW:
          adcCalibrationPos[0] += knobChange;
        break;
        case STATE_CALIB_INPUT0_HIGH:
          adcCalibrationNeg[0] += knobChange;
        break;

        case STATE_CALIB_INPUT0_OFFSET:
          adcCalibrationOffset[0] += knobChange;
        break;
        case STATE_CALIB_INPUT1_OFFSET:
          adcCalibrationOffset[1] += knobChange;
        break;
        case STATE_CALIB_INPUT2_OFFSET:
          adcCalibrationOffset[2] += knobChange;
        break;
        case STATE_CALIB_INPUT3_OFFSET:
          adcCalibrationOffset[3] += knobChange;
        break;
        case STATE_CALIB_INPUT1_LOW:
          adcCalibrationPos[1] += knobChange;
        break;
        case STATE_CALIB_INPUT1_HIGH:
          adcCalibrationNeg[1] += knobChange;
        break;
        case STATE_CALIB_INPUT2_LOW:
          adcCalibrationPos[2] += knobChange;
        break;
        case STATE_CALIB_INPUT2_HIGH:
          adcCalibrationNeg[2] += knobChange;
        break;
        case STATE_CALIB_INPUT3_LOW:
          adcCalibrationPos[3] += knobChange;
        break;
        case STATE_CALIB_INPUT3_HIGH:
          adcCalibrationNeg[3] += knobChange;
        break;
        case STATE_CALIB_OUTPUT0_LOW:
          dacCalibrationNeg[dacMap[0]] += knobChange;
        break;
        case STATE_CALIB_OUTPUT0_HIGH:
          dacCalibrationPos[dacMap[0]] += knobChange;
        break;
        case STATE_CALIB_OUTPUT1_LOW:
          dacCalibrationNeg[dacMap[1]] += knobChange;
        break;
        case STATE_CALIB_OUTPUT1_HIGH:
          dacCalibrationPos[dacMap[1]] += knobChange;
        break;
        case STATE_CALIB_OUTPUT2_LOW:
          dacCalibrationNeg[dacMap[2]] += knobChange;
        break;
        case STATE_CALIB_OUTPUT2_HIGH:
          dacCalibrationPos[dacMap[2]] += knobChange;
        break;
        case STATE_CALIB_OUTPUT3_LOW:
          dacCalibrationNeg[dacMap[3]] += knobChange;
        break;
        case STATE_CALIB_OUTPUT3_HIGH:
          dacCalibrationPos[dacMap[3]] += knobChange;
        break;
        case STATE_CALIB_OUTPUT4_LOW:
          dacCalibrationNeg[dacMap[4]] += knobChange;
        break;
        case STATE_CALIB_OUTPUT4_HIGH:
          dacCalibrationPos[dacMap[4]] += knobChange;
        break;
        case STATE_CALIB_OUTPUT5_LOW:
          dacCalibrationNeg[dacMap[5]] += knobChange;
        break;
        case STATE_CALIB_OUTPUT5_HIGH:
          dacCalibrationPos[dacMap[5]] += knobChange;
        break;
        case STATE_CALIB_OUTPUT6_LOW:
          dacCalibrationNeg[dacMap[6]] += knobChange;
        break;
        case STATE_CALIB_OUTPUT6_HIGH:
          dacCalibrationPos[dacMap[6]] += knobChange;
        break;
        case STATE_CALIB_OUTPUT7_LOW:
          dacCalibrationNeg[dacMap[7]] += knobChange;
        break;
        case STATE_CALIB_OUTPUT7_HIGH:
          dacCalibrationPos[dacMap[7]] += knobChange;
        break;
      }
    }
  }

}
