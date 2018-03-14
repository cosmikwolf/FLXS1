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
  oled.println("NOT WAR"); delay(10);
  oled.println("NOT WAR"); delay(10);
  oled.println("NOT WAR"); delay(10);
  oled.println("NOT WAR"); delay(10);
  oled.println("NOT WAR"); delay(10);
  oled.println("NOT WAR"); delay(10);      delay(100);
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
  oled.println("beta17l");
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


void DisplayModule::renderStringBox(uint8_t index, uint8_t highlight, int16_t x, int16_t y, int16_t w, int16_t h, bool border, uint8_t textSize, uint16_t color, uint16_t bgColor) {
  // renders a string box only once.
  uint16_t color1;
  uint16_t color2;

  bool refresh = 0;
  if (strcmp(displayElement[index], displayCache[index]) != 0 ){ refresh = 1;};
  if( previousStepMode != highlight && highlight == stepMode)  { refresh = 1;};
  if (previousStepMode == highlight && highlight != stepMode)  { refresh = 1;};
  if (previousParameterSelect != globalObj->parameterSelect )  {
    refresh = 1;
  };

  if ( refresh ) {
    if ( (highlight == stepMode) && !globalObj->parameterSelect){
      color1 = color;
      color2 = bgColor;
    } else {
      color1 = bgColor;
      color2 = color;
    }

    oled.fillRect(x,y,w,h, color2);
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
    if ((globalObj->parameterSelect && highlight == stepMode) || border){
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
  String gateTypeArray[] = { "off", "on", "tie","1hit" };

  sprintf(buf, "ch%d pt:%02d", selectedChannel+1, sequenceArray[selectedChannel].pattern+1 );
  displayElement[2] = strdup(buf);
  sprintf(buf, "p%d: %02d-%02d", notePage+1,  notePage*16+1, (notePage+1)*16 );
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
    sprintf(buf, "stepdata");
    displayElement[0] = strdup(buf);

    switch(sequenceArray[selectedChannel].quantizeScale){
      case COLUNDI:
        displayElement[1] = strdup(colundiNotes[min_max(sequenceArray[selectedChannel].stepData[selectedStep].pitch[0], 0, COLUNDINOTECOUNT)] );
      break;
      case SEMITONE:
        displayElement[1] = strdup(midiNotes[(uint16_t)globalObj->quantizeSemitonePitch(sequenceArray[selectedChannel].stepData[selectedStep].pitch[0],  sequenceArray[selectedChannel].quantizeKey, sequenceArray[selectedChannel].quantizeMode, 0)]);
        //displayElement[1] = strdup(midiNotes[sequenceArray[selectedChannel].stepData[selectedStep].pitch[0]]);
      break;
      default:
      displayElement[1] = strdup(String(sequenceArray[selectedChannel].stepData[selectedStep].pitch[0]).c_str());
    }

    if ( sequenceArray[selectedChannel].stepData[selectedStep].gateType == GATETYPE_REST ){
      displayElement[5] = strdup("rest");
    } else {
      sprintf(buf, "%d.%02d", (sequenceArray[selectedChannel].stepData[selectedStep].gateLength+1)/4, (sequenceArray[selectedChannel].stepData[selectedStep].gateLength+1)%4*100/4  );
      displayElement[5] = strdup(buf);
    }
    if (sequenceArray[selectedChannel].stepData[selectedStep].glide == 0) {
      sprintf(buf, "off");
    } else {
      sprintf(buf, "%d", sequenceArray[selectedChannel].stepData[selectedStep].glide);
    }
    displayElement[9] = strdup(buf);

    displayElement[7] = strdup(gateTypeArray[sequenceArray[selectedChannel].stepData[selectedStep].gateType].c_str() );
  }

  if(globalObj->multiSelectSwitch) {
    renderStringBox(0,  DISPLAY_LABEL,  0,  0, 86, 16, false, STYLE1X, contrastColor, background  ); //  digitalWriteFast(PIN_EXT_RX, LOW);
  } else {
    renderStringBox(0,  DISPLAY_LABEL,  0,  0, 86, 16, false, STYLE1X, background, contrastColor );
  }
  renderStringBox(2,  DISPLAY_LABEL,  86,  0, 42, 8, false, REGULAR1X, background, contrastColor );
  renderStringBox(3,  DISPLAY_LABEL,  86,  8, 42, 8, false, REGULAR1X, background, contrastColor );

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

    displayElement[2] = strdup(arpTypeArray[sequenceArray[selectedChannel].stepData[selectedStep].arpType]);

    sprintf(buf, "%d/", sequenceArray[selectedChannel].stepData[selectedStep].arpSpdNum);
    displayElement[5] = strdup(buf);
    sprintf(buf, "%d",  sequenceArray[selectedChannel].stepData[selectedStep].arpSpdDen);
    displayElement[6] = strdup(buf);
    sprintf(buf, "%doct", sequenceArray[selectedChannel].stepData[selectedStep].arpOctave);
    displayElement[9] = strdup(buf);
    displayElement[11] = strdup(chordSelectionArray[sequenceArray[selectedChannel].stepData[selectedStep].chord]);
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
   const char * const velTypeArray[] = { "off","trigger","voltage","Env Decay","Env Attack","Env AR","Env ASR","LFO Sine","LFO Tri","LFO Square", "LFO RndSq", "LFO SawUp","LFO SawDn","LFO S+H" };

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

    voltageToText(buf,sequenceArray[selectedChannel].stepData[selectedStep].velocity);

    displayElement[2] = strdup(buf);
      displayElement[4] = strdup(velTypeArray[sequenceArray[selectedChannel].stepData[selectedStep].velocityType]);
    //displayElement[4] = strdup(String(sequenceArray[selectedChannel].stepData[selectedStep].velocityType).c_str());

    /*
    Speed of 128
    is 1/2 step wavelength
    which is 64/128
    speed of 64 is 1 step wavelength
    speed of 32 is 2 step wavelenth
    which is 64/32
    */
    // if (sequenceArray[selectedChannel].stepData[selectedStep].velocityType > 6){
    //     displayElement[5] = strdup("Wavelnt:");
    //     sprintf(buf, "%d.%02d x",
    //       sequenceArray[selectedChannel].stepData[selectedStep].cv2speed/64, (100*(sequenceArray[selectedChannel].stepData[selectedStep].cv2speed%64))/64
    //    );
    //    displayElement[6] = strdup(buf);
    //
    // } else {
    //   displayElement[5] = strdup("EnvLng:");
    //   sprintf(buf, "%d", sequenceArray[selectedChannel].stepData[selectedStep].cv2speed);
    //   displayElement[6] = strdup(buf);
    //
    // }
    sprintf(buf, "%d", sequenceArray[selectedChannel].stepData[selectedStep].cv2speed);
    displayElement[6] = strdup(buf);

    voltageToText(buf,sequenceArray[selectedChannel].stepData[selectedStep].cv2offset);

    displayElement[11] = strdup(buf);
  }

  if(globalObj->multiSelectSwitch){
    renderStringBox(0,  DISPLAY_LABEL,  0,  0, 86, 16, false, STYLE1X, contrastColor, background ); //  digitalWriteFast(PIN_EXT_RX, LOW);
  } else {
    renderStringBox(0,  DISPLAY_LABEL,    0,  0, 128, 15, false, STYLE1X , background, contrastColor);
  }
    renderStringBox(3,  DISPLAY_LABEL,        0, 20,63,17, false, STYLE1X, background , foreground);
    renderStringBox(1,  DISPLAY_LABEL,        0, 37,63,17, false, STYLE1X, background , foreground);
    renderStringBox(5,  DISPLAY_LABEL,        0, 54,63,17, false, STYLE1X, background , foreground);
    renderStringBox(10,  DISPLAY_LABEL,       0, 71,63,17, false, STYLE1X, background , foreground);
  //  renderStringBox(11,  DISPLAY_LABEL,       0, 88,63,17, false, STYLE1X, background , foreground);

    renderStringBox(4,  STATE_CV2_TYPE,   64, 20,63,17, false, STYLE1X, background , foreground);
    renderStringBox(2,  STATE_CV2_LEVEL,       64, 37,63,17, false, STYLE1X, background , foreground);
    renderStringBox(6,  STATE_CV2_SPEED,       64, 54,63,17, false, STYLE1X, background , foreground);

    renderStringBox(11,  STATE_CV2_OFFSET,         64, 71,63,17, false, STYLE1X, background , foreground);

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

   if(sequenceArray[selectedChannel].skipStepCount == 0){
     sprintf(buf, "skip rndm:" );
   } else {
     sprintf(buf, "skip %d:", sequenceArray[selectedChannel].skipStepCount);
   }

   displayElement[5] = strdup(buf);

    gateMappingText(buf, sequenceArray[selectedChannel].gpio_skipstep);
    displayElement[6] = strdup(buf);

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

   renderStringBox(5,  STATE_SKIPSTEPCOUNT,   0,  63,89,16, false, STYLE1X, background , foreground);
   renderStringBox(6,  STATE_SKIPSTEP,     90, 63,37,16, false, STYLE1X, background , foreground);

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

     switch(sequenceArray[selectedChannel].playMode){
       case PLAY_FORWARD:
        displayElement[10] = strdup("FWD");
       break;
       case PLAY_REVERSE:
        displayElement[10] = strdup("REV");
       break;
       case PLAY_RANDOM:
        displayElement[10] = strdup("RNDM");
       break;
       case PLAY_PENDULUM:
        displayElement[10] = strdup("PEND");
       break;
     }
     displayElement[9] = strdup("direction:");


     renderStringBox(0,  DISPLAY_LABEL,    0,  0, 128, 15, false, STYLE1X, background , contrastColor);

     renderStringBox(3,  DISPLAY_LABEL,           0, 15,48,16, false, STYLE1X, background , foreground);
     renderStringBox(4,  STATE_QUANTIZESCALE,    48, 15,79,16, false, STYLE1X, background , foreground);

     renderStringBox(5,  DISPLAY_LABEL,        0, 31,48,16, false, STYLE1X, background , foreground);
     renderStringBox(6,  STATE_QUANTIZEKEY,     48, 31,79,16, false, STYLE1X, background , foreground);

     renderStringBox(7,  DISPLAY_LABEL,        0, 47,48,16, false, STYLE1X, background , foreground);
     renderStringBox(8,  STATE_QUANTIZEMODE,     48, 47, 79,16, false, STYLE1X, background , foreground);

     renderStringBox(9,  DISPLAY_LABEL,   0,  63,89,16, false, STYLE1X, background , foreground);
     renderStringBox(10,  STATE_PLAYMODE,    72, 63, 55,16, false, STYLE1X, background , foreground);

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

   displayElement[1] = strdup("gate length:");

  cvMappingText(buf, sequenceArray[selectedChannel].cv_gatemod);
  displayElement[2] = strdup(buf);

  displayElement[3] = strdup("gatemute:");
 gateMappingText(buf, sequenceArray[selectedChannel].gpio_gatemute);
 displayElement[4] = strdup(buf);

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
   renderStringBox(1,  DISPLAY_LABEL,      0,   15,  64,16, false, STYLE1X, background , foreground);
   renderStringBox(2,  STATE_GATEMOD,     96,   15,  31,16, false, STYLE1X, background , foreground);
   renderStringBox(3,  DISPLAY_LABEL,      0,   31,  64,16, false, STYLE1X, background , foreground);
   renderStringBox(4,  STATE_GATEMUTE,    96,   31,  31,16, false, STYLE1X, background , foreground);
   renderStringBox(5,  DISPLAY_LABEL,      0,   47,  64,16, false, STYLE1X, background , foreground);
   renderStringBox(6,  STATE_RANDOMPITCH, 96,   47,  31,16, false, STYLE1X, background , foreground);
   renderStringBox(11,  STATE_RANDOMHIGH, 67,   47,  29, 8, false, REGULAR1X, background , foreground);
   renderStringBox(12,  STATE_RANDOMLOW,  67,   55,  29, 8, false, REGULAR1X, background , foreground);


   renderStringBox(7,  DISPLAY_LABEL,      0,   63,  64,16, false, STYLE1X, background , foreground);
   renderStringBox(8,  STATE_PITCHMOD,    90,   63,  37,16, false, STYLE1X, background , foreground);
   renderStringBox(9,  DISPLAY_LABEL,      0,   79,  64,16, false, STYLE1X, background , foreground);
   renderStringBox(10,  STATE_GLIDEMOD,   90,   79,  37,16, false, STYLE1X, background , foreground);

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
   renderStringBox(2,  STATE_ARPTYPEMOD,  90,   15,  37,16, false, STYLE1X, background , foreground);
   renderStringBox(3,  DISPLAY_LABEL,      0,   31,  64,16, false, STYLE1X, background , foreground);
   renderStringBox(4,  STATE_ARPSPDMOD,  90,   31,  37,16, false, STYLE1X, background , foreground);
   renderStringBox(5,  DISPLAY_LABEL,      0,   47,  64,16, false, STYLE1X, background , foreground);
   renderStringBox(6,  STATE_ARPOCTMOD, 90,   47,  37,16, false, STYLE1X, background , foreground);
   renderStringBox(7,  DISPLAY_LABEL,      0,   63,  64,16, false, STYLE1X, background , foreground);
   renderStringBox(8,  STATE_ARPINTMOD,    90,   63,  37,16, false, STYLE1X, background , foreground);
   //renderStringBox(9,  STATE_SKIPSTEPCOUNT, 0,   79,  64,16, false, STYLE1X, background , foreground);
   //renderStringBox(10,  STATE_SKIPSTEP,    90,   79,  37,16, false, STYLE1X, background , foreground);
   //renderStringBox(9,  DISPLAY_LABEL,      0,   79,  64,16, false, STYLE1X, background , foreground);
   //renderStringBox(10,  STATE_GLIDEMOD,   96,   79,  32,16, false, STYLE1X, background , foreground);


 };



 void DisplayModule::patternSelectDisplay(){

   highlight = currentPattern;

   displayElement[0] = strdup("01");
   displayElement[1] = strdup("02");
   displayElement[2] = strdup("03");
   displayElement[3] = strdup("04");
   displayElement[4] = strdup("05");
   displayElement[5] = strdup("06");
   displayElement[6] = strdup("07");
   displayElement[7] = strdup("08");
   displayElement[8] = strdup("09");
   displayElement[9] = strdup("10");
   displayElement[10] = strdup("11");
   displayElement[11] = strdup("12");
   displayElement[12] = strdup("13");
   displayElement[13] = strdup("14");
   displayElement[14] = strdup("15");
   displayElement[15] = strdup("16");

   displayElement[16] = strdup("PATTERN SELECT");
   sprintf(buf, "current: %02d", currentPattern );
   sprintf(buf, "ch1:%02d ch2:%02d ch3:%02d ch4:%02d ", sequenceArray[0].pattern+1,sequenceArray[1].pattern+1, sequenceArray[2].pattern+1, sequenceArray[3].pattern+1 );
   displayElement[17] = strdup(buf);

   renderStringBox( 0, DISPLAY_LABEL,  0, 30, 32 , 16, false, STYLE1X, BLACK, RED);
   renderStringBox( 1, DISPLAY_LABEL, 32, 30, 32 , 16, false, STYLE1X, BLACK, GREEN);
   renderStringBox( 2, DISPLAY_LABEL, 64, 30, 32 , 16, false, STYLE1X, BLACK, BLUE);
   renderStringBox( 3, DISPLAY_LABEL, 96, 30, 32 , 16, false, STYLE1X, BLACK, PURPLE);
   renderStringBox( 4, DISPLAY_LABEL,  0, 47, 32 , 16, false, STYLE1X, BLACK, RED);
   renderStringBox( 5, DISPLAY_LABEL, 32, 47, 32 , 16, false, STYLE1X, BLACK, GREEN);
   renderStringBox( 6, DISPLAY_LABEL, 64, 47, 32 , 16, false, STYLE1X, BLACK, BLUE);
   renderStringBox( 7, DISPLAY_LABEL, 96, 47, 32 , 16, false, STYLE1X, BLACK, PURPLE);
   renderStringBox( 8, DISPLAY_LABEL,  0, 63, 32 , 16, false, STYLE1X, BLACK, RED);
   renderStringBox( 9, DISPLAY_LABEL, 32, 63, 32 , 16, false, STYLE1X, BLACK, GREEN);
   renderStringBox(10, DISPLAY_LABEL, 64, 63, 32 , 16, false, STYLE1X, BLACK, BLUE);
   renderStringBox(11, DISPLAY_LABEL, 96, 63, 32 , 16, false, STYLE1X, BLACK, PURPLE);
   renderStringBox(12, DISPLAY_LABEL,  0, 79, 32 , 16, false, STYLE1X, BLACK, RED);
   renderStringBox(13, DISPLAY_LABEL, 32, 79, 32 , 16, false, STYLE1X, BLACK, GREEN);
   renderStringBox(14, DISPLAY_LABEL, 64, 79, 32 , 16, false, STYLE1X, BLACK, BLUE);
   renderStringBox(15, DISPLAY_LABEL, 96, 79, 32 , 16, false, STYLE1X, BLACK, PURPLE);


   renderStringBox(16, DISPLAY_LABEL, 0, 0, 128 , 8, false, STYLE1X, BLACK, PINK);
   renderStringBox(17, DISPLAY_LABEL, 0, 16, 128 , 8, false, REGULAR1X, BLACK, PINK);

 }

void DisplayModule::saveMenuDisplayHandler(){
  if(prevPtrnChannelSelector != patternChannelSelector){
    for (int i=0; i<8; i++){
      free(displayCache[i]);
      displayCache[i] = nullptr;
    }
  }
  prevPtrnChannelSelector = patternChannelSelector;

  displayElement[0] = strdup("ch1");
  displayElement[1] = strdup("ch2");
  displayElement[2] = strdup("ch3");
  displayElement[3] = strdup("ch4");
  if(patternChannelSelector & 0b0001){
    sprintf(buf, "%02d", saveDestination[0]+1);
    displayElement[4] = strdup(buf);
  } else {
    displayElement[4] = strdup("--");
  }
  if(patternChannelSelector & 0b0010){
    sprintf(buf, "%02d", saveDestination[1]+1);
    displayElement[5] = strdup(buf);
  } else {
    displayElement[5] = strdup("--");
  }
  if(patternChannelSelector & 0b0100){
    sprintf(buf, "%02d", saveDestination[2]+1);
    displayElement[6] = strdup(buf);
  } else {
    displayElement[6] = strdup("--");
  }
  if(patternChannelSelector & 0b1000){
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

  if(patternChannelSelector & 0b0001){
    renderStringBox( 0, DISPLAY_LABEL,  0, 48, 32 , 16, false, STYLE1X, BLACK, RED);
  } else {
    renderStringBox( 0, DISPLAY_LABEL,  0, 48, 32 , 16, false, STYLE1X, BLACK, DARK_GREY);
  }
  if(patternChannelSelector & 0b0010){
    renderStringBox( 1, DISPLAY_LABEL, 32, 48, 32 , 16, false, STYLE1X, BLACK, GREEN);
  } else {
    renderStringBox( 1, DISPLAY_LABEL, 32, 48, 32 , 16, false, STYLE1X, BLACK, DARK_GREY);
  }
  if(patternChannelSelector & 0b0100){
    renderStringBox( 2, DISPLAY_LABEL, 64, 48, 32 , 16, false, STYLE1X, BLACK, BLUE);
  } else {
    renderStringBox( 2, DISPLAY_LABEL, 64, 48, 32 , 16, false, STYLE1X, BLACK, DARK_GREY);
  }
  if(patternChannelSelector & 0b1000){
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
