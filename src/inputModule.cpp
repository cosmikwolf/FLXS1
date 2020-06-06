#include <Arduino.h>
#include "InputModule.h"

Zetaohm_MAX7301 max7301();

InputModule::InputModule() : knob(ENCODER1LEFTPIN, ENCODER1RIGHTPIN)
{
  // Encoder lib has no default constructor, so need to use initializer list instead
  // from https://forum.pjrc.com/threads/25082-How-do-I-include-a-library-in-a-library

  //pinMode(ENCODER1LEFTPIN, INPUT);
  //pinMode(ENCODER1RIGHTPIN, INPUT);

  knobRead = 0;
  knobBuffer = 0;
  knobPrevious = 0;
  knobChange = 0;

};

void InputModule::initialize(OutputController *outputControl, Zetaohm_MAX7301 *midplaneGPIO, Zetaohm_MAX7301 *backplaneGPIO, FlashMemory *saveFile, Sequencer *sequenceArray, MasterClock *clockMaster, DisplayModule *display, GlobalVariable *globalObj)
{
  Serial.println("button setup start");

  this->saveFile = saveFile;
  this->sequenceArray = sequenceArray;
  this->outputControl = outputControl;
  this->clockMaster = clockMaster;
  this->midplaneGPIO = midplaneGPIO;
  this->backplaneGPIO = backplaneGPIO;
  this->globalObj = globalObj;
  this->display = display;
  this->shortcutRandomOctaveSpan = 2;
  this->heldButton = 255;

  midplaneGPIO->begin(MIDPLANE_MAX7301_CS_PIN);

  midplaneGPIO->initPort(0, 22, INPUT_PULLUP);  // SW_00
  midplaneGPIO->initPort(1, 21, INPUT_PULLUP);  // SW_01
  midplaneGPIO->initPort(2, 6, INPUT_PULLUP);   // SW_02
  midplaneGPIO->initPort(3, 9, INPUT_PULLUP);   // SW_03
  midplaneGPIO->initPort(4, 23, INPUT_PULLUP);  // SW_04
  midplaneGPIO->initPort(5, 20, INPUT_PULLUP);  // SW_05
  midplaneGPIO->initPort(6, 11, INPUT_PULLUP);  // SW_06
  midplaneGPIO->initPort(7, 7, INPUT_PULLUP);   // SW_07
  midplaneGPIO->initPort(8, 24, INPUT_PULLUP);  // SW_08
  midplaneGPIO->initPort(9, 19, INPUT_PULLUP);  // SW_09
  midplaneGPIO->initPort(10, 14, INPUT_PULLUP); // SW_10
  midplaneGPIO->initPort(11, 13, INPUT_PULLUP); // SW_11
  midplaneGPIO->initPort(12, 3, INPUT_PULLUP);  // SW_12
  midplaneGPIO->initPort(13, 18, INPUT_PULLUP); // SW_13
  midplaneGPIO->initPort(14, 17, INPUT_PULLUP); // SW_14
  midplaneGPIO->initPort(15, 16, INPUT_PULLUP); // SW_15
  midplaneGPIO->initPort(16, 0, INPUT_PULLUP);  // SW_PLAY
  midplaneGPIO->initPort(17, 27, INPUT_PULLUP); // SW_PAUSE
  midplaneGPIO->initPort(18, 4, INPUT_PULLUP);  // SW_STOP
  midplaneGPIO->initPort(19, 1, INPUT_PULLUP);  // SW_CH0
  midplaneGPIO->initPort(20, 26, INPUT_PULLUP); // SW_CH1
  midplaneGPIO->initPort(21, 2, INPUT_PULLUP);  // SW_CH2
  midplaneGPIO->initPort(22, 25, INPUT_PULLUP); // SW_CH3
  midplaneGPIO->initPort(23, 5, INPUT_PULLUP);  // SW_PGDN
  midplaneGPIO->initPort(24, 8, INPUT_PULLUP);  // SW_PGUP
  midplaneGPIO->initPort(25, 10, INPUT_PULLUP); // SW_PATTERN
  midplaneGPIO->initPort(26, 12, INPUT_PULLUP); // SW_MENU
  midplaneGPIO->initPort(27, 15, INPUT_PULLUP); // SW_SHIFT

  midplaneGPIO->updateGpioPinModes();
  midplaneGPIO->update();

  if (midplaneGPIO->pressed(SW_PLAY) &&
      midplaneGPIO->pressed(SW_REC) &&
      midplaneGPIO->pressed(SW_STOP))
  {
    eraseAllFlag = true;
  }
}

void InputModule::multiSelectInputHandler()
{
  for (int i = 0; i < 16; i++)
  {
    if (midplaneGPIO->fell(i))
    {
      // select the step
      globalObj->selectedStep = getNote(i);

      buttonMode = BUTTON_MODE_MULTISELECT;
      globalObj->multiSelection[getNote(i)] = !globalObj->multiSelection[getNote(i)];
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
    if (midplaneGPIO->rose(i))
    {
    };
  };

  if (knobChange)
  {
    if (globalObj->parameterSelect)
    { // Encoder Switch
      changeState(min_max(stepMode + knobChange, STATE_PITCH0, STATE_CV2_OFFSET));
    }
    else
    {
      switch (stepMode)
      {
      case STATE_PITCH0:
        if (!globalObj->multi_pitch_switch)
        {
          globalObj->multi_pitch_switch = true;
        }
        else
        {
          globalObj->multi_pitch += knobChange;
        }
        break;
      case STATE_CHORD:
        if (!globalObj->multi_arpInterval_switch)
        {
          globalObj->multi_arpInterval_switch = true;
        }
        else
        {
          globalObj->multi_arpInterval = min_max(globalObj->multi_arpInterval + knobChange, 0, 26);
        }
        break;
      case STATE_GATETYPE:
        if (!globalObj->multi_gateType_switch)
        {
          globalObj->multi_gateType_switch = true;
        }
        else
        {
          globalObj->multi_gateType = min_max(globalObj->multi_gateType + knobChange, 0, 8);
        }
        break;
      case STATE_GATELENGTH:
        if (!globalObj->multi_gateLength_switch)
        {
          globalObj->multi_gateLength_switch = true;
        }
        else
        {
          globalObj->multi_gateLength = min_max(globalObj->multi_gateLength + knobChange, 0, 64);
        }
        break;
      case STATE_ARPTYPE:
        if (!globalObj->multi_arpType_switch)
        {
          globalObj->multi_arpType_switch = true;
        }
        else
        {
          globalObj->multi_arpType = min_max(globalObj->multi_arpType + knobChange, 0, 5);
        }
        break;
      case STATE_ARPOCTAVE:
        if (!globalObj->multi_arpOctave_switch)
        {
          globalObj->multi_arpOctave_switch = true;
        }
        else
        {
          globalObj->multi_arpOctave = min_max(globalObj->multi_arpOctave + knobChange, 0, 5);
        }
        break;
      case STATE_ARPSPEEDNUM:
        if (!globalObj->multi_arpSpdNum_switch)
        {
          globalObj->multi_arpSpdNum_switch = true;
        }
        else
        {
          globalObj->multi_arpSpdNum = min_max(globalObj->multi_arpSpdNum + knobChange, 0, 16);
        }
        break;
      case STATE_ARPSPEEDDEN:
        if (!globalObj->multi_arpSpdDen_switch)
        {
          globalObj->multi_arpSpdDen_switch = true;
        }
        else
        {
          globalObj->multi_arpSpdDen = min_max(globalObj->multi_arpSpdDen + knobChange, 0, 64);
        }
        break;
      case STATE_GLIDE:
        if (!globalObj->multi_glide_switch)
        {
          globalObj->multi_glide_switch = true;
        }
        else
        {
          globalObj->multi_glide = min_max(globalObj->multi_glide + knobChange, 0, 127);
        }
        break;
      case STATE_BEATCOUNT:
        if (!globalObj->multi_beatDiv_switch)
        {
          globalObj->multi_beatDiv_switch = true;
        }
        else
        {
          globalObj->multi_beatDiv += knobChange;
        }
        break;
      case STATE_CV2_LEVEL:
        if (!globalObj->multi_velocity_switch)
        {
          globalObj->multi_velocity_switch = true;
        }
        else
        {
          globalObj->multi_velocity += knobChange;
        }
        break;
      case STATE_CV2_TYPE:
        if (!globalObj->multi_velocityType_switch)
        {
          globalObj->multi_velocityType_switch = true;
        }
        else
        {
          globalObj->multi_velocityType = min_max(globalObj->multi_velocityType + knobChange, 0, 14);
        }
        break;
      case STATE_CV2_SPEED:
        if (!globalObj->multi_cv2speed_switch)
        {
          globalObj->multi_cv2speed_switch = true;
        }
        else
        {
          globalObj->multi_cv2speed = min_max(globalObj->multi_cv2speed + knobChange, 0, 255);
          ;
        }
        break;
      case STATE_CV2_OFFSET:
        if (!globalObj->multi_cv2offset_switch)
        {
          globalObj->multi_cv2offset_switch = true;
        }
        else
        {
          globalObj->multi_cv2offset = min_max(globalObj->multi_cv2offset + knobChange, -64, 64);
        }
        break;
      }

      for (int i = 0; i < MAX_STEPS_PER_SEQUENCE; i++)
      {
        if (globalObj->multiSelection[i])
        { // if step is selected in multi select

          switch (stepMode)
          {
          case STATE_PITCH0:
            sequenceArray[globalObj->selectedChannel].setStepPitch(i, min_max_cycle(globalObj->multi_pitch, 0, 120), 0);

            Serial.println("Changing pitch - knobChange: : " + String(knobChange) + "\tmultiPitch: " + String(globalObj->multi_pitch));

            break;
          case STATE_CHORD:
            //  setStepAbsolute(globalObj->selectedChannel, i, globalObj->multi_arpInterval);
            sequenceArray[globalObj->selectedChannel].stepData[i].chord = globalObj->multi_arpInterval;
            sequenceArray[globalObj->selectedChannel].setStepPitch(i, chordArray[globalObj->multi_arpInterval][0], 1);
            sequenceArray[globalObj->selectedChannel].setStepPitch(i, chordArray[globalObj->multi_arpInterval][1], 2);
            sequenceArray[globalObj->selectedChannel].setStepPitch(i, chordArray[globalObj->multi_arpInterval][2], 3);
            break;

          case STATE_GATETYPE:
            sequenceArray[globalObj->selectedChannel].stepData[i].gateType = globalObj->multi_gateType;
            //sequenceArray[globalObj->selectedChannel].stepData[i].gateType =  min_max(globalObj->multi_gateType, 0, 3);
            Serial.println("Changing gatetype - knobChange: : " + String(knobChange) + "\tglobalObj->multi_gateType: " + String(globalObj->multi_gateType) + "\tquant vale: " + String(globalObj->multi_gateType));

            break;
          case STATE_GATELENGTH:
            if (globalObj->multi_gateLength == 0)
            {
              sequenceArray[globalObj->selectedChannel].stepData[i].gateType = GATETYPE_REST;
            }
            else if (globalObj->multi_gateLength > 0)
            {
              if (sequenceArray[globalObj->selectedChannel].stepData[i].gateType == GATETYPE_REST)
              {
                sequenceArray[globalObj->selectedChannel].stepData[i].gateType = GATETYPE_STEP;
              }
            }

            sequenceArray[globalObj->selectedChannel].stepData[i].gateLength = min_max(globalObj->multi_gateLength, 0, 255);
            break;
          case STATE_ARPTYPE:
            sequenceArray[globalObj->selectedChannel].stepData[i].arpType = min_max(globalObj->multi_arpType, 0, 5);

            break;
          case STATE_ARPOCTAVE:
            sequenceArray[globalObj->selectedChannel].stepData[i].arpOctave = min_max(globalObj->multi_arpOctave, 1, 5);
            break;
          case STATE_ARPSPEEDNUM:
            sequenceArray[globalObj->selectedChannel].stepData[i].arpSpdNum = min_max(globalObj->multi_arpSpdNum, 1, 16);
            break;
          case STATE_ARPSPEEDDEN:
            sequenceArray[globalObj->selectedChannel].stepData[i].arpSpdDen = min_max(globalObj->multi_arpSpdDen, 1, 64);
            break;
          case STATE_GLIDE:
            sequenceArray[globalObj->selectedChannel].stepData[i].glide = min_max(globalObj->multi_glide, 0, 127);
            break;
          case STATE_CV2_LEVEL:
            sequenceArray[globalObj->selectedChannel].stepData[i].velocity = positive_modulo(globalObj->multi_velocity, 127);
            break;
          case STATE_CV2_TYPE:
            sequenceArray[globalObj->selectedChannel].stepData[i].velocityType = min_max(globalObj->multi_velocityType, 0, 13);
            break;
          case STATE_CV2_SPEED:
            sequenceArray[globalObj->selectedChannel].stepData[i].cv2speed = min_max(globalObj->multi_cv2speed, 1, 255);
            break;
          case STATE_CV2_OFFSET:
            sequenceArray[globalObj->selectedChannel].stepData[i].cv2offset = min_max(globalObj->multi_cv2offset, -64, 64);
            break;
          }
        }
      }
    }
  }
};

void InputModule::loop(uint16_t frequency)
{
  if (inputTimer < frequency)
  {
    return;
  }
  inputTimer = 0;
  knobPrevious = knobRead;
  knobRead = 1 * knob.read() / 4;
  //knobRead = -1 * knob.read()/2  ;
  knobChange = knobRead - knobPrevious;
  midplaneGPIO->update();

  if (midplaneGPIO->activityCheck() || knobChange)
  {
    globalObj->screenSaverTimeout = 0;
  }

  if (midplaneGPIO->roseCheck())
  { // if any buttons are pressed during sysex import, either exit import, or ignore
    switch (globalObj->sysex_status)
    {
    case SYSEX_IDLE:
      break;
    case SYSEX_PREPARING:
    case SYSEX_READYFORDATA:
      globalObj->sysex_status = SYSEX_IDLE;
      changeState(STATE_PITCH0);
      break;
    case SYSEX_IMPORTING:
    case SYSEX_PROCESSING:
    case SYSEX_IMPORTCOMPLETE:
    case SYSEX_ERROR:
      return;
      break;
    }
  }

  if (globalObj->dataInputStyle)
  {
    if (backplaneGPIO->fell(SW_ENCODER_BACKPLANE))
    {
      globalObj->parameterSelect = !globalObj->parameterSelect;
    }
  }
  else
  {
    if (backplaneGPIO->pressed(SW_ENCODER_BACKPLANE) || midplaneGPIO->pressed(SW_SHIFT))
    {
      globalObj->parameterSelect = true;
    }
    else
    {
      globalObj->parameterSelect = false;
    }
  }

  // if(backplaneGPIO->pressed(SW_ENCODER_BACKPLANE) || midplaneGPIO->pressed(SW_SHIFT) ){ //encoder knob
  //   globalObj->parameterSelect = true;
  // } else {
  //   globalObj->parameterSelect = false;
  // }

  if (midplaneGPIO->fell(SW_SHIFT))
  {
    //tap tempo
    if ((globalObj->clockMode == INTERNAL_CLOCK) && (globalObj->currentMenu == TEMPO_MENU))
    {

      if (globalObj->tapTempoCount == 0)
      {
        globalObj->tapTempoMasterClkCounter = 0; // if its the first tap, reset the tempo counter
        for (int i = 0; i < 4; i++)
        {
          globalObj->tapTempoClockValues[i] = 0;
        }
      }
      else
      {
        globalObj->tapTempoClockValues[(globalObj->tapTempoCount - 1) % 4] = globalObj->tapTempoMasterClkCounter;
        globalObj->tapTempoMasterClkCounter = 0;
      };
      globalObj->tapTempoCount++;

      if (globalObj->tapTempoCount > 4)
      {
        uint32_t tempTempo = 0;
        for (int i = 0; i < 4; i++)
        {
          tempTempo += globalObj->tapTempoClockValues[i] * kMasterClockInterval;
        }

        globalObj->tempoX100 = 6000000000 / (tempTempo / 4);
        // clockMaster->changeTempo(globalObj->tempoX100);
        Serial.println("Setting tap tempo to: " + String(globalObj->tempoX100) + "\ttempTempo: " + String(tempTempo));
      }
    }
    else
    {
      if (globalObj->tapTempoCount)
      {
        if (globalObj->tapTempoMasterClkCounter > 1000000 / kMasterClockInterval)
        {
          Serial.println("Resetting tap tempo : prev count: " + String(globalObj->tapTempoCount));
          globalObj->tapTempoCount = 0;
        }
      }
    }
  }

  if (midplaneGPIO->pressed(SW_SHIFT) && midplaneGPIO->pressed(SW_PLAY) && midplaneGPIO->pressed(SW_MENU))
  {
    changeState(STATE_CALIBRATION);
  }

  if (midplaneGPIO->pressed(SW_SHIFT) && midplaneGPIO->pressed(SW_REC))
  {
    changeState(STATE_INPUTDEBUG);
  }
  if (knobChange)
  {
    switch (globalObj->currentMenu)
    {
    case SEQUENCE_MENU:
    case MOD_MENU_1:
    case MOD_MENU_2:
    case QUANTIZE_MENU:
      if (globalObj->parameterSelect)
      {
        changeState(min_max_cycle(stepMode + knobChange, STATE_FIRSTSTEP, STATE_ARPINTMOD));
      }
      break;
    }
  }
  //we always want the alt (non matrix) buttons to behave the same way

  // now to handle the rest of the buttons.
  bool didAltButtonsFire = altButtonHandler();

  if (didAltButtonsFire == false)
  {
    switch (globalObj->currentMenu)
    {
    case PITCH_GATE_MENU:
    case ARPEGGIO_MENU:
    case VELOCITY_MENU:
      if (globalObj->multiSelectSwitch)
      {
        multiSelectInputHandler();
      }
      else
      {
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
    case GLOBAL_MENU_3:
      globalMenuHandler();
      break;

    case INPUT_DEBUG_MENU:
      if (knobChange)
      {
        selectedText = positive_modulo(selectedText + knobChange, 5);
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
      if (modalTimer > 1000)
      {
        changeState(STATE_PITCH0);
      }
      break;

    case SAVE_MENU:
      saveMenuInputHandler();
      break;

    case MENU_RANDOM:
      randomShortcutHandler();
      break;

    case MENU_TRANSPOSE:
      transposeShortcutHandler();
      break;

    case MENU_TUNER:
      tunerShortcutHandler();
      break;

    case MENU_FILL:
      fillShortcutHandler();
      break;

    case MENU_SKIP:
      skipShortcutHandler();
      break;

    case MENU_CLKDIV:
      clkdivShortcutHandler();
      break;

    case MENU_RESET:
      resetShortcutHandler();
      break;

    case MENU_REVERSE:
      reverseShortcutHandler();
      break;

    case MENU_PATTERN_CHAIN:
    case MENU_CHAIN_HELP:
      patternChainInputHandler();
      break;
    }
  }
  else
  {
    //  Serial.println("AltButton: " + String(didAltButtonsFire));
  }
}

void InputModule::changeState(uint8_t targetState)
{
  //Serial.println("change state: " + String(targetState));

  if (stepMode != targetState)
  {
    if (targetState == STATE_TEST_MIDI)
    {
      midiTestActive = true;

      Serial.println("Beginning Midi Test");
    }
  }

  stepMode = targetState;
  switch (stepMode)
  {
  case STATE_PITCH0:
  case STATE_GATELENGTH:
  case STATE_GLIDE:
  case STATE_GATETYPE:
    globalObj->currentMenu = PITCH_GATE_MENU;
    break;
  case STATE_CHORD:
  case STATE_ARPTYPE:
  case STATE_ARPSPEEDNUM:
  case STATE_ARPSPEEDDEN:
  case STATE_ARPOCTAVE:
    globalObj->currentMenu = ARPEGGIO_MENU;
    break;
  case STATE_CV2_LEVEL:
  case STATE_CV2_TYPE:
  case STATE_CV2_SPEED:
  case STATE_CV2_OFFSET:
    globalObj->currentMenu = VELOCITY_MENU;
    break;
  case STATE_FIRSTSTEP:
  case STATE_STEPCOUNT:
  case STATE_BEATCOUNT:
  case STATE_PLAYMODE:
  case STATE_SWING:
    globalObj->currentMenu = SEQUENCE_MENU;
    break;
  case STATE_QUANTIZESCALE:
  case STATE_QUANTIZEKEY:
  case STATE_QUANTIZEMODE:
    globalObj->currentMenu = QUANTIZE_MENU;
    break;
  case STATE_NOTEDISPLAY:
    globalObj->currentMenu = NOTE_DISPLAY;
    break;
  case STATE_PATTERNSELECT:
    globalObj->currentMenu = PATTERN_SELECT;
    break;
  case STATE_SAVE:
    globalObj->currentMenu = SAVE_MENU;
    break;
  case STATE_CALIBRATION:
    globalObj->currentMenu = CALIBRATION_MENU;
    break;
  case STATE_EXTCLOCK:
  case STATE_TEMPO:
  case STATE_RESETINPUT:
    globalObj->currentMenu = TEMPO_MENU;
    break;
  case STATE_SKIPSTEPCOUNT:
  case STATE_SKIPSTEP:
  case STATE_GATEMUTE:
  case STATE_RANDOMPITCH:
  case STATE_PITCHMOD:
  case STATE_GLIDEMOD:
  case STATE_GATEMOD:
    globalObj->currentMenu = MOD_MENU_1;
    break;
  case STATE_ARPTYPEMOD:
  case STATE_ARPSPDMOD:
  case STATE_ARPOCTMOD:
  case STATE_ARPINTMOD:
    globalObj->currentMenu = MOD_MENU_2;
    break;
  case STATE_INPUTDEBUG:
    globalObj->currentMenu = INPUT_DEBUG_MENU;
    break;
  case STATE_PG_BTN_SWITCH:
  case STATE_DATA_KNOB_SWITCH:
    globalObj->currentMenu = GLOBAL_MENU_1;
    break;
  case STATE_CH1_VOLT_RANGE:
  case STATE_CH2_VOLT_RANGE:
  case STATE_CH3_VOLT_RANGE:
  case STATE_CH4_VOLT_RANGE:
    globalObj->currentMenu = GLOBAL_MENU_2;
    break;
  case STATE_MIDI_CHANNEL_1:
  case STATE_MIDI_CHANNEL_2:
  case STATE_MIDI_CHANNEL_3:
  case STATE_MIDI_CHANNEL_4:
    globalObj->currentMenu = GLOBAL_MENU_3;
    break;
  case STATE_SYSEX_EXPORT:
  case STATE_SYSEX_IMPORT:
    globalObj->currentMenu = SYSEX_MENU;
    break;
  case STATE_SYSEX_IMPORTINPROGRESS:
    globalObj->currentMenu = SYSEX_IMPORT_INPROGRESS;
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
    globalObj->currentMenu = CALIBRATION_MENU;
    break;
  case STATE_CALIBRATION_SAVE_MODAL:
    globalObj->currentMenu = MENU_MODAL;
    break;
  case STATE_MULTISELECT:
    //  globalObj->currentMenu = MENU_MULTISELECT;
    break;
  case STATE_SHORTCUT_RANDOM_PARAM:
  case STATE_SHORTCUT_RANDOM_VAR1:
  case STATE_SHORTCUT_RANDOM_VAR2:
  case STATE_SHORTCUT_RANDOM_VAR3:
    globalObj->currentMenu = MENU_RANDOM;
    break;
  case STATE_SHORTCUT_TRANSPOSE:
    globalObj->currentMenu = MENU_TRANSPOSE;
    break;
  case STATE_SHORTCUT_FILL:
    globalObj->currentMenu = MENU_FILL;
    break;
  case STATE_SHORTCUT_CLKDIV:
    globalObj->currentMenu = MENU_CLKDIV;
    break;
  case STATE_SHORTCUT_SKIP:
  case STATE_SHORTCUT_SKIP_TRIGGER:
    globalObj->currentMenu = MENU_SKIP;
    break;
  case STATE_SHORTCUT_TUNER:
    globalObj->currentMenu = MENU_TUNER;
    break;
  case STATE_SHORTCUT_PAGE:
    globalObj->currentMenu = MENU_PAGE;
    break;
  case STATE_SHORTCUT_FN1:
    globalObj->currentMenu = MENU_FN1;
    break;
  case STATE_SHORTCUT_FN2:
    globalObj->currentMenu = MENU_FN2;
    break;
  case STATE_SHORTCUT_FN3:
    globalObj->currentMenu = MENU_FN3;
    break;
  case STATE_SHORTCUT_FN4:
    globalObj->currentMenu = MENU_FN4;
    break;

  case STATE_PATTERN_CHAIN:
    globalObj->currentMenu = MENU_PATTERN_CHAIN;
    break;
  case STATE_CHAIN_HELP:
    globalObj->currentMenu = MENU_CHAIN_HELP;
    break;
  default:
    Serial.println("This state has no menu selection! " + String(targetState));
    break;
  }

  if (globalObj->currentMenu != GLOBAL_MENU_1 && globalObj->currentMenu != GLOBAL_MENU_2 && globalObj->currentMenu != SYSEX_MENU && globalObj->currentMenu != TEMPO_MENU && (previousMenu == TEMPO_MENU || previousMenu == GLOBAL_MENU_1 || previousMenu == GLOBAL_MENU_2 || previousMenu == SYSEX_MENU))
  {
    saveFile->saveGlobalData();
    // Serial.println("Saved GlobalData");
  }

  if ((globalObj->currentMenu != MENU_PATTERN_CHAIN) && (previousMenu == MENU_PATTERN_CHAIN))
  {
    saveFile->saveSongData();
    Serial.println("Saved Song Data");
  }

  previousMenu = globalObj->currentMenu;
}

// STATE VARIABLE INPUT HANDLERS

void InputModule::patternSelectHandler()
{
  if (knobChange)
  {
    globalObj->patternChangeTrigger = min_max(globalObj->patternChangeTrigger + knobChange, 0, 4);
  }
  int i = 0;
  for (int n = 0; n < 16; n++)
  {
    i = n + globalObj->pattern_page * 16;
    if (midplaneGPIO->fell(n))
    {
      if (heldButton == 255)
      {
        heldButton = n;
        globalObj->fastChainModePatternCount = 0;
        globalObj->fastChainModeCurrentIndex = 0;
      }
      if (midplaneGPIO->pressed(heldButton))
      {
        if (globalObj->fastChainModePatternCount < 16)
        {
          globalObj->fastChainPatternSelect[globalObj->fastChainModePatternCount] = i;
          globalObj->fastChainModePatternCount++;
        }
      }
      else
      {
        heldButton = 255; //if the first held button is pressed
      }
      Serial.println("fell heldbutton: " + String(heldButton) + "\tpatternCount: " + String(globalObj->fastChainModePatternCount) + "\tcurrentIndex: " + String(globalObj->fastChainModeCurrentIndex));
    }
  }
  for (int n = 0; n < 16; n++)
  {
    i = n + globalObj->pattern_page * 16;
    if (midplaneGPIO->rose(n))
    {
      if (heldButton == n)
      {
        heldButton = 255;
        if (globalObj->fastChainModePatternCount > 0)
        {
          for (int j = 0; j < 16; j++)
          {
            if (midplaneGPIO->pressed(j))
            {
              heldButton = j;
            }
          }
        }
      }
      if (globalObj->fastChainModePatternCount <= 1)
      { // if pattern chain is not happening, then simply change the pattern
        if (globalObj->playing)
        {
          saveFile->changePattern(i, globalObj->patternChannelSelector, globalObj->patternChangeTrigger);
        }
        else
        {
          saveFile->changePattern(i, globalObj->patternChannelSelector, 0);
        }
        changeState(STATE_PITCH0);
      }
      Serial.println("rose heldbutton: " + String(heldButton) + "\tpatternCount: " + String(globalObj->fastChainModePatternCount) + "\tcurrentIndex: " + String(globalObj->fastChainModeCurrentIndex));
    }
  }
  //  changeState(STATE_PITCH0);
}

void InputModule::tempoMenuHandler()
{
  for (int i = 0; i < 16; i++)
  {
    if (midplaneGPIO->fell(i))
    {
      changeState(STATE_PITCH0);
      globalObj->selectedStep = getNote(i);
    }
  }

  if (knobChange)
  {
    if (globalObj->parameterSelect)
    {
      changeState(min_max_cycle(stepMode + knobChange, STATE_TEMPO, STATE_RESETINPUT));
    }
    else
    {
      switch (stepMode)
      {
      case STATE_TEMPO:
        //  if (globalObj->tempoX100 > 100200) {
        //    globalObj->tempoX100 = 100200;
        //  }
        globalObj->tempoX100 = min_max_cycle(globalObj->tempoX100 + knobChange * 100, 100, 100000);
        //  if(globalObj->tempoX100 == 0){
        //    globalObj->tempoX100 = 100;
        //  }
        // clockMaster->changeTempo(globalObj->tempoX100);
        break;

      case STATE_EXTCLOCK:
        globalObj->clockMode = positive_modulo(globalObj->clockMode + knobChange, 8);
        if (globalObj->clockMode == EXTERNAL_CLOCK_BIDIRECTIONAL_INPUT)
        {
          Serial.println("Setting clock port to input");
          globalObj->setClockPortDirection(CLOCK_PORT_INPUT);
        }
        else
        {
          globalObj->setClockPortDirection(CLOCK_PORT_OUTPUT);
        }
        break;

      case STATE_RESETINPUT:
        sequenceArray[globalObj->selectedChannel].gpio_reset = min_max(sequenceArray[globalObj->selectedChannel].gpio_reset + knobChange, 0, 8);
        break;
      }
    }
  }
}

void InputModule::sequenceMenuHandler()
{
  int8_t newBeatDiv;
  uint16_t previousQuantizeMode;

  for (int i = 0; i < 16; i++)
  {
    if (midplaneGPIO->fell(i))
    {
      if (stepMode == STATE_QUANTIZEMODE)
      {
        // put in quantize mode custom button input code here
        previousQuantizeMode = sequenceArray[globalObj->selectedChannel].quantizeMode;
        sequenceArray[globalObj->selectedChannel].quantizeMode ^= (1 << i);
        if (sequenceArray[globalObj->selectedChannel].quantizeMode == 0)
        {
          sequenceArray[globalObj->selectedChannel].quantizeMode = previousQuantizeMode;
        }
      }
      else
      {
        changeState(STATE_PITCH0);
        globalObj->selectedStep = sequenceArray[globalObj->selectedChannel].getActivePage() * 16 + i;
        notePage = sequenceArray[globalObj->selectedChannel].getActivePage();
      }
    }
  }
  if (knobChange)
  {
    if (globalObj->parameterSelect)
    { // Encoder Switch
      //  changeState(min_max_cycle(stepMode + knobChange,  STATE_STEPCOUNT,  STATE_QUANTIZEMODE));
    }
    else
    {
      switch (stepMode)
      {
      case STATE_FIRSTSTEP:
        sequenceArray[globalObj->selectedChannel].firstStep = min_max(sequenceArray[globalObj->selectedChannel].firstStep + knobChange, 0, 63);
        //sequenceArray[globalObj->selectedChannel].firstStepChanged == TRUE;
        break;
      case STATE_STEPCOUNT:
        //sequenceArray[globalObj->selectedChannel].stepCount = min_max(sequenceArray[globalObj->selectedChannel].stepCount + knobChange, 1, 64);
        sequenceArray[globalObj->selectedChannel].setStepCount(min_max(sequenceArray[globalObj->selectedChannel].stepCount + knobChange, 1, 64));
        // if (sequenceArray[globalObj->selectedChannel].stepCount == 0) {
        //   sequenceArray[globalObj->selectedChannel].stepCount = 64;
        // }
        break;
      case STATE_BEATCOUNT:
        newBeatDiv = min_max(sequenceArray[globalObj->selectedChannel].clockDivision + knobChange, -13, 16);
        sequenceArray[globalObj->selectedChannel].updateClockDivision(newBeatDiv);
        //Serial.println("newBeatDiv: " + String(newBeatDiv));
        // Serial.println("clockDiv: " + String(sequenceArray[globalObj->selectedChannel].clockDivision));
        break;

      case STATE_QUANTIZEKEY:
        sequenceArray[globalObj->selectedChannel].quantizeKey = positive_modulo(sequenceArray[globalObj->selectedChannel].quantizeKey + knobChange, 12);
        break;
      case STATE_QUANTIZEMODE:
        sequenceArray[globalObj->selectedChannel].quantizeModeIndex = positive_modulo(sequenceArray[globalObj->selectedChannel].quantizeModeIndex + knobChange, 19);
        switch (sequenceArray[globalObj->selectedChannel].quantizeModeIndex)
        {
        case 0:
          sequenceArray[globalObj->selectedChannel].quantizeMode = SEMITONE_SCALE_12;
          break;
        case 1:
          sequenceArray[globalObj->selectedChannel].quantizeMode = MAJOR_SCALE_12;
          break;
        case 2:
          sequenceArray[globalObj->selectedChannel].quantizeMode = MINOR_SCALE_12;
          break;
        case 3:
          sequenceArray[globalObj->selectedChannel].quantizeMode = MAJORMINOR_SCALE_12;
          break;
        case 4:
          sequenceArray[globalObj->selectedChannel].quantizeMode = BLUESMAJOR_SCALE_12;
          break;
        case 5:
          sequenceArray[globalObj->selectedChannel].quantizeMode = BLUESMINOR_SCALE_12;
          break;
        case 6:
          sequenceArray[globalObj->selectedChannel].quantizeMode = PENT_MAJOR_SCALE_12;
          break;
        case 7:
          sequenceArray[globalObj->selectedChannel].quantizeMode = PENT_MINOR_SCALE_12;
          break;
        case 8:
          sequenceArray[globalObj->selectedChannel].quantizeMode = FOLK_SCALE_12;
          break;
        case 9:
          sequenceArray[globalObj->selectedChannel].quantizeMode = AEOLIAN_SCALE_12;
          break;
        case 10:
          sequenceArray[globalObj->selectedChannel].quantizeMode = DORIAN_SCALE_12;
          break;
        case 11:
          sequenceArray[globalObj->selectedChannel].quantizeMode = MIXOLYDIAN_SCALE_12;
          break;
        case 12:
          sequenceArray[globalObj->selectedChannel].quantizeMode = PHRYGIAN_SCALE_12;
          break;
        case 13:
          sequenceArray[globalObj->selectedChannel].quantizeMode = LYDIAN_SCALE_12;
          break;
        case 14:
          sequenceArray[globalObj->selectedChannel].quantizeMode = LOCRIAN_SCALE_12;
          break;
        case 15:
          sequenceArray[globalObj->selectedChannel].quantizeMode = GAMELAN_SCALE_12;
          break;
        case 16:
          sequenceArray[globalObj->selectedChannel].quantizeMode = JAPANESE_SCALE_12;
          break;
        case 17:
          sequenceArray[globalObj->selectedChannel].quantizeMode = GYPSY_SCALE_12;
          break;
        case 18:
          sequenceArray[globalObj->selectedChannel].quantizeMode = ARABIAN_SCALE_12;
          break;
        case 19:
          sequenceArray[globalObj->selectedChannel].quantizeMode = FLAMENCO_SCALE_12;
          break;
        case 20:
          sequenceArray[globalObj->selectedChannel].quantizeMode = WHOLETONE_SCALE_12;
          break;
        }
        break;
      case STATE_QUANTIZESCALE:
        sequenceArray[globalObj->selectedChannel].quantizeScale = min_max(sequenceArray[globalObj->selectedChannel].quantizeScale + knobChange, 1, 64);
        break;
      case STATE_PLAYMODE:
        sequenceArray[globalObj->selectedChannel].playMode = positive_modulo(sequenceArray[globalObj->selectedChannel].playMode + knobChange, 4);
        break;
      case STATE_RESETINPUT:
        sequenceArray[globalObj->selectedChannel].gpio_reset = min_max_skip(sequenceArray[globalObj->selectedChannel].gpio_reset, knobChange, -9, 8, globalObj->selectedChannel + 5);
        break;

        // mod menu 1
      case STATE_GLIDEMOD:
        sequenceArray[globalObj->selectedChannel].cv_glidemod = min_max(sequenceArray[globalObj->selectedChannel].cv_glidemod + knobChange, 0, 12);
        break;

      case STATE_GATEMOD:
        sequenceArray[globalObj->selectedChannel].cv_gatemod = min_max(sequenceArray[globalObj->selectedChannel].cv_gatemod + knobChange, 0, 12);
        break;

      case STATE_GATEMUTE:
        sequenceArray[globalObj->selectedChannel].gpio_gatemute = min_max_skip(sequenceArray[globalObj->selectedChannel].gpio_gatemute, knobChange, -9, 8, globalObj->selectedChannel + 5);
        break;

      case STATE_RANDOMPITCH:
        sequenceArray[globalObj->selectedChannel].gpio_randompitch = min_max_skip(sequenceArray[globalObj->selectedChannel].gpio_randompitch, knobChange, -9, 8, globalObj->selectedChannel + 5);
        break;
      case STATE_RANDOMHIGH:
        sequenceArray[globalObj->selectedChannel].randomHigh = min_max(sequenceArray[globalObj->selectedChannel].randomHigh + knobChange, 0, 127);
        break;
      case STATE_RANDOMLOW:
        sequenceArray[globalObj->selectedChannel].randomLow = min_max(sequenceArray[globalObj->selectedChannel].randomLow + knobChange, 0, 127);
        break;

      case STATE_PITCHMOD:
        sequenceArray[globalObj->selectedChannel].cv_pitchmod = min_max(sequenceArray[globalObj->selectedChannel].cv_pitchmod + knobChange, 0, 12);
        break;
        // mod menu 2
      case STATE_ARPTYPEMOD:
        sequenceArray[globalObj->selectedChannel].cv_arptypemod = min_max(sequenceArray[globalObj->selectedChannel].cv_arptypemod + knobChange, 0, 12);
        break;

      case STATE_ARPSPDMOD:
        sequenceArray[globalObj->selectedChannel].cv_arpspdmod = min_max(sequenceArray[globalObj->selectedChannel].cv_arpspdmod + knobChange, 0, 12);
        break;

      case STATE_ARPOCTMOD:
        sequenceArray[globalObj->selectedChannel].cv_arpoctmod = min_max(sequenceArray[globalObj->selectedChannel].cv_arpoctmod + knobChange, 0, 12);
        break;

      case STATE_ARPINTMOD:
        sequenceArray[globalObj->selectedChannel].cv_arpintmod = min_max(sequenceArray[globalObj->selectedChannel].cv_arpintmod + knobChange, 0, 12);
        break;

      case STATE_SKIPSTEP:
        sequenceArray[globalObj->selectedChannel].gpio_skipstep = min_max_skip(sequenceArray[globalObj->selectedChannel].gpio_skipstep, knobChange, -9, 8, globalObj->selectedChannel + 5);
        break;

      case STATE_SKIPSTEPCOUNT:
        sequenceArray[globalObj->selectedChannel].skipStepCount = min_max(sequenceArray[globalObj->selectedChannel].skipStepCount + knobChange, 0, 63);
        break;

      case STATE_SWING:
        sequenceArray[globalObj->selectedChannel].swingX100 = min_max(sequenceArray[globalObj->selectedChannel].swingX100 + knobChange, 1, 99);
        break;
      }
    }
  }
}

void InputModule::inputMenuHandler()
{
  if (knobChange)
  {
    if (globalObj->parameterSelect)
    {
      changeState(min_max_cycle(stepMode + knobChange, STATE_STEPCOUNT, STATE_SKIPSTEP));
    }
    else
    {
      switch (stepMode)
      {
      }
    }
  }
}

void InputModule::globalMenuHandler()
{
  for (int i = 0; i < 16; i++)
  {
    if (midplaneGPIO->fell(i))
    {
      changeState(STATE_PITCH0);
      globalObj->selectedStep = getNote(i);
    }
  }

  if (knobChange)
  {
    if (globalObj->parameterSelect)
    { // Encoder Switch
      changeState(min_max_cycle(stepMode + knobChange, STATE_PG_BTN_SWITCH, STATE_SYSEX_IMPORT));
    }
    else
    {
      switch (stepMode)
      {

      case STATE_PG_BTN_SWITCH:
        globalObj->pageButtonStyle = min_max(globalObj->pageButtonStyle + knobChange, 0, 1);
        break;
      case STATE_DATA_KNOB_SWITCH:
        globalObj->dataInputStyle = min_max(globalObj->dataInputStyle + knobChange, 0, 1);
        break;
      case STATE_CH1_VOLT_RANGE:
        globalObj->outputNegOffset[0] = min_max(globalObj->outputNegOffset[0] + knobChange, 0, 5);
        break;
      case STATE_CH2_VOLT_RANGE:
        globalObj->outputNegOffset[1] = min_max(globalObj->outputNegOffset[1] + knobChange, 0, 5);
        break;
      case STATE_CH3_VOLT_RANGE:
        globalObj->outputNegOffset[2] = min_max(globalObj->outputNegOffset[2] + knobChange, 0, 5);
        break;
      case STATE_CH4_VOLT_RANGE:
        globalObj->outputNegOffset[3] = min_max(globalObj->outputNegOffset[3] + knobChange, 0, 5);
        break;
      case STATE_MIDI_CHANNEL_1:
        globalObj->midiChannel[0] = min_max(globalObj->midiChannel[0] + knobChange, 1, 16);
        break;
      case STATE_MIDI_CHANNEL_2:
        globalObj->midiChannel[1] = min_max(globalObj->midiChannel[1] + knobChange, 1, 16);
        break;
      case STATE_MIDI_CHANNEL_3:
        globalObj->midiChannel[2] = min_max(globalObj->midiChannel[2] + knobChange, 1, 16);
        break;
      case STATE_MIDI_CHANNEL_4:
        globalObj->midiChannel[3] = min_max(globalObj->midiChannel[3] + knobChange, 1, 16);
        break;
      }
    }
  }
}

void InputModule::patternChainInputHandler()
{
  //  globalObj->previousChainSelectedPattern = globalObj->chainSelectedPattern;

  if (midplaneGPIO->fell(SW_PATTERN))
  {
    // this->altButtonPatternHandler();
  }
  if (midplaneGPIO->fell(SW_MENU))
  {
    this->altButtonTempoHandler();
  }
  // if (midplaneGPIO->fell(SW_PGUP) && midplaneGPIO->pressed(SW_SHIFT)){
  //  changeState(STATE_CHAIN_HELP);
  // }
  // if (midplaneGPIO->fell(SW_PGDN) ){ globalObj->chainPatternRepeatCount[globalObj->chainSelectedPattern]--; }

  if (midplaneGPIO->fell(SW_PLAY))
  {
    if (!globalObj->chainModeActive)
    {
      saveFile->changePattern(globalObj->chainPatternSelect[globalObj->chainModeIndex], globalObj->patternChannelSelector, 0);
      globalObj->chainModeActive = 1;
      globalObj->fastChainModePatternCount = 0;
      globalObj->queuePattern = 255;
      globalObj->chainModeCount[globalObj->chainModeIndex] = 0;
    }

    if (globalObj->playing)
    {
      globalObj->playing = false;
    }
    else
    {
      globalObj->playing = true;
    }
  };

  if (midplaneGPIO->fell(SW_STOP))
  {
    globalObj->chainModeActive = 0;
    globalObj->chainModeIndex = 0;
    for (int chainNum = 0; chainNum < 16; chainNum++)
    {
      globalObj->chainModeCount[chainNum] = 0;
    }
    globalObj->playing = false;
    for (int s = 0; s < SEQUENCECOUNT; s++)
    {
      sequenceArray[s].clockReset(true);
    }
  }

  if (knobChange)
  {
    if (globalObj->parameterSelect)
    { // Encoder Switch
      uint8_t selectedPatternMax;
      for (int chain = 0; chain < CHAIN_COUNT_MAX; chain++)
      {
        if (globalObj->chainPatternRepeatCount[chain] == 0)
        {
          selectedPatternMax = chain;
          break;
        };
      };
      globalObj->chainSelectedPattern = min_max(globalObj->chainSelectedPattern + knobChange, 0, selectedPatternMax);
    }
    else
    {
      if (globalObj->chainSelectedPattern == 0)
      {
        globalObj->chainPatternRepeatCount[globalObj->chainSelectedPattern] = min_max(globalObj->chainPatternRepeatCount[globalObj->chainSelectedPattern] + knobChange, 1, 64);
      }
      else
      {
        globalObj->chainPatternRepeatCount[globalObj->chainSelectedPattern] = min_max(globalObj->chainPatternRepeatCount[globalObj->chainSelectedPattern] + knobChange, -64, 64);
        ;
      }
    }
  }

  for (int matrix = 0; matrix < 16; matrix++)
  {
    if (midplaneGPIO->fell(matrix))
    {
      globalObj->chainPatternSelect[globalObj->chainSelectedPattern] = matrix;
    }
  }
  uint8_t channelSwitch;
  for (int channel = 0; channel < 4; channel++)
  {
    switch (channel)
    {
    case 0:
      channelSwitch = SW_CH0;
      break;
    case 1:
      channelSwitch = SW_CH1;
      break;
    case 2:
      channelSwitch = SW_CH2;
      break;
    case 3:
      channelSwitch = SW_CH3;
      break;
    }
    if (midplaneGPIO->fell(channelSwitch))
    {
      buttonHoldTimer = 0;
      heldButton = channelSwitch;
      if (channel != globalObj->chainModeMasterChannel[globalObj->chainSelectedPattern])
      {
        if (globalObj->chainChannelSelect[channel][globalObj->chainSelectedPattern] && !globalObj->chainChannelMute[channel][globalObj->chainSelectedPattern])
        {
          globalObj->chainChannelMute[channel][globalObj->chainSelectedPattern] = 1;
          globalObj->chainChannelSelect[channel][globalObj->chainSelectedPattern] = 1;
        }
        else if (globalObj->chainChannelSelect[channel][globalObj->chainSelectedPattern] && globalObj->chainChannelMute[channel][globalObj->chainSelectedPattern])
        {
          globalObj->chainChannelMute[channel][globalObj->chainSelectedPattern] = 0;
          globalObj->chainChannelSelect[channel][globalObj->chainSelectedPattern] = 0;
        }
        else
        {
          globalObj->chainChannelSelect[channel][globalObj->chainSelectedPattern] = 1;
          globalObj->chainChannelMute[channel][globalObj->chainSelectedPattern] = 0;
        }
      }
    }
    if (midplaneGPIO->pressed(channelSwitch))
    {
      if ((buttonHoldTimer > 500) && (heldButton = channelSwitch))
      {
        globalObj->chainModeMasterChannel[globalObj->chainSelectedPattern] = channel;
        globalObj->chainChannelSelect[channel][globalObj->chainSelectedPattern] = true;
        globalObj->chainChannelMute[channel][globalObj->chainSelectedPattern] = 0;
        heldButton = 255;
      }
    }
  }
}

void InputModule::channelButtonShiftHandler(uint8_t channel)
{
  if (globalObj->selectedChannel != channel)
  {
    globalObj->selectedChannel = channel;
  }
  changeState(STATE_TUNER);
};

void InputModule::channelButtonShiftMenuHandler(uint8_t channel)
{
  changeState(STATE_CALIB_INPUT0_OFFSET);
  Serial.println("CALIB MENU " + String(channel) + "\t" + String(CALIBRATION_MENU));
}

void InputModule::channelButtonChannelSelectorHandler(uint8_t channel)
{
  switch (channel)
  {
  case 0:
    globalObj->patternChannelSelector = globalObj->patternChannelSelector ^ 0b0001;
    break;
  case 1:
    globalObj->patternChannelSelector = globalObj->patternChannelSelector ^ 0b0010;
    break;
  case 2:
    globalObj->patternChannelSelector = globalObj->patternChannelSelector ^ 0b0100;
    break;
  case 3:
    globalObj->patternChannelSelector = globalObj->patternChannelSelector ^ 0b1000;
    break;
  }
}

void InputModule::channelButtonHandler(uint8_t channel)
{
  //uint8_t previous = globalObj->patternChannelSelector;

  // resetKnobValues();
  // if (globalObj->selectedChannel != channel){
  //   globalObj->selectedChannel = channel;
  //   return;
  // }
  //  changeState(min_max_cycle(++stepMode,STATE_PITCH0, STATE_CV2_OFFSET));
}

void InputModule::altButtonChannelHandler(uint8_t switchIndex)
{

  switch (globalObj->currentMenu)
  {
  case PATTERN_SELECT:
  case SAVE_MENU:
    globalObj->multiSelectSwitch = false;
    channelButtonChannelSelectorHandler(getChannelFromSw(switchIndex));
    break;
  case CALIBRATION_MENU:
    if (midplaneGPIO->pressed(SW_SHIFT))
    {
      calibrationSaveHandler();
    }
    else
    {
      calibrationMenuHandler();
    }
    break;
  default:

    if (midplaneGPIO->pressed(SW_SHIFT))
    {
      if (midplaneGPIO->pressed(SW_MENU))
      {
        channelButtonShiftMenuHandler(getChannelFromSw(switchIndex));
      }
      else
      {
        channelButtonShiftHandler(getChannelFromSw(switchIndex));
      }
    }
    else if (globalObj->selectedChannel == getChannelFromSw(switchIndex))
    {
      channelButtonOperationInProgress = true;
      switch (globalObj->currentMenu)
      {
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
    }
    else
    {
      if (globalObj->selectedChannel != getChannelFromSw(switchIndex))
      {
        globalObj->selectedChannel = getChannelFromSw(switchIndex);
        channelButtonOperationInProgress = true;
      }
    }
  }
}

void InputModule::altButtonPlayHandler()
{
  if (globalObj->currentMenu == SAVE_MENU)
  {
    for (int sw = 0; sw < 16; sw++)
    {
      if (midplaneGPIO->pressed(sw))
      {
        globalObj->patternCopyIndex = sw + globalObj->pattern_page * 16;
        display->displayModal(750, MODAL_COPY_PATTERN);
        return;
      }
    }
  }
  else if (globalObj->currentMenu != PATTERN_SELECT)
  {
    for (int sw = 0; sw < 16; sw++)
    {
      if (midplaneGPIO->pressed(sw))
      {
        globalObj->stepCopyIndex = getNote(sw);
        display->displayModal(750, MODAL_COPY_STEP);
        return;
      }
    }

    for (int ch = 0; ch < 4; ch++)
    {
      if (midplaneGPIO->pressed(getChannelButtonSw(ch)))
      {
        globalObj->chCopyIndex = ch;
        display->displayModal(750, MODAL_COPY_CHANNEL);
        return;
      }
    }

    if (midplaneGPIO->pressed(SW_PGUP))
    {
      skipPgUpRise = true;
      globalObj->copiedChannel = globalObj->selectedChannel;
      globalObj->copiedPage = notePage;
      // Serial.println("PAGEUP COPY pg:" + String(globalObj->copiedPage) + " ch:" + String(globalObj->copiedChannel));
      display->displayModal(750, MODAL_COPY_PAGE);

      return;
    }
    if (midplaneGPIO->pressed(SW_PGDN))
    {
      skipPgDnRise = true;
      globalObj->copiedChannel = globalObj->selectedChannel;
      globalObj->copiedPage = notePage;
      // Serial.println("PAGEDN COPY pg:" + String(notePage) + " ch:" + String(globalObj->selectedChannel));
      display->displayModal(750, MODAL_COPY_PAGE);

      return;
    }
  }

  globalObj->playing = !globalObj->playing;
}

void InputModule::altButtonRecHandler()
{
  if (globalObj->currentMenu == SAVE_MENU)
  {
    if (globalObj->patternCopyIndex < 128)
    {
      for (int sw = 0; sw < 16; sw++)
      {
        if (midplaneGPIO->pressed(sw))
        {
          globalObj->patternPasteIndex = sw + globalObj->pattern_page * 16;
          if (pattern_paste_clear_timer < 750)
          {
            for (int channel = 0; channel < 4; channel++)
            {
              saveFile->copySequenceData(channel, globalObj->patternCopyIndex, channel, globalObj->patternPasteIndex);
            }
            display->displayModal(750, MODAL_PASTE_PATTERN_CONFIRMED);
          }
          else
          {
            display->displayModal(750, MODAL_PASTE_PATTERN_ARMED);
            pattern_paste_clear_timer = 0;
          }
        }
      }
    }
  }
  else
  {
    if (globalObj->stepCopyIndex < 64)
    {
      for (int sw = 0; sw < 16; sw++)
      {
        if (midplaneGPIO->pressed(sw))
        {
          display->displayModal(750, MODAL_PASTE_STEP);
          sequenceArray[globalObj->selectedChannel].stepData[getNote(sw)] = sequenceArray[globalObj->selectedChannel].stepData[globalObj->stepCopyIndex];
        }
      }
    }
    if (globalObj->chCopyIndex < 4)
    {
      for (int ch = 0; ch < 4; ch++)
      {
        if (midplaneGPIO->pressed(getChannelButtonSw(ch)))
        {
          sequenceArray[ch] = sequenceArray[globalObj->chCopyIndex];
          sequenceArray[ch].channel = ch;
          display->displayModal(750, MODAL_PASTE_CHANNEL);
        }
      }
    }

    if (midplaneGPIO->pressed(SW_PGDN) || midplaneGPIO->pressed(SW_PGUP))
    {
      if (midplaneGPIO->pressed(SW_PGDN))
      {
        skipPgDnRise = true;
      }
      if (midplaneGPIO->pressed(SW_PGUP))
      {
        skipPgUpRise = true;
      }
      for (int index = 0; index < 16; index++)
      {
        sequenceArray[globalObj->selectedChannel].stepData[16 * notePage + index] = sequenceArray[globalObj->copiedChannel].stepData[16 * globalObj->copiedPage + index];
        // Serial.println("Pasting step " + String(16*globalObj->copiedPage+index) + " to step: " +  String(16*notePage+index) );
      }
      // Serial.println("Pasting channel " + String(globalObj->copiedChannel) + " pg " + String(globalObj->copiedPage) + " to ch " + String(globalObj->selectedChannel) + " pg " + String(notePage) + "skipPgDnRise:" + String(skipPgDnRise));
      display->displayModal(750, MODAL_PASTE_PAGE);
    }
  }
}
void InputModule::altButtonStopHandler()
{

  if (globalObj->currentMenu == SAVE_MENU)
  {
    for (int sw = 0; sw < 16; sw++)
    {
      if (midplaneGPIO->pressed(sw))
      {
        if (pattern_paste_clear_timer < 750)
        {
          for (int channel = 0; channel < 4; channel++)
          {
            saveFile->saveSequenceData(channel, sw + globalObj->pattern_page * 16, true);
          }
          display->displayModal(750, MODAL_CLEAR_PATTERN_CONFIRMED);
        }
        else
        {
          display->displayModal(750, MODAL_CLEAR_PATTERN_ARMED);
          pattern_paste_clear_timer = 0;
        }
      }
    }
  }
  else
  {
    for (int sw = 0; sw < 16; sw++)
    {
      if (midplaneGPIO->pressed(sw))
      {
        sequenceArray[globalObj->selectedChannel].initializeStep(sw);
        display->displayModal(750, MODAL_CLEAR_STEP);
        goto STOPEND;
      }
    }

    if (chPressedSelector && channel_erase_timer > 750)
    {
      channel_erase_timer = 0;
      display->displayModal(750, MODAL_ERASEARMED, chPressedSelector);
    }
    else if (chPressedSelector && channel_erase_timer < 750)
    {
      for (int i = 0; i < 4; i++)
      {
        if ((0b001 << i) & chPressedSelector)
        {
          sequenceArray[i].initNewSequence(currentPattern, i);
        }
      }
      display->displayModal(750, MODAL_ERASED, chPressedSelector);
    }
    else
    {
      if (globalObj->playing == false)
      {
        globalObj->chainModeActive = false;
      }
      globalObj->playing = false;
      for (int s = 0; s < SEQUENCECOUNT; s++)
      {
        sequenceArray[s].clockReset(true);
      }
    }
  }

  if (midplaneGPIO->pressed(SW_PGDN) || midplaneGPIO->pressed(SW_PGUP))
  {
    if (midplaneGPIO->pressed(SW_PGDN))
    {
      skipPgDnRise = true;
    }
    if (midplaneGPIO->pressed(SW_PGUP))
    {
      skipPgUpRise = true;
    }
    for (int index = 0; index < 16; index++)
    {
      sequenceArray[globalObj->selectedChannel].stepData[16 * notePage + index] = sequenceArray[globalObj->copiedChannel].stepData[16 * globalObj->copiedPage + index];
      Serial.println("Pasting step " + String(16 * globalObj->copiedPage + index) + " to step: " + String(16 * notePage + index));
    }
    Serial.println("Pasting channel " + String(globalObj->copiedChannel) + " pg " + String(globalObj->copiedPage) + " to ch " + String(globalObj->selectedChannel) + " pg " + String(notePage) + "skipPgDnRise:" + String(skipPgDnRise));
  }

STOPEND:;
}

void InputModule::altButtonPatternHandler()
{
  globalObj->multiSelectSwitch = false;

  if (globalObj->currentMenu == SYSEX_MENU)
  { //shift-pattern shortcut is used to export and import sysex data
    if (midplaneGPIO->pressed(SW_SHIFT))
    {
      switch (stepMode)
      {
      case STATE_SYSEX_EXPORT:
        globalObj->playing = 0;
        // globalObj->importExportDisplaySwitch = 1;
        display->displayModal(100, MODAL_EXPORTING);
        display->displayLoop(0);
        saveFile->exportSysexData();
        display->displayModal(1000, MODAL_EXPORTCOMPLETE);
        // globalObj->importExportDisplaySwitch = 2;
        break;
      case STATE_SYSEX_IMPORT:
        Serial.println("import button");
        globalObj->playing = 0;
        // display->displayModal(250, MODAL_PREPARING_SYSEX);
        globalObj->sysex_status = SYSEX_PREPARING;

        changeState(STATE_SYSEX_IMPORTINPROGRESS);
        display->displayLoop(0);
        saveFile->cacheWriteSwitch = true;
        while (!saveFile->are_all_cache_statuses_zeroed())
        {
          saveFile->cacheWriteLoop();
          // Serial.println("running cache write loop...");
        }
        delay(2000);
        
        changeState(STATE_SYSEX_IMPORTINPROGRESS);
        globalObj->sysex_status = SYSEX_READYFORDATA;

        break;
      }
    }
    return;
  }

  if (midplaneGPIO->pressed(SW_CH0) || midplaneGPIO->pressed(SW_CH1) || midplaneGPIO->pressed(SW_CH2) || midplaneGPIO->pressed(SW_CH3))
    return;

  switch (globalObj->currentMenu)
  {
  case PATTERN_SELECT:
    changeState(STATE_PITCH0);
    break;
  case SAVE_MENU:
    if (midplaneGPIO->pressed(SW_SHIFT))
    {
      saveFile->savePattern(globalObj->patternChannelSelector, globalObj->saveDestination);
      display->displayModal(750, MODAL_SAVE);
    }
    else
    {
      display->displayModal(1000, MODAL_DIDDNTSAVE);
    }
    changeState(STATE_PITCH0);
    break;

  default:
    if (midplaneGPIO->pressed(SW_SHIFT))
    {
      for (int i = 0; i < SEQUENCECOUNT; i++)
      {
        globalObj->saveDestination[i] = currentPattern;
      }
      changeState(STATE_SAVE);
    }
    else
    {
      globalObj->multiSelectSwitch = 0;
      changeState(STATE_PATTERNSELECT);
      heldButton == 255;
    }
    break;
  }
}

void InputModule::altButtonTempoHandler()
{
  if (midplaneGPIO->pressed(SW_SHIFT))
  {
    if (globalObj->currentMenu == GLOBAL_MENU_1)
    {
      changeState(STATE_CH1_VOLT_RANGE);
    }
    else if (globalObj->currentMenu == GLOBAL_MENU_2)
    {
      changeState(STATE_PITCH0);
    }
    else
    {
      changeState(STATE_PG_BTN_SWITCH);
    }
  }
  else
  {
    //  if (globalObj->currentMenu == SEQUENCE_MENU || globalObj->currentMenu == INPUT_MENU || globalObj->currentMenu == MOD_MENU_1 || globalObj->currentMenu == MOD_MENU_2){
    //    changeState(min_max_cycle(stepMode+1, STATE_STEPCOUNT , STATE_SKIPSTEP));
    //  } else {
    //    changeState(STATE_STEPCOUNT);
    //  }
    if (globalObj->currentMenu == TEMPO_MENU)
    {
      changeState(min_max_cycle(stepMode + 1, STATE_TEMPO, STATE_SKIPSTEP));
    }
    else
    {
      changeState(STATE_TEMPO);
    }
  }
}
void InputModule::altButtonShiftHandler()
{
  if (globalObj->currentMenu == SAVE_MENU)
  {
    Serial.println("change menu");
    changeState(STATE_PITCH0);
  }
};
void InputModule::altButtonPgupHandler()
{
  if (skipPgUpRise)
  {
    skipPgUpRise = false;
    return;
  }
  switch (globalObj->currentMenu)
  {
  case PATTERN_SELECT:
  case SAVE_MENU:
    globalObj->pattern_page = min_max(globalObj->pattern_page + 1, 0, 7);
    break;
  default:
    if (midplaneGPIO->pressed(SW_SHIFT))
    {
      changeState(STATE_PATTERN_CHAIN);
    }
    else
    {
      if (globalObj->pageButtonStyle)
      {
        notePage = positive_modulo(notePage + 1, 4);
      }
      else
      {
        notePage = positive_modulo(notePage - 1, 4);
      }
    }
    break;
  }
};
void InputModule::altButtonPgdnHandler()
{
  if (skipPgDnRise)
  {
    skipPgDnRise = false;
    return;
  }

  switch (globalObj->currentMenu)
  {
  case PATTERN_SELECT:
  case SAVE_MENU:
    globalObj->pattern_page = min_max(globalObj->pattern_page - 1, 0, 7);
    break;
  default:
    if (midplaneGPIO->pressed(SW_SHIFT))
    {
      if (globalObj->multiSelectSwitch)
      {
        globalObj->multiSelectSwitch = false;
        changeState(STATE_PITCH0);
      }
      else
      {
        globalObj->multiSelectSwitch = true;
        changeState(STATE_PITCH0);
      }
    }
    else
    {
      if (!globalObj->pageButtonStyle)
      {
        notePage = positive_modulo(notePage + 1, 4);
      }
      else
      {
        notePage = positive_modulo(notePage - 1, 4);
      }
    }
    break;
  }
};

bool InputModule::channelButtonShortcutHandler()
{
  // shortcut button loop
  uint8_t chanSwIndex;
  for (int chan = 0; chan < 4; chan++)
  {
    switch (chan)
    {
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
    if (midplaneGPIO->pressed(chanSwIndex))
    {
      for (int i = 0; i < 16; i++)
      {
        if (midplaneGPIO->fell(i))
        {
          globalObj->muteChannelSelect[0] = chPressedSelector & 0b0001;
          globalObj->muteChannelSelect[1] = chPressedSelector & 0b0010;
          globalObj->muteChannelSelect[2] = chPressedSelector & 0b0100;
          globalObj->muteChannelSelect[3] = chPressedSelector & 0b1000;
          channelButtonOperationInProgress = true;
          switch (i)
          {
          case SW_00:
            if (sequenceArray[chan].toggleMute(0))
            {
              display->displayModal(750, MODAL_MUTE_GATE);
            }
            else
            {
              display->displayModal(750, MODAL_UNMUTE_GATE);
            }
            break;
          case SW_01:
            if (sequenceArray[chan].toggleMute(1))
            {
              display->displayModal(750, MODAL_MUTE_CVA);
            }
            else
            {
              display->displayModal(750, MODAL_UNMUTE_CVA);
            }
            break;
          case SW_02:
            if (sequenceArray[chan].toggleMute(2))
            {
              display->displayModal(750, MODAL_MUTE_CVB);
            }
            else
            {
              display->displayModal(750, MODAL_UNMUTE_CVB);
            }
            break;
          case SW_03:
            changeState(STATE_SHORTCUT_FILL);
            break;
          case SW_04:
            changeState(STATE_SHORTCUT_CLKDIV);
            break;
          case SW_05:
            sequenceArray[chan].clockReset(true);
            display->displayModal(300, MODAL_SHORTCUT_RESET);
            break;
          case SW_06: // shortcut reverse
            switch (sequenceArray[chan].playMode)
            {
            case PLAY_PENDULUM:
              sequenceArray[chan].pendulumSwitch = !sequenceArray[chan].pendulumSwitch;
              break;
            case PLAY_FORWARD:
              sequenceArray[chan].playMode = PLAY_REVERSE;
              break;
            case PLAY_REVERSE:
              sequenceArray[chan].playMode = PLAY_FORWARD;
              break;
            }
            display->displayModal(300, MODAL_SHORTCUT_REVERSE);
            break;
          case SW_07:
            changeState(STATE_SHORTCUT_SKIP);
            break;
          case SW_08: // shortcut_random
            switch (globalObj->currentMenu)
            {
            case MENU_RANDOM:
              this->executeRandomization(chan);
              changeState(STATE_PITCH0);
              display->displayModal(300, MODAL_RANDOM_PITCH_GATE);
              break;
            default:
              changeState(STATE_SHORTCUT_RANDOM_PARAM);
            }
            break;
          case SW_09: // shortcut transpose
            changeState(STATE_SHORTCUT_TRANSPOSE);
            break;
          case SW_10:
            changeState(STATE_SHORTCUT_TUNER);
            break;
          case SW_11:
            changeState(STATE_SHORTCUT_PAGE);
            break;
          case SW_12:
            changeState(STATE_SHORTCUT_FN1);
            break;
          case SW_13:
            changeState(STATE_SHORTCUT_FN2);
            break;
          case SW_14:
            changeState(STATE_SHORTCUT_FN3);
            break;
          case SW_15:
            changeState(STATE_SHORTCUT_FN4);
            break;
          }
          return 1;
        }
      }
    }
  }
}

bool InputModule::altButtonHandler()
{
  if (globalObj->currentMenu == MENU_PATTERN_CHAIN)
  {
    return false;
  }

  chPressedSelector = 0;
  if (midplaneGPIO->pressed(SW_CH0))
  {
    chPressedSelector = chPressedSelector | 0b0001;
  }
  else
  {
    chPressedSelector = chPressedSelector & ~0b0001;
  }
  if (midplaneGPIO->pressed(SW_CH1))
  {
    chPressedSelector = chPressedSelector | 0b0010;
  }
  else
  {
    chPressedSelector = chPressedSelector & ~0b0010;
  }
  if (midplaneGPIO->pressed(SW_CH2))
  {
    chPressedSelector = chPressedSelector | 0b0100;
  }
  else
  {
    chPressedSelector = chPressedSelector & ~0b0100;
  }
  if (midplaneGPIO->pressed(SW_CH3))
  {
    chPressedSelector = chPressedSelector | 0b1000;
  }
  else
  {
    chPressedSelector = chPressedSelector & ~0b1000;
  }

  if (globalObj->currentMenu != PATTERN_SELECT && globalObj->currentMenu != SAVE_MENU)
  {
    // don't do channel shortcuts for pattern select or save menu
    if (this->channelButtonShortcutHandler())
    {
      return true;
    };
  }

  // non matrix button loop
  for (uint8_t i = 16; i < 28; i++)
  {
    if (midplaneGPIO->fell(i))
    {
      switch (i)
      {
      // left row bottom up
      case SW_CH0:
      case SW_CH1:
      case SW_CH2:
      case SW_CH3:
        this->altButtonChannelHandler(i);
        break;
      case SW_PLAY:
        this->altButtonPlayHandler();
        break;
      case SW_REC:
        this->altButtonRecHandler();
        break;
      case SW_STOP:
        this->altButtonStopHandler();
        break;
      case SW_PATTERN:
        this->altButtonPatternHandler();
        break;
      case SW_MENU:
        this->altButtonTempoHandler();
        break;
      case SW_SHIFT:
        this->altButtonShiftHandler();
        break;
      }
    }
    if (midplaneGPIO->rose(i))
    {
      //this section controls the behavior of the channel buttons on press
      // if(globalObj->currentMenu == PATTERN_SELECT || globalObj->currentMenu == SAVE_MENU) break;
      switch (i)
      {
      case SW_PGDN:
        this->altButtonPgdnHandler();
        break;
      case SW_PGUP:
        this->altButtonPgupHandler();
        break;

      // left row bottom up
      case SW_CH0:
      case SW_CH1:
      case SW_CH2:
      case SW_CH3:
        if (channelButtonOperationInProgress)
        {
          channelButtonOperationInProgress = false;
        }
        else
        {
          changeState(STATE_FIRSTSTEP);
        }
        break;
      }
      return true;
    }
  }
  //Serial.println("Return at the end of alt");

  return false;
}

void InputModule::channelPitchModeInputHandler()
{
  // globalObj->selectedStep == getNote(i) means that the user pressed the button that is selected.
  bool skipStepDataChangeSwitch = 0;
  for (int i = 0; i < 16; i++)
  {
    if (midplaneGPIO->fell(i))
    {
      // select the step
      globalObj->selectedStep = getNote(i);

      buttonMode = BUTTON_MODE_XOX;

      //  changeState(STATE_PITCH0);
      // i is the first step pressed
      // n is the second step pressed
      if (globalObj->playing)
      {
        //two butons are pressed simultaneously, loop between the buttons
        //play direction is dependent upon the order in which buttons are pressed.
        for (int n = 0; n < 16; n++)
        {
          if (i == n)
            continue;
          if (midplaneGPIO->pressed(n))
          {
            buttonMode = BUTTON_MODE_PLAYRANGE;
            if (lastselectedStep == globalObj->selectedStep && selectedStepTimer < DOUBLECLICKMS)
            {
              sequenceArray[globalObj->selectedChannel].setPlayRange(n, i);
              break;
            }
          }
        }
      }

      if (!globalObj->playing)
      {
        sequenceArray[globalObj->selectedChannel].stoppedTrig(globalObj->selectedStep, true, true);
      }
    }
    else if (midplaneGPIO->rose(i))
    {
      if (!globalObj->playing)
      {
        sequenceArray[globalObj->selectedChannel].stoppedTrig(getNote(i), false, false);
      }

      if (buttonMode == BUTTON_MODE_XOX)
      {
        // tap to turn a step on, double tap to turn step off
        // skip turning step on or of it was just copied
        globalObj->selectedStep = getNote(i);
        if (lastselectedStep == globalObj->selectedStep && selectedStepTimer < DOUBLECLICKMS)
        {
          sequenceArray[globalObj->selectedChannel].stepData[globalObj->selectedStep].gateType = GATETYPE_REST;
        }
        else
        {
          if (sequenceArray[globalObj->selectedChannel].stepData[globalObj->selectedStep].gateType == GATETYPE_REST)
          {
            sequenceArray[globalObj->selectedChannel].stepData[globalObj->selectedStep].gateType = GATETYPE_STEP;
          }
          //            sequenceArray[globalObj->selectedChannel].stepData[globalObj->selectedStep].gateLength = 1;
        }
      }

      lastselectedStep = globalObj->selectedStep;
      selectedStepTimer = 0;
    }
  }
  if (knobChange)
  {

    if ((globalObj->currentMenu == PITCH_GATE_MENU) || (globalObj->currentMenu == ARPEGGIO_MENU) || (globalObj->currentMenu == VELOCITY_MENU))
    {
      for (int i = 0; i < 16; i++)
      {
        if (midplaneGPIO->pressed(i))
        {
          sequenceArray[globalObj->selectedChannel].setStepPitch(getNote(i), min_max(sequenceArray[globalObj->selectedChannel].getStepPitch(getNote(i), 0) + knobChange, 0, 120), 0);
          skipStepDataChangeSwitch = true;
        }
      }
      if (skipStepDataChangeSwitch)
      {
        goto SKIPSTEPDATACHANGE;
      }
    }

    if (globalObj->parameterSelect)
    { // Encoder Switch
      changeState(min_max_cycle(stepMode + knobChange, STATE_PITCH0, STATE_CV2_OFFSET));
    }
    else
    {
      changeStepData(globalObj->selectedChannel, globalObj->selectedStep, knobChange);
    }
  SKIPSTEPDATACHANGE:;
  }
};

void InputModule::changeStepData(uint8_t channel, uint8_t stepNum, int change)
{
  //knobPrev = knobRead;
  uint8_t chrd;

  //      if (midplaneGPIO->pressed(globalObj->selectedStep%16) ){

  switch (stepMode)
  {
  case STATE_PITCH0:
    // just change the note

    //  if(sequenceArray[globalObj->selectedChannel].stepData[globalObj->selectedStep].gateType == GATETYPE_REST){
    //    // if a note is not active, turn it on and give it a length.
    //    sequenceArray[globalObj->selectedChannel].stepData[globalObj->selectedStep].gateType = GATETYPE_STEP;
    //    sequenceArray[globalObj->selectedChannel].stepData[globalObj->selectedStep].gateLength = 1;
    //  }
    // and finally set the new step value!
    // monophonic so pitch[0] only
    sequenceArray[channel].setStepPitch(stepNum, min_max(sequenceArray[channel].getStepPitch(stepNum, 0) + change, 0, globalObj->get_maximum_note_count(sequenceArray[channel].quantize_is_edo_scale, sequenceArray[channel].quantizeScale)), 0);
    if (!globalObj->playing)
    {
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
    sequenceArray[channel].setStepPitch(stepNum, chordArray[chrd][0], 1);
    sequenceArray[channel].setStepPitch(stepNum, chordArray[chrd][1], 2);
    sequenceArray[channel].setStepPitch(stepNum, chordArray[chrd][2], 3);
    break;

  case STATE_GATELENGTH:

    // change the gate type
    if ((sequenceArray[channel].stepData[stepNum].gateLength == 0) && (change < 0))
    {
      sequenceArray[channel].stepData[stepNum].gateType = GATETYPE_REST;
    }
    else if (change > 0)
    {
      if (sequenceArray[channel].stepData[stepNum].gateType == GATETYPE_REST)
      {
        sequenceArray[channel].stepData[stepNum].gateType = GATETYPE_STEP;
      }
    }
    if (sequenceArray[channel].stepData[stepNum].gateType > 0)
    {
      sequenceArray[channel].stepData[stepNum].gateLength = min_max(sequenceArray[channel].stepData[stepNum].gateLength + change, 0, 255);
      //  Serial.println("Setting Gatelength: " + String(sequenceArray[channel].stepData[stepNum].gateLength) + " change: " + String(change));
    }
    break;

  case STATE_GATETYPE:
    sequenceArray[channel].stepData[stepNum].gateType = min_max(sequenceArray[channel].stepData[stepNum].gateType + change, 0, 8);
    break;

  case STATE_GLIDE:
    sequenceArray[channel].stepData[stepNum].glide = min_max(sequenceArray[channel].stepData[stepNum].glide + change, 0, 127);
    break;

  case STATE_ARPTYPE:
    sequenceArray[channel].stepData[stepNum].arpType = min_max(sequenceArray[channel].stepData[stepNum].arpType + change, 0, 5);
    break;

  case STATE_ARPSPEEDNUM:
    sequenceArray[channel].stepData[stepNum].arpSpdNum = min_max(sequenceArray[channel].stepData[stepNum].arpSpdNum + change, 1, 16);
    break;
  case STATE_ARPSPEEDDEN:
    sequenceArray[channel].stepData[stepNum].arpSpdDen = min_max(sequenceArray[channel].stepData[stepNum].arpSpdDen + change, 1, 32);
    break;

  case STATE_ARPOCTAVE:
    sequenceArray[channel].stepData[stepNum].arpOctave = min_max(sequenceArray[channel].stepData[stepNum].arpOctave + change, 1, 5);
    break;
  case STATE_CV2_LEVEL:
    sequenceArray[channel].stepData[stepNum].velocity = min_max(sequenceArray[channel].stepData[stepNum].velocity + change, -127, 127);
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
    outputControl->cv2settingsChange(channel, stepNum, sequenceArray[channel].stepData[stepNum].velocity, sequenceArray[channel].stepData[stepNum].velocityType, sequenceArray[channel].stepData[stepNum].cv2speed, sequenceArray[channel].stepData[stepNum].cv2offset);
    break;
  }
}

void InputModule::transposeShortcutHandler()
{
  if (knobChange)
  {
    if (globalObj->parameterSelect)
    { // Encoder Switch
      changeState(min_max_cycle(stepMode + knobChange, STATE_SHORTCUT_RANDOM_PARAM, STATE_SHORTCUT_RANDOM_VAR2));
    }
    else
    {
      switch (stepMode)
      {
      case STATE_SHORTCUT_TRANSPOSE:
        //        globalObj->randomizeParamSelect = min_max(globalObj->randomizeParamSelect + knobChange, 0, 12);
        sequenceArray[globalObj->selectedChannel].transpose = min_max(sequenceArray[globalObj->selectedChannel].transpose + knobChange, -64, 64);
        break;
      }
    }
  }
}

void InputModule::executeRandomization(uint8_t channel)
{
  sequenceArray[channel].randomize(globalObj->randomizeParamSelect);
}

void InputModule::randomShortcutHandler()
{
  if (knobChange)
  {
    if (globalObj->parameterSelect)
    { // Encoder Switch
      switch (globalObj->randomizeParamSelect)
      {
      case RANDOMIZE_PARAM_CV2_SPEED:
        changeState(min_max_cycle(stepMode + knobChange, STATE_SHORTCUT_RANDOM_PARAM, STATE_SHORTCUT_RANDOM_VAR3));
        break;

      default:
        changeState(min_max_cycle(stepMode + knobChange, STATE_SHORTCUT_RANDOM_PARAM, STATE_SHORTCUT_RANDOM_VAR2));
        break;
      }
    }
    else
    {

      if (stepMode == STATE_SHORTCUT_RANDOM_PARAM)
      {
        globalObj->randomizeParamSelect = min_max(globalObj->randomizeParamSelect + knobChange, RANDOMIZE_PARAM_PITCHGATE, RANDOMIZE_PARAM_CV2_OFFSET);
      }
      else
      {
        switch (globalObj->randomizeParamSelect)
        {
        case RANDOMIZE_PARAM_PITCHGATE:
        case RANDOMIZE_PARAM_PITCH:
        case RANDOMIZE_PARAM_GATE:
          switch (stepMode)
          {
          case STATE_SHORTCUT_RANDOM_VAR1:
            globalObj->randomizeLow = min_max(globalObj->randomizeLow + knobChange, 0, 120);
            break;
          case STATE_SHORTCUT_RANDOM_VAR2:
            globalObj->randomizeSpan = min_max(globalObj->randomizeSpan + knobChange, 1, 8);
            break;
          case STATE_SHORTCUT_RANDOM_VAR3:
            break;
          }

          break;
        case RANDOMIZE_PARAM_CV2_TYPE:
          switch (stepMode)
          {
          case STATE_SHORTCUT_RANDOM_VAR1:
            globalObj->randomize_cv2_type = min_max(globalObj->randomize_cv2_type + knobChange, 0, 1);
            break;
          case STATE_SHORTCUT_RANDOM_VAR2:
            globalObj->randomize_cv2_type_include_skip = min_max(globalObj->randomize_cv2_type_include_skip + knobChange, 0, 1);
            break;
          case STATE_SHORTCUT_RANDOM_VAR3:
            break;
          }
          break;

        case RANDOMIZE_PARAM_CV2_SPEED:
          switch (stepMode)
          {
          case STATE_SHORTCUT_RANDOM_VAR1:
            globalObj->randomize_cv2_speedmin = min_max(globalObj->randomize_cv2_speedmin + knobChange, 1, globalObj->randomize_cv2_speedmax);
            break;
          case STATE_SHORTCUT_RANDOM_VAR2:
            globalObj->randomize_cv2_speedmax = min_max(globalObj->randomize_cv2_speedmax + knobChange, globalObj->randomize_cv2_speedmin, 255);
            break;
          case STATE_SHORTCUT_RANDOM_VAR3:
            globalObj->randomize_cv2_speedsync = min_max(globalObj->randomize_cv2_speedsync + knobChange, 1, 8);
            break;
          }
          break;
        case RANDOMIZE_PARAM_CV2_AMPLITUDE:
          switch (stepMode)
          {
          case STATE_SHORTCUT_RANDOM_VAR1:
            globalObj->randomize_cv2_amplitude_min = min_max(globalObj->randomize_cv2_amplitude_min + knobChange, -127, globalObj->randomize_cv2_amplitude_max);
            break;
          case STATE_SHORTCUT_RANDOM_VAR2:
            globalObj->randomize_cv2_amplitude_max = min_max(globalObj->randomize_cv2_amplitude_max + knobChange, globalObj->randomize_cv2_amplitude_min, 127);
            break;
          case STATE_SHORTCUT_RANDOM_VAR3:
            break;
          }
          break;
        case RANDOMIZE_PARAM_CV2_OFFSET:
          switch (stepMode)
          {
          case STATE_SHORTCUT_RANDOM_VAR1:
            globalObj->randomize_cv2_offset_min = min_max(globalObj->randomize_cv2_offset_min + knobChange, -127, globalObj->randomize_cv2_offset_max);
            break;
          case STATE_SHORTCUT_RANDOM_VAR2:
            globalObj->randomize_cv2_offset_max = min_max(globalObj->randomize_cv2_offset_max + knobChange, globalObj->randomize_cv2_offset_min, 127);
            break;
          case STATE_SHORTCUT_RANDOM_VAR3:
            break;
          }
          break;
        }
      }
      //       switch(stepMode){
      //         case STATE_SHORTCUT_RANDOM_PARAM:
      // //        globalObj->randomizeParamSelect = min_max(globalObj->randomizeParamSelect + knobChange, 0, 12);
      //           globalObj->randomizeParamSelect = min_max(globalObj->randomizeParamSelect + knobChange, RANDOMIZE_PARAM_PITCHGATE, RANDOMIZE_PARAM_CV2_SPEED);
      //         break;
      //         case STATE_SHORTCUT_RANDOM_VAR1:
      //           switch(globalObj->randomizeParamSelect){
      //             case RANDOMIZE_PARAM_PITCHGATE:
      //             case RANDOMIZE_PARAM_PITCH:
      //             case RANDOMIZE_PARAM_GATE:
      //               globalObj->randomizeLow = min_max(globalObj->randomizeLow + knobChange, 0, 120);
      //               break;

      //             case RANDOMIZE_PARAM_CV2_TYPE:
      //               globalObj->randomize_cv2_type = min_max(globalObj->randomize_cv2_type + knobChange, 0,1);
      //               break;

      //             case RANDOMIZE_PARAM_CV2_SPEED:
      //               globalObj->randomize_cv2_speedmin = min_max(globalObj->randomize_cv2_speedmin + knobChange, 1, 127);
      //               break;

      //           }
      //         break;
      //         case STATE_SHORTCUT_RANDOM_VAR2:
      //           switch(globalObj->randomizeParamSelect){
      //             case RANDOMIZE_PARAM_PITCHGATE:
      //             case RANDOMIZE_PARAM_PITCH:
      //             case RANDOMIZE_PARAM_GATE:
      //               globalObj->randomizeSpan = min_max(globalObj->randomizeSpan + knobChange, 1, 8);
      //               break;
      //             case RANDOMIZE_PARAM_CV2_SPEED:
      //               globalObj->randomize_cv2_speedmax = min_max(globalObj->randomize_cv2_speedmax + knobChange, 1, 127);
      //               break;
      //             case RANDOMIZE_PARAM_CV2_TYPE:
      //               globalObj->randomize_cv2_type_include_skip = min_max(globalObj->randomize_cv2_type_include_skip + knobChange, 0, 1);
      //               break;
      //           }
      //         break;
      //         case STATE_SHORTCUT_RANDOM_VAR2:

      //         break;
      //       }
    }
  }
}

void InputModule::tunerShortcutHandler(){

};
void InputModule::fillShortcutHandler(){

};
void InputModule::skipShortcutHandler()
{

  if (knobChange)
  {
    if (globalObj->parameterSelect)
    { // Encoder Switch
      changeState(min_max_cycle(stepMode + knobChange, STATE_SHORTCUT_SKIP, STATE_SHORTCUT_SKIP_TRIGGER));
    }
    else
    {
      switch (stepMode)
      {
      case STATE_SHORTCUT_SKIP:
        //        globalObj->randomizeParamSelect = min_max(globalObj->randomizeParamSelect + knobChange, 0, 12);
        sequenceArray[globalObj->selectedChannel].skipStepCount = min_max(sequenceArray[globalObj->selectedChannel].skipStepCount + knobChange, 0, 63);
        break;
      case STATE_SHORTCUT_SKIP_TRIGGER:
        sequenceArray[globalObj->selectedChannel].gpio_skipstep = min_max(sequenceArray[globalObj->selectedChannel].gpio_skipstep + knobChange, -9, 8);
        break;
      }
    }
  }
};

void InputModule::clkdivShortcutHandler(){

};
void InputModule::resetShortcutHandler(){

};
void InputModule::reverseShortcutHandler(){

};

void InputModule::saveMenuInputHandler()
{
  uint8_t chButtonMask = 0;
  for (int i = 0; i < 16; i++)
  {
    if (midplaneGPIO->fell(i))
    {

      if (midplaneGPIO->pressed(SW_CH0))
      {
        chButtonMask |= 0b0001;
      }
      if (midplaneGPIO->pressed(SW_CH1))
      {
        chButtonMask |= 0b0010;
      }
      if (midplaneGPIO->pressed(SW_CH2))
      {
        chButtonMask |= 0b0100;
      }
      if (midplaneGPIO->pressed(SW_CH3))
      {
        chButtonMask |= 0b1000;
      }

      // if channel buttons are pressed, matrix buttons sets individual channel destinations
      // if (chButtonMask & 0b0001) {
      //   saveDestination[0]=i;
      // };
      // if (chButtonMask & 0b0010) {
      //   saveDestination[1]=i;
      // };
      // if (chButtonMask & 0b0100) {
      //   saveDestination[2]=i;
      // };
      // if (chButtonMask & 0b1000) {
      //   saveDestination[3]=i;
      // };
      if (chButtonMask == 0)
      {
        //if no ch buttons are pressed, matrix buttons sets save destination for all channels
        globalObj->saveDestination[0] = i + globalObj->pattern_page * 16;
        globalObj->saveDestination[1] = i + globalObj->pattern_page * 16;
        globalObj->saveDestination[2] = i + globalObj->pattern_page * 16;
        globalObj->saveDestination[3] = i + globalObj->pattern_page * 16;
      }
    }
  }
};

void InputModule::debugScreenInputHandler()
{
  if (midplaneGPIO->pressed(SW_MENU))
  {
    //voltManual = positive_modulo(voltManual + 10*knobChange, 65535);
  }
  else
  {
    //voltManual = positive_modulo(voltManual + knobChange, 65535);
  }
}

void InputModule::resetKnobValues()
{
  knob.write(knob.read() % 4);
  knobRead = -1 * knob.read() / 2;
  knobPrevious = knobRead;
  //Serial.println("resetting knob: " + String(knob.read()));
};

void InputModule::calibrationSaveHandler()
{
  modalTimer = 0;
  saveFile->saveCalibrationEEPROM();
  changeState(STATE_CALIBRATION_SAVE_MODAL);
}

void InputModule::calibrationMenuHandler()
{
  //  uint8_t multiplier = 100;
  globalObj->playing = 0;

  uint32_t calibHigh;
  uint32_t calibLow;
  elapsedMillis timeoutTimer;
  channelButtonOperationInProgress = true;

  //if (midplaneGPIO->pressed(SW_REC)){
  //  multiplier = 10;
  ///  }
  switch (stepMode)
  {
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
    if (midplaneGPIO->pressed(SW_CH0))
    {
      globalObj->adcCalibrationOffset[0] = (globalObj->cvInputRaw[0] + 5 + 9 * globalObj->adcCalibrationOffset[0]) / 10;
    }
    if (midplaneGPIO->pressed(SW_CH1))
    {
      globalObj->adcCalibrationOffset[1] = (globalObj->cvInputRaw[1] + 5 + 9 * globalObj->adcCalibrationOffset[1]) / 10;
    }
    if (midplaneGPIO->pressed(SW_CH2))
    {
      globalObj->adcCalibrationOffset[2] = (globalObj->cvInputRaw[2] + 5 + 9 * globalObj->adcCalibrationOffset[2]) / 10;
    }
    if (midplaneGPIO->pressed(SW_CH3))
    {
      globalObj->adcCalibrationOffset[3] = (globalObj->cvInputRaw[3] + 5 + 9 * globalObj->adcCalibrationOffset[3]) / 10;
    }

    if (midplaneGPIO->pressed(SW_00))
    {
      globalObj->adcCalibrationNeg[0] = (globalObj->adcCalibrationNeg[0] * 9 + min_max(globalObj->adcCalibrationOffset[0] + ((globalObj->adcCalibrationOffset[0] - 5 - globalObj->cvInputRaw[0]) / 3) * 5, 0, 65535)) / 10;
      globalObj->adcCalibrationPos[0] = (globalObj->adcCalibrationPos[0] * 9 + min_max(globalObj->adcCalibrationOffset[0] - ((globalObj->adcCalibrationOffset[0] - 5 - globalObj->cvInputRaw[0]) / 3) * 5, 0, 65535)) / 10;
    }
    if (midplaneGPIO->pressed(SW_04))
    {
      globalObj->adcCalibrationNeg[1] = (globalObj->adcCalibrationNeg[1] * 9 + min_max(globalObj->adcCalibrationOffset[1] + ((globalObj->adcCalibrationOffset[1] - 5 - globalObj->cvInputRaw[1]) / 3) * 5, 0, 65535)) / 10;
      globalObj->adcCalibrationPos[1] = (globalObj->adcCalibrationPos[1] * 9 + min_max(globalObj->adcCalibrationOffset[1] - ((globalObj->adcCalibrationOffset[1] - 5 - globalObj->cvInputRaw[1]) / 3) * 5, 0, 65535)) / 10;
    }
    if (midplaneGPIO->pressed(SW_08))
    {
      globalObj->adcCalibrationNeg[2] = (globalObj->adcCalibrationNeg[2] * 9 + min_max(globalObj->adcCalibrationOffset[2] + ((globalObj->adcCalibrationOffset[2] - 5 - globalObj->cvInputRaw[2]) / 3) * 5, 0, 65535)) / 10;
      globalObj->adcCalibrationPos[2] = (globalObj->adcCalibrationPos[2] * 9 + min_max(globalObj->adcCalibrationOffset[2] - ((globalObj->adcCalibrationOffset[2] - 5 - globalObj->cvInputRaw[2]) / 3) * 5, 0, 65535)) / 10;
    }
    if (midplaneGPIO->pressed(SW_12))
    {
      globalObj->adcCalibrationNeg[3] = (globalObj->adcCalibrationNeg[3] * 9 + min_max(globalObj->adcCalibrationOffset[3] + ((globalObj->adcCalibrationOffset[3] - 5 - globalObj->cvInputRaw[3]) / 3) * 5, 0, 65535)) / 10;
      globalObj->adcCalibrationPos[3] = (globalObj->adcCalibrationPos[3] * 9 + min_max(globalObj->adcCalibrationOffset[3] - ((globalObj->adcCalibrationOffset[3] - 5 - globalObj->cvInputRaw[3]) / 3) * 5, 0, 65535)) / 10;
      Serial.println("RAW: " + String(globalObj->cvInputRaw[3]) + "\t1v: " + String((globalObj->adcCalibrationOffset[3] - globalObj->cvInputRaw[3]) / 3) + "\t5v: " + String(((globalObj->adcCalibrationOffset[3] - globalObj->cvInputRaw[0]) / 3) * 5));
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
    if (midplaneGPIO->pressed(SW_15))
    {
      for (int i = 0; i < 8; i++)
      {
        globalObj->dacCalibrationPos[dacMap[i]] = 65535;
        globalObj->dacCalibrationNeg[dacMap[i]] = 0;
      }
    }
    if (midplaneGPIO->pressed(SW_CH0))
    {
      Serial.println("Calibrating DAC");
      for (int n = 0; n < 8; n++)
      {
        for (int i = 0; i < 8; i++)
        {
          // set all DACs low
          outputControl->setDacVoltage(dacMap[i], 0);
        }
        delay(10);
        outputControl->setDacVoltage(dacMap[n], 65535);
        delay(100);
        outputControl->inputRead();
        delay(10);
        Serial.print("dac ch " + String(n) + " dacMap: " + String(dacMap[n]));
        for (int adc = 0; adc < 4; adc++)
        {
          Serial.print("  adc" + String(adc) + "\t: " + String(globalObj->cvInputMapped1024[adc]) + "\t");
          if (globalObj->cvInputMapped1024[adc] > 512)
          {

            Serial.println("DAC " + String(n) + " is connected to ADC " + String(adc) + "\t with val: " + String(globalObj->cvInputMapped1024[adc]) + "\tcalibrating....");
            // coarse tuning
            calibHigh = 65535;
            timeoutTimer = 0;
            while (globalObj->cvInputMapped1024[adc] > 512)
            {
              calibHigh = calibHigh - 100;
              outputControl->setDacVoltage(dacMap[n], calibHigh);
              delay(1);
              outputControl->inputRead();
              delay(1);
              if (timeoutTimer > 30000)
              {
                Serial.println("timeout, ending calibration routine");
                break;
              };
            };
            //fine adjustment
            calibHigh += 150;
            outputControl->setDacVoltage(dacMap[n], calibHigh);
            delay(5);
            outputControl->inputRead();
            delay(5);
            timeoutTimer = 0;
            while (globalObj->cvInputMapped1024[adc] > 512)
            {
              calibHigh = calibHigh - 1;
              outputControl->setDacVoltage(dacMap[n], calibHigh);
              delay(5);
              outputControl->inputRead();
              delay(5);
              if (timeoutTimer > 30000)
              {
                Serial.println("timeout, ending calibration routine");
                break;
              };
            };
            Serial.println("Calibration +2.5v found: " + String(calibHigh) + "\t adc value:  " + String(globalObj->cvInputMapped1024[adc]));
            // coarse tuning

            calibLow = 0;
            outputControl->setDacVoltage(dacMap[n], calibLow);
            delay(1);
            outputControl->inputRead();
            delay(1);
            timeoutTimer = 0;

            while (globalObj->cvInputMapped1024[adc] < -512)
            {
              calibLow = calibLow + 100;
              outputControl->setDacVoltage(dacMap[n], calibLow);
              delay(1);
              outputControl->inputRead();
              delay(1);
              if (timeoutTimer > 30000)
              {
                Serial.println("timeout, ending calibration routine");
                break;
              };
            };
            // fine adjustment
            calibLow -= 150;
            outputControl->setDacVoltage(dacMap[n], calibLow);
            delay(5);
            outputControl->inputRead();
            delay(5);

            while (globalObj->cvInputMapped1024[adc] < -512)
            {
              calibLow = calibLow + 1;
              outputControl->setDacVoltage(dacMap[n], calibLow);
              delay(5);
              outputControl->inputRead();
              delay(5);
              if (timeoutTimer > 30000)
              {
                Serial.println("timeout, ending calibration routine");
                break;
              };
            };

            Serial.println("Calibration -2.5v found: " + String(calibLow) + "\t adc value:  " + String(globalObj->cvInputMapped1024[adc]));
            //calibHigh = calibHigh * 100 / 102; //offset for impedance mismatch
            //calibLow = calibLow * 102 / 100; //offset for impedance mismatch
            globalObj->dacCalibrationPos[dacMap[n]] = (5 * calibHigh - 3 * calibLow) / 2;
            globalObj->dacCalibrationNeg[dacMap[n]] = (5 * calibLow - 3 * calibHigh) / 2;

            //int compensation = (globalObj->dacCalibrationPos[dacMap[n]] - globalObj->dacCalibrationNeg[dacMap[n]] )/115;  // kickstarter input impedance
            int compensation = (globalObj->dacCalibrationPos[dacMap[n]] - globalObj->dacCalibrationNeg[dacMap[n]]) / 500; // post kickstarter input impedance
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
  if (knobChange)
  {
    if (globalObj->parameterSelect)
    {
      changeState(min_max(stepMode + knobChange, STATE_CALIB_INPUT0_OFFSET, STATE_TEST_RHEOSTAT));
    }
    else
    {
      switch (stepMode)
      {
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

uint8_t InputModule::getChannelButtonSw(uint8_t channel)
{
  switch (channel)
  {
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

int InputModule::getChannelFromSw(int switchNum)
{
  switch (switchNum)
  {
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
