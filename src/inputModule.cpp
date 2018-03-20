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

void InputModule::initialize(OutputController* outputControl, Zetaohm_MAX7301* midplaneGPIO, Zetaohm_MAX7301* backplaneGPIO, FlashMemory* saveFile, Sequencer *sequenceArray, MasterClock* clockMaster, DisplayModule* display, GlobalVariable* globalObj){
  Serial.println("button setup start");

  this->saveFile = saveFile;
  this->sequenceArray = sequenceArray;
  this->outputControl = outputControl;
  this->clockMaster= clockMaster;
  this->midplaneGPIO= midplaneGPIO;
  this->backplaneGPIO= backplaneGPIO;
  this->globalObj = globalObj;
  this->display = display;
  this->shortcutRandomOctaveSpan = 2;
  this->shortcutRandomSwitch = 5000;

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


void InputModule::multiSelectInputHandler(){
  for (int i=0; i < 16; i++){
    if (midplaneGPIO->fell(i)){
      // select the step
      selectedStep = getNote(i);

      buttonMode = BUTTON_MODE_MULTISELECT;
      globalObj->multiSelection[getNote(i)] =  !globalObj->multiSelection[getNote(i)];
      globalObj->multi_pitch_switch = 0;
      globalObj->multi_arpInterval_switch = 0;
      globalObj->multi_gateType_switch = 0;
      globalObj->multi_gateLength_switch = 0;
      globalObj->multi_arpType_switch = 0;
      globalObj->multi_arpOctave_switch = 0;
      globalObj->multi_arpSpdNum_switch = 0;
      globalObj->multi_arpSpdDen_switch = 0;
      globalObj->multi_glide_switch = 0;
      globalObj->multi_beatDiv_switch = 0;
      globalObj->multi_velocity_switch = 0;
      globalObj->multi_velocityType_switch = 0;
      globalObj->multi_cv2speed_switch = 0;
      globalObj->multi_cv2offset_switch = 0;

      Serial.println("Setting " + String(getNote(i)) + " to " + String(globalObj->multiSelection[getNote(i)]));
    };
    if (midplaneGPIO->rose(i)){ };
  };

  if (knobChange){
    if ( globalObj->parameterSelect ) {// Encoder Switch
      changeState(min_max(stepMode + knobChange,STATE_PITCH0, STATE_CV2_OFFSET));
    } else {
      switch(stepMode){
        case STATE_PITCH0:
          if(!globalObj->multi_pitch_switch){  globalObj->multi_pitch_switch = true;
          } else { globalObj->multi_pitch += knobChange; }
          break;
        case STATE_CHORD:
          if(!globalObj->multi_arpInterval_switch){  globalObj->multi_arpInterval_switch = true;
          } else { globalObj->multi_arpInterval = min_max(globalObj->multi_arpInterval + knobChange, 0, 26 ); }
          break;
        case STATE_GATETYPE:
          if(!globalObj->multi_gateType_switch){  globalObj->multi_gateType_switch = true;
          } else { globalObj->multi_gateType = min_max(globalObj->multi_gateType + knobChange, 0, 3); }
          break;
        case STATE_GATELENGTH:
          if(!globalObj->multi_gateLength_switch){  globalObj->multi_gateLength_switch = true;
          } else { globalObj->multi_gateLength = min_max(globalObj->multi_gateLength + knobChange, 0, 64); }
          break;
        case STATE_ARPTYPE:
          if(!globalObj->multi_arpType_switch){  globalObj->multi_arpType_switch = true;
          } else { globalObj->multi_arpType = min_max(globalObj->multi_arpType + knobChange, 0, 5); }
          break;
        case STATE_ARPOCTAVE:
          if(!globalObj->multi_arpOctave_switch){  globalObj->multi_arpOctave_switch = true;
          } else { globalObj->multi_arpOctave = min_max(globalObj->multi_arpOctave + knobChange, 0, 5); }
          break;
        case STATE_ARPSPEEDNUM:
          if(!globalObj->multi_arpSpdNum_switch){  globalObj->multi_arpSpdNum_switch = true;
          } else { globalObj->multi_arpSpdNum  =min_max(globalObj->multi_arpSpdNum + knobChange,0,16); }
          break;
        case STATE_ARPSPEEDDEN:
          if(!globalObj->multi_arpSpdDen_switch){  globalObj->multi_arpSpdDen_switch = true;
          } else { globalObj->multi_arpSpdDen = min_max(globalObj->multi_arpSpdDen + knobChange,0,64); }
          break;
        case STATE_GLIDE:
          if(!globalObj->multi_glide_switch){  globalObj->multi_glide_switch = true;
          } else { globalObj->multi_glide = min_max(globalObj->multi_glide + knobChange,0,255); }
          break;
        case STATE_BEATCOUNT:
          if(!globalObj->multi_beatDiv_switch){  globalObj->multi_beatDiv_switch = true;
          } else { globalObj->multi_beatDiv += knobChange; }
          break;
        case STATE_CV2_LEVEL:
          if(!globalObj->multi_velocity_switch){  globalObj->multi_velocity_switch = true;
          } else { globalObj->multi_velocity += knobChange; }
          break;
        case STATE_CV2_TYPE:
          if(!globalObj->multi_velocityType_switch){  globalObj->multi_velocityType_switch = true;
          } else { globalObj->multi_velocityType = min_max(globalObj->multi_velocityType+ knobChange, 0 , 14); }
          break;
        case STATE_CV2_SPEED:
          if(!globalObj->multi_cv2speed_switch){  globalObj->multi_cv2speed_switch = true;
          } else { globalObj->multi_cv2speed = min_max(globalObj->multi_cv2speed+ knobChange, 0 , 255);; }
          break;
        case STATE_CV2_OFFSET:
          if(!globalObj->multi_cv2offset_switch){  globalObj->multi_cv2offset_switch = true;
          } else { globalObj->multi_cv2offset = min_max(globalObj->multi_cv2offset+ knobChange, -64 , 64); }
          break;

      }

      for(int i=0; i< MAX_STEPS_PER_SEQUENCE; i++){
        if(globalObj->multiSelection[i]){ // if step is selected in multi select

          switch(stepMode){
            case STATE_PITCH0:
              sequenceArray[selectedChannel].setStepPitch(i, min_max_cycle(globalObj->multi_pitch, 0,120), 0);

              Serial.println("Changing pitch - knobChange: : " + String(knobChange) + "\tmultiPitch: " + String(globalObj->multi_pitch));

              break;
            case STATE_CHORD:
            //  setStepAbsolute(selectedChannel, i, globalObj->multi_arpInterval);
              sequenceArray[selectedChannel].stepData[i].chord = globalObj->multi_arpInterval;
              sequenceArray[selectedChannel].setStepPitch(i, chordArray[globalObj->multi_arpInterval][0],1);
              sequenceArray[selectedChannel].setStepPitch(i, chordArray[globalObj->multi_arpInterval][1],2);
              sequenceArray[selectedChannel].setStepPitch(i, chordArray[globalObj->multi_arpInterval][2],3);
              break;

            case STATE_GATETYPE:
             sequenceArray[selectedChannel].stepData[i].gateType =  globalObj->multi_gateType;
           //sequenceArray[selectedChannel].stepData[i].gateType =  min_max(globalObj->multi_gateType, 0, 3);
             Serial.println("Changing gatetype - knobChange: : " + String(knobChange) + "\tglobalObj->multi_gateType: " + String(globalObj->multi_gateType) + "\tquant vale: " + String(globalObj->multi_gateType) );

              break;
            case STATE_GATELENGTH:
                if ( globalObj->multi_gateLength == 0  ) {
                  sequenceArray[selectedChannel].stepData[i].gateType = GATETYPE_REST;
                } else if(globalObj->multi_gateLength > 0) {
                  if (sequenceArray[selectedChannel].stepData[i].gateType == GATETYPE_REST){
                    sequenceArray[selectedChannel].stepData[i].gateType = GATETYPE_STEP;
                  }
                }

                sequenceArray[selectedChannel].stepData[i].gateLength =  min_max(globalObj->multi_gateLength, 0, 255);
              break;
            case STATE_ARPTYPE:
              sequenceArray[selectedChannel].stepData[i].arpType = min_max(globalObj->multi_arpType, 0, 5);

              break;
            case STATE_ARPOCTAVE:
              sequenceArray[selectedChannel].stepData[i].arpOctave=  min_max(globalObj->multi_arpOctave, 1, 5);
            break;
            case STATE_ARPSPEEDNUM:
              sequenceArray[selectedChannel].stepData[i].arpSpdNum = min_max(globalObj->multi_arpSpdNum, 1, 16) ;
              break;
            case STATE_ARPSPEEDDEN:
              sequenceArray[selectedChannel].stepData[i].arpSpdDen = min_max(globalObj->multi_arpSpdDen, 1, 64);
              break;
            case STATE_GLIDE:
              sequenceArray[selectedChannel].stepData[i].glide =  min_max(globalObj->multi_glide, 0, 128);
              break;
            case STATE_CV2_LEVEL:
              sequenceArray[selectedChannel].stepData[i].velocity =  positive_modulo(globalObj->multi_velocity, 127);
              break;
            case STATE_CV2_TYPE:
              sequenceArray[selectedChannel].stepData[i].velocityType = min_max(globalObj->multi_velocityType, 0, 13);
              break;
            case STATE_CV2_SPEED:
              sequenceArray[selectedChannel].stepData[i].cv2speed = min_max(globalObj->multi_cv2speed, 1, 255);
              break;
            case STATE_CV2_OFFSET:
              sequenceArray[selectedChannel].stepData[i].cv2offset = min_max(globalObj->multi_cv2offset, -64, 64);
            break;
          }
        }
      }
    }
  }

};

void InputModule::loop(uint16_t frequency){
  if (inputTimer > frequency){
    inputTimer = 0;
    knobPrevious = knobRead;
    knobRead = 1 * knob.read()/4  ;
    //knobRead = -1 * knob.read()/2  ;
    knobChange = knobRead - knobPrevious;
    midplaneGPIO->update();

     if(globalObj->dataInputStyle){
      if(backplaneGPIO->fell(SW_ENCODER_BACKPLANE)){
        globalObj->parameterSelect = !globalObj->parameterSelect;
      }
    } else {
      if(backplaneGPIO->pressed(SW_ENCODER_BACKPLANE) || midplaneGPIO->pressed(SW_SHIFT)){
        globalObj->parameterSelect = true;
      } else {
        globalObj->parameterSelect = false;
      }
    }

    // if(backplaneGPIO->pressed(SW_ENCODER_BACKPLANE) || midplaneGPIO->pressed(SW_SHIFT) ){ //encoder knob
    //   globalObj->parameterSelect = true;
    // } else {
    //   globalObj->parameterSelect = false;
    // }

    if (midplaneGPIO->fell(SW_SHIFT)){
      //tap tempo
      if (globalObj->tapTempoCount == 0){
        globalObj->tapTempoMasterClkCounter = 0;  // if its the first tap, reset the tempo counter
        for(int i = 0; i<4; i++){
          globalObj->tapTempoClockValues[i] = 0;
        }
      } else {
        globalObj->tapTempoClockValues[(globalObj->tapTempoCount-1)%4] = globalObj->tapTempoMasterClkCounter;
        globalObj->tapTempoMasterClkCounter = 0;
      };
      globalObj->tapTempoCount++;

      if(globalObj->tapTempoCount > 4){
        uint32_t tempTempo = 0;
        for(int i = 0; i<4; i++){
          tempTempo += globalObj->tapTempoClockValues[i] * kMasterClockInterval;
        }

        globalObj->tempoX100 = 6000000000/(tempTempo/4);
        // clockMaster->changeTempo(globalObj->tempoX100);
        Serial.println("Setting tap tempo to: " + String(globalObj->tempoX100) + "\ttempTempo: " + String(tempTempo));
      }
    } else {
      if(globalObj->tapTempoCount){
        if (globalObj->tapTempoMasterClkCounter > 1000000/kMasterClockInterval){
          Serial.println("Resetting tap tempo : prev count: " + String(globalObj->tapTempoCount));
          globalObj->tapTempoCount = 0;
        }
      }
    }

    if (midplaneGPIO->pressed(SW_SHIFT) && midplaneGPIO->pressed(SW_PLAY) && midplaneGPIO->pressed(SW_MENU) ){
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
          if(globalObj->parameterSelect){
            changeState(min_max_cycle(stepMode+knobChange, STATE_FIRSTSTEP , STATE_ARPINTMOD ));
          }
          break;
      }
    }
    //we always want the alt (non matrix) buttons to behave the same way

    // now to handle the rest of the buttons.
    bool didAltButtonsFire = altButtonHandler();

    if (didAltButtonsFire == false){
      switch (currentMenu) {
        case PITCH_GATE_MENU:
        case ARPEGGIO_MENU:
        case VELOCITY_MENU:
          if(globalObj->multiSelectSwitch){
            multiSelectInputHandler();
          } else{
            channelPitchModeInputHandler();
          }
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

        case SYSEX_MENU:
        case GLOBAL_MENU_1:
        case GLOBAL_MENU_2:
          globalMenuHandler();
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

        case MENU_RANDOM:
          randomShortcutHandler();
        break;
      }
    } else {
    //  Serial.println("AltButton: " + String(didAltButtonsFire));
    }
  }
}

void InputModule::changeState(uint8_t targetState){
  //Serial.println("change state: " + String(targetState));

  if(stepMode != targetState){
    if (targetState == STATE_TEST_MIDI){
      midiTestActive = true;

      Serial.println("Beginning Midi Test");

    }
  }

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
    case STATE_CV2_LEVEL:
    case STATE_CV2_TYPE:
    case STATE_CV2_SPEED:
    case STATE_CV2_OFFSET:
      currentMenu = VELOCITY_MENU;
      break;
    case STATE_FIRSTSTEP:
    case STATE_STEPCOUNT:
    case STATE_BEATCOUNT:
    case STATE_PLAYMODE:
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
    case STATE_SKIPSTEPCOUNT:
    case STATE_SKIPSTEP:
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
    case STATE_PG_BTN_SWITCH:
    case STATE_DATA_KNOB_SWITCH:
      currentMenu = GLOBAL_MENU_1;
      break;
    case STATE_CH1_VOLT_RANGE:
    case STATE_CH2_VOLT_RANGE:
    case STATE_CH3_VOLT_RANGE:
    case STATE_CH4_VOLT_RANGE:
      currentMenu = GLOBAL_MENU_2;
      break;
    case STATE_SYSEX_EXPORT:
    case STATE_SYSEX_IMPORT:
      currentMenu = SYSEX_MENU;
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
    case STATE_TEST_MIDI:
    case STATE_TEST_GATES:
    case STATE_TEST_RHEOSTAT:
      currentMenu = CALIBRATION_MENU;
      break;
    case STATE_CALIBRATION_SAVE_MODAL:
      currentMenu = MENU_MODAL;
      break;
    case STATE_MULTISELECT:
    //  currentMenu = MENU_MULTISELECT;
      break;
    case STATE_SHORTCUT_RANDOM_PARAM:
    case STATE_SHORTCUT_RANDOM_LOW:
    case STATE_SHORTCUT_RANDOM_SPAN:
      currentMenu = MENU_RANDOM;
    break;
    default:
      Serial.println("This state has no menu selection! " + String(targetState));
    break;
  }

  if(currentMenu !=  GLOBAL_MENU_1 && currentMenu != GLOBAL_MENU_2 && currentMenu != SYSEX_MENU && currentMenu != TEMPO_MENU && (previousMenu == TEMPO_MENU || previousMenu == GLOBAL_MENU_1 || previousMenu == GLOBAL_MENU_2 || previousMenu == SYSEX_MENU) ){
    saveFile->saveGlobalData();
    Serial.println("Saved GlobalData");
  }

  previousMenu = currentMenu;
}

// STATE VARIABLE INPUT HANDLERS

void InputModule::patternSelectHandler(){

  for (int i=0; i < 16; i++){
    if (midplaneGPIO->rose(i)){
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
    if(globalObj->parameterSelect){
      changeState(min_max_cycle(stepMode+knobChange, STATE_TEMPO , STATE_RESETINPUT ));
    } else {
      switch (stepMode){
        case STATE_TEMPO:
        //  if (globalObj->tempoX100 > 100200) {
        //    globalObj->tempoX100 = 100200;
        //  }
          globalObj->tempoX100 = min_max_cycle(globalObj->tempoX100 + knobChange*100, 100, 100000 );
        //  if(globalObj->tempoX100 == 0){
        //    globalObj->tempoX100 = 100;
        //  }
          // clockMaster->changeTempo(globalObj->tempoX100);
        break;

        case STATE_EXTCLOCK:
          globalObj->clockMode = positive_modulo(globalObj->clockMode + knobChange, 8);
          if(globalObj->clockMode == EXTERNAL_CLOCK_BIDIRECTIONAL_INPUT){
            Serial.println("Setting clock port to input");
            globalObj->setClockPortDirection(CLOCK_PORT_INPUT);
          } else {
            globalObj->setClockPortDirection(CLOCK_PORT_OUTPUT);
          }
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
      if(stepMode == STATE_QUANTIZEMODE){
        // put in quantize mode custom button input code here
        sequenceArray[selectedChannel].quantizeMode ^= (1<<i);
      } else {
        changeState(STATE_PITCH0);
        selectedStep = sequenceArray[selectedChannel].getActivePage()*16 + i;
        notePage =   sequenceArray[selectedChannel].getActivePage();
      }
    }
  }
  if(knobChange){
    if ( globalObj->parameterSelect ) {// Encoder Switch
    //  changeState(min_max_cycle(stepMode + knobChange,  STATE_STEPCOUNT,  STATE_QUANTIZEMODE));
    } else {


      switch(stepMode){
        case STATE_FIRSTSTEP:
          sequenceArray[selectedChannel].firstStep = min_max(sequenceArray[selectedChannel].firstStep + knobChange, 0, 63);
          //sequenceArray[selectedChannel].firstStepChanged == TRUE;
          break;
        case STATE_STEPCOUNT:
          //sequenceArray[selectedChannel].stepCount = min_max(sequenceArray[selectedChannel].stepCount + knobChange, 1, 64);
          sequenceArray[selectedChannel].setStepCount(min_max(sequenceArray[selectedChannel].stepCount + knobChange, 1, 64));
          // if (sequenceArray[selectedChannel].stepCount == 0) {
          //   sequenceArray[selectedChannel].stepCount = 64;
          // }
          break;
        case STATE_BEATCOUNT:
           newBeatDiv = min_max(sequenceArray[selectedChannel].clockDivision + knobChange, -16, 16);
           sequenceArray[selectedChannel].updateClockDivision(newBeatDiv);
            //Serial.println("newBeatDiv: " + String(newBeatDiv));
            // Serial.println("clockDiv: " + String(sequenceArray[selectedChannel].clockDivision));
          break;

        case STATE_QUANTIZEKEY:
          sequenceArray[selectedChannel].quantizeKey = positive_modulo(sequenceArray[selectedChannel].quantizeKey + knobChange, 12);
          break;
        case STATE_QUANTIZEMODE:
          sequenceArray[selectedChannel].quantizeModeIndex = positive_modulo(sequenceArray[selectedChannel].quantizeModeIndex + knobChange, 19);
          switch(sequenceArray[selectedChannel].quantizeModeIndex){
            case 0: sequenceArray[selectedChannel].quantizeMode = SEMITONE_SCALE_12;    break;
            case 1: sequenceArray[selectedChannel].quantizeMode = MAJOR_SCALE_12;       break;
            case 2: sequenceArray[selectedChannel].quantizeMode = MINOR_SCALE_12;       break;
            case 3: sequenceArray[selectedChannel].quantizeMode = MAJORMINOR_SCALE_12;  break;
            case 4: sequenceArray[selectedChannel].quantizeMode = BLUESMAJOR_SCALE_12;  break;
            case 5: sequenceArray[selectedChannel].quantizeMode = BLUESMINOR_SCALE_12;  break;
            case 6: sequenceArray[selectedChannel].quantizeMode = PENT_MAJOR_SCALE_12;  break;
            case 7: sequenceArray[selectedChannel].quantizeMode = PENT_MINOR_SCALE_12;  break;
            case 8: sequenceArray[selectedChannel].quantizeMode = FOLK_SCALE_12;        break;
            case 9: sequenceArray[selectedChannel].quantizeMode = AEOLIAN_SCALE_12;     break;
            case 10: sequenceArray[selectedChannel].quantizeMode = DORIAN_SCALE_12;      break;
            case 11: sequenceArray[selectedChannel].quantizeMode = MIXOLYDIAN_SCALE_12;  break;
            case 12: sequenceArray[selectedChannel].quantizeMode = PHRYGIAN_SCALE_12;    break;
            case 13: sequenceArray[selectedChannel].quantizeMode = LYDIAN_SCALE_12;      break;
            case 14: sequenceArray[selectedChannel].quantizeMode = LOCRIAN_SCALE_12;     break;
            case 15: sequenceArray[selectedChannel].quantizeMode = GAMELAN_SCALE_12;     break;
            case 16: sequenceArray[selectedChannel].quantizeMode = JAPANESE_SCALE_12;    break;
            case 17: sequenceArray[selectedChannel].quantizeMode = GYPSY_SCALE_12;       break;
            case 18: sequenceArray[selectedChannel].quantizeMode = ARABIAN_SCALE_12;     break;
            case 19: sequenceArray[selectedChannel].quantizeMode = FLAMENCO_SCALE_12;    break;
            case 20: sequenceArray[selectedChannel].quantizeMode = WHOLETONE_SCALE_12;   break;
          }
          break;
        case STATE_QUANTIZESCALE:
          sequenceArray[selectedChannel].quantizeScale = positive_modulo(sequenceArray[selectedChannel].quantizeScale + knobChange, 3);
          break;
        case STATE_PLAYMODE:
          sequenceArray[selectedChannel].playMode = positive_modulo(sequenceArray[selectedChannel].playMode + knobChange, 4);
          break;
        case STATE_RESETINPUT:
          sequenceArray[selectedChannel].gpio_reset = min_max_skip(sequenceArray[selectedChannel].gpio_reset, knobChange, -9, 8, selectedChannel+5);

          break;

// mod menu 1
        case STATE_GLIDEMOD:
          sequenceArray[selectedChannel].cv_glidemod = min_max(sequenceArray[selectedChannel].cv_glidemod + knobChange, 0, 12);
        break;

        case STATE_GATEMOD:
          sequenceArray[selectedChannel].cv_gatemod = min_max(sequenceArray[selectedChannel].cv_gatemod + knobChange, 0, 12);
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
          sequenceArray[selectedChannel].cv_pitchmod = min_max(sequenceArray[selectedChannel].cv_pitchmod + knobChange, 0, 12);
        break;
// mod menu 2
        case STATE_ARPTYPEMOD:
          sequenceArray[selectedChannel].cv_arptypemod = min_max(sequenceArray[selectedChannel].cv_arptypemod + knobChange, 0, 12);
        break;

        case STATE_ARPSPDMOD:
          sequenceArray[selectedChannel].cv_arpspdmod = min_max(sequenceArray[selectedChannel].cv_arpspdmod + knobChange, 0, 12);
        break;

        case STATE_ARPOCTMOD:
          sequenceArray[selectedChannel].cv_arpoctmod = min_max(sequenceArray[selectedChannel].cv_arpoctmod + knobChange, 0, 12);
        break;

        case STATE_ARPINTMOD:
          sequenceArray[selectedChannel].cv_arpintmod = min_max(sequenceArray[selectedChannel].cv_arpintmod + knobChange,0, 12);
        break;

        case STATE_SKIPSTEP:
          sequenceArray[selectedChannel].gpio_skipstep = min_max_skip(sequenceArray[selectedChannel].gpio_skipstep, knobChange, -9, 8, selectedChannel+5);
        break;

        case STATE_SKIPSTEPCOUNT:
          sequenceArray[selectedChannel].skipStepCount = min_max(sequenceArray[selectedChannel].skipStepCount + knobChange, 0, 63);
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
    if ( globalObj->parameterSelect ) {
      changeState(min_max_cycle(stepMode+knobChange, STATE_STEPCOUNT , STATE_SKIPSTEP));
    } else {
      switch(stepMode){
      }
    }
  }
}



void InputModule::globalMenuHandler(){
  for (int i=0; i < 16; i++){
    if (midplaneGPIO->fell(i)){
      changeState(STATE_PITCH0);
      selectedStep = getNote(i);
    }
  }

  if(knobChange){
    if ( globalObj->parameterSelect ) {// Encoder Switch
      changeState(min_max_cycle(stepMode + knobChange,  STATE_PG_BTN_SWITCH,  STATE_SYSEX_IMPORT));
    } else {
      switch(stepMode){

        case STATE_PG_BTN_SWITCH:
        globalObj->pageButtonStyle = min_max( globalObj->pageButtonStyle + knobChange, 0, 1);

            break;
        case STATE_DATA_KNOB_SWITCH:
        globalObj->dataInputStyle = min_max( globalObj->dataInputStyle + knobChange, 0, 1);

            break;
        case STATE_CH1_VOLT_RANGE:
        globalObj->outputNegOffset[0] = min_max( globalObj->outputNegOffset[0] + knobChange, 0, 5);

            break;
        case STATE_CH2_VOLT_RANGE:
        globalObj->outputNegOffset[1] = min_max( globalObj->outputNegOffset[1] + knobChange, 0, 5);

            break;
        case STATE_CH3_VOLT_RANGE:
        globalObj->outputNegOffset[2] = min_max( globalObj->outputNegOffset[2] + knobChange, 0, 5);

            break;
        case STATE_CH4_VOLT_RANGE:
        globalObj->outputNegOffset[3] = min_max( globalObj->outputNegOffset[3] + knobChange, 0, 5);

            break;
        }
      }
    }

}


void InputModule::channelButtonShiftHandler(uint8_t channel){
  if (selectedChannel != channel){
    selectedChannel = channel;
  }
  changeState(STATE_TUNER);
};

void InputModule::channelButtonShiftMenuHandler(uint8_t channel){
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

  // resetKnobValues();
  // if (selectedChannel != channel){
  //   selectedChannel = channel;
  //   return;
  // }
//  changeState(min_max_cycle(++stepMode,STATE_PITCH0, STATE_CV2_OFFSET));

}


bool InputModule::altButtonHandler(){

  chPressedSelector = 0;
  if (midplaneGPIO->pressed(SW_CH0)){
    chPressedSelector = chPressedSelector | 0b0001;
  } else {
    chPressedSelector = chPressedSelector & ~0b0001;
  }
  if (midplaneGPIO->pressed(SW_CH1)){
    chPressedSelector = chPressedSelector | 0b0010;
  } else {
    chPressedSelector = chPressedSelector & ~0b0010;
  }
  if (midplaneGPIO->pressed(SW_CH2)){
    chPressedSelector = chPressedSelector | 0b0100;
  } else {
    chPressedSelector = chPressedSelector & ~0b0100;
  }
  if (midplaneGPIO->pressed(SW_CH3)){
    chPressedSelector = chPressedSelector | 0b1000;
  } else {
    chPressedSelector = chPressedSelector & ~0b1000;
  }


// shortcut button loop
uint8_t chanSwIndex;
  for(int chan=0; chan<4; chan++){
    switch(chan){
      case 0:
        chanSwIndex = SW_CH0;
        break;
      case 1:
        chanSwIndex = SW_CH1;
        break;
      case 2:
        chanSwIndex = SW_CH2;
        break;
      case 3:
        chanSwIndex = SW_CH3;
        break;
    }
    if(midplaneGPIO->pressed(chanSwIndex)){

      for (int i=0; i <16; i++){
        if (midplaneGPIO->fell(i) ){
          globalObj->muteChannelSelect[0] = chPressedSelector & 0b0001;
          globalObj->muteChannelSelect[1] = chPressedSelector & 0b0010;
          globalObj->muteChannelSelect[2] = chPressedSelector & 0b0100;
          globalObj->muteChannelSelect[3] = chPressedSelector & 0b1000;
          channelButtonOperationInProgress = true;
          switch (i){
            case SW_00:
                if( sequenceArray[chan].toggleMute(0) ){
                  display->displayModal(750, MODAL_MUTE_GATE);
                } else {
                  display->displayModal(750, MODAL_UNMUTE_GATE);
                }
            break;
            case SW_01:
                if( sequenceArray[chan].toggleMute(1) ){
                  display->displayModal(750, MODAL_MUTE_CVA);
                } else {
                  display->displayModal(750, MODAL_UNMUTE_CVA);
                }
           break;
           case SW_02:
                if( sequenceArray[chan].toggleMute(2) ){
                  display->displayModal(750, MODAL_MUTE_CVB);
                } else {
                  display->displayModal(750, MODAL_UNMUTE_CVB);
                }
            break;
            case SW_03:
              display->displayModal(300, MODAL_TOBEIMPLEMENTED);
            break;
            case SW_04:
              display->displayModal(300, MODAL_TOBEIMPLEMENTED);
            break;
            case SW_05:
              sequenceArray[chan].clockReset(true);
              display->displayModal(300, MODAL_SHORTCUT_RESET);
            break;
            case SW_06: // shortcut reverse
              switch(sequenceArray[chan].playMode){
                case PLAY_PENDULUM:
                    sequenceArray[chan].pendulumSwitch = !sequenceArray[chan].pendulumSwitch;
                break;
                case  PLAY_FORWARD:
                  sequenceArray[chan].playMode = PLAY_REVERSE;
                break;
                case  PLAY_REVERSE:
                  sequenceArray[chan].playMode = PLAY_FORWARD;
                break;
              }
              display->displayModal(300, MODAL_SHORTCUT_REVERSE);
            break;
            case SW_07:
              display->displayModal(300, MODAL_TOBEIMPLEMENTED);
            break;
            case SW_08:// shortcut_random
              switch(currentMenu){
                case MENU_RANDOM:
                  sequenceArray[chan].randomize(globalObj->randomizeParamSelect, globalObj->randomizeLow, globalObj->randomizeSpan);
                  changeState(STATE_PITCH0);
                  display->displayModal(300, MODAL_RANDOM_PITCH_GATE);
                break;
                default:
                changeState(STATE_SHORTCUT_RANDOM_PARAM);
              }
            break;
            case SW_09:
                display->displayModal(300, MODAL_TOBEIMPLEMENTED);
            break;
            case SW_10:
                display->displayModal(300, MODAL_TOBEIMPLEMENTED);
            break;
            case SW_11:
                display->displayModal(300, MODAL_TOBEIMPLEMENTED);
            break;
            case SW_12:
                display->displayModal(300, MODAL_TOBEIMPLEMENTED);
            break;
            case SW_13:
                display->displayModal(300, MODAL_TOBEIMPLEMENTED);
            break;
            case SW_14:
                display->displayModal(300, MODAL_TOBEIMPLEMENTED);
            break;
            case SW_15:
                display->displayModal(300, MODAL_TOBEIMPLEMENTED);
            break;
            }
            return 1;
        }
      }
    }
  }

// non matrix button loop
  for (uint8_t i=16; i <28; i++){
    if (midplaneGPIO->fell(i) ){
      switch (i){
        // left row bottom up
        case SW_CH0:
        case SW_CH1:
        case SW_CH2:
        case SW_CH3:

         switch(currentMenu){
           case PATTERN_SELECT:
           case SAVE_MENU:
            globalObj->multiSelectSwitch = false;
            channelButtonChannelSelectorHandler(getChannelFromSw(i));
            break;
           case CALIBRATION_MENU:
            if (midplaneGPIO->pressed(SW_SHIFT)){
              calibrationSaveHandler();
            } else {
              calibrationMenuHandler();
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
                 channelButtonShiftMenuHandler(getChannelFromSw(i));
               } else {
                 channelButtonShiftHandler(getChannelFromSw(i));
               }
             } else if (selectedChannel == getChannelFromSw(i) ) {
               channelButtonOperationInProgress = true;
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
                    channelButtonOperationInProgress = false;
              //    changeState(STATE_FIRSTSTEP);
                 break;
               }
             } else {
               if (selectedChannel != getChannelFromSw(i)){
                 selectedChannel = getChannelFromSw(i);
                 channelButtonOperationInProgress = true;
               }
             }
         }
        break;
        case SW_PLAY:
          for(int sw=0; sw<16; sw++){
            if(midplaneGPIO->pressed(sw)){
              globalObj->stepCopyIndex = getNote(sw);
              display->displayModal(750, MODAL_COPY_STEP);
              goto PLAYEND;
            }
          }

          for(int ch=0; ch<4; ch++){
            if(midplaneGPIO->pressed(getChannelButtonSw(ch))){
              globalObj->chCopyIndex = ch;
              display->displayModal(750, MODAL_COPY_CHANNEL);
              goto PLAYEND;
            }
          }
          playing = !playing;
          PLAYEND:
        break;

        case SW_REC:
          if(globalObj->stepCopyIndex < 64){
            for(int sw=0; sw<16; sw++){
              if(midplaneGPIO->pressed(sw)){
                display->displayModal(750, MODAL_PASTE_STEP);
                sequenceArray[selectedChannel].stepData[getNote(sw)] = sequenceArray[selectedChannel].stepData[globalObj->stepCopyIndex];
              }
            }
          }
          if(globalObj->chCopyIndex < 4){
            for(int ch=0; ch<4; ch++){
              if(midplaneGPIO->pressed(getChannelButtonSw(ch))){
                sequenceArray[ch] = sequenceArray[globalObj->chCopyIndex];
                sequenceArray[ch].channel = ch;
                display->displayModal(750, MODAL_PASTE_CHANNEL);
              }
            }
          }

        break;

      case SW_STOP:
        for(int sw=0; sw<16; sw++){
          if(midplaneGPIO->pressed(sw)){
            sequenceArray[selectedChannel].initializeStep(sw);
            display->displayModal(750, MODAL_CLEAR_STEP);
            goto STOPEND;
          }
        }

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

          playing = false;
          for(int s = 0; s < SEQUENCECOUNT; s++){
            sequenceArray[s].clockReset(true);
          }

        }
        STOPEND:
        break;

        case SW_PATTERN:
          globalObj->multiSelectSwitch = false;

          if(currentMenu == SYSEX_MENU){  //shift-pattern shortcut is used to export and import sysex data
            if (midplaneGPIO->pressed(SW_SHIFT)){
              switch (stepMode){
                case STATE_SYSEX_EXPORT:
                  playing = 0;
                  // globalObj->importExportDisplaySwitch = 1;
                  display->displayModal(100, MODAL_EXPORTING);
                  display->displayLoop(0);
                  saveFile->exportSysexData();
                  display->displayModal(1000, MODAL_EXPORTCOMPLETE);
                  // globalObj->importExportDisplaySwitch = 2;
                break;
                case STATE_SYSEX_IMPORT:
                  playing = 0;
                  display->displayModal(1000, MODAL_IMPORTING);

//                  saveFile->importSysexData();

                break;
              }
            }
            break;
          }

          if (midplaneGPIO->pressed(SW_SHIFT)){
            for(int i=0; i<SEQUENCECOUNT; i++){
              //saveDestination[i] = sequenceArray[i].pattern;
              saveDestination[i] = currentPattern;
            }
            changeState(STATE_SAVE);
            break;
          }

          if(midplaneGPIO->pressed(SW_CH0) || midplaneGPIO->pressed(SW_CH1) || midplaneGPIO->pressed(SW_CH2) || midplaneGPIO->pressed(SW_CH3)) break;

          switch(currentMenu){
            case PATTERN_SELECT:
              changeState(STATE_PITCH0);
            break;
            case SAVE_MENU:
              saveFile->savePattern(patternChannelSelector, saveDestination);
              midplaneGPIO->clearBuffers();

              changeState(STATE_PITCH0);
              display->displayModal(750, MODAL_SAVE);

            break;
            default:
              globalObj->multiSelectSwitch = 0;
              midplaneGPIO->clearBuffers();
              changeState(STATE_PATTERNSELECT);
            break;
          }
        break;

        case SW_MENU: //switch M3 toggles the sequence menu
         if (midplaneGPIO->pressed(SW_SHIFT)){
           if(currentMenu == GLOBAL_MENU_1){
             changeState(STATE_CH1_VOLT_RANGE);
           } else if(currentMenu == GLOBAL_MENU_2)  {
             changeState(STATE_PITCH0);
           } else {
             changeState(STATE_PG_BTN_SWITCH);
           }
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
            if(globalObj->multiSelectSwitch){
              globalObj->multiSelectSwitch = false;
              changeState(STATE_PITCH0);
            } else {
              globalObj->multiSelectSwitch = true;
              changeState(STATE_PITCH0);
            }
          } else {
            if(!globalObj->pageButtonStyle){
              notePage = positive_modulo(notePage + 1, 4);
            } else {
              notePage = positive_modulo(notePage - 1, 4);
            }
          }
        break;

        case SW_PGUP:
          if (midplaneGPIO->pressed(SW_SHIFT)){
            //changeState(GLOBAL_MENU_1);
          } else {
            if(globalObj->pageButtonStyle){
              notePage = positive_modulo(notePage + 1, 4);
            } else {
              notePage = positive_modulo(notePage - 1, 4);
            }
          }

        break;

        // right two, bottom up


        }
      }
      if (midplaneGPIO->rose(i) ){
        //this section controls the behavior of the channel buttons on press
        if(currentMenu == PATTERN_SELECT || currentMenu == SAVE_MENU) break;
        switch (i){
          // left row bottom up
          case SW_CH0:
          case SW_CH1:
          case SW_CH2:
          case SW_CH3:
            if(channelButtonOperationInProgress){
                channelButtonOperationInProgress = false;
            } else{
              changeState(STATE_FIRSTSTEP);
            }
          break;
         }
      }
    }
    //Serial.println("Return at the end of alt");

    return false;
  }


  void InputModule::channelPitchModeInputHandler(){
  // selectedStep == getNote(i) means that the user pressed the button that is selected.
  bool skipStepDataChangeSwitch = 0;
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


      } else if (midplaneGPIO->rose(i)){
        if (!playing){
          sequenceArray[selectedChannel].stoppedTrig(getNote(i), false, false);
        }

        if(buttonMode == BUTTON_MODE_XOX ){
          // tap to turn a step on, double tap to turn step off
          // skip turning step on or of it was just copied
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

      }
    }
    if (knobChange){

      if( (currentMenu == PITCH_GATE_MENU) || (currentMenu == ARPEGGIO_MENU) || (currentMenu == VELOCITY_MENU) ){
        for (int i=0; i < 16; i++){
          if (midplaneGPIO->pressed(i)){
            sequenceArray[selectedChannel].setStepPitch(getNote(i), min_max(sequenceArray[selectedChannel].getStepPitch(getNote(i), 0) + knobChange, 0,120), 0);
            skipStepDataChangeSwitch = true;
          }
        }
        if(skipStepDataChangeSwitch){
            goto SKIPSTEPDATACHANGE;
        }
      }

      if ( globalObj->parameterSelect ) {// Encoder Switch
        changeState(min_max_cycle(stepMode + knobChange,  STATE_PITCH0,  STATE_CV2_OFFSET));
      } else {
        changeStepData(selectedChannel, selectedStep, knobChange);
      }
       SKIPSTEPDATACHANGE: ;

    }


};

void InputModule::changeStepData(uint8_t channel, uint8_t stepNum, int change){
    //knobPrev = knobRead;
    uint8_t chrd;

//      if (midplaneGPIO->pressed(selectedStep%16) ){

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
          sequenceArray[channel].setStepPitch(stepNum, min_max(sequenceArray[channel].getStepPitch(stepNum, 0) + change, 0,120), 0);
          if(!playing){
            sequenceArray[channel].stoppedTrig(stepNum, true, false);
          }

        break;

        case STATE_PITCH1:
          sequenceArray[channel].setStepPitch(stepNum, positive_modulo(sequenceArray[channel].getStepPitch(stepNum, 1) + change, 1), 72);
        break;

        case STATE_PITCH2:
          sequenceArray[channel].setStepPitch(stepNum, positive_modulo(sequenceArray[channel].getStepPitch(stepNum, 2) + change, 2), 72);
        break;

        case STATE_PITCH3:
          sequenceArray[channel].setStepPitch(stepNum, positive_modulo(sequenceArray[channel].getStepPitch(stepNum, 3) + change, 3), 72);
        break;

        case STATE_CHORD:
          chrd = positive_modulo(sequenceArray[channel].stepData[stepNum].chord + change, 27);
          sequenceArray[channel].stepData[stepNum].chord = chrd;
          sequenceArray[channel].setStepPitch(stepNum, chordArray[chrd][0],1);
          sequenceArray[channel].setStepPitch(stepNum, chordArray[chrd][1],2);
          sequenceArray[channel].setStepPitch(stepNum, chordArray[chrd][2],3);
        break;

        case STATE_GATELENGTH:

    // change the gate type
            if ((sequenceArray[channel].stepData[stepNum].gateLength == 0) && (change < 0)  ) {
              sequenceArray[channel].stepData[stepNum].gateType = GATETYPE_REST;
            } else if(change > 0) {
              if (sequenceArray[channel].stepData[stepNum].gateType == GATETYPE_REST){
                sequenceArray[channel].stepData[stepNum].gateType = GATETYPE_STEP;
              }
            }
            if (sequenceArray[channel].stepData[stepNum].gateType > 0){
              sequenceArray[channel].stepData[stepNum].gateLength =  min_max(sequenceArray[channel].stepData[stepNum].gateLength + change, 0, 255);
            //  Serial.println("Setting Gatelength: " + String(sequenceArray[channel].stepData[stepNum].gateLength) + " change: " + String(change));

            }
        break;



        case STATE_GATETYPE:
          sequenceArray[channel].stepData[stepNum].gateType =  min_max(sequenceArray[channel].stepData[stepNum].gateType + change, 0, 3);
          break;

        case STATE_GLIDE:
          sequenceArray[channel].stepData[stepNum].glide =  min_max(sequenceArray[channel].stepData[stepNum].glide + change, 0, 128);
          break;

        case STATE_ARPTYPE:
          sequenceArray[channel].stepData[stepNum].arpType = min_max(sequenceArray[channel].stepData[stepNum].arpType + change, 0, 5);
        break;

        case STATE_ARPSPEEDNUM:
          sequenceArray[channel].stepData[stepNum].arpSpdNum = min_max(sequenceArray[channel].stepData[stepNum].arpSpdNum + change, 1, 16) ;
        break;
        case STATE_ARPSPEEDDEN:
          sequenceArray[channel].stepData[stepNum].arpSpdDen = min_max(sequenceArray[channel].stepData[stepNum].arpSpdDen + change, 1, 32);
        break;

        case STATE_ARPOCTAVE:
          sequenceArray[channel].stepData[stepNum].arpOctave=  min_max(sequenceArray[channel].stepData[stepNum].arpOctave + change, 1, 5);
        break;
        case STATE_CV2_LEVEL:
          sequenceArray[channel].stepData[stepNum].velocity =  min_max(sequenceArray[channel].stepData[stepNum].velocity + change, -127, 127);
          goto CV2UPDATE;
        case STATE_CV2_TYPE:
          sequenceArray[channel].stepData[stepNum].velocityType = min_max(sequenceArray[channel].stepData[stepNum].velocityType + change, 0, 14);
          goto CV2UPDATE;
        case STATE_CV2_SPEED:
          sequenceArray[channel].stepData[stepNum].cv2speed = min_max(sequenceArray[channel].stepData[stepNum].cv2speed + change, 1, 255);
          goto CV2UPDATE;
        case STATE_CV2_OFFSET:
          sequenceArray[channel].stepData[stepNum].cv2offset = min_max(sequenceArray[channel].stepData[stepNum].cv2offset + change, -64, 64);
          CV2UPDATE:
          outputControl->cv2settingsChange(channel,stepNum, sequenceArray[channel].stepData[stepNum].velocity, sequenceArray[channel].stepData[stepNum].velocityType, sequenceArray[channel].stepData[stepNum].cv2speed, sequenceArray[channel].stepData[stepNum].cv2offset);
          break;

      }
}

void InputModule::randomShortcutHandler(){
  if(knobChange){
    if ( globalObj->parameterSelect ) {// Encoder Switch
      changeState(min_max_cycle(stepMode + knobChange,  STATE_SHORTCUT_RANDOM_PARAM,  STATE_SHORTCUT_RANDOM_SPAN));
    } else {
      switch(stepMode){
        case STATE_SHORTCUT_RANDOM_PARAM:
//        globalObj->randomizeParamSelect = min_max(globalObj->randomizeParamSelect + knobChange, 0, 12);
          globalObj->randomizeParamSelect = min_max(globalObj->randomizeParamSelect + knobChange, 0, 2);
        break;
        case STATE_SHORTCUT_RANDOM_LOW:
          globalObj->randomizeLow = min_max(globalObj->randomizeLow + knobChange, 0, 120);
        break;
        case STATE_SHORTCUT_RANDOM_SPAN:
          globalObj->randomizeSpan = min_max(globalObj->randomizeSpan + knobChange, 1, 8);
        break;
      }
    }
  }
  midplaneGPIO->clearBuffers();
}

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
//  uint8_t multiplier = 100;
  playing = 0;

  uint32_t calibHigh;
  uint32_t calibLow;
  elapsedMillis timeoutTimer;
  channelButtonOperationInProgress = true;

  //if (midplaneGPIO->pressed(SW_REC)){
  //  multiplier = 10;
///  }
  switch (stepMode){
    case STATE_CALIB_INPUT0_LOW:
    case STATE_CALIB_INPUT0_HIGH:
    case STATE_CALIB_INPUT0_OFFSET:
    case STATE_CALIB_INPUT1_LOW:
    case STATE_CALIB_INPUT1_HIGH:
    case STATE_CALIB_INPUT1_OFFSET:
    case STATE_CALIB_INPUT2_LOW:
    case STATE_CALIB_INPUT2_HIGH:
    case STATE_CALIB_INPUT2_OFFSET:
    case STATE_CALIB_INPUT3_LOW:
    case STATE_CALIB_INPUT3_HIGH:
    case STATE_CALIB_INPUT3_OFFSET:
      if(midplaneGPIO->pressed(SW_CH0)){
        globalObj->adcCalibrationOffset[0] = (globalObj->cvInputRaw[0] +5 + 9* globalObj->adcCalibrationOffset[0])/10 ;
      }
      if(midplaneGPIO->pressed(SW_CH1)){
        globalObj->adcCalibrationOffset[1] = (globalObj->cvInputRaw[1] +5 + 9* globalObj->adcCalibrationOffset[1])/10 ;
      }
      if(midplaneGPIO->pressed(SW_CH2)){
        globalObj->adcCalibrationOffset[2] = (globalObj->cvInputRaw[2] +5 + 9* globalObj->adcCalibrationOffset[2])/10 ;
      }
      if(midplaneGPIO->pressed(SW_CH3)){
        globalObj->adcCalibrationOffset[3] = (globalObj->cvInputRaw[3] +5 + 9* globalObj->adcCalibrationOffset[3])/10 ;
      }

      if(midplaneGPIO->pressed(SW_00)){
        globalObj->adcCalibrationNeg[0] =(globalObj->adcCalibrationNeg[0]*9 + min_max(globalObj->adcCalibrationOffset[0] + ((globalObj->adcCalibrationOffset[0] -5 - globalObj->cvInputRaw[0])/3)*5, 0,65535 ))/10 ;
        globalObj->adcCalibrationPos[0] =(globalObj->adcCalibrationPos[0]*9 + min_max(globalObj->adcCalibrationOffset[0] - ((globalObj->adcCalibrationOffset[0] -5 - globalObj->cvInputRaw[0])/3)*5, 0,65535 ))/10 ;
      }
      if(midplaneGPIO->pressed(SW_04)){
        globalObj->adcCalibrationNeg[1] =(globalObj->adcCalibrationNeg[1]*9 + min_max(globalObj->adcCalibrationOffset[1] + ((globalObj->adcCalibrationOffset[1] -5 - globalObj->cvInputRaw[1])/3)*5, 0,65535 ))/10 ;
        globalObj->adcCalibrationPos[1] =(globalObj->adcCalibrationPos[1]*9 + min_max(globalObj->adcCalibrationOffset[1] - ((globalObj->adcCalibrationOffset[1] -5 - globalObj->cvInputRaw[1])/3)*5, 0,65535 ))/10 ;
      }
      if(midplaneGPIO->pressed(SW_08)){
        globalObj->adcCalibrationNeg[2] =(globalObj->adcCalibrationNeg[2]*9 + min_max(globalObj->adcCalibrationOffset[2] + ((globalObj->adcCalibrationOffset[2] -5 - globalObj->cvInputRaw[2])/3)*5, 0,65535 ))/10 ;
        globalObj->adcCalibrationPos[2] =(globalObj->adcCalibrationPos[2]*9 + min_max(globalObj->adcCalibrationOffset[2] - ((globalObj->adcCalibrationOffset[2] -5 - globalObj->cvInputRaw[2])/3)*5, 0,65535 ))/10 ;
      }
      if(midplaneGPIO->pressed(SW_12)){
        globalObj->adcCalibrationNeg[3] =(globalObj->adcCalibrationNeg[3]*9 + min_max(globalObj->adcCalibrationOffset[3] + ((globalObj->adcCalibrationOffset[3] -5 - globalObj->cvInputRaw[3])/3)*5, 0,65535 ))/10 ;
        globalObj->adcCalibrationPos[3] =(globalObj->adcCalibrationPos[3]*9 + min_max(globalObj->adcCalibrationOffset[3] - ((globalObj->adcCalibrationOffset[3] -5 - globalObj->cvInputRaw[3])/3)*5, 0,65535 ))/10 ;
        Serial.println("RAW: " + String(globalObj->cvInputRaw[3]) + "\t1v: " + String((globalObj->adcCalibrationOffset[3] - globalObj->cvInputRaw[3])/3) + "\t5v: " + String(((globalObj->adcCalibrationOffset[3] - globalObj->cvInputRaw[0])/3) *5));

      }
      /*

      offset = 0v level - 32767 in an ideal situation
      pos = -5v level - something around 62000
      neg = 5v level - something around 1024

      calibration occurs at 3v which should be around 15000
      (around 6000 per volt)


      adcCalibrationNeg = offset + (offset - 3vVal)*5/3
      adcCalibrationPos = offset - (offset - 3vVal)*5/3

      */
    break;
    case STATE_CALIB_OUTPUT0_LOW:
    case STATE_CALIB_OUTPUT0_HIGH:
    case STATE_CALIB_OUTPUT1_LOW:
    case STATE_CALIB_OUTPUT1_HIGH:
    case STATE_CALIB_OUTPUT2_LOW:
    case STATE_CALIB_OUTPUT2_HIGH:
    case STATE_CALIB_OUTPUT3_LOW:
    case STATE_CALIB_OUTPUT3_HIGH:
    case STATE_CALIB_OUTPUT4_LOW:
    case STATE_CALIB_OUTPUT4_HIGH:
    case STATE_CALIB_OUTPUT5_LOW:
    case STATE_CALIB_OUTPUT5_HIGH:
    case STATE_CALIB_OUTPUT6_LOW:
    case STATE_CALIB_OUTPUT6_HIGH:
    case STATE_CALIB_OUTPUT7_LOW:
    case STATE_CALIB_OUTPUT7_HIGH:
      if(midplaneGPIO->pressed(SW_15)){
        for(int i =0; i<8; i++){
          globalObj->dacCalibrationPos[dacMap[i]] = 65535;
          globalObj->dacCalibrationNeg[dacMap[i]] = 0;
        }
      }
      if(midplaneGPIO->pressed(SW_CH0)){
        Serial.println("Calibrating DAC");
        for(int n = 0; n <8; n++){
          for(int i = 0; i<8; i++){
            // set all DACs low
            outputControl->setDacVoltage(dacMap[i], 0);
          }
          delay(10);
          outputControl->setDacVoltage(dacMap[n], 65535);
          delay(100);
          outputControl->inputRead();
          delay(10);
          Serial.print("dac ch " + String(n) + " dacMap: " + String(dacMap[n]));
          for(int adc = 0; adc < 4; adc++){
            Serial.print("  adc" + String(adc) + "\t: " + String(globalObj->cvInputMapped1024[adc] ) + "\t");
            if(globalObj->cvInputMapped1024[adc] > 512){

              Serial.println("DAC " + String(n) + " is connected to ADC " + String(adc) + "\t with val: " + String(globalObj->cvInputMapped1024[adc]) + "\tcalibrating....");
              // coarse tuning
              calibHigh = 65535;
              timeoutTimer = 0;
              while( globalObj->cvInputMapped1024[adc] > 512 ){
                calibHigh = calibHigh -100;
                outputControl->setDacVoltage(dacMap[n], calibHigh);
                delay(1); outputControl->inputRead(); delay(1);
                if(timeoutTimer > 30000){Serial.println("timeout, ending calibration routine"); break;};
              };
              //fine adjustment
              calibHigh += 150;
              outputControl->setDacVoltage(dacMap[n], calibHigh);
              delay(5); outputControl->inputRead(); delay(5);
              timeoutTimer = 0;
              while( globalObj->cvInputMapped1024[adc] > 512 ){
                calibHigh = calibHigh - 1;
                outputControl->setDacVoltage(dacMap[n], calibHigh);
                delay(5); outputControl->inputRead(); delay(5);
                if(timeoutTimer > 30000){Serial.println("timeout, ending calibration routine"); break;};
              };
              Serial.println("Calibration +2.5v found: " + String(calibHigh) + "\t adc value:  " + String(globalObj->cvInputMapped1024[adc]));
              // coarse tuning

              calibLow = 0;
              outputControl->setDacVoltage(dacMap[n], calibLow);
              delay(1); outputControl->inputRead(); delay(1);
              timeoutTimer = 0;

              while( globalObj->cvInputMapped1024[adc] < -512 ){
                calibLow = calibLow + 100;
                outputControl->setDacVoltage(dacMap[n], calibLow);
                delay(1); outputControl->inputRead(); delay(1);
                if(timeoutTimer > 30000){Serial.println("timeout, ending calibration routine"); break;};
              };
              // fine adjustment
              calibLow -= 150;
              outputControl->setDacVoltage(dacMap[n], calibLow);
              delay(5); outputControl->inputRead(); delay(5);

              while( globalObj->cvInputMapped1024[adc] < -512 ){
                calibLow = calibLow + 1;
                outputControl->setDacVoltage(dacMap[n], calibLow);
                delay(5); outputControl->inputRead(); delay(5);
                if(timeoutTimer > 30000){Serial.println("timeout, ending calibration routine"); break;};
              };

              Serial.println("Calibration -2.5v found: " + String(calibLow) + "\t adc value:  " + String(globalObj->cvInputMapped1024[adc]));
              //calibHigh = calibHigh * 100 / 102; //offset for impedance mismatch
              //calibLow = calibLow * 102 / 100; //offset for impedance mismatch
              globalObj->dacCalibrationPos[dacMap[n]] = (5*calibHigh - 3*calibLow)/2;
              globalObj->dacCalibrationNeg[dacMap[n]] = (5*calibLow - 3*calibHigh)/2;


              //int compensation = (globalObj->dacCalibrationPos[dacMap[n]] - globalObj->dacCalibrationNeg[dacMap[n]] )/115;
              int compensation = (globalObj->dacCalibrationPos[dacMap[n]] - globalObj->dacCalibrationNeg[dacMap[n]] )/500;
            //  int compensation = 0 ;
              globalObj->dacCalibrationPos[dacMap[n]] = globalObj->dacCalibrationPos[dacMap[n]] - compensation;
              globalObj->dacCalibrationNeg[dacMap[n]] = globalObj->dacCalibrationNeg[dacMap[n]] + compensation;

              Serial.println("Calibration Results - pos: " + String(globalObj->dacCalibrationPos[dacMap[n]]) + "\tneg: " + String(globalObj->dacCalibrationNeg[dacMap[n]]));

            };
          }
          Serial.println("");

          delay(100);
        }
        Serial.println("Calibration Complete");

      }

      /*
        calibration routine:
          1. set all DAC outputs to max neg
          2. set one to max high
          3. read ADC inputs to see if any of them have went HIGH
          4. continue through ADC inputs til all have been checked
          5. if more than one is high, error out
          6. else, ADC and DAC are now connected.

      */
    break;
  }
  if(knobChange){
    if(globalObj->parameterSelect){
      changeState(min_max(stepMode+knobChange, STATE_CALIB_INPUT0_OFFSET , STATE_TEST_RHEOSTAT ));
    } else {
      switch (stepMode){
        case STATE_CALIB_INPUT0_LOW:
          globalObj->adcCalibrationPos[0] += knobChange;
        break;
        case STATE_CALIB_INPUT0_HIGH:
          globalObj->adcCalibrationNeg[0] += knobChange;
        break;

        case STATE_CALIB_INPUT0_OFFSET:
          globalObj->adcCalibrationOffset[0] += knobChange;
        break;
        case STATE_CALIB_INPUT1_OFFSET:
          globalObj->adcCalibrationOffset[1] += knobChange;
        break;
        case STATE_CALIB_INPUT2_OFFSET:
          globalObj->adcCalibrationOffset[2] += knobChange;
        break;
        case STATE_CALIB_INPUT3_OFFSET:
          globalObj->adcCalibrationOffset[3] += knobChange;
        break;
        case STATE_CALIB_INPUT1_LOW:
          globalObj->adcCalibrationPos[1] += knobChange;
        break;
        case STATE_CALIB_INPUT1_HIGH:
          globalObj->adcCalibrationNeg[1] += knobChange;
        break;
        case STATE_CALIB_INPUT2_LOW:
          globalObj->adcCalibrationPos[2] += knobChange;
        break;
        case STATE_CALIB_INPUT2_HIGH:
          globalObj->adcCalibrationNeg[2] += knobChange;
        break;
        case STATE_CALIB_INPUT3_LOW:
          globalObj->adcCalibrationPos[3] += knobChange;
        break;
        case STATE_CALIB_INPUT3_HIGH:
          globalObj->adcCalibrationNeg[3] += knobChange;
        break;
        case STATE_CALIB_OUTPUT0_LOW:
          globalObj->dacCalibrationNeg[dacMap[0]] += knobChange;
        break;
        case STATE_CALIB_OUTPUT0_HIGH:
          globalObj->dacCalibrationPos[dacMap[0]] += knobChange;
        break;
        case STATE_CALIB_OUTPUT1_LOW:
          globalObj->dacCalibrationNeg[dacMap[1]] += knobChange;
        break;
        case STATE_CALIB_OUTPUT1_HIGH:
          globalObj->dacCalibrationPos[dacMap[1]] += knobChange;
        break;
        case STATE_CALIB_OUTPUT2_LOW:
          globalObj->dacCalibrationNeg[dacMap[2]] += knobChange;
        break;
        case STATE_CALIB_OUTPUT2_HIGH:
          globalObj->dacCalibrationPos[dacMap[2]] += knobChange;
        break;
        case STATE_CALIB_OUTPUT3_LOW:
          globalObj->dacCalibrationNeg[dacMap[3]] += knobChange;
        break;
        case STATE_CALIB_OUTPUT3_HIGH:
          globalObj->dacCalibrationPos[dacMap[3]] += knobChange;
        break;
        case STATE_CALIB_OUTPUT4_LOW:
          globalObj->dacCalibrationNeg[dacMap[4]] += knobChange;
        break;
        case STATE_CALIB_OUTPUT4_HIGH:
          globalObj->dacCalibrationPos[dacMap[4]] += knobChange;
        break;
        case STATE_CALIB_OUTPUT5_LOW:
          globalObj->dacCalibrationNeg[dacMap[5]] += knobChange;
        break;
        case STATE_CALIB_OUTPUT5_HIGH:
          globalObj->dacCalibrationPos[dacMap[5]] += knobChange;
        break;
        case STATE_CALIB_OUTPUT6_LOW:
          globalObj->dacCalibrationNeg[dacMap[6]] += knobChange;
        break;
        case STATE_CALIB_OUTPUT6_HIGH:
          globalObj->dacCalibrationPos[dacMap[6]] += knobChange;
        break;
        case STATE_CALIB_OUTPUT7_LOW:
          globalObj->dacCalibrationNeg[dacMap[7]] += knobChange;
        break;
        case STATE_CALIB_OUTPUT7_HIGH:
          globalObj->dacCalibrationPos[dacMap[7]] += knobChange;
        break;
      }
    }
  }

}

uint8_t InputModule::getChannelButtonSw(uint8_t channel){
  switch(channel){
    case 0:
      return SW_CH0;
    break;
    case 1:
      return SW_CH1;
    break;
    case 2:
      return SW_CH2;
    break;
    case 3:
      return SW_CH3;
    break;
  }
};

int InputModule::getChannelFromSw(int switchNum){
  switch(switchNum){
    case SW_CH0:
      return 0;
    break;
    case SW_CH1:
      return 1;
    break;
    case SW_CH2:
      return 2;
    break;
    case SW_CH3:
      return 3;
    break;
  }
};
