#include <Arduino.h>
#include "DisplayModule.h"

DisplayModule::DisplayModule(){
};

void DisplayModule::initialize(Sequencer *sequenceArray, MasterClock* clockMaster, GlobalVariable* globalObj, MidiModule *midiControl){

  Serial.println("Initializing Display");
  this->clockMaster = clockMaster;
  this->sequenceArray = sequenceArray;
  this->globalObj = globalObj;
  currentMenu = 255;
  oled.begin();
  delay(100);
  oled.setBrightness(16);
  oled.setTextWrap(false);
  //oled.setFont(&Font);//this will load the font
  oled.fillScreen(BLACK);     delay(10);
  oled.fillScreen(RED);     delay(10);
  oled.fillScreen(ORANGE);      delay(10);
  oled.fillScreen(YELLOW);      delay(10);
  oled.fillScreen(GREEN);     delay(10);
  oled.fillScreen(BLUE);      delay(10);
  oled.fillScreen(NAVY);      delay(10);
  oled.fillScreen(PURPLE);      delay(10);
  oled.fillScreen(RED);        delay(10);
  oled.fillScreen(ORANGE, YELLOW);
  oled.setFont(&OrbitBold_14);//this will load the font
  oled.setTextScale(1);
  oled.setCursor(0,0);
  oled.println("MAKE MUSIC");delay(10);
  oled.println("MAKE MUSIC");delay(10);
  oled.println("MAKE MUSIC");delay(10);
  oled.println("MAKE MUSIC");delay(10);
  oled.println("MAKE MUSIC");delay(10);
  oled.println("MAKE MUSIC");               delay(100);
  oled.fillScreen(GREEN, ORANGE);
  oled.setCursor(0,0);
  oled.println("NOT WAR"); delay(15);
  oled.println("NOT WAR"); delay(15);
  oled.println("NOT WAR"); delay(15);
  oled.println("NOT WAR"); delay(15);
  oled.println("NOT WAR"); delay(15);
  oled.println("NOT WAR"); delay(15);      delay(100);
  oled.fillScreen(BLUE, GREEN);      delay(50);
  oled.fillScreen(PURPLE, NAVY);       delay(50);
  oled.fillScreen(LIGHTPINK,LIGHTGREEN);  delay(50);
  oled.fillScreen(NAVY);
  oled.setCursor(CENTER,8);
  oled.setTextColor(WHITE);
  oled.setFont(&LadyRadical_16);//this will load the font
  oled.setTextScale(2);
  oled.println("Zetaohm");
  delay(50);
  oled.setFont(&NeueHaasXBlack_28);//this will load the font

  oled.setCursor(CENTER,35);
  oled.setTextColor(WHITE);
  oled.setTextScale(1);
  oled.println("FLXS1");
  oled.setFont(&flxs1_menu);//this will load the font
  oled.setTextScale(1);
  oled.setTextColor(WHITE);
  oled.setCursor(100,110);
  oled.println("firmware18a");
  this->midiControl = midiControl;
  //  delay(1000);
  Serial.println("Display Initialization Complete");
}

void DisplayModule::calibrationWarning(){
  oled.fillScreen(RED); delay(250);
  oled.fillScreen(WHITE); delay(250);
  oled.fillScreen(RED); delay(250);
  oled.fillScreen(WHITE); delay(250);
  oled.fillScreen(RED); delay(250);
  oled.fillScreen(WHITE); delay(250);
  oled.fillScreen(BLACK);

  oled.setCursor(0,35);
  oled.setTextColor(WHITE);
  oled.setTextScale(1);
  oled.println("CALIBRATION");
  oled.println("NOT FOUND");
  delay(500);
}

void DisplayModule::saveFileWarning(){
  oled.fillScreen(YELLOW); delay(250);
  oled.fillScreen(WHITE); delay(250);
  oled.fillScreen(YELLOW); delay(250);
  oled.fillScreen(WHITE); delay(250);
  oled.fillScreen(YELLOW); delay(250);
  oled.fillScreen(WHITE); delay(250);
  oled.fillScreen(BLACK);

  oled.setCursor(0,35);
  oled.setTextColor(WHITE);
  oled.setTextScale(1);
  oled.println("SAVE FILE");
  oled.println("NOT FOUND");
  delay(500);
}
void DisplayModule::clearDisplay(){
  oled.clearScreen();
//  oled.fillScreen(BLUE);
}

void DisplayModule::freeDisplayCache(){
  for (int i=0; i< MAX_DISPLAY_ELEMENTS; i++){
    //clear displaycache so all data redraws.
      free(displayCache[i]);
      displayCache[i] = nullptr;
      free(displayElement[i]);
      displayElement[i] = nullptr;
  }
}

void DisplayModule::cleanupTextBuffers(){
  for( int i=0; i< MAX_DISPLAY_ELEMENTS; i++ ){
    if (displayElement[i] == nullptr) {
      displayElement[i] = strdup("-");
    };
      free(displayCache[i]);
      displayCache[i] = nullptr;
      displayCache[i] = strdup(displayElement[i]);
      free(displayElement[i]);
      displayElement[i] = nullptr;
  };
};

void DisplayModule::displayLoop(uint16_t frequency) {

  if( displayTimer > frequency){

	  displayTimer = 0;

    switch (selectedChannel){
      case 0:
        foreground = ORANGE;
        background = BLACK;
        contrastColor = LIGHTORANGE;
        break;
      case 1:
        foreground = DARKGREEN;
        background = BLACK;
        contrastColor = LIGHTGREEN;
        break;
      case 2:
        foreground = PINK;
        background = BLACK;
        contrastColor = LIGHTPINK;
        break;
      case 3:
        foreground = RED;
        background = BLACK;
        contrastColor = LIGHTGREY;
        break;
    }


    if (modaltimer < modalMaxTime ) {
      modalDisplay();
    } else {
      if (previousMenu != currentMenu || previouslySelectedChannel != selectedChannel || modalRefreshSwitch){
         modalRefreshSwitch = false;
         freeDisplayCache();
         oled.fillScreen(background);
//         Serial.println("Changing Menu: " + String(currentMenu));
      }
      switch(currentMenu) {
        case PITCH_GATE_MENU:
          stateDisplay_pitch(buf);

          // if(globalObj->multiSelectSwitch){
          //   stateDisplay_pitchMulti(buf);
          // } else {
          //   stateDisplay_pitch(buf);
          // }

        break;

        case ARPEGGIO_MENU:
          stateDisplay_arp(buf);
        break;

        case VELOCITY_MENU:
          stateDisplay_velocity(buf);
        break;

        case SEQUENCE_MENU:
          sequenceMenuDisplay();
        break;

        case INPUT_MENU:
          inputMenuDisplay();
        break;

        case QUANTIZE_MENU:
          scaleMenuDisplay();
        break;

        case PATTERN_SELECT:
          patternSelectDisplay();
        break;

        case INPUT_DEBUG_MENU:
          inputDebugMenuDisplay();
        break;

        case TUNER_MENU:
          //channelTunerDisplay(buf);
        break;

        case GLOBAL_MENU_1:
          globalMenuDisplay();
        break;

        case GLOBAL_MENU_2:
          globalMenuDisplay2();
        break;

        case GLOBAL_MENU_3:
          globalMenuDisplay3();
        break;

        case TEMPO_MENU:
          tempoMenuDisplay();
        break;

        case CALIBRATION_MENU:
          if (stepMode < STATE_CALIB_OUTPUT0_LOW) {
            inputCalibrationMenuDisplay();
          } else if (stepMode == STATE_TEST_MIDI){
            midiTestDisplay();
          } else if (stepMode == STATE_TEST_GATES){
            gateTestDisplay();
          } else if (stepMode == STATE_TEST_RHEOSTAT){
            rheostatTestDisplay();
          } else {
            outputCalibrationMenuDisplay();
          }
        break;

        case MOD_MENU_1:
          modMenu1_DisplayHandler();
        break;

        case MOD_MENU_2:
          modMenu2_DisplayHandler();
        break;

        case NOTE_DISPLAY:
          noteDisplayHandler();
        break;

        case SAVE_MENU:
          saveMenuDisplayHandler();
          break;
        case MENU_MODAL:
          modalPopup();
        break;

        case MENU_MULTISELECT:
          multiSelectMenu(buf);
        break;

        case MENU_RANDOM:
          shortcutRandomMenu();
        break;

        case SYSEX_MENU:
          sysexMenuHandler();
          break;

        case MENU_PATTERN_CHAIN:
          patternChainMenuHandler();
        break;

        case MENU_CHAIN_HELP:
          patternChainHelpHandler();
        break;
      }

    }


    // if (previousMenu != currentMenu){
    //   Serial.println("finished first loop of displaying new state");
    // }
    cleanupTextBuffers();
    // if (previousMenu != currentMenu){
    //   Serial.println("text buffers cleaned up");
    //   Serial.println("Freeram: " + String(FreeRam2()));
    // }
    previouslySelectedChannel = selectedChannel;
    previousStepMode = stepMode;
    previousMenu = currentMenu;
    previousParameterSelect = globalObj->parameterSelect;
    globalObj->previousChainSelectedPattern = globalObj->chainSelectedPattern;
  };
};

void DisplayModule::displayModal(uint16_t ms, uint8_t select, uint8_t chSelector){
  displayModal(ms, select);
  this->chSelector = chSelector;
}

void DisplayModule::resetModalTimer(){
  modaltimer = 0;
}

void DisplayModule::displayModal(uint16_t ms, uint8_t select){
  chSelector = 0;
  modaltimer = 0;
  modalMaxTime = ms;
  modalRefreshSwitch = true;
  this->modalSelect = select;
//  Serial.println("resetting modal timer");
}

void DisplayModule::createChList(char * buf){
  uint8_t count = 0;
  uint8_t numbers[4] = {0,0,0,0};

  for(int i=0; i<4; i++){
    if(globalObj->muteChannelSelect[i]){
      numbers[count] = i+1;
    }
    count += globalObj->muteChannelSelect[i];
  }

  switch(count){
    case 1:
      sprintf(buf, "ch: %d", numbers[0]);
    break;
    case 2:
      sprintf(buf, "ch: %d,%d", numbers[0], numbers[1]);
    break;
    case 3:
      sprintf(buf, "ch: %d,%d,%d", numbers[0], numbers[1], numbers[2]);
    break;
    case 4:
      sprintf(buf, "ch: %d,%d,%d,%d", numbers[0], numbers[1], numbers[2], numbers[3]);
    break;
  }
}

void DisplayModule::modalDisplay(){
  //Serial.println("displaying modal");

  switch (modalSelect){
    case MODAL_ERASEARMED:
    displayElement[0] = strdup("press again");
    displayElement[1] = strdup("to clear");

    goto singleTextDisplay;
    case MODAL_ERASED:
      switch (chSelector){
        case 0b0001:
        displayElement[0] = strdup("Channel 1");
        displayElement[1] = strdup("CLEARED");
          break;
        case 0b0010:
        displayElement[0] = strdup("Channel 2");
        displayElement[1] = strdup("CLEARED");
          break;
        case 0b0011:
          displayElement[0] = strdup("CH1 + CH2");
          displayElement[1] = strdup("CLEARED");
          break;
        case 0b0100:
        displayElement[0] = strdup("Channel 3");
        displayElement[1] = strdup("CLEARED");
          break;
        case 0b0101:
        displayElement[0] = strdup("CH1 + CH3");
        displayElement[1] = strdup("CLEARED");
          break;
        case 0b0111:
        displayElement[0] = strdup("CH1 CH2 CH3");
        displayElement[1] = strdup("CLEARED");
          break;
        case 0b1000:
        displayElement[0] = strdup("Channel 4");
        displayElement[1] = strdup("CLEARED");
          break;
        case 0b1001:
        displayElement[0] = strdup("CH1 + CH4");
        displayElement[1] = strdup("CLEARED");
          break;
        case 0b1010:
        displayElement[0] = strdup("CH4 CH2");
        displayElement[1] = strdup("CLEARED");
          break;
        case 0b1011:
        displayElement[0] = strdup("CH4 CH2 CH1");
        displayElement[1] = strdup("CLEARED");
          break;
        case 0b1100:
        displayElement[0] = strdup("CH3 + CH4");
        displayElement[1] = strdup("CLEARED");
          break;
        case 0b1101:
        displayElement[0] = strdup("CH4 CH3 CH1");
        displayElement[1] = strdup("CLEARED");
          break;
        case 0b1111:
        displayElement[0] = strdup("all channels");
        displayElement[1] = strdup("cleared");
          break;      }

      goto singleTextDisplay;
    case MODAL_MUTE_GATE:
      this->createChList(buf);
      displayElement[1] = strdup(buf);
      displayElement[0] = strdup("MUTE GATE");
      goto singleTextDisplay;
    case MODAL_MUTE_CVA:
      this->createChList(buf);
      displayElement[1] = strdup(buf);
      displayElement[0] = strdup("MUTE CV A");
      goto singleTextDisplay;
    case MODAL_MUTE_CVB:
      this->createChList(buf);
      displayElement[1] = strdup(buf);
      displayElement[0] = strdup("MUTE CV B");
      goto singleTextDisplay;
    case MODAL_MUTE_CH4:
      displayElement[0] = strdup("CH4");
      displayElement[1] = strdup("MUTE");
      goto singleTextDisplay;
    case MODAL_UNMUTE_GATE:
      this->createChList(buf);
      displayElement[1] = strdup(buf);
      displayElement[0] = strdup("UNMUTE GATE");
      goto singleTextDisplay;
    case MODAL_UNMUTE_CVA:
      this->createChList(buf);
      displayElement[1] = strdup(buf);
      displayElement[0] = strdup("UNMUTE CV A");
      goto singleTextDisplay;
    case MODAL_UNMUTE_CVB:
      this->createChList(buf);
      displayElement[1] = strdup(buf);
      displayElement[0] = strdup("UNMUTE CV B");
      goto singleTextDisplay;
    case MODAL_UNMUTE_CH4:
      displayElement[0] = strdup("CH4");
      displayElement[1] = strdup("UNMUTE");
      goto singleTextDisplay;

    case MODAL_SELECT_CH1:
      displayElement[0] = strdup("CHANNEL 1");
      goto singleTextDisplay;
    case MODAL_SELECT_CH2:
      displayElement[0] = strdup("CHANNEL 2");
      goto singleTextDisplay;
    case MODAL_SELECT_CH3:
      displayElement[0] = strdup("CHANNEL 3");
      goto singleTextDisplay;
    case MODAL_SELECT_CH4:
      displayElement[0] = strdup("CHANNEL 4");
      goto singleTextDisplay;
    case MODAL_SAVE:
      displayElement[0] = strdup("SAVED");
      goto singleTextDisplay;
    case MODAL_COPY_STEP:
    sprintf(buf, "STEP %d COPY", globalObj->stepCopyIndex+1 );
      displayElement[0] = strdup(buf);
      goto singleTextDisplay;
    case MODAL_PASTE_STEP:
      displayElement[0] = strdup("STEP PASTE");
      goto singleTextDisplay;
    case MODAL_CLEAR_STEP:
      sprintf(buf, "STEP %d CLEAR", globalObj->stepCopyIndex+1 );
      displayElement[0] = strdup(buf);
      goto singleTextDisplay;
    case MODAL_COPY_CHANNEL:
      sprintf(buf, "CHANNEL %d", globalObj->chCopyIndex+1 );
      displayElement[0] = strdup(buf);
      displayElement[1] = strdup("COPY");
      goto singleTextDisplay;
    case MODAL_PASTE_CHANNEL:
      displayElement[0] = strdup("CHANNEL PASTE");
      goto singleTextDisplay;
    case MODAL_CLEAR_CHANNEL:
      sprintf(buf, "CHANNEL %d", globalObj->chCopyIndex+1 );
      displayElement[0] = strdup(buf);
      displayElement[1] = strdup("CHANNEL CLEAR");
      goto singleTextDisplay;
    case MODAL_RANDOM_PITCH_GATE:
      sprintf(buf, "CHANNEL %d", selectedChannel+1 );
      displayElement[0] = strdup(buf);
      displayElement[1] = strdup("RNDM PTCH&GT");
      goto singleTextDisplay;
    case MODAL_RANDOM_PITCH_GATE_WARNING:
      sprintf(buf, "CHANNEL %d", selectedChannel+1 );
      displayElement[0] = strdup("Confirm randomize");
      displayElement[1] = strdup(buf);
      goto singleTextDisplay;
    case MODAL_TOBEIMPLEMENTED:
      displayElement[0] = strdup("shortcut");
      displayElement[1] = strdup("coming soon");
      goto singleTextDisplay;
    case MODAL_SHORTCUT_RESET:
      sprintf(buf, "CHANNEL %d", selectedChannel+1 );
      displayElement[0] = strdup("Reset");
      displayElement[1] = strdup(buf);
      goto singleTextDisplay;
    case MODAL_SHORTCUT_REVERSE:
      sprintf(buf, "CHANNEL %d", selectedChannel+1 );
      switch(sequenceArray[selectedChannel].playMode){
        case PLAY_PENDULUM:
          displayElement[0] = strdup("Pendulum");
        break;
        case  PLAY_FORWARD:
          displayElement[0] = strdup("forward");
        break;
        case  PLAY_REVERSE:
          displayElement[0] = strdup("reverse");
        break;
      }
      break;
    case MODAL_EXPORTING:
      displayElement[0] = strdup("SYSEX EXPORT");
      displayElement[1] = strdup("exporting...");
      goto singleTextDisplay;
    break;

    case MODAL_EXPORTCOMPLETE:
      displayElement[0] = strdup("SYSEX EXPORT");
      displayElement[1] = strdup("complete...");
      goto singleTextDisplay;
    break;

    case MODAL_IMPORTING:
      displayElement[0] = strdup("SYSEX IMPORT");
      displayElement[1] = strdup("coming soon...");
      goto singleTextDisplay;
    break;

    case MODAL_IMPORTCOMPLETE:
      displayElement[0] = strdup("SYSEX IMPORT");
      displayElement[1] = strdup("complete...");
      goto singleTextDisplay;
    break;

      displayElement[1] = strdup(buf);
      goto singleTextDisplay;
      singleTextDisplay:

    //  if(!displayElement[0]){
    displayElement[5] = strdup(" ");

    if(strlen(displayElement[1]) < 1 ){
      renderStringBox(5,  DISPLAY_LABEL,    10,  30, 107, 26, true, MODALBOLD, BLACK , WHITE);
      renderStringBox(0,  DISPLAY_LABEL,    12,  36, 103, 18, false, MODALBOLD, BLACK , WHITE);
    } else {
      renderStringBox(5,  DISPLAY_LABEL,    10,  18, 107, 51, true, MODALBOLD, BLACK , WHITE);
      renderStringBox(0,  DISPLAY_LABEL,    12,  24, 103, 20, false, MODALBOLD, BLACK , WHITE);
      renderStringBox(1,  DISPLAY_LABEL,    12,  48, 103, 20, false, MODALBOLD, BLACK , WHITE);
    }
    //  } else {
    //    renderStringBox(0,  DISPLAY_LABEL,    18,  28, 96, 20, true, MODALBOLD, BLACK , WHITE);
    //    renderStringBox(1,  DISPLAY_LABEL,    18,  64, 96, 20, true, MODALBOLD, BLACK , WHITE);
    //  }
    break;
  }

}

void DisplayModule::renderStringBox(uint8_t index, uint8_t highlight, int16_t x, int16_t y, int16_t w, int16_t h, bool border, uint8_t textSize, uint16_t color, uint16_t bgColor){
  this->renderStringBox(index, highlight, x, y, w, h, border, textSize, color, bgColor, false);
};

void DisplayModule::renderStringBox(uint8_t index, uint8_t highlight, int16_t x, int16_t y, int16_t w, int16_t h, bool border, uint8_t textSize, uint16_t color, uint16_t bgColor, bool forceHighlight) {
  // renders a string box only once.
  uint16_t color1;
  uint16_t color2;

  bool refresh = 0;
  if( previousColor[index] != color){ refresh = 1 ;};
  previousColor[index] = color;
  if (strcmp(displayElement[index], displayCache[index]) != 0 ){ refresh = 1;};
  if (previousStepMode != highlight && highlight == stepMode)  { refresh = 1;};
  if (previousStepMode == highlight && highlight != stepMode)  { refresh = 1;};
  if (previousParameterSelect != globalObj->parameterSelect )  { refresh = 1;};
  if ((globalObj->chainSelectedPattern != globalObj->previousChainSelectedPattern)) {
    refresh = 1;
};


  if ( refresh ) {
    if ( (((highlight == stepMode) ) && !globalObj->parameterSelect)
    || forceHighlight
    || ((currentMenu == MENU_PATTERN_CHAIN) && (highlight == globalObj->chainSelectedPattern) && !globalObj->parameterSelect) )  {
      color1 = color;
      color2 = bgColor;
    } else {
      color1 = bgColor;
      color2 = color;
    }
  //  Serial.println("redrawing");

    oled.fillRect(x,y,w,h, color2);
    oled.setCursor(x, y);

    switch(textSize){
      case REGULARSPECIAL:
        oled.drawRect(x,y,w,h, color2);
        oled.setCursor(x+w/2, y+h/2, REL_XY);//now draw text in the middle of the rect
        oled.setFont(&a04b03);
        oled.setTextScale(1);
        break;

      case REGULAR1X:
        oled.setFont(&a04b03);
        oled.setTextScale(1);
        break;
      case REGULAR2X:
        oled.setFont(&a04b03);
        oled.setTextScale(2);
        break;
      case BOLD1X:
        oled.setFont(&a04b03);
        oled.setTextScale(1);
        break;
      case BOLD2X:
        oled.setCursor(CENTER,y+1);
        oled.setFont(&a04b03);
        oled.setTextScale(2);
        break;
      case BOLD4X:
        oled.setCursor(CENTER,y);
        oled.setFont(&a04b03);
        oled.setTextScale(4);
        break;
      case STYLE1X:
        oled.setCursor(x+2,y+2);
        oled.setFont(&flxs1_menu);
        oled.setTextScale(1);
        break;
      case MODALBOLD:
        oled.setTextWrap(false);
        oled.setCursor(CENTER,y);
        oled.setFont(&flxs1_menu);
        oled.setTextScale(1);
        break;
    }

    oled.setTextColor(color1);
    oled.print(displayElement[index]);
    if ((globalObj->parameterSelect && highlight == stepMode) || border ||
    (( (currentMenu == MENU_PATTERN_CHAIN) && (highlight == globalObj->chainSelectedPattern)) && globalObj->parameterSelect)  ){
      oled.drawRect(x,y,w,h, color1);
    }

  }
}


void DisplayModule::renderString(uint8_t index, uint8_t highlight, int16_t x, int16_t y, bool border, uint8_t textSize, uint16_t color, uint16_t bgColor) {
  // renders a string box only once.
  uint16_t color1;
  uint16_t color2;

  bool refresh = 0;
  if (strcmp(displayElement[index], displayCache[index]) != 0 ){ refresh = 1; };
  if( previousStepMode != highlight && highlight == stepMode){ refresh = 1;};
  if (previousStepMode == highlight && highlight != stepMode) {refresh = 1 ;};

  if ( refresh ) {
    if (highlight == stepMode){
      color1 = color;
      color2 = bgColor;
    } else {
      color1 = bgColor;
      color2 = color;
    }

    oled.setCursor(x, y);

    switch(textSize){
      case REGULAR1X:
        oled.setFont(&a04b03);
        oled.setTextScale(1);
        break;
      case REGULAR2X:
        oled.setFont(&a04b03);
        oled.setTextScale(2);
        break;
      case BOLD1X:
        oled.setFont(&a04b03);
        oled.setTextScale(1);
        break;
      case BOLD2X:
      oled.setCursor(CENTER,y+1);
        oled.setFont(&a04b03);
        oled.setTextScale(2);
        break;
      case BOLD4X:
        oled.setCursor(CENTER,y+1);
        oled.setFont(&a04b03);

        oled.setTextScale(4);
        break;
      case STYLE1X:
        oled.setCursor(x+1,y+1);
        oled.setFont(&a04b03);

        oled.setTextScale(2);
        break;
    }

    oled.setTextColor(color1);
    oled.print(displayElement[index]);
  }
}

void DisplayModule::multiSelectMenu(char* buf){
  sprintf(buf, "multi select" );

  displayElement[0] = strdup(buf);
  renderStringBox(0,  DISPLAY_LABEL,  0,  0, 128, 15, false, STYLE1X, background , contrastColor); //  digitalWriteFast(PIN_EXT_RX, LOW);

};


void DisplayModule::stateDisplay_pitch(char*buf){
  String gateTypeArray[] = { "off", "on", "tie", "1hit", "hold", "half", "rand33%", "rand50%", "rand66%" };

  if(globalObj->chainModeActive){
    sprintf(buf, "pt:%02d>%02d", sequenceArray[selectedChannel].pattern+1 , abs(globalObj->chainPatternSelect[globalObj->chainModeIndex]+1)+1);
  } else {
    sprintf(buf, "ch%d pt%02d", selectedChannel+1, sequenceArray[selectedChannel].pattern+1 );
  }
  displayElement[2] = strdup(buf);

  if(globalObj->chainModeActive){
    sprintf(buf, "ch%01d->%02dx",globalObj->chainModeMasterChannel[globalObj->chainModeIndex]+1, globalObj->chainPatternRepeatCount[globalObj->chainModeIndex] - globalObj->chainModeCount[globalObj->chainModeIndex]);
  } else {
    sprintf(buf, "stp:%02d-%02d",   notePage*16+1, (notePage+1)*16 );
  }
  displayElement[3] = strdup(buf);

  sprintf(buf, "gate%d", selectedChannel+1);
  displayElement[4] = strdup(buf);
  displayElement[6] = strdup("glide");
  displayElement[8] = strdup("type:");


  if(globalObj->multiSelectSwitch){
    sprintf(buf, "Multiselect");
      displayElement[0] = strdup(buf);
    if(globalObj->multi_pitch_switch){
      switch(sequenceArray[selectedChannel].quantizeScale){
        case COLUNDI:
          displayElement[1] = strdup(colundiNotes[min_max(globalObj->multi_pitch, 0, COLUNDINOTECOUNT)] );
        break;
        case SEMITONE:
          displayElement[1] = strdup(midiNotes[min_max(globalObj->multi_pitch, 0, 127)]);
        break;
        default:
        displayElement[1] = strdup(String(globalObj->multi_pitch).c_str());
      }
    } else {
      displayElement[1] = strdup("--");
    }

    if(globalObj->multi_gateLength_switch){
      if ( globalObj->multi_gateLength == 0 ){
        displayElement[5] = strdup("rest");
      } else {
        sprintf(buf, "%d.%02d", (globalObj->multi_gateLength+1)/4, (globalObj->multi_gateLength+1)%4*100/4  );
        displayElement[5] = strdup(buf);
      }
    } else {
      displayElement[5] = strdup("--");
    }

    if( globalObj->multi_gateType_switch ){
      displayElement[7] = strdup(gateTypeArray[globalObj->multi_gateType].c_str() );
    } else {
      displayElement[7] = strdup("--");
    }

    if (globalObj->multi_glide_switch){
      if (globalObj->multi_glide == 0) {
        sprintf(buf, "off");
      } else {
        sprintf(buf, "%d", globalObj->multi_glide);
      }
      displayElement[9] = strdup(buf);

    } else {
      displayElement[9] = strdup("--");
    }
  } else {
    if(globalObj->chainModeActive){
      sprintf(buf, "songmode");
    } else {
      sprintf(buf, "stepdata");
    }
    displayElement[0] = strdup(buf);

    switch(sequenceArray[selectedChannel].quantizeScale){
      case COLUNDI:
        displayElement[1] = strdup(colundiNotes[min_max(sequenceArray[selectedChannel].stepData[globalObj->selectedStep].pitch[0], 0, COLUNDINOTECOUNT)] );
      break;
      case SEMITONE:
        displayElement[1] = strdup(midiNotes[(uint16_t)globalObj->quantizeSemitonePitch(sequenceArray[selectedChannel].stepData[globalObj->selectedStep].pitch[0],  sequenceArray[selectedChannel].quantizeKey, sequenceArray[selectedChannel].quantizeMode, 0)]);
        //displayElement[1] = strdup(midiNotes[sequenceArray[selectedChannel].stepData[globalObj->selectedStep].pitch[0]]);
      break;
      default:
      displayElement[1] = strdup(String(sequenceArray[selectedChannel].stepData[globalObj->selectedStep].pitch[0]).c_str());
    }

    if ( sequenceArray[selectedChannel].stepData[globalObj->selectedStep].gateType == GATETYPE_REST ){
      displayElement[5] = strdup("rest");
    } else {
      sprintf(buf, "%d.%02d", (sequenceArray[selectedChannel].stepData[globalObj->selectedStep].gateLength+1)/4, (sequenceArray[selectedChannel].stepData[globalObj->selectedStep].gateLength+1)%4*100/4  );
      displayElement[5] = strdup(buf);
    }
    if (sequenceArray[selectedChannel].stepData[globalObj->selectedStep].glide == 0) {
      sprintf(buf, "off");
    } else {
      sprintf(buf, "%d", sequenceArray[selectedChannel].stepData[globalObj->selectedStep].glide);
    }
    displayElement[9] = strdup(buf);

    displayElement[7] = strdup(gateTypeArray[sequenceArray[selectedChannel].stepData[globalObj->selectedStep].gateType].c_str() );
  }
  if(globalObj->multiSelectSwitch || globalObj->chainModeActive ) {
    renderStringBox(0,  DISPLAY_LABEL,  0,  0, 86, 16, false, STYLE1X, contrastColor, background  ); //  digitalWriteFast(PIN_EXT_RX, LOW);
  } else {
    renderStringBox(0,  DISPLAY_LABEL,  0,  0, 86, 16, false, STYLE1X, background, contrastColor );
  }
  if(!globalObj->chainModeActive){
    renderStringBox(2,  DISPLAY_LABEL,  86,  0, 42, 8, false, REGULAR1X, background, contrastColor );
    renderStringBox(3,  DISPLAY_LABEL,  86,  8, 42, 8, false, REGULAR1X, background, contrastColor );
  } else {
    renderStringBox(2,  DISPLAY_LABEL,  86,  0, 42, 8, false, REGULAR1X, contrastColor, background );
    renderStringBox(3,  DISPLAY_LABEL,  86,  8, 42, 8, false, REGULAR1X, contrastColor, background );
  }

  renderStringBox(1,  STATE_PITCH0, 0, 17 , 127, 27, false, BOLD4X, background , foreground);     //  digitalWriteFast(PIN_EXT_RX, HIGH);

  renderStringBox(4,  DISPLAY_LABEL, 0,  47,63,16, false, STYLE1X, background , foreground); //  digitalWriteFast(PIN_EXT_RX, LOW);
  renderStringBox(8,  DISPLAY_LABEL, 0,  63,63,16, false, STYLE1X, background , foreground); //  digitalWriteFast(PIN_EXT_RX, HIGH);
  renderStringBox(6,  DISPLAY_LABEL, 0,  79,63,16, false, STYLE1X, background , foreground); //  digitalWriteFast(PIN_EXT_RX, LOW);

  renderStringBox(5,  STATE_GATELENGTH,63, 47,64,16, false, STYLE1X, background , foreground); //  digitalWriteFast(PIN_EXT_RX, HIGH);
  renderStringBox(7,  STATE_GATETYPE,  63, 63,64,16, false, STYLE1X, background , foreground); //  digitalWriteFast(PIN_EXT_RX, LOW);
  renderStringBox(9,  STATE_GLIDE,     63, 79,64,16, false, STYLE1X, background , foreground); //  digitalWriteFast(PIN_EXT_RX, HIGH);

};



void DisplayModule::stateDisplay_arp(char *buf){

  const char* const chordSelectionArray[] = { "unison", "maj", "min", "7th", "m7 ", "maj7", "m/maj7", "6th", "m6th", "aug", "flat5", "sus", "7sus4", "add9", "7#5", "m7#5", "maj7#5", "7b5",  "m7b5", "maj7b5", "sus2", "7sus2",  "dim7", "dim", "Ø7", "5th", "7#9" };
  const char*  const arpTypeArray[] = { "off","up","down","up dn 1","up dn 2","random" };

  displayElement[1]  = strdup("algo");
  displayElement[4] = strdup("speed");
  displayElement[8] = strdup("octve");
  displayElement[10] = strdup("intvl");

  if(globalObj->multiSelectSwitch) {
    sprintf(buf, "arpmulti");
    displayElement[0] = strdup(buf);
    sprintf(buf, "ch%d pt:%02d", selectedChannel+1, sequenceArray[selectedChannel].pattern+1 );
    displayElement[7] = strdup(buf);
    sprintf(buf, "p%d: %02d-%02d", notePage+1,  notePage*16+1, (notePage+1)*16 );
    displayElement[12] = strdup(buf);

    if(globalObj->multi_arpType_switch){
     displayElement[2] = strdup(arpTypeArray[globalObj->multi_arpType]);
    }  else {
     displayElement[2]  = strdup("--");
    }

    if(globalObj->multi_arpSpdNum_switch){
     sprintf(buf, "%d/", globalObj->multi_arpSpdNum);
     displayElement[5] = strdup(buf);
    } else {
     displayElement[5]  = strdup("-");
    }

    if(globalObj->multi_arpSpdDen_switch){
     sprintf(buf, "%d",  globalObj->multi_arpSpdDen);
     displayElement[6] = strdup(buf);
    } else {
     displayElement[6]  = strdup("-");
    }

    if(globalObj->multi_arpOctave_switch){
     sprintf(buf, "%doct", globalObj->multi_arpOctave);
     displayElement[9] = strdup(buf);
    } else {
     displayElement[9]  = strdup("--");
    }

   if(globalObj->multi_arpInterval_switch){
     displayElement[11] = strdup(chordSelectionArray[globalObj->multi_arpInterval]);
   } else {
     displayElement[11]  = strdup("--");
   }

  } else {
    sprintf(buf, "arpeggio");
    displayElement[0] = strdup(buf);
    sprintf(buf, "ch%d pt:%02d", selectedChannel+1, sequenceArray[selectedChannel].pattern+1 );
    displayElement[7] = strdup(buf);
    sprintf(buf, "p%d: %02d-%02d", notePage+1,  notePage*16+1, (notePage+1)*16 );
    displayElement[12] = strdup(buf);

    displayElement[2] = strdup(arpTypeArray[sequenceArray[selectedChannel].stepData[globalObj->selectedStep].arpType]);

    sprintf(buf, "%d/", sequenceArray[selectedChannel].stepData[globalObj->selectedStep].arpSpdNum);
    displayElement[5] = strdup(buf);
    sprintf(buf, "%d",  sequenceArray[selectedChannel].stepData[globalObj->selectedStep].arpSpdDen);
    displayElement[6] = strdup(buf);
    sprintf(buf, "%doct", sequenceArray[selectedChannel].stepData[globalObj->selectedStep].arpOctave);
    displayElement[9] = strdup(buf);
    displayElement[11] = strdup(chordSelectionArray[sequenceArray[selectedChannel].stepData[globalObj->selectedStep].chord]);
   }
    if(globalObj->multiSelectSwitch) {
      renderStringBox(0,  DISPLAY_LABEL,  0,  0, 86, 16, false, STYLE1X, contrastColor, background );
    } else {
      renderStringBox(0,  DISPLAY_LABEL,  0,  0, 86, 16, false, STYLE1X, background, contrastColor );
    }
    renderStringBox(7,  DISPLAY_LABEL,  86,  0, 42, 8, false, REGULAR1X, background, contrastColor );
    renderStringBox(12,  DISPLAY_LABEL,  86,  8, 42, 8, false, REGULAR1X, background, contrastColor );

    renderStringBox(1,  DISPLAY_LABEL,      0, 20,63,17, false, STYLE1X, background , foreground);
    renderStringBox(2,  STATE_ARPTYPE,     63, 20,64,17, false, STYLE1X, background , foreground);
    renderStringBox(4,  DISPLAY_LABEL,      0, 37,63,17, false, STYLE1X, background , foreground);
    renderStringBox(5,  STATE_ARPSPEEDNUM, 63, 37,32,17, false, STYLE1X, background , foreground);
    renderStringBox(6,  STATE_ARPSPEEDDEN, 95, 37,32,17, false, STYLE1X, background , foreground);

    renderStringBox(8,  DISPLAY_LABEL,      0, 54,63,17, false, STYLE1X, background , foreground);
    renderStringBox(9,  STATE_ARPOCTAVE,   63, 54,64,17, false, STYLE1X, background , foreground);

    renderStringBox(10,  DISPLAY_LABEL,     0, 71,63,17, false, STYLE1X, background , foreground);
    renderStringBox(11,  STATE_CHORD,      63, 71,64,17, false, STYLE1X, background , foreground);
 };


void DisplayModule::voltageToText(char *buf, int voltageValue){

  if(voltageValue < 0){
    voltageValue = abs(voltageValue);
    sprintf(buf, "-%d.%02dv",voltageValue*10/120, (1000*voltageValue)/120-100*(voltageValue*10/120) );
  } else {
    sprintf(buf, "%d.%02dv",voltageValue*10/120, (1000*voltageValue)/120-100*(voltageValue*10/120) );
  }
}

 void DisplayModule::stateDisplay_velocity(char *buf) {
   const char * const velTypeArray[] = { "skip","trigger","quantized", "voltage","Env Decay","Env Attack","Env AR","Env ASR","LFO Sine","LFO Tri","LFO Square", "LFO RndSq", "LFO SawUp","LFO SawDn","LFO S+H" };

       displayElement[1] = strdup("ampl:");
       displayElement[3] = strdup("type:");
       displayElement[5] = strdup("Speed:");
       displayElement[10] = strdup("offset:");

   if(globalObj->multiSelectSwitch){
     sprintf(buf, "lfoenvmult ");
      displayElement[0] = strdup(buf);
      sprintf(buf, "ch%d pt:%02d", selectedChannel+1, sequenceArray[selectedChannel].pattern+1 );
      displayElement[7] = strdup(buf);
      sprintf(buf, "p%d: %02d-%02d", notePage+1,  notePage*16+1, (notePage+1)*16 );
      displayElement[8] = strdup(buf);

    if(globalObj->multi_velocity_switch){
      voltageToText(buf,globalObj->multi_velocity);
      displayElement[2] = strdup(buf);
    } else {
      displayElement[2]  = strdup("--");
    }

   if(globalObj->multi_velocityType_switch){
       displayElement[4] = strdup(velTypeArray[globalObj->multi_velocityType]);
   } else {
       displayElement[4]  = strdup("--");
   }

    if(globalObj->multi_cv2speed_switch){
      sprintf(buf, "%d", globalObj->multi_cv2speed);
      displayElement[6] = strdup(buf);
    } else {
      displayElement[6]  = strdup("--");
    }

    if(globalObj->multi_cv2offset_switch){
      voltageToText(buf,globalObj->multi_cv2offset);
      displayElement[11] = strdup(buf);
    } else {
      displayElement[11]  = strdup("--");
    }

  } else {
    sprintf(buf, "LFO & ENV");
    displayElement[0] = strdup(buf);
    sprintf(buf, "ch%d pt:%02d", selectedChannel+1, sequenceArray[selectedChannel].pattern+1 );
    displayElement[7] = strdup(buf);
    sprintf(buf, "p%d: %02d-%02d", notePage+1,  notePage*16+1, (notePage+1)*16 );
    displayElement[8] = strdup(buf);
    if((sequenceArray[selectedChannel].stepData[globalObj->selectedStep].velocityType == 2) && (sequenceArray[selectedChannel].stepData[globalObj->selectedStep].velocity > 0)) {
      displayElement[2] = strdup(midiNotes[sequenceArray[selectedChannel].stepData[globalObj->selectedStep].velocity]);
    } else {
      voltageToText(buf,sequenceArray[selectedChannel].stepData[globalObj->selectedStep].velocity);
      displayElement[2] = strdup(buf);
    }
      displayElement[4] = strdup(velTypeArray[sequenceArray[selectedChannel].stepData[globalObj->selectedStep].velocityType]);
    //displayElement[4] = strdup(String(sequenceArray[selectedChannel].stepData[globalObj->selectedStep].velocityType).c_str());

    /*
    Speed of 128
    is 1/2 step wavelength
    which is 64/128
    speed of 64 is 1 step wavelength
    speed of 32 is 2 step wavelenth
    which is 64/32
    */
    // if (sequenceArray[selectedChannel].stepData[globalObj->selectedStep].velocityType > 6){
    //     displayElement[5] = strdup("Wavelnt:");
    //     sprintf(buf, "%d.%02d x",
    //       sequenceArray[selectedChannel].stepData[globalObj->selectedStep].cv2speed/64, (100*(sequenceArray[selectedChannel].stepData[globalObj->selectedStep].cv2speed%64))/64
    //    );
    //    displayElement[6] = strdup(buf);
    //
    // } else {
    //   displayElement[5] = strdup("EnvLng:");
    //   sprintf(buf, "%d", sequenceArray[selectedChannel].stepData[globalObj->selectedStep].cv2speed);
    //   displayElement[6] = strdup(buf);
    //
    // }
    sprintf(buf, "%d", sequenceArray[selectedChannel].stepData[globalObj->selectedStep].cv2speed);
    displayElement[6] = strdup(buf);

    voltageToText(buf,sequenceArray[selectedChannel].stepData[globalObj->selectedStep].cv2offset);

    displayElement[11] = strdup(buf);
  }

  if(globalObj->multiSelectSwitch){
    renderStringBox(0,  DISPLAY_LABEL,  0,  0, 86, 16, false, STYLE1X, contrastColor, background ); //  digitalWriteFast(PIN_EXT_RX, LOW);
  } else {
    renderStringBox(0,  DISPLAY_LABEL,    0,  0, 128, 15, false, STYLE1X , background, contrastColor);
  }
    renderStringBox(3,  DISPLAY_LABEL,        0, 20,57,17, false, STYLE1X, background , foreground);
    renderStringBox(1,  DISPLAY_LABEL,        0, 37,57,17, false, STYLE1X, background , foreground);
    renderStringBox(5,  DISPLAY_LABEL,        0, 54,57,17, false, STYLE1X, background , foreground);
    renderStringBox(10,  DISPLAY_LABEL,       0, 71,57,17, false, STYLE1X, background , foreground);
  //  renderStringBox(11,  DISPLAY_LABEL,       0, 88,63,17, false, STYLE1X, background , foreground);

    renderStringBox(4,  STATE_CV2_TYPE,      58, 20,69,17, false, STYLE1X, background , foreground);
    renderStringBox(2,  STATE_CV2_LEVEL,     58, 37,69,17, false, STYLE1X, background , foreground);
    renderStringBox(6,  STATE_CV2_SPEED,     58, 54,69,17, false, STYLE1X, background , foreground);

    renderStringBox(11,  STATE_CV2_OFFSET,   58, 71,69,17, false, STYLE1X, background , foreground);

}

 void DisplayModule::sequenceMenuDisplay(){

   sprintf(buf, "ch%d transport", selectedChannel+1);

   displayElement[0] = strdup(buf);

   displayElement[9] = strdup("first step:");
   sprintf(buf, "%d", sequenceArray[selectedChannel].firstStep+1);
   displayElement[10] = strdup(buf);

   displayElement[1] = strdup("step count:");

   sprintf(buf, "%d", sequenceArray[selectedChannel].stepCount);
   displayElement[2] = strdup(buf);

   displayElement[3] = strdup("clock div:");

   if (sequenceArray[selectedChannel].clockDivision == 1){
     sprintf(buf, "%d", sequenceArray[selectedChannel].clockDivision);
   } else if (sequenceArray[selectedChannel].clockDivision > 0){
     sprintf(buf, "1/%d", sequenceArray[selectedChannel].clockDivision);
   } else {
     sprintf(buf, "%d", abs(sequenceArray[selectedChannel].clockDivision)+2 );
   }
   displayElement[4] = strdup(buf);

   // if(sequenceArray[selectedChannel].skipStepCount == 0){
   //   sprintf(buf, "skip rndm:" );
   // } else {
   //   sprintf(buf, "skip %d:", sequenceArray[selectedChannel].skipStepCount);
   // }
   //
   // displayElement[5] = strdup(buf);
   //
   //  gateMappingText(buf, sequenceArray[selectedChannel].gpio_skipstep);
   //  displayElement[6] = strdup(buf);

   displayElement[5] = strdup("direction:");

  switch(sequenceArray[selectedChannel].playMode){
    case PLAY_FORWARD:
     displayElement[6] = strdup("FWD");
    break;
    case PLAY_REVERSE:
     displayElement[6] = strdup("REV");
    break;
    case PLAY_RANDOM:
     displayElement[6] = strdup("RNDM");
    break;
    case PLAY_PENDULUM:
     displayElement[6] = strdup("PEND");
    break;
  }

    displayElement[7] = strdup("swing:");

    sprintf(buf, "%d%%", sequenceArray[selectedChannel].swingX100);
    displayElement[8] = strdup(buf);

   renderStringBox(0,  DISPLAY_LABEL,    0,  0, 128, 15, false, STYLE1X, background , contrastColor);

   renderStringBox(9,  DISPLAY_LABEL,       0, 15,89,16, false, STYLE1X, background , foreground);
   renderStringBox(10,  STATE_FIRSTSTEP,    90, 15,37,16, false, STYLE1X, background , foreground);

   renderStringBox(1,  DISPLAY_LABEL,        0, 31,89,16, false, STYLE1X, background , foreground);
   renderStringBox(2,  STATE_STEPCOUNT,     90, 31,37,16, false, STYLE1X, background , foreground);

   renderStringBox(3,  DISPLAY_LABEL,        0, 47,89,16, false, STYLE1X, background , foreground);
   renderStringBox(4,  STATE_BEATCOUNT,     90, 47, 37,16, false, STYLE1X, background , foreground);

   renderStringBox(5,  DISPLAY_LABEL,   0,  63,89,16, false, STYLE1X, background , foreground);
   renderStringBox(6,  STATE_PLAYMODE,     90, 63,37,16, false, STYLE1X, background , foreground);

   renderStringBox(7,  DISPLAY_LABEL,        0, 79,50,16, false, STYLE1X, background , foreground);
   renderStringBox(8,  STATE_SWING,        90, 79,37,16, false, STYLE1X, background , foreground);
 }


 void DisplayModule::gateMappingText(char *buf, int8_t mapping){
   switch (mapping){
     case -9: sprintf(buf, "%d%%", 90 ); break;
     case -8: sprintf(buf, "%d%%", 80 ); break;
     case -7: sprintf(buf, "%d%%", 70 ); break;
     case -6: sprintf(buf, "%d%%", 60 ); break;
     case -5: sprintf(buf, "%d%%", 50 ); break;
     case -4: sprintf(buf, "%d%%", 40 ); break;
     case -3: sprintf(buf, "%d%%", 30 ); break;
     case -2: sprintf(buf, "%d%%", 20 ); break;
     case -1: sprintf(buf, "%d%%", 10 ); break;
     case 0: sprintf(buf, "off" ); break;
     case 1: sprintf(buf, "gt%d", 1 ); break;
     case 2: sprintf(buf, "gt%d", 2 ); break;
     case 3: sprintf(buf, "gt%d", 3 ); break;
     case 4: sprintf(buf, "gt%d", 4 ); break;
     case 5: sprintf(buf, "ch%da", 1 ); break;
     case 6: sprintf(buf, "ch%da", 2 ); break;
     case 7: sprintf(buf, "ch%da", 3 ); break;
     case 8: sprintf(buf, "ch%da", 4 ); break;
   }
 }

void DisplayModule::cvMappingText(char *buf, int8_t mapping){
  switch(mapping){
    case 0: sprintf(buf, "off"); break;
    case 1:
    case 2:
    case 3:
    case 4: sprintf(buf, "cv%d", mapping); break;
    case 5: sprintf(buf, "cv1A"); break;
    case 6: sprintf(buf, "cv1B"); break;
    case 7: sprintf(buf, "cv2A"); break;
    case 8: sprintf(buf, "cv2B"); break;
    case 9: sprintf(buf, "cv3A"); break;
    case 10: sprintf(buf, "cv3B"); break;
    case 11: sprintf(buf, "cv4a"); break;
    case 12: sprintf(buf, "cv4B"); break;
  }
};

void DisplayModule::scaleMenuDisplay(){
   sprintf(buf, "ch%d quantizer", selectedChannel+1);

     displayElement[0] = strdup(buf);

     displayElement[3] = strdup("scale:");
     const char * const scaleArray[] = {"semitone", "pythagorean", "colundi"};

      displayElement[4] = strdup(scaleArray[sequenceArray[selectedChannel].quantizeScale]);

     displayElement[5] = strdup("key:");
     const char * const keyArray[] = { "c","c#","d","d#","e","f","f#","g","g#","a","a#","b" };
     displayElement[6] = strdup(keyArray[sequenceArray[selectedChannel].quantizeKey]);
     displayElement[7] = strdup("mode:");
     //char *modeArray[] = {"chromatic", "ionian", "dorian", "phrygian", "lydian", "mixolydian", "aeolian", "locrian", "bluesmajor", "bluesminor", "pent_major", "pent_minor", "folk", "japanese", "gamelan", "gypsy", "arabian", "flamenco", "wholetone" };

     // switch(sequenceArray[selectedChannel].quantizeScale){
     //   case SEMITONE:
     //   case PYTHAGOREAN:
     //     displayElement[8] = strdup(modeArray[sequenceArray[selectedChannel].quantizeMode]);
     //   break;
     //   case COLUNDI:
     //     displayElement[8] = strdup("--");
     //   break;
     // }

    switch(sequenceArray[selectedChannel].quantizeMode){
      case SEMITONE_SCALE_12:     displayElement[8] = strdup("CHROMATIC"); break;
      case MAJOR_SCALE_12:        displayElement[8] = strdup("MAJOR"); break;
      case MINOR_SCALE_12:        displayElement[8] = strdup("MINOR"); break;
      case MAJORMINOR_SCALE_12:   displayElement[8] = strdup("MAJORMINOR"); break;
      case BLUESMAJOR_SCALE_12:   displayElement[8] = strdup("BLUESMAJOR"); break;
      case BLUESMINOR_SCALE_12:   displayElement[8] = strdup("BLUESMINOR"); break;
      case PENT_MAJOR_SCALE_12:   displayElement[8] = strdup("PENT_MAJOR"); break;
      case PENT_MINOR_SCALE_12:   displayElement[8] = strdup("PENT_MINOR"); break;
      case FOLK_SCALE_12:         displayElement[8] = strdup("FOLK"); break;
      case AEOLIAN_SCALE_12:      displayElement[8] = strdup("AEOLIAN"); break;
      case DORIAN_SCALE_12:       displayElement[8] = strdup("DORIAN"); break;
      case MIXOLYDIAN_SCALE_12:   displayElement[8] = strdup("MIXOLYDIAN"); break;
      case PHRYGIAN_SCALE_12:     displayElement[8] = strdup("PHRYGIAN"); break;
      case LYDIAN_SCALE_12:       displayElement[8] = strdup("LYDIAN"); break;
      case LOCRIAN_SCALE_12:      displayElement[8] = strdup("LOCRIAN"); break;
      case GAMELAN_SCALE_12:      displayElement[8] = strdup("GAMELAN"); break;
      case JAPANESE_SCALE_12:     displayElement[8] = strdup("JAPANESE"); break;
      case GYPSY_SCALE_12:        displayElement[8] = strdup("GYPSY"); break;
      case ARABIAN_SCALE_12:      displayElement[8] = strdup("ARABIAN"); break;
      case FLAMENCO_SCALE_12:     displayElement[8] = strdup("FLAMENCO"); break;
      case WHOLETONE_SCALE_12:    displayElement[8] = strdup("WHOLETONE"); break;
      default:                    displayElement[8] = strdup("CUSTOM"); break;
      }


     renderStringBox(0,  DISPLAY_LABEL,    0,  0, 128, 15, false, STYLE1X, background , contrastColor);

     renderStringBox(3,  DISPLAY_LABEL,           0, 15,48,16, false, STYLE1X, background , foreground);
     renderStringBox(4,  STATE_QUANTIZESCALE,    48, 15,79,16, false, STYLE1X, background , foreground);

     renderStringBox(5,  DISPLAY_LABEL,        0, 31,48,16, false, STYLE1X, background , foreground);
     renderStringBox(6,  STATE_QUANTIZEKEY,     48, 31,79,16, false, STYLE1X, background , foreground);

     renderStringBox(7,  DISPLAY_LABEL,        0, 47,48,16, false, STYLE1X, background , foreground);
     renderStringBox(8,  STATE_QUANTIZEMODE,     48, 47, 79,16, false, STYLE1X, background , foreground);

     // renderStringBox(9,  DISPLAY_LABEL,   0,  63,89,16, false, STYLE1X, background , foreground);
     // renderStringBox(10,  STATE_PLAYMODE,    72, 63, 55,16, false, STYLE1X, background , foreground);

};


void DisplayModule::inputMenuDisplay(){
  sprintf(buf, "CH%d INPUT MAP", selectedChannel+1);

  displayElement[0] = strdup(buf);
/*
  displayElement[1] = strdup("X-AXIS:");
  if (sequenceArray[selectedChannel].gpio_xaxis < 4){
   sprintf(buf, "GT%d", sequenceArray[selectedChannel].gpio_xaxis +1 );
   displayElement[2] = strdup(buf);
 } else {
   displayElement[2] = strdup("NONE");
 }
*/

 renderStringBox(0,  DISPLAY_LABEL,     0,    0, 128, 15, false, STYLE1X, background , contrastColor);
 renderStringBox(1,  DISPLAY_LABEL,     0,   16,  64,16, false, STYLE1X, background , foreground);
 //renderStringBox(2,  STATE_XAXISINPUT,  74,  16,  54,16, false, STYLE1X, background , foreground);
 //renderStringBox(3,  DISPLAY_LABEL,     0,   32,  64,16, false, STYLE1X, background , foreground);
 //renderStringBox(4,  STATE_SKIPSTEP,  74,  32,  54,16, false, STYLE1X, background , foreground);

}



 void DisplayModule::tempoMenuDisplay(){

   displayElement[0] = strdup("TEMPO");
   displayElement[2] = strdup("SYNC");
   displayElement[4] = strdup("RESET:");

  gateMappingText(buf, sequenceArray[selectedChannel].gpio_reset);
  displayElement[5] = strdup(buf);

 //
 //  displayElement[6] = strdup("Y-AXIS:");
 //  if (sequenceArray[selectedChannel].gpio_skipstep < 4){
 //   sprintf(buf, "GT%d", sequenceArray[selectedChannel].gpio_skipstep +1 );
 //   displayElement[7] = strdup(buf);
 // } else {
 //   displayElement[7] = strdup("NONE");
 // }

   switch(globalObj->clockMode){
     case INTERNAL_CLOCK:
       sprintf(buf, "%dbpm", int(globalObj->tempoX100/100) );
       displayElement[1] = strdup(buf);
       displayElement[3] = strdup("INTERNAL CLOCK");
     break;
     case EXTERNAL_MIDI_35_CLOCK:
       displayElement[1] = strdup("35MIDI");
       displayElement[3] = strdup("MIDI SYNC 3.5mm");
     break;
     case EXTERNAL_MIDI_USB_CLOCK:
       displayElement[1] = strdup("USBMID");
       displayElement[3] = strdup("MIDI SYNC USB");
     break;
     case EXTERNAL_CLOCK_GATE_0:
       displayElement[1] = strdup("EXT CLK");
       displayElement[3] = strdup("GATE INPUT 1");
     break;
     case EXTERNAL_CLOCK_GATE_1:
       displayElement[1] = strdup("EXT CLK");
       displayElement[3] = strdup("GATE INPUT 2");
     break;
     case EXTERNAL_CLOCK_GATE_2:
       displayElement[1] = strdup("EXT CLK");
       displayElement[3] = strdup("GATE INPUT 3");
     break;
     case EXTERNAL_CLOCK_GATE_3:
       displayElement[1] = strdup("EXT CLK");
       displayElement[3] = strdup("GATE INPUT 4");
     break;
     case EXTERNAL_CLOCK_BIDIRECTIONAL_INPUT:
       displayElement[1] = strdup("EXT CLK");
       displayElement[3] = strdup("CLOCK PORT");
     break;


   }
   renderStringBox(0,  DISPLAY_LABEL, 0, 0 , 128, 15, false, STYLE1X, background , contrastColor);

   renderStringBox(1,  STATE_TEMPO, 0, 16 , 127, 29, false, BOLD4X, background , foreground);

   renderStringBox(3,  STATE_EXTCLOCK, 0,  45,127,17, false, STYLE1X, background , foreground);
   ///renderStringBox(3,  STATE_EXTCLOCK,60, 45,68,17, false, STYLE1X, background , foreground);

   renderStringBox(4,  DISPLAY_LABEL, 0,  62,73,17, false, STYLE1X, background , foreground);

   renderStringBox(5,  STATE_RESETINPUT,  74, 62,53,17, false, STYLE1X, background , foreground);

  //  renderStringBox(6,  DISPLAY_LABEL,    0,  79,64,16, false, STYLE1X, background , foreground);
  //  renderStringBox(7,  STATE_SKIPSTEP,     74,  79,54,16, false, STYLE1X, background , foreground);

 }

 void DisplayModule::modMenu1_DisplayHandler(){
   sprintf(buf, "ch%d modulation 1", selectedChannel+1);
   displayElement[0] = strdup(buf);

   if(sequenceArray[selectedChannel].skipStepCount == 0){
     sprintf(buf, "skip rndm:" );
   }else {
     sprintf(buf, "skip -> %d:", sequenceArray[selectedChannel].skipStepCount);
   }

   displayElement[1] = strdup(buf);

    gateMappingText(buf, sequenceArray[selectedChannel].gpio_skipstep);
    displayElement[2] = strdup(buf);

   displayElement[3] = strdup("gate lngth:");

  cvMappingText(buf, sequenceArray[selectedChannel].cv_gatemod);
  displayElement[4] = strdup(buf);

 //  displayElement[3] = strdup("gatemute:");
 // gateMappingText(buf, sequenceArray[selectedChannel].gpio_gatemute);
 // displayElement[4] = strdup(buf);

 displayElement[5] = strdup("rndm pch");
 gateMappingText(buf, sequenceArray[selectedChannel].gpio_randompitch);
 displayElement[6] = strdup(buf);


 sprintf(buf, "add:%d", sequenceArray[selectedChannel].randomHigh);
 displayElement[11] = strdup(buf);
 sprintf(buf, "sub:%d", sequenceArray[selectedChannel].randomLow);
 displayElement[12] = strdup(buf);

displayElement[7] = strdup("transpose:");

// if (sequenceArray[selectedChannel].cv_pitchmod < 4){
//  sprintf(buf, "cv%d", sequenceArray[selectedChannel].cv_pitchmod +1 );
//  displayElement[8] = strdup(buf);
// } else {
//  displayElement[8] = strdup("off");
// }

cvMappingText(buf, sequenceArray[selectedChannel].cv_pitchmod);
displayElement[8] = strdup(buf);


displayElement[9] = strdup("glide time:");

cvMappingText(buf, sequenceArray[selectedChannel].cv_glidemod);
displayElement[10] = strdup(buf);

   renderStringBox(0,  DISPLAY_LABEL,      0,    0, 128, 15, false, STYLE1X, background , contrastColor);
   renderStringBox(1,  STATE_SKIPSTEPCOUNT, 0,   15,  82,16, false, STYLE1X, background , foreground);
   renderStringBox(2,  STATE_SKIPSTEP,     83,   15,  40,16, false, STYLE1X, background , foreground);
   renderStringBox(3,  DISPLAY_LABEL,      0,   31,  64,16, false, STYLE1X, background , foreground);
   renderStringBox(4,  STATE_GATEMOD,    83,   31,  40,16, false, STYLE1X, background , foreground);
   renderStringBox(5,  DISPLAY_LABEL,      0,   47,  64,16, false, STYLE1X, background , foreground);
   renderStringBox(6,  STATE_RANDOMPITCH, 96,   47,  31,16, false, STYLE1X, background , foreground);
   renderStringBox(11,  STATE_RANDOMHIGH, 67,   47,  29, 8, false, REGULAR1X, background , foreground);
   renderStringBox(12,  STATE_RANDOMLOW,  67,   55,  29, 8, false, REGULAR1X, background , foreground);


   renderStringBox(7,  DISPLAY_LABEL,      0,   63,  64,16, false, STYLE1X, background , foreground);
   renderStringBox(8,  STATE_PITCHMOD,    83,   63,  40,16, false, STYLE1X, background , foreground);
   renderStringBox(9,  DISPLAY_LABEL,      0,   79,  64,16, false, STYLE1X, background , foreground);
   renderStringBox(10,  STATE_GLIDEMOD,   83,   79,  40,16, false, STYLE1X, background , foreground);

 };

 void DisplayModule::modMenu2_DisplayHandler(){
   sprintf(buf, "ch%d modulation 2", selectedChannel+1);
   displayElement[0] = strdup(buf);

   displayElement[1] = strdup("arp type:");

  cvMappingText(buf, sequenceArray[selectedChannel].cv_arptypemod);
  displayElement[2] = strdup(buf);

  displayElement[3] = strdup("arp speed:");

 cvMappingText(buf, sequenceArray[selectedChannel].cv_arpspdmod);
 displayElement[4] = strdup(buf);

 displayElement[5] = strdup("arp octve:");

  cvMappingText(buf, sequenceArray[selectedChannel].cv_arpoctmod);
  displayElement[6] = strdup(buf);

displayElement[7] = strdup("arp intvl:");

  cvMappingText(buf, sequenceArray[selectedChannel].cv_arpintmod);
 displayElement[8] = strdup(buf);


 if(sequenceArray[selectedChannel].skipStepCount == 0){
   sprintf(buf, "skip rndm:" );
 } else {
   sprintf(buf, "skip %d:", sequenceArray[selectedChannel].skipStepCount);
 }

 displayElement[9] = strdup(buf);

  gateMappingText(buf, sequenceArray[selectedChannel].gpio_skipstep);
  displayElement[10] = strdup(buf);

/*
displayElement[9] = strdup("GLIDETIME:");

if (sequenceArray[selectedChannel].cv_glidemod < 4){
 sprintf(buf, "CV%d", sequenceArray[selectedChannel].cv_glidemod +1 );
 displayElement[10] = strdup(buf);
} else {
 displayElement[10] = strdup("OFF");
}
*/
   renderStringBox(0,  DISPLAY_LABEL,      0,    0, 128, 15, false, STYLE1X, background , contrastColor);
   renderStringBox(1,  DISPLAY_LABEL,      0,   15,  64,16, false, STYLE1X, background , foreground);
   renderStringBox(2,  STATE_ARPTYPEMOD,  80,   15,  47,16, false, STYLE1X, background , foreground);
   renderStringBox(3,  DISPLAY_LABEL,      0,   31,  64,16, false, STYLE1X, background , foreground);
   renderStringBox(4,  STATE_ARPSPDMOD,   80,   31,  47,16, false, STYLE1X, background , foreground);
   renderStringBox(5,  DISPLAY_LABEL,      0,   47,  64,16, false, STYLE1X, background , foreground);
   renderStringBox(6,  STATE_ARPOCTMOD,    80,   47,  47,16, false, STYLE1X, background , foreground);
   renderStringBox(7,  DISPLAY_LABEL,      0,   63,  64,16, false, STYLE1X, background , foreground);
   renderStringBox(8,  STATE_ARPINTMOD,    80,   63,  47,16, false, STYLE1X, background , foreground);
   //renderStringBox(9,  STATE_SKIPSTEPCOUNT, 0,   79,  64,16, false, STYLE1X, background , foreground);
   //renderStringBox(10,  STATE_SKIPSTEP,    90,   79,  37,16, false, STYLE1X, background , foreground);
   //renderStringBox(9,  DISPLAY_LABEL,      0,   79,  64,16, false, STYLE1X, background , foreground);
   //renderStringBox(10,  STATE_GLIDEMOD,   96,   79,  32,16, false, STYLE1X, background , foreground);
 };

 void DisplayModule::patternSelectDisplay(){

  highlight = currentPattern;

  uint8_t dispSelector;

  for(int pattern=0; pattern<16; pattern++){
    if(globalObj->fastChainModePatternCount > 1){
      if(globalObj->fastChainModePatternCount > pattern){
        sprintf(buf, "pt%d >", globalObj->fastChainPatternSelect[pattern]+1);
      } else {
        sprintf(buf, "--" );
      }
    } else {
      dispSelector = 0;
      for(int channel=0; channel<4; channel++){
        if(globalObj->savedSequences[channel][pattern]){
          dispSelector |= 1 << channel;
        }
       }
       switch(dispSelector){
         case 0b0000: sprintf(buf, "empty" ); break;
         case 0b0001: sprintf(buf, "1___" ); break;
         case 0b0010: sprintf(buf, "_2__" ); break;
         case 0b0011: sprintf(buf, "12__" ); break;
         case 0b0100: sprintf(buf, "__3_" ); break;
         case 0b0101: sprintf(buf, "1_3_" ); break;
         case 0b0110: sprintf(buf, "_23_" ); break;
         case 0b0111: sprintf(buf, "123_" ); break;
         case 0b1000: sprintf(buf, "___4" ); break;
         case 0b1001: sprintf(buf, "1__4" ); break;
         case 0b1010: sprintf(buf, "_2_4" ); break;
         case 0b1011: sprintf(buf, "12_4" ); break;
         case 0b1100: sprintf(buf, "__34" ); break;
         case 0b1101: sprintf(buf, "1_34" ); break;
         case 0b1111: sprintf(buf, "1234" ); break;
       }
    }
     displayElement[pattern]  = strdup(buf);
   }
   if(globalObj->fastChainModePatternCount > 1){
     displayElement[16] = strdup("CHAIN MODE");
   } else {
     displayElement[16] = strdup("PATTERN SELECT");
   }
  sprintf(buf, "current: %02d", currentPattern );
  sprintf(buf, "ch1:%02d ch2:%02d ch3:%02d ch4:%02d ", sequenceArray[0].pattern+1,sequenceArray[1].pattern+1, sequenceArray[2].pattern+1, sequenceArray[3].pattern+1 );

  sprintf(buf, "ch1:%02d", sequenceArray[0].pattern + 1);
  displayElement[17] = strdup(buf);
  sprintf(buf, "ch2:%02d", sequenceArray[1].pattern + 1);
  displayElement[18] = strdup(buf);
  sprintf(buf, "ch3:%02d", sequenceArray[2].pattern + 1);
  displayElement[19] = strdup(buf);
  sprintf(buf, "ch4:%02d", sequenceArray[3].pattern + 1);
  displayElement[20] = strdup(buf);

  displayElement[21] = strdup("trigger:");
  switch(globalObj->patternChangeTrigger){
    case PATTERN_CHANGE_IMMEDIATE: displayElement[22] = strdup("instant"); break;
    case PATTERN_CHANGE_CHANNEL_0: displayElement[22] = strdup("ch1 reset"); break;
    case PATTERN_CHANGE_CHANNEL_1: displayElement[22] = strdup("ch2 reset"); break;
    case PATTERN_CHANGE_CHANNEL_2: displayElement[22] = strdup("ch3 reset"); break;
    case PATTERN_CHANGE_CHANNEL_3: displayElement[22] = strdup("ch4 reset"); break;

  }

  uint8_t y = 46;
  uint8_t yoffset = 12;
  uint8_t x = 3;
  uint8_t xoffset = 30;
  uint8_t xoffset1 = 32;
  uint16_t colorA;
  uint16_t colorB;
  if(globalObj->fastChainModePatternCount > 1){
    renderStringBox(16, DISPLAY_LABEL, 0, 0, 128 , 16, false, STYLE1X,  LIGHTGREY, BLACK);
  } else {
    renderStringBox(16, DISPLAY_LABEL, 0, 0, 128 , 16, false, STYLE1X, BLACK, LIGHTGREY);
  }

  renderStringBox(21,  DISPLAY_LABEL,      0,   28,  53,16, false, STYLE1X, BLACK , LIGHTGREY);
  renderStringBox(22,  STATE_ARPTYPEMOD,  54,   28,  74,16, false, STYLE1X, LIGHTGREY , BLACK );

  if(globalObj->patternChannelSelector & 0b0001){ colorA = BLACK; colorB = LIGHTGREY; } else {colorA = LIGHTGREY; colorB = BLACK;};
  renderStringBox(17, DISPLAY_LABEL, 0*xoffset1, 16, 32 ,12, false, REGULARSPECIAL, colorB, colorA);
  if(globalObj->patternChannelSelector & 0b0010){ colorA = BLACK; colorB = LIGHTGREY; } else {colorA = LIGHTGREY; colorB = BLACK;};
  renderStringBox(18, DISPLAY_LABEL, 1*xoffset1, 16, 32 ,12, false, REGULARSPECIAL, colorB, colorA);
  if(globalObj->patternChannelSelector & 0b0100){ colorA = BLACK; colorB = LIGHTGREY; } else {colorA = LIGHTGREY; colorB = BLACK;};
  renderStringBox(19, DISPLAY_LABEL, 2*xoffset1, 16, 32 ,12, false, REGULARSPECIAL, colorB, colorA);
  if(globalObj->patternChannelSelector & 0b1000){ colorA = BLACK; colorB = LIGHTGREY; } else {colorA = LIGHTGREY; colorB = BLACK;};
  renderStringBox(20, DISPLAY_LABEL, 3*xoffset1, 16, 32 ,12, false, REGULARSPECIAL, colorB, colorA);

  x = 3;
  uint8_t textType;
  if(globalObj->fastChainModePatternCount > 1){ textType = REGULARSPECIAL; } else { textType = STYLE1X; }
  renderStringBox( 0, DISPLAY_LABEL, x+0*xoffset, y+0*yoffset, 31 , 13, false, REGULARSPECIAL, BLACK, ORANGE);
  renderStringBox( 1, DISPLAY_LABEL, x+1*xoffset, y+0*yoffset, 31 , 13, false, REGULARSPECIAL, BLACK, ORANGE);
  renderStringBox( 2, DISPLAY_LABEL, x+2*xoffset, y+0*yoffset, 31 , 13, false, REGULARSPECIAL, BLACK, ORANGE);
  renderStringBox( 3, DISPLAY_LABEL, x+3*xoffset, y+0*yoffset, 31 , 13, false, REGULARSPECIAL, BLACK, ORANGE);
  renderStringBox( 4, DISPLAY_LABEL, x+0*xoffset, y+1*yoffset, 31 , 13, false, REGULARSPECIAL, BLACK, ORANGE);
  renderStringBox( 5, DISPLAY_LABEL, x+1*xoffset, y+1*yoffset, 31 , 13, false, REGULARSPECIAL, BLACK, ORANGE);
  renderStringBox( 6, DISPLAY_LABEL, x+2*xoffset, y+1*yoffset, 31 , 13, false, REGULARSPECIAL, BLACK, ORANGE);
  renderStringBox( 7, DISPLAY_LABEL, x+3*xoffset, y+1*yoffset, 31 , 13, false, REGULARSPECIAL, BLACK, ORANGE);
  renderStringBox( 8, DISPLAY_LABEL, x+0*xoffset, y+2*yoffset, 31 , 13, false, REGULARSPECIAL, BLACK, ORANGE);
  renderStringBox( 9, DISPLAY_LABEL, x+1*xoffset, y+2*yoffset, 31 , 13, false, REGULARSPECIAL, BLACK, ORANGE);
  renderStringBox(10, DISPLAY_LABEL, x+2*xoffset, y+2*yoffset, 31 , 13, false, REGULARSPECIAL, BLACK, ORANGE);
  renderStringBox(11, DISPLAY_LABEL, x+3*xoffset, y+2*yoffset, 31 , 13, false, REGULARSPECIAL, BLACK, ORANGE);
  renderStringBox(12, DISPLAY_LABEL, x+0*xoffset, y+3*yoffset, 31 , 13, false, REGULARSPECIAL, BLACK, ORANGE);
  renderStringBox(13, DISPLAY_LABEL, x+1*xoffset, y+3*yoffset, 31 , 13, false, REGULARSPECIAL, BLACK, ORANGE);
  renderStringBox(14, DISPLAY_LABEL, x+2*xoffset, y+3*yoffset, 31 , 13, false, REGULARSPECIAL, BLACK, ORANGE);
  renderStringBox(15, DISPLAY_LABEL, x+3*xoffset, y+3*yoffset, 31 , 13, false, REGULARSPECIAL, BLACK, ORANGE);



 }

void DisplayModule::saveMenuDisplayHandler(){
  if(globalObj->prevPtrnChannelSelector != globalObj->patternChannelSelector){
    for (int i=0; i<8; i++){
      free(displayCache[i]);
      displayCache[i] = nullptr;
    }
  }
  globalObj->prevPtrnChannelSelector = globalObj->patternChannelSelector;


  displayElement[0] = strdup("ch1");
  displayElement[1] = strdup("ch2");
  displayElement[2] = strdup("ch3");
  displayElement[3] = strdup("ch4");
  if(globalObj->patternChannelSelector & 0b0001){
    sprintf(buf, "%02d", saveDestination[0]+1);
    displayElement[4] = strdup(buf);
  } else {
    displayElement[4] = strdup("--");
  }
  if(globalObj->patternChannelSelector & 0b0010){
    sprintf(buf, "%02d", saveDestination[1]+1);
    displayElement[5] = strdup(buf);
  } else {
    displayElement[5] = strdup("--");
  }
  if(globalObj->patternChannelSelector & 0b0100){
    sprintf(buf, "%02d", saveDestination[2]+1);
    displayElement[6] = strdup(buf);
  } else {
    displayElement[6] = strdup("--");
  }
  if(globalObj->patternChannelSelector & 0b1000){
    sprintf(buf, "%02d", saveDestination[3]+1);
    displayElement[7] = strdup(buf);
  } else {
    displayElement[7] = strdup("--");
  }

  displayElement[16] = strdup("PATTERN SAVE");
  displayElement[17] = strdup("CH BUTTONS -> MASK SAVE");
  displayElement[19] = strdup("4x4 MATRIX -> SET ALL INDEXES");
  displayElement[21] = strdup("CH + MATRIX -> SET CH INDEX");

  displayElement[18] = strdup("CHANNEL");

  displayElement[22] = strdup("SAVE DESTINATION INDEX");

  displayElement[20] = strdup("PATTERN->SAVE  SHIFT->EXIT ");

  renderStringBox(16, DISPLAY_LABEL, 0, 0, 128 , 16, false, STYLE1X, RED, BLACK);
  renderStringBox(17, DISPLAY_LABEL, 0, 16, 128 , 8, false, REGULAR1X, RED, BLACK);
  renderStringBox(19, DISPLAY_LABEL, 0, 24, 128 , 8, false, REGULAR1X, RED, BLACK);
  renderStringBox(21, DISPLAY_LABEL, 0, 32, 128 , 8, false, REGULAR1X, RED, BLACK);

  renderStringBox(18, DISPLAY_LABEL, 0, 40, 128 , 8, false, REGULAR1X, BLACK, PINK);

  if(globalObj->patternChannelSelector & 0b0001){
    renderStringBox( 0, DISPLAY_LABEL,  0, 48, 32 , 16, false, STYLE1X, BLACK, RED);
  } else {
    renderStringBox( 0, DISPLAY_LABEL,  0, 48, 32 , 16, false, STYLE1X, BLACK, DARK_GREY);
  }
  if(globalObj->patternChannelSelector & 0b0010){
    renderStringBox( 1, DISPLAY_LABEL, 32, 48, 32 , 16, false, STYLE1X, BLACK, GREEN);
  } else {
    renderStringBox( 1, DISPLAY_LABEL, 32, 48, 32 , 16, false, STYLE1X, BLACK, DARK_GREY);
  }
  if(globalObj->patternChannelSelector & 0b0100){
    renderStringBox( 2, DISPLAY_LABEL, 64, 48, 32 , 16, false, STYLE1X, BLACK, BLUE);
  } else {
    renderStringBox( 2, DISPLAY_LABEL, 64, 48, 32 , 16, false, STYLE1X, BLACK, DARK_GREY);
  }
  if(globalObj->patternChannelSelector & 0b1000){
    renderStringBox( 3, DISPLAY_LABEL, 96, 48, 32 , 16, false, STYLE1X, BLACK, PURPLE);
  } else {
    renderStringBox( 3, DISPLAY_LABEL, 96, 48, 32 , 16, false, STYLE1X, BLACK, DARK_GREY);
  }

  renderStringBox(22, DISPLAY_LABEL, 0, 64, 128 , 8, false, REGULAR1X, BLACK, PINK);


  renderStringBox( 4, DISPLAY_LABEL,  0, 72, 32 , 16, false, STYLE1X, BLACK, RED);
  renderStringBox( 5, DISPLAY_LABEL, 32, 72, 32 , 16, false, STYLE1X, BLACK, GREEN);
  renderStringBox( 6, DISPLAY_LABEL, 64, 72, 32 , 16, false, STYLE1X, BLACK, BLUE);
  renderStringBox( 7, DISPLAY_LABEL, 96, 72, 32 , 16, false, STYLE1X, BLACK, PURPLE);

  renderStringBox(20, DISPLAY_LABEL, 0, 88, 128 , 8, false, REGULAR1X, BLACK, PINK);


}
 void DisplayModule::noteDisplayHandler(){


   uint8_t currentStep = sequenceArray[selectedChannel].getActivePage()*16;
   for(int i = 0; i<16; i++){
     displayElement[i] = strdup(midiNotes[sequenceArray[selectedChannel].stepData[currentStep+i].pitch[0]]);
     renderStringBox( i, DISPLAY_LABEL, 32*(i%4), 16*(i/4+1), 32 , 16, false, STYLE1X, BLACK, GREEN);
   }

   displayElement[16] = strdup("NOTES:");
   renderStringBox( 16, DISPLAY_LABEL, 0, 0, 128 , 16, false, STYLE1X, BLACK, GREEN);


 }

void DisplayModule::modalPopup(){
  highlight = currentPattern;

  sprintf(buf, "Calibration Saved!");
  displayElement[0] = strdup(buf);

  renderStringBox(0, DISPLAY_LABEL,  0, 0, 128 , 8, false, REGULAR3X, BLACK, WHITE);
}


 void DisplayModule::inputCalibrationMenuDisplay(){
   highlight = currentPattern;
   oled.setTextWrap(true);

   sprintf(buf, "Input Calib: SHIFT-CH1 to save");
   displayElement[0] = strdup(buf);


   sprintf(buf, "CV IN 1: %d %d.%02dv %d", globalObj->cvInputMapped[0],    intFloatHundreds(globalObj->cvInputMapped1024[0]*500/1024),    intFloatTensOnes(globalObj->cvInputMapped1024[0]*500/1024), globalObj->cvInputMapped1024[0] );
   displayElement[1] = strdup(buf);
   sprintf(buf, "O:%d", globalObj->adcCalibrationOffset[0]);
   displayElement[2] = strdup(buf);
   sprintf(buf, "L:%d", globalObj->adcCalibrationPos[0]);
   displayElement[3] = strdup(buf);
   sprintf(buf, "H:%d", globalObj->adcCalibrationNeg[0]);
   displayElement[4] = strdup(buf);

   sprintf(buf, "CV IN 2: %d %d.%02dv %d", globalObj->cvInputMapped[1],    intFloatHundreds(globalObj->cvInputMapped1024[1]*500/1024),    intFloatTensOnes(globalObj->cvInputMapped1024[1]*500/1024), globalObj->cvInputMapped1024[1] );
   displayElement[5] = strdup(buf);
   sprintf(buf, "O:%d", globalObj->adcCalibrationOffset[1]);
   displayElement[6] = strdup(buf);
   sprintf(buf, "L: %d", globalObj->adcCalibrationPos[1]);
   displayElement[7] = strdup(buf);
   sprintf(buf, "H: %d", globalObj->adcCalibrationNeg[1]);
   displayElement[8] = strdup(buf);

   sprintf(buf, "CV IN 3: %d %d.%02dv %d", globalObj->cvInputMapped[2],    intFloatHundreds(globalObj->cvInputMapped1024[2]*500/1024),    intFloatTensOnes(globalObj->cvInputMapped1024[2]*500/1024), globalObj->cvInputMapped1024[2] );
   displayElement[9] = strdup(buf);
   sprintf(buf, "O:%d", globalObj->adcCalibrationOffset[2]);
   displayElement[10] = strdup(buf);
   sprintf(buf, "L: %d", globalObj->adcCalibrationPos[2]);
   displayElement[11] = strdup(buf);
   sprintf(buf, "H: %d", globalObj->adcCalibrationNeg[2]);
   displayElement[12] = strdup(buf);

   sprintf(buf, "CV IN 4: %d %d.%02dv %d", globalObj->cvInputMapped[3],    intFloatHundreds(globalObj->cvInputMapped1024[3]*500/1024),    intFloatTensOnes(globalObj->cvInputMapped1024[3]*500/1024), globalObj->cvInputMapped1024[3] );
   displayElement[13] = strdup(buf);
   sprintf(buf, "O:%d", globalObj->adcCalibrationOffset[3]);
   displayElement[14] = strdup(buf);
   sprintf(buf, "L: %d", globalObj->adcCalibrationPos[3]);
   displayElement[15] = strdup(buf);
   sprintf(buf, "H: %d", globalObj->adcCalibrationNeg[3]);
   displayElement[16] = strdup(buf);


   renderStringBox(0, DISPLAY_LABEL,  0, 0, 128 , 8, false, REGULAR1X, BLACK, WHITE);

   renderStringBox(1, DISPLAY_LABEL,              0,  16,128 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(2, STATE_CALIB_INPUT0_OFFSET,  0, 24, 42 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(3, STATE_CALIB_INPUT0_LOW,    42, 24, 42 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(4, STATE_CALIB_INPUT0_HIGH,   84, 24, 42 , 8, false, REGULAR1X, BLACK, WHITE);

   renderStringBox(5, DISPLAY_LABEL,               0, 32, 128, 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(6, STATE_CALIB_INPUT1_OFFSET,   0, 40, 42 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(7, STATE_CALIB_INPUT1_LOW,     42, 40, 42 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(8, STATE_CALIB_INPUT1_HIGH,    84, 40, 42 , 8, false, REGULAR1X, BLACK, WHITE);

   renderStringBox(9, DISPLAY_LABEL,               0, 48, 128, 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(10, STATE_CALIB_INPUT2_OFFSET,  0, 56, 42 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(11, STATE_CALIB_INPUT2_LOW,    42, 56, 42 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(12, STATE_CALIB_INPUT2_HIGH,   84, 56, 42 , 8, false, REGULAR1X, BLACK, WHITE);

   renderStringBox(13, DISPLAY_LABEL,              0, 64,  128 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(14, STATE_CALIB_INPUT3_OFFSET,  0, 72, 42 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(15, STATE_CALIB_INPUT3_LOW,    42, 72, 42 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(16, STATE_CALIB_INPUT3_HIGH,   84, 72, 42 , 8, false, REGULAR1X, BLACK, WHITE);

  //  renderStringBox(5, DISPLAY_LABEL,  0, 40, 128 , 8, false, REGULAR1X, WHITE, BLACK);
  //  renderStringBox(7, DISPLAY_LABEL,  0, 48, 128 , 8, false, REGULAR1X, WHITE, BLACK);
   //
  //  renderStringBox(8, DISPLAY_LABEL,  0, 56, 128 , 8, false, REGULAR1X, WHITE, BLACK);
  //  renderStringBox(9, DISPLAY_LABEL,   0, 62, 128 , 8, false, REGULAR1X, WHITE, BLACK);

 }

 void DisplayModule::outputCalibrationMenuDisplay(){
   oled.setTextWrap(true);
   sprintf(buf, "Ouput Calibration:\rSHIFT-CH1 to save");
   displayElement[0] = strdup(buf);


   sprintf(buf, "CV1A %d", globalObj->dacCalibrationNeg[dacMap[0]]);
   displayElement[1] = strdup(buf);
   sprintf(buf, ":%d", globalObj->dacCalibrationPos[dacMap[0]]);
   displayElement[2] = strdup(buf);


   sprintf(buf, "CV1B %d", globalObj->dacCalibrationNeg[dacMap[1]]);
   displayElement[4] = strdup(buf);
   sprintf(buf, ":%d", globalObj->dacCalibrationPos[dacMap[1]]);
   displayElement[5] = strdup(buf);


   sprintf(buf, "CV2A %d", globalObj->dacCalibrationNeg[dacMap[2]]);
   displayElement[7] = strdup(buf);
   sprintf(buf, ":%d", globalObj->dacCalibrationPos[dacMap[2]]);
   displayElement[8] = strdup(buf);


   sprintf(buf, "CV2B %d", globalObj->dacCalibrationNeg[dacMap[3]]);
   displayElement[10] = strdup(buf);
   sprintf(buf, ":%d", globalObj->dacCalibrationPos[dacMap[3]]);
   displayElement[11] = strdup(buf);


   sprintf(buf, "CV3A %d", globalObj->dacCalibrationNeg[dacMap[4]]);
   displayElement[13] = strdup(buf);
   sprintf(buf, ":%d", globalObj->dacCalibrationPos[dacMap[4]]);
   displayElement[14] = strdup(buf);


   sprintf(buf, "CV3B %d", globalObj->dacCalibrationNeg[dacMap[5]]);
   displayElement[16] = strdup(buf);
   sprintf(buf, ":%d", globalObj->dacCalibrationPos[dacMap[5]]);
   displayElement[17] = strdup(buf);


   sprintf(buf, "CV4A %d", globalObj->dacCalibrationNeg[dacMap[6]]);
   displayElement[19] = strdup(buf);
   sprintf(buf, ":%d", globalObj->dacCalibrationPos[dacMap[6]]);
   displayElement[20] = strdup(buf);


   sprintf(buf, "CV4B %d", globalObj->dacCalibrationNeg[dacMap[7]]);
   displayElement[22] = strdup(buf);
   sprintf(buf, ":%d", globalObj->dacCalibrationPos[dacMap[7]]);
   displayElement[23] = strdup(buf);


   renderStringBox(0, DISPLAY_LABEL,  0, 0, 128 , 8, false, REGULAR1X, BLACK, WHITE);

   renderStringBox(1, STATE_CALIB_OUTPUT0_LOW,      0, 16, 64 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(2, STATE_CALIB_OUTPUT0_HIGH,    63, 16, 64 , 8, false, REGULAR1X, BLACK, WHITE);

   renderStringBox(4, STATE_CALIB_OUTPUT1_LOW,      0, 24, 64 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(5, STATE_CALIB_OUTPUT1_HIGH,    63, 24, 64 , 8, false, REGULAR1X, BLACK, WHITE);

   renderStringBox(7, STATE_CALIB_OUTPUT2_LOW,      0, 32, 64 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(8, STATE_CALIB_OUTPUT2_HIGH,    63, 32, 64 , 8, false, REGULAR1X, BLACK, WHITE);

   renderStringBox(10, STATE_CALIB_OUTPUT3_LOW,      0, 40, 64 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(11, STATE_CALIB_OUTPUT3_HIGH,    63, 40, 64 , 8, false, REGULAR1X, BLACK, WHITE);

   renderStringBox(13, STATE_CALIB_OUTPUT4_LOW,      0, 48, 64 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(14, STATE_CALIB_OUTPUT4_HIGH,    63, 48, 64 , 8, false, REGULAR1X, BLACK, WHITE);

   renderStringBox(16, STATE_CALIB_OUTPUT5_LOW,      0, 56, 64 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(17, STATE_CALIB_OUTPUT5_HIGH,    63, 56, 64 , 8, false, REGULAR1X, BLACK, WHITE);

   renderStringBox(19, STATE_CALIB_OUTPUT6_LOW,      0, 64, 64 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(20, STATE_CALIB_OUTPUT6_HIGH,    63, 64, 64 , 8, false, REGULAR1X, BLACK, WHITE);

   renderStringBox(22, STATE_CALIB_OUTPUT7_LOW,      0, 72, 64 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(23, STATE_CALIB_OUTPUT7_HIGH,    63, 72, 64 , 8, false, REGULAR1X, BLACK, WHITE);
/*
CV1A   - OUT7
CV1B   - OUT1
CV2A   - OUT0
CV2B   - OUT6
CV3A   - OUT5
CV3B   - OUT2
CV4A  - OUT3
CV4B   - OUT4
*/

  //  STATE_CALIB_OUTPUT0_LOW
  //  STATE_CALIB_OUTPUT0_HIGH
  //  STATE_CALIB_OUTPUT1_LOW
  //  STATE_CALIB_OUTPUT1_HIGH
  //  STATE_CALIB_OUTPUT2_LOW
  //  STATE_CALIB_OUTPUT2_HIGH
  //  STATE_CALIB_OUTPUT3_LOW
  //  STATE_CALIB_OUTPUT3_HIGH
 }
 void DisplayModule::gateTestDisplay(){
   displayElement[0] = strdup("GATE TEST ");

   for(int i=0; i<4; i++){
     if(globalObj->gateTestArray[i] != 255){
       sprintf(buf, "Output %d <--> Input %d" , i+1, globalObj->gateTestArray[i]+1  );
       displayElement[i+1] = strdup(buf);
     } else {
       sprintf(buf, "Output %d disconnected" , i +1);
       displayElement[i+1] = strdup(buf);
     }
   }


   renderStringBox(0, DISPLAY_LABEL,  0, 0, 128 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(1, DISPLAY_LABEL,  0, 8, 128 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(2, DISPLAY_LABEL,  0, 16, 128 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(3, DISPLAY_LABEL,  0, 24, 128 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(4, DISPLAY_LABEL,  0, 32, 128 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(9, DISPLAY_LABEL,  0, 40, 128 , 56, false, REGULAR1X, BLACK, WHITE);

 }
 void DisplayModule::rheostatTestDisplay(){
   displayElement[0] = strdup("RHEO TEST  ");
   displayElement[1] = strdup("View CV outs with scope ");
   renderStringBox(0, DISPLAY_LABEL,  0, 0, 128 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(1, DISPLAY_LABEL,  0, 8, 128 , 88, false, REGULAR1X, BLACK, WHITE);

 }

void DisplayModule::midiTestDisplay(){
  if(midiTestActive){
    displayElement[0] = strdup("MIDI TEST  -- TESTING");
  } else {
    displayElement[0] = strdup("MIDI TEST  -- COMPLETE!");
  }

  for (size_t i = 0; i < 8; i++) {
    sprintf(buf, "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d" , midiControl->midiTestArray[16*i+0], midiControl->midiTestArray[16*i+1], midiControl->midiTestArray[16*i+2], midiControl->midiTestArray[16*i+3], midiControl->midiTestArray[16*i+4], midiControl->midiTestArray[16*i+5], midiControl->midiTestArray[16*i+6], midiControl->midiTestArray[16*i+7], midiControl->midiTestArray[16*i+8], midiControl->midiTestArray[16*i+9], midiControl->midiTestArray[16*i+10], midiControl->midiTestArray[16*i+11], midiControl->midiTestArray[16*i+12], midiControl->midiTestArray[16*i+13], midiControl->midiTestArray[16*i+14], midiControl->midiTestArray[16*i+15]);
    displayElement[i+1] = strdup(buf);
  }
  renderStringBox(0, DISPLAY_LABEL,  0, 0, 128 , 8, false, REGULAR1X, BLACK, WHITE);
  renderStringBox(1, DISPLAY_LABEL,  0, 8, 128 , 8, false, REGULAR1X, BLACK, WHITE);
  renderStringBox(2, DISPLAY_LABEL,  0, 16, 128 , 8, false, REGULAR1X, BLACK, WHITE);
  renderStringBox(3, DISPLAY_LABEL,  0, 24, 128 , 8, false, REGULAR1X, BLACK, WHITE);
  renderStringBox(4, DISPLAY_LABEL,  0, 32, 128 , 8, false, REGULAR1X, BLACK, WHITE);
  renderStringBox(5, DISPLAY_LABEL,  0, 40, 128 , 8, false, REGULAR1X, BLACK, WHITE);
  renderStringBox(6, DISPLAY_LABEL,  0, 48, 128 , 8, false, REGULAR1X, BLACK, WHITE);
  renderStringBox(7, DISPLAY_LABEL,  0, 56, 128 , 8, false, REGULAR1X, BLACK, WHITE);
  renderStringBox(8, DISPLAY_LABEL,  0, 64, 128 , 8, false, REGULAR1X, BLACK, WHITE);
  renderStringBox(9, DISPLAY_LABEL,  0, 72, 128 , 24, false, REGULAR1X, BLACK, WHITE);
}

 void DisplayModule::inputDebugMenuDisplay(){

   switch (selectedText){
     case 0:
        if (prevSelectedText != selectedText){
          oled.fillScreen(NAVY,RED);
        }
       displayElement[0] = strdup("HELLO");
       displayElement[1] = strdup("WORLD");

       renderString(0, DISPLAY_LABEL,  0, 20,  false, BOLD4X, BLACK, CYAN);
       renderString(1, DISPLAY_LABEL,  0, 48,  false, BOLD4X, BLACK, PINK);
      break;
      case 1:
         if (prevSelectedText != selectedText){
           oled.fillScreen(ORANGE,PINK);
         }
        displayElement[0] = strdup("TENKAI");
        displayElement[1] = strdup("KARIYA");

        renderString(0, DISPLAY_LABEL,  0, 20,  false, BOLD4X, BLACK, NAVY);
        renderString(1, DISPLAY_LABEL,  0, 48,  false, BOLD4X, BLACK, RED);
       break;
       case 2:
          if (prevSelectedText != selectedText){
            oled.fillScreen(NAVY,DARK_GREY);
          }
          displayElement[0] = strdup("modulation");
          displayElement[3] = strdup("input");
          displayElement[1] = strdup("4x GATE");
         displayElement[2] = strdup("4x CV");

         renderString(3, DISPLAY_LABEL,  0, 3,  false, BOLD2X, BLACK, YELLOW);
         renderString(0, DISPLAY_LABEL,  0, 18,  false, BOLD2X, BLACK, YELLOW);
         renderString(1, DISPLAY_LABEL,  0, 30,  false, BOLD4X, BLACK, YELLOW);
         renderString(2, DISPLAY_LABEL,  0, 55,  false, BOLD4X, BLACK, YELLOW);
        break;
        case 3:
           if (prevSelectedText != selectedText){
             oled.fillScreen(BLACK);
           }
          displayElement[0] = strdup("4 CHANNELS");
          displayElement[1] = strdup("64 STEPS");

          renderString(0, DISPLAY_LABEL,  0, 20,  false, BOLD2X, BLACK, YELLOW);
          renderString(1, DISPLAY_LABEL,  0, 48,  false, BOLD2X, BLACK, YELLOW);
         break;
   }
prevSelectedText = selectedText;
  //  renderStringBox(2, DISPLAY_LABEL,  0, 20, 96 , 8, false, REGULAR1X, WHITE, BLACK);
   //
  //  renderStringBox(3, DISPLAY_LABEL,  0, 30, 96 , 8, false, REGULAR1X, WHITE, BLACK);
  //  renderStringBox(4, DISPLAY_LABEL,  0, 40, 96 , 8, false, REGULAR1X, WHITE, BLACK);
   //
  //  renderStringBox(5, DISPLAY_LABEL,  0, 50, 96 , 8, false, REGULAR1X, WHITE, BLACK);
  //  renderStringBox(6, DISPLAY_LABEL,  0, 60, 96 , 8, false, REGULAR1X, WHITE, BLACK);
   //
  //  renderStringBox(7, DISPLAY_LABEL,  0, 70, 96 , 8, false, REGULAR1X, WHITE, BLACK);
  //  renderStringBox(8, DISPLAY_LABEL,  0, 80, 96 , 8, false, REGULAR1X, WHITE, BLACK);

 }


 void DisplayModule::globalMenuDisplay(){

   displayElement[0] = strdup("GLOBAL settings");

   displayElement[1] = strdup("pg up/dn: ");
   if(globalObj->pageButtonStyle){
     displayElement[2] = strdup("forward");
   } else {
     displayElement[2] = strdup("reverse");
   }

   displayElement[3] = strdup("data knb:");
   if(globalObj->dataInputStyle){
     displayElement[4] = strdup("toggle");
   } else {
     displayElement[4] = strdup("moment");
   }


//   displayElement[4] = strdup("moment");

   renderStringBox(0,  DISPLAY_LABEL,    0,  0,128, 15, false, STYLE1X, background , contrastColor);

   renderStringBox(1,  DISPLAY_LABEL,         0, 15, 95,16, false, STYLE1X, background , foreground);
   renderStringBox(2,  STATE_PG_BTN_SWITCH,   70, 15, 57,16, false, STYLE1X, background , foreground);

   renderStringBox(3,  DISPLAY_LABEL,              0, 31, 95,16, false, STYLE1X, background , foreground);
   renderStringBox(4,  STATE_DATA_KNOB_SWITCH,   70, 31, 57,16, false, STYLE1X, background , foreground);

 }

void DisplayModule::cvOutputRangeText(uint8_t dispElement, uint8_t outputRangeValue){
  switch(outputRangeValue){
    case 5:
      displayElement[dispElement] = strdup("-5v to 5v");
      break;
    case 4:
      displayElement[dispElement] = strdup("-4v to 6v");
      break;
    case 3:
      displayElement[dispElement] = strdup("-3v to 7v");
      break;
    case 2:
      displayElement[dispElement] = strdup("-2v to 8v");
      break;
    case 1:
      displayElement[dispElement] = strdup("-1v to 9v");
      break;
    case 0:
      displayElement[dispElement] = strdup("0v to 10v");
      break;
  }
}
 void DisplayModule::globalMenuDisplay2(){
   displayElement[0] = strdup("GLOBAL settings");

   displayElement[5] = strdup("pitch CV range");

   displayElement[6] = strdup("cv1a:");
   this->cvOutputRangeText(7, globalObj->outputNegOffset[0]);

   displayElement[8] = strdup("cv2a:");
   this->cvOutputRangeText(9, globalObj->outputNegOffset[1]);

   displayElement[10] = strdup("cv3a:");
   this->cvOutputRangeText(11, globalObj->outputNegOffset[2]);

   displayElement[12] = strdup("cv4a:");
   this->cvOutputRangeText(13, globalObj->outputNegOffset[3]);

   renderStringBox(0,  DISPLAY_LABEL,    0,  0,128, 15, false, STYLE1X, background , contrastColor);

   renderStringBox(5,  DISPLAY_LABEL,    0, 15, 90,17, false, STYLE1X, background , contrastColor);

   renderStringBox(6,  DISPLAY_LABEL,            0, 31, 32,16, false, STYLE1X, background , foreground);
   renderStringBox(7,  STATE_CH1_VOLT_RANGE,    48, 31, 79,16, false, STYLE1X, background , foreground);

   renderStringBox(8,  DISPLAY_LABEL,           0, 47, 32,16, false, STYLE1X, background , foreground);
   renderStringBox(9,  STATE_CH2_VOLT_RANGE,    48, 47, 79,16, false, STYLE1X, background , foreground);

   renderStringBox(10,  DISPLAY_LABEL,           0, 63, 32,16, false, STYLE1X, background , foreground);
   renderStringBox(11,  STATE_CH3_VOLT_RANGE,   48, 63, 79,16, false, STYLE1X, background , foreground);

   renderStringBox(12,  DISPLAY_LABEL,          0, 79, 32,16, false, STYLE1X, background , foreground);
   renderStringBox(13,  STATE_CH4_VOLT_RANGE,   48, 79, 79,16, false, STYLE1X, background , foreground);

}


 void DisplayModule::globalMenuDisplay3(){
   displayElement[0] = strdup("global settings");

   displayElement[5] = strdup("MIDI output channels");
   displayElement[6] = strdup("ch1:");

    sprintf(buf, "MIDI ch%d", globalObj->midiChannel[0] );
    displayElement[7] = strdup(buf);
   displayElement[8] = strdup("ch2:");
    sprintf(buf, "MIDI ch%d", globalObj->midiChannel[1]);
    displayElement[9] = strdup(buf);
   displayElement[10] = strdup("ch3:");
    sprintf(buf, "MIDI ch%d", globalObj->midiChannel[2] );
    displayElement[11] = strdup(buf);
   displayElement[12] = strdup("ch4:");
    sprintf(buf, "MIDI ch%d", globalObj->midiChannel[3] );
    displayElement[13] = strdup(buf);

   renderStringBox(0,  DISPLAY_LABEL,    0,  0,128, 15, false, STYLE1X, background , contrastColor);

   renderStringBox(5,  DISPLAY_LABEL,    0, 15, 90,17, false, STYLE1X, background , contrastColor);

   renderStringBox(6,  DISPLAY_LABEL,            0, 31, 32,16, false, STYLE1X, background , foreground);
   renderStringBox(7,  STATE_MIDI_CHANNEL_1,    48, 31, 79,16, false, STYLE1X, background , foreground);

   renderStringBox(8,  DISPLAY_LABEL,           0, 47, 32,16, false, STYLE1X, background , foreground);
   renderStringBox(9,  STATE_MIDI_CHANNEL_2,    48, 47, 79,16, false, STYLE1X, background , foreground);

   renderStringBox(10,  DISPLAY_LABEL,           0, 63, 32,16, false, STYLE1X, background , foreground);
   renderStringBox(11,  STATE_MIDI_CHANNEL_3,   48, 63, 79,16, false, STYLE1X, background , foreground);

   renderStringBox(12,  DISPLAY_LABEL,          0, 79, 32,16, false, STYLE1X, background , foreground);
   renderStringBox(13,  STATE_MIDI_CHANNEL_4,   48, 79, 79,16, false, STYLE1X, background , foreground);

}

void DisplayModule::sysexMenuHandler(){
  displayElement[0] = strdup("data Backup");

  switch(globalObj->importExportDisplaySwitch){
    case 0:
      displayElement[1] = strdup("sysex export");
      break;
    case 1:
      displayElement[1] = strdup("exporting..");
      break;
    case 2:
      displayElement[1] = strdup("export complete");
      break;
  }
  displayElement[2] = strdup("press shift+pattern to export");
  displayElement[3] = strdup("seq data to usb midi");

  displayElement[4] = strdup("sysex import");
   displayElement[5] = strdup("coming soon... ");
   displayElement[6] = strdup("");
   displayElement[7] = strdup("");
   displayElement[8] = strdup("");

  // displayElement[5] = strdup("press shift+pattern to import");
  // displayElement[6] = strdup("and replace all seq data");
  // displayElement[7] = strdup("this will delete and");
  // displayElement[8] = strdup("replace all sequences");

  renderStringBox(0,  DISPLAY_LABEL,        0,  0, 128,15, false, STYLE1X, background , contrastColor);
  renderStringBox(1,  STATE_SYSEX_EXPORT,   0, 15, 127,16, false, STYLE1X, background , foreground);
  renderStringBox(2,  DISPLAY_LABEL,        0, 31, 127,8,  false, REGULAR1X, background , foreground);
  renderStringBox(3,  DISPLAY_LABEL,        0, 39, 127,8,  false, REGULAR1X, background , foreground);
  renderStringBox(4,  STATE_SYSEX_IMPORT,   0, 47, 127,16, false, STYLE1X, background , foreground);
  renderStringBox(5,  DISPLAY_LABEL,        0, 63, 127,8,  false, REGULAR1X, background , foreground);
  renderStringBox(6,  DISPLAY_LABEL,        0, 71, 127,8,  false, REGULAR1X, background , foreground);
  renderStringBox(7,  DISPLAY_LABEL,        0, 79, 127,8,  false, REGULAR1X, background , foreground);
  renderStringBox(8,  DISPLAY_LABEL,        0, 87, 127,8,  false, REGULAR1X, background , foreground);
};

void DisplayModule::patternChainHelpHandler(){
  displayElement[0] = strdup("SONG HELP");

  displayElement[1]  = strdup( "a song is divided into events");
  displayElement[2]  = strdup( "press+turn knob: slect event");
  displayElement[3]  = strdup( "turn knob: slect repeat count.");
  displayElement[4]  = strdup( "repeat values below 1 include:");
  displayElement[5]  = strdup( "repeat from start");
  displayElement[6]  = strdup( "stop - song ends, play stops");
  displayElement[7]  = strdup( "Jump to index: jumps to a ");
  displayElement[8]  = strdup( "different song event X times");
  displayElement[9]  = strdup( "see manual for more info");

  uint8_t ystart = 16;
  uint8_t yplus  = 9;
  uint8_t i_ = 0;
  renderStringBox(0,    DISPLAY_LABEL,  0,                0, 86, 15, false, STYLE1X, contrastColor, background);
  renderStringBox(i_+1, DISPLAY_LABEL,  0,  ystart+i_*yplus, 128, 8, false, REGULAR1X, background , contrastColor); i_++;
  renderStringBox(i_+1, DISPLAY_LABEL,  0,  ystart+i_*yplus, 128, 8, false, REGULAR1X, contrastColor, background ); i_++;
  renderStringBox(i_+1, DISPLAY_LABEL,  0,  ystart+i_*yplus, 128, 8, false, REGULAR1X, background , contrastColor); i_++;
  renderStringBox(i_+1, DISPLAY_LABEL,  0,  ystart+i_*yplus, 128, 8, false, REGULAR1X, contrastColor, background ); i_++;
  renderStringBox(i_+1, DISPLAY_LABEL,  0,  ystart+i_*yplus, 128, 8, false, REGULAR1X, background , contrastColor); i_++;
  renderStringBox(i_+1, DISPLAY_LABEL,  0,  ystart+i_*yplus, 128, 8, false, REGULAR1X, contrastColor, background ); i_++;
  renderStringBox(i_+1, DISPLAY_LABEL,  0,  ystart+i_*yplus, 128, 8, false, REGULAR1X, background , contrastColor); i_++;
  renderStringBox(i_+1, DISPLAY_LABEL,  0,  ystart+i_*yplus, 128, 8, false, REGULAR1X, contrastColor, background ); i_++;
  renderStringBox(i_+1, DISPLAY_LABEL,  0,  ystart+i_*yplus, 128, 8, false, REGULAR1X, background , contrastColor); i_++;
  renderStringBox(i_+1, DISPLAY_LABEL,  0,  ystart+i_*yplus, 128, 8, false, REGULAR1X, contrastColor, background ); i_++;

}

void DisplayModule::patternChainMenuHandler(){

  displayElement[0] = strdup("SONG EDIT");

  if(globalObj->chainModeActive){
    sprintf(buf, "pt:%02d>%02d", sequenceArray[selectedChannel].pattern+1 , abs(globalObj->chainPatternSelect[globalObj->chainModeIndex]+1)+1);
  } else {
    sprintf(buf, "push play" );
  }
  displayElement[7] = strdup(buf);

  if(globalObj->chainModeActive){
    sprintf(buf, "ch%01d->%02dx",globalObj->chainModeMasterChannel[globalObj->chainModeIndex]+1, globalObj->chainPatternRepeatCount[globalObj->chainModeIndex] - globalObj->chainModeCount[globalObj->chainModeIndex]);

  } else {
    sprintf(buf, "to start" );
  }
  displayElement[8] = strdup(buf);



  uint8_t chainIndexOffset = (globalObj->chainSelectedPattern/4) * 4;
  bool repeatSet = false;
  for(int patternChainIndex = chainIndexOffset; patternChainIndex < chainIndexOffset+4 ; patternChainIndex++ ){
    if(repeatSet){
      sprintf(buf, "------");
    } else {
      if(globalObj->chainPatternRepeatCount[patternChainIndex] > 0){
        sprintf(buf, "%s%02d:%02d %02dx %s%s%s%s",
        patternChainIndex == globalObj->chainModeIndex ? ">" : "-",
        patternChainIndex+1,
        globalObj->chainPatternSelect[patternChainIndex]+1,
        globalObj->chainPatternRepeatCount[patternChainIndex],
        (globalObj->chainModeMasterChannel[patternChainIndex] == 0) ? "k" :
          (globalObj->chainChannelMute[0][patternChainIndex] ? "m" :
            (globalObj->chainChannelSelect[0][patternChainIndex] ? "1" : "_")),
        (globalObj->chainModeMasterChannel[patternChainIndex] == 1) ? "k" :
          (globalObj->chainChannelMute[1][patternChainIndex] ? "m" :
            (globalObj->chainChannelSelect[1][patternChainIndex] ? "2" : "_")),
        (globalObj->chainModeMasterChannel[patternChainIndex] == 2) ? "k" :
          (globalObj->chainChannelMute[2][patternChainIndex] ? "m" :
            (globalObj->chainChannelSelect[2][patternChainIndex] ? "3" : "_")),
        (globalObj->chainModeMasterChannel[patternChainIndex] == 3) ? "k" :
          (globalObj->chainChannelMute[3][patternChainIndex] ? "m" :
            (globalObj->chainChannelSelect[3][patternChainIndex] ? "4" : "_")) );
      } else if(globalObj->chainPatternRepeatCount[patternChainIndex] == 0){
        sprintf(buf, "%s%02d: repeat song",
        patternChainIndex == globalObj->chainModeIndex ? ">" : "-",
        patternChainIndex+1);
        repeatSet = true;
      } else if(globalObj->chainPatternRepeatCount[patternChainIndex] == -1){
        sprintf(buf, "%s%02d: stop song",
        patternChainIndex == globalObj->chainModeIndex ? ">" : "-",
        patternChainIndex+1);
      } else {
        sprintf(buf, "%s%02d:%02d %02dx jump",
        patternChainIndex == globalObj->chainModeIndex ? ">" : "-",
        patternChainIndex+1,
        globalObj->chainPatternSelect[patternChainIndex],
        abs(globalObj->chainPatternRepeatCount[patternChainIndex]+1) );
      }
    }
    displayElement[patternChainIndex+1-chainIndexOffset] = strdup(buf);
  }

//  displayElement[6] = strdup("shift-pgup to toggle help");

  displayElement[5] = strdup("event pattern count channels");

  uint8_t yindex = 30;
  uint8_t yoffset = 16;
  uint8_t chainIndex = 0;
  //bool highlightBool;
  //renderStringBox(6,  DISPLAY_LABEL,  0,               14, 128, 15, false, REGULAR1X, background , contrastColor);

  renderStringBox(5,  DISPLAY_LABEL,  0,              20 , 128, 15, false, REGULAR1X, background , contrastColor);

  if(globalObj->chainModeActive){
    renderStringBox(0,  DISPLAY_LABEL,  0,   0, 86, 16, false, STYLE1X, contrastColor, background);
    renderStringBox(7,  DISPLAY_LABEL,  86,  0, 42, 8, false, REGULAR1X, contrastColor, background );
    renderStringBox(8,  DISPLAY_LABEL,  86,  8, 42, 8, false, REGULAR1X, contrastColor, background );
  } else {
    renderStringBox(0,  DISPLAY_LABEL,  0, 0, 128, 16, false, STYLE1X, background , contrastColor);
    renderStringBox(7,  DISPLAY_LABEL,  86,  0, 42, 8, false, REGULAR1X, background, contrastColor );
    renderStringBox(8,  DISPLAY_LABEL,  86,  8, 42, 8, false, REGULAR1X, background, contrastColor );
  }


  //highlightBool = (chainIndexOffset == globalObj->chainSelectedPattern); chainIndexOffset++;
  renderStringBox(1,  chainIndexOffset+chainIndex++,  0, yindex+0*yoffset, 127, 16, false, STYLE1X, background , contrastColor); //, highlightBool);
  //highlightBool = (chainIndexOffset == globalObj->chainSelectedPattern); chainIndexOffset++;
  renderStringBox(2,  chainIndexOffset+chainIndex++,  0, yindex+1*yoffset, 127, 16, false, STYLE1X, background , contrastColor); //, highlightBool);
  //highlightBool = (chainIndexOffset == globalObj->chainSelectedPattern); chainIndexOffset++;
  renderStringBox(3,  chainIndexOffset+chainIndex++,  0, yindex+2*yoffset, 127, 16, false, STYLE1X, background , contrastColor); //, highlightBool);
  //highlightBool = (chainIndexOffset == globalObj->chainSelectedPattern); chainIndexOffset++;
  renderStringBox(4,  chainIndexOffset+chainIndex++,  0, yindex+3*yoffset, 127, 16, false, STYLE1X, background , contrastColor); //, highlightBool);
};


void DisplayModule::shortcutRandomMenu(){

  const char * const paramsArray[] = {"pitch/gate", "pitch", "gate", "tie", "arp all", "arpalgo", "arpspeed", "arpoctave", "arpintv", "cv2type", "cv2amp", "cv2speed", "cv2offset"};
  displayElement[0] = strdup("Random");

  displayElement[11] = strdup("press ch+rndm");
  displayElement[12] = strdup("to execute");

  displayElement[1] = strdup("param:");
  displayElement[2] = strdup("min:");
  displayElement[3] = strdup("span:");

  displayElement[5] = strdup(paramsArray[globalObj->randomizeParamSelect]);
  displayElement[6] = strdup(midiNotes[globalObj->randomizeLow]);
  sprintf(buf, "%d octave", globalObj->randomizeSpan);
  displayElement[7] = strdup(buf);

  if(globalObj->multiSelectSwitch){
    renderStringBox(0,  DISPLAY_LABEL,  0,  0, 86, 16, false, STYLE1X, contrastColor, background ); //  digitalWriteFast(PIN_EXT_RX, LOW);
  } else {
    renderStringBox(0,  DISPLAY_LABEL,    0,  0, 128, 15, false, STYLE1X , background, contrastColor);
  }

    renderStringBox(11,  DISPLAY_LABEL,  64,  0, 63, 8, false, REGULAR1X, background, contrastColor );
    renderStringBox(12,  DISPLAY_LABEL,  64,  8, 63, 8, false, REGULAR1X, background, contrastColor );

    renderStringBox(1,  DISPLAY_LABEL,        0, 20,46,17, false, STYLE1X, background , foreground);
    renderStringBox(2,  DISPLAY_LABEL,        0, 37,46,17, false, STYLE1X, background , foreground);
    renderStringBox(3,  DISPLAY_LABEL,        0, 54,46,17, false, STYLE1X, background , foreground);
    //renderStringBox(10,  DISPLAY_LABEL,       0, 71,46,17, false, STYLE1X, background , foreground);
  //  renderStringBox(11,  DISPLAY_LABEL,       0, 88,46,17, false, STYLE1X, background , foreground);

    renderStringBox(5,  STATE_SHORTCUT_RANDOM_PARAM,   47, 20,80,17, false, STYLE1X, background , foreground);
    renderStringBox(6,  STATE_SHORTCUT_RANDOM_LOW,       47, 37,80,17, false, STYLE1X, background , foreground);
    renderStringBox(7,  STATE_SHORTCUT_RANDOM_SPAN,       47, 54,80,17, false, STYLE1X, background , foreground);

    //renderStringBox(11,  STATE_CV2_OFFSET,         64, 71,63,17, false, STYLE1X, background , foreground);

}
