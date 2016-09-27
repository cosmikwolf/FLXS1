#include <Arduino.h>
#include "DisplayModule.h"

/*#include "../commonFonts/fonts/font_ArialBold.cpp"
#include "../commonFonts/fonts/font_Arial.cpp"
*/
//    coord_t   i, j;
elapsedMicros displayTimer;
DisplayModule::DisplayModule(){
};

void DisplayModule::initialize(Sequencer *sequenceArray){

  Serial.println("Initializing Display");

  this->sequenceArray = sequenceArray;
  oled.begin();
  //oled.setFont(&Font);//this will load the font
  oled.setFont(&unborn_small);//this will load the font
  oled.setTextScale(2);

  oled.fillScreen(BLACK);     delay(10);
  oled.fillScreen(RED);     delay(10);
  oled.fillScreen(ORANGE);      delay(10);
  oled.fillScreen(YELLOW);      delay(10);
  oled.fillScreen(GREEN);     delay(10);
  oled.fillScreen(BLUE);      delay(10);
  oled.fillScreen(NAVY);      delay(10);
  oled.fillScreen(PURPLE);      delay(10);
  oled.fillScreen(RED);        delay(10);
  oled.fillScreen(ORANGE);      delay(10);
  oled.fillScreen(YELLOW);      delay(10);
  oled.fillScreen(GREEN);     delay(10);
  oled.fillScreen(BLUE);      delay(10);
  oled.fillScreen(NAVY);      delay(10);
  oled.fillScreen(PURPLE);       delay(100);
  oled.fillScreen(RED);
  oled.setCursor(0, 5);
  oled.setTextColor(BLACK);
  oled.println("ZETAOHM");    delay(100);
  oled.fillScreen(GREEN);
  oled.setCursor(0, 5);
  oled.setTextColor(PINK);
  oled.println("FLXS1");    delay(100);
  oled.fillScreen(BLUE);
  oled.setCursor(0, 5);
  oled.setTextColor(ORANGE);
  oled.println("MUSIKCOMPUTER");    delay(100);
  oled.fillScreen(BLUE);

  Serial.println("Display Initialization Complete");
}

void DisplayModule::clearDisplay(){
  oled.clearScreen();
//  oled.fillScreen(BLUE);
}

void DisplayModule::freeDisplayCache(){
  for (int i=0; i< MAX_DISPLAY_ELEMENTS; i++){
    //clear displaycache so all data redraws.
      free(displayCache[i]);
      displayCache[i] = NULL;
      free(displayElement[i]);
      displayElement[i] = NULL;
  }
}

void DisplayModule::cleanupTextBuffers(){
  for( int i=0; i< MAX_DISPLAY_ELEMENTS; i++ ){
    if (displayElement[i] == NULL) {
      displayElement[i] = strdup("--------");
    };
      free(displayCache[i]);
      displayCache[i] = NULL;
      displayCache[i] = strdup(displayElement[i]);
      free(displayElement[i]);
      displayElement[i] = NULL;
  };
  //delete buf;
};

void DisplayModule::displayLoop(uint16_t frequency) {

  if( displayTimer > frequency){
	  displayTimer = 0;

   // if (previousState != currentState) { memset(displayCache, 0, sizeof(displayCache));}
    if (previousState != currentState || previouslySelectedChannel != selectedChannel){
       Serial.println("about to display a new state");
       freeDisplayCache();
       oled.clearScreen();
    }
    switch(currentState) {

      case CHANNEL_PITCH_MODE:
        channelPitchMenuDisplay(buf);
      break;
      case CHANNEL_GATE_MODE:
        channelGateMenuDisplay(buf);
      break;
      case CHANNEL_ENVELOPE_MODE:
        channelEnvelopeMenuDisplay(buf);
      break;
      case CHANNEL_STEP_MODE:
        channelStepMenuDisplay(buf);
      break;


      case PATTERN_SELECT:
        patternSelectDisplay();
      break;

      case SEQUENCE_MENU:
        sequenceMenuDisplay();
      break;

      case INSTRUMENT_MENU:
        instrumentSelectDisplay();
      break;

      case TIMING_MENU:
        timingMenuDisplay();
      break;

      case DELETE_MENU:
        deleteMenuDisplay();
      break;

      case CALIBRATION_MENU:
        calibrationMenuDisplay();
      break;
      /*
      case DEBUG_SCREEN:
        debugScreenDisplay();
      break;
      */
    }
    if (previousState != currentState){
      Serial.println("finished first loop of displaying new state");
    }
    cleanupTextBuffers();
    previousState = currentState;
    previouslySelectedChannel = selectedChannel;
  };
};

// STATE VARIABLE DISPLAY HANDLERS

void DisplayModule::renderOnce_StringBox(uint8_t index, uint8_t highlight, uint8_t previousHighlight, int16_t x, int16_t y, int16_t w, int16_t h, bool border, uint8_t textSize, uint16_t color, uint16_t bgColor) {
  // renders a string box only once.
  uint16_t color1;
  uint16_t color2;

  if ( strcmp(displayElement[index], displayCache[index]) != 0  || previousHighlight != highlight ) {
    if (highlight == index){
      color1 = color;
      color2 = bgColor;
    } else {
      color1 = bgColor;
      color2 = color;
    }

    oled.fillRect(x,y,w,h, color2);

    if (textSize < 2){
      oled.setCursor(x+1, y-1);
      oled.setFont(&unborn_small);
      oled.setTextScale(textSize);
    } else {
      oled.setCursor(x+1, y-(textSize*2));
      oled.setFont(&Font);
      oled.setTextScale(textSize-2, textSize-1);
    }

    oled.setTextColor(color1);
    oled.print(displayElement[index]);
    if (border){
      oled.drawRect(x,y,w,h, color1);
    }
  }
}

void DisplayModule::channelPitchMenuDisplay(char *buf){

  //  char *buf = new char[51]; // sprintf buffer
  uint8_t previousHighlight = highlight;
  uint16_t foreground, background;
  //oled.setFont(Arial_8);

  switch (selectedChannel){
    case 0:
      foreground = RED;
      background = BLACK;
      break;
    case 1:
      foreground = GREEN;
      background = BLUE;
      break;
    case 2:
      foreground = PINK;
      background = PURPLE;
      break;
    case 3:
      foreground = ORANGE;
      background = NAVY;
      break;
  }

  switch (stepMode) {
    case STEPMODE_PITCH0:
      highlight = 1;    break;
    case STEPMODE_GATELENGTH:
      highlight = 2;    break;
    case STEPMODE_TEMPO:
      highlight = 6;    break;
    case STEPMODE_STEPCOUNT:
      highlight = 8;    break;
    case STEPMODE_BEATCOUNT:
      highlight = 9;    break;
    case STEPMODE_GATETYPE:
      highlight = 4;    break;
    case STEPMODE_PITCH1:
      highlight = 13;   break;
    case STEPMODE_PITCH2:
      highlight = 14;   break;
    case STEPMODE_PITCH3:
      highlight = 15;   break;
    case STEPMODE_GLIDE:
      highlight = 16;   break;

  }

  displayElement[0] = strdup("pitch set");
  displayElement[1] = strdup(midiNotes[sequenceArray[selectedChannel].stepData[selectedStep].pitch[0]]);
  displayElement[13] = strdup(midiNotes[sequenceArray[selectedChannel].stepData[selectedStep].pitch[0] + sequenceArray[selectedChannel].stepData[selectedStep].pitch[1]]);
  displayElement[14] = strdup(midiNotes[sequenceArray[selectedChannel].stepData[selectedStep].pitch[0] + sequenceArray[selectedChannel].stepData[selectedStep].pitch[2]]);
  displayElement[15] = strdup(midiNotes[sequenceArray[selectedChannel].stepData[selectedStep].pitch[0] + sequenceArray[selectedChannel].stepData[selectedStep].pitch[3]]);



  if ( sequenceArray[selectedChannel].stepData[selectedStep].gateType == GATETYPE_REST ){
    displayElement[2] = strdup("L: rest");
  } else if (sequenceArray[selectedChannel].stepData[selectedStep].gateLength == 0){
    displayElement[2] = strdup("L: pulse");
  } else if (sequenceArray[selectedChannel].stepData[selectedStep].gateLength == 1){
    displayElement[2] = strdup("L: 1 step");
  } else {
    sprintf(buf, "L: %d steps", sequenceArray[selectedChannel].stepData[selectedStep].gateLength);
    displayElement[2] = strdup(buf);
  }


  if (sequenceArray[selectedChannel].stepData[selectedStep].gateType == GATETYPE_REST){
    displayElement[4] = strdup("Rest");
  } else if (sequenceArray[selectedChannel].stepData[selectedStep].gateType == GATETYPE_STEP){
    displayElement[4] = strdup("Step");
  } else if (sequenceArray[selectedChannel].stepData[selectedStep].gateType > GATETYPE_STEP ){
    sprintf(buf, "Arp %d", sequenceArray[selectedChannel].stepData[selectedStep].gateType);
    displayElement[4] = strdup(buf);
  }

  if (sequenceArray[selectedChannel].stepData[selectedStep].glide == 0) {
    sprintf(buf, "Glide off");
  } else {
    sprintf(buf, "Glide %d", sequenceArray[selectedChannel].stepData[selectedStep].glide);
  }
  displayElement[16] = strdup(buf);

  //sprintf(buf, "%s", instrumentNames[sequenceArray[selectedChannel].instrument]);
  //displayElement[4] = strdup(buf);

  //sprintf(buf, "vol: %d", sequenceArray[selectedChannel].volume);

  if (extClock) {
    displayElement[6] = strdup("MIDI");
  } else {
    sprintf(buf, "%d bpm", int(tempoX100/100) );
    displayElement[6] = strdup(buf);
  };

  sprintf(buf, "%d steps", sequenceArray[selectedChannel].stepCount);
  displayElement[8] = strdup(buf);

  sprintf(buf, "/ %d beats", sequenceArray[selectedChannel].beatCount);
  displayElement[9] = strdup(buf);

  sprintf(buf, "stmd: %d", stepMode);
  displayElement[10] = strdup(buf);

  sprintf(buf, "pg: %d-%d", notePage*16 , (notePage+1)*16 );
  displayElement[11] = strdup(buf);

  sprintf(buf, "ch/pt: %d/%d", selectedChannel , sequenceArray[selectedChannel].patternIndex );
  displayElement[12] = strdup(buf);

// PITCH INFO
  renderOnce_StringBox(0,  highlight, previousHighlight, 0,   0, 64, 14, false, 1, background, foreground);
  renderOnce_StringBox(1,  highlight, previousHighlight, 0,  15, 64, 30, true, 3, background , foreground);
  renderOnce_StringBox(13, highlight, previousHighlight, 80,  0, 48, 15,  true, 2 ,background , foreground);
  renderOnce_StringBox(14, highlight, previousHighlight, 80,  15, 48, 15, true, 2 ,background , foreground);
  renderOnce_StringBox(15, highlight, previousHighlight, 80,  30, 48, 15, true, 2 ,background , foreground);

  renderOnce_StringBox(2,  highlight, previousHighlight, 0,  46, 64, 12, false, 1, background , foreground);
  renderOnce_StringBox(4,  highlight, previousHighlight, 0,  66, 64, 12, false, 1 ,background , foreground);
  renderOnce_StringBox(16,  highlight, previousHighlight, 0, 76, 64, 12, false, 1 ,background , foreground);

  //renderOnce_StringBox(5,  highlight, previousHighlight, 0,  60, 64, 10, false, 1 ,background , foreground);   // UNUSED

  renderOnce_StringBox(11, highlight, previousHighlight, 0,  86, 64, 10, false, 1 ,background , foreground);
  renderOnce_StringBox(12, highlight, previousHighlight, 0,  96, 64, 10, false, 1 ,background , foreground);

  renderOnce_StringBox(6,  highlight, previousHighlight, 64, 56, 64, 10, false, 1 ,BLACK , WHITE);
  renderOnce_StringBox(8,  highlight, previousHighlight, 64, 66, 64, 10, false, 0 ,BLACK , WHITE);
  renderOnce_StringBox(9,  highlight, previousHighlight, 64, 76, 64, 10, false, 0 ,BLACK , WHITE);
  renderOnce_StringBox(10, highlight, previousHighlight, 64, 86, 64, 10, false, 1 ,BLACK , WHITE);


  //sprintf(buf, "state: %d", currentState);
  //sprintf(buf, "chan: %d", sequenceArray[selectedChannel].activeStep);
  //sprintf(buf, "stmd: %d", stepMode);
  //sprintf(buf, "sqnc: %d", selectedChannel);
  //sprintf(buf, "step: %d", selectedStep);

  // for( int i=0; i< MAX_DISPLAY_ELEMENTS; i++ ){
  //     free(displayCache[i]);
  //     displayCache[i] = strdup(displayElement[i]);
  //     free(displayElement[i]);
  // }

  //  cleanupTextBuffers(buf, 13);
};

void DisplayModule::channelGateMenuDisplay(char *buf) {
  uint8_t previousHighlight = highlight;
  uint16_t foreground, background;

  switch (selectedChannel){
    case 0:
      foreground = RED;
      background = BLACK;
      break;
    case 1:
      foreground = GREEN;
      background = BLUE;
      break;
    case 2:
      foreground = PINK;
      background = PURPLE;
      break;
    case 3:
      foreground = ORANGE;
      background = NAVY;
      break;
  }




  displayElement[0] = strdup("_-*^*-_ arp set _-*^*-_");
  displayElement[1] = strdup("gate type:");

  switch (sequenceArray[selectedChannel].stepData[selectedStep].gateType){
    case 0:
    displayElement[2] = strdup("none");
    break;
    case 1:
    displayElement[2] = strdup("norm");
    break;
    case 2:
    displayElement[2] = strdup("1 shot");
    break;
    case 3:
    displayElement[2] = strdup("hold");
    break;
    default:
    displayElement[2] = strdup("unknown");
    break;
  }

  displayElement[3] = strdup("arp type:");

  switch (sequenceArray[selectedChannel].stepData[selectedStep].arpType){
    case 0:
      displayElement[4] = strdup("up");
    break;
    case 1:
      displayElement[4] = strdup("dwn");
    break;
    case 2:
      displayElement[4] = strdup("u&d1");
    break;
    case 3:
      displayElement[4] = strdup("U&D2");
    break;
    case 4:
      displayElement[4] = strdup("rndm");
    break;
    default:
      displayElement[4] = strdup("");
    break;
  }



  displayElement[5] = strdup("speed:");
  sprintf(buf, "%d/%d beat", sequenceArray[selectedChannel].stepData[selectedStep].arpSpeedNumerator, sequenceArray[selectedChannel].stepData[selectedStep].arpSpeedDenominator);
  displayElement[6] = strdup(buf);


  switch (stepMode) {
    case STEPMODE_GATETYPE:
      highlight = 2;
    break;
    case STEPMODE_ARPTYPE:
      highlight = 4;
    break;
    case STEPMODE_ARPSPEED:
      highlight = 6;
    break;
    case STEPMODE_ARPOCTAVE:
      highlight = 6;
    break;
  }


//Title - arp set
  renderOnce_StringBox(0,  highlight, previousHighlight, 0 , 90, 128 , 6, false, 1,  background, foreground);
//gate type
  renderOnce_StringBox(1,  highlight, previousHighlight, 0,  0, 128, 10, false, 0, background, foreground);
  renderOnce_StringBox(2,  highlight, previousHighlight, 0,  6, 128, 15, false, 2, background, foreground);
//arp type
  renderOnce_StringBox(3,  highlight, previousHighlight, 0,  23, 128, 10, false, 0, background, foreground);
  renderOnce_StringBox(4,  highlight, previousHighlight, 0,  29, 128, 15, false, 2, background, foreground);
//arp speed
  renderOnce_StringBox(5,  highlight, previousHighlight, 0,  46, 128, 10, false, 0, background, foreground);
  renderOnce_StringBox(6,  highlight, previousHighlight, 0,  52, 128, 15, false, 2, background, foreground);

}

void DisplayModule::channelEnvelopeMenuDisplay(char *buf) {
  uint8_t previousHighlight = highlight;
  uint16_t foreground, background;

  displayElement[0] = strdup("ENVELOPE SET");
  switch (selectedChannel){
    case 0:
      foreground = RED;
      background = BLACK;
      break;
    case 1:
      foreground = GREEN;
      background = BLUE;
      break;
    case 2:
      foreground = PINK;
      background = PURPLE;
      break;
    case 3:
      foreground = ORANGE;
      background = NAVY;
      break;
  }

  switch (stepMode) {
    case STEPMODE_VELOCITY:
      highlight = 1;    break;
  }

  sprintf(buf, "V: %d", sequenceArray[selectedChannel].stepData[selectedStep].velocity);
  displayElement[2] = strdup(buf);

  renderOnce_StringBox(0, highlight, previousHighlight,  0,  0, 128, 16, false, 0,  background, foreground);
  renderOnce_StringBox(1,  highlight, previousHighlight, 0, 16, 64 , 16, false, 2, background, foreground);


}

void DisplayModule::channelStepMenuDisplay(char *buf) {
  uint8_t previousHighlight = highlight;
  displayElement[0] = strdup("STEP SET");
  renderOnce_StringBox(0, highlight, previousHighlight, 0 , 0  , 128 , 16, false, 1,  WHITE, RED);
}

void DisplayModule::sequenceMenuDisplay(){

  uint8_t previousHighlight = highlight;
  //uint16_t foreground, background;
  //oled.setFont(Arial_8);

  displayElement[0] = strdup("CHANNEL MENU");
  displayElement[1] = strdup("INIT CH");
  displayElement[2] = strdup("INIT PAT");
  displayElement[3] = strdup("DELETE SAVE FILE");
  displayElement[4] = strdup("TOGGLE GAME OF LIFE");
  displayElement[5] = strdup("EXTCLOCK");

  renderOnce_StringBox(0, highlight, previousHighlight, 0 , 0  , 128 , 16, false, 1,  WHITE, RED);
  renderOnce_StringBox(1, highlight, previousHighlight, 0 , 16 , 64 , 16, false, 1, WHITE, GREEN);
  renderOnce_StringBox(2, highlight, previousHighlight, 0 , 32 , 64 , 16, false, 1, WHITE, BLUE);
  renderOnce_StringBox(3, highlight, previousHighlight ,0, 48 , 128 , 16, false, 1, WHITE, BLACK);
  renderOnce_StringBox(4, highlight, previousHighlight ,0, 64 , 128 , 16, false, 1, WHITE, BLACK);
  renderOnce_StringBox(5, highlight, previousHighlight, 64 , 16 , 64 , 16, false, 1, WHITE, GREEN);
  renderOnce_StringBox(6, highlight, previousHighlight, 64 , 32 , 64 , 16, false, 1, WHITE, BLUE);

  sprintf(buf, "step: %d", selectedStep);

}


void DisplayModule::patternSelectDisplay(){

  uint8_t previousHighlight = highlight;
  highlight = currentPattern;

  displayElement[0] = strdup("0");
  displayElement[1] = strdup("1");
  displayElement[2] = strdup("2");
  displayElement[3] = strdup("3");
  displayElement[4] = strdup("4");
  displayElement[5] = strdup("5");
  displayElement[6] = strdup("6");
  displayElement[7] = strdup("7");
  displayElement[8] = strdup("8");
  displayElement[9] = strdup("9");
  displayElement[10] = strdup("A");
  displayElement[11] = strdup("B");
  displayElement[12] = strdup("C");
  displayElement[13] = strdup("D");
  displayElement[14] = strdup("E");
  displayElement[15] = strdup("F");
  displayElement[16] = strdup("PATTERN SELECT");

  renderOnce_StringBox(0, highlight, previousHighlight,  0, 0, 32 , 24, false, 3, BLACK, RED);
  renderOnce_StringBox(1, highlight, previousHighlight, 32, 0, 32 , 24, false, 3, BLACK, GREEN);
  renderOnce_StringBox(2, highlight, previousHighlight, 64, 0, 32 , 24, false, 3, BLACK, BLUE);
  renderOnce_StringBox(3, highlight, previousHighlight, 96, 0, 32 , 24, false, 3, BLACK, PURPLE);
  renderOnce_StringBox(4, highlight, previousHighlight,  0, 24, 32 , 24, false, 3, BLACK, RED);
  renderOnce_StringBox(5, highlight, previousHighlight, 32, 24, 32 , 24, false, 3, BLACK, GREEN);
  renderOnce_StringBox(6, highlight, previousHighlight, 64, 24, 32 , 24, false, 3, BLACK, BLUE);
  renderOnce_StringBox(7, highlight, previousHighlight, 96, 24, 32 , 24, false, 3, BLACK, PURPLE);
  renderOnce_StringBox(8, highlight, previousHighlight,  0, 48, 32 , 24, false, 3, BLACK, RED);
  renderOnce_StringBox(9, highlight, previousHighlight, 32, 48, 32 , 24, false, 3, BLACK, GREEN);
  renderOnce_StringBox(10, highlight,previousHighlight, 64, 48, 32 , 24, false, 3, BLACK, BLUE);
  renderOnce_StringBox(11, highlight,previousHighlight, 96, 48, 32 , 24, false, 3, BLACK, PURPLE);
  renderOnce_StringBox(12, highlight,previousHighlight,  0, 64, 32 , 24, false, 3, BLACK, RED);
  renderOnce_StringBox(13, highlight,previousHighlight, 32, 64, 32 , 24, false, 3, BLACK, GREEN);
  renderOnce_StringBox(14, highlight,previousHighlight, 64, 64, 32 , 24, false, 3, BLACK, BLUE);
  renderOnce_StringBox(15, highlight,previousHighlight, 96, 64, 32 , 24, false, 3, BLACK, PURPLE);
  renderOnce_StringBox(16, highlight, previousHighlight, 0, 0, 128 , 8, false, 1, BLACK, PINK);

}

void DisplayModule::calibrationMenuDisplay(){
  uint8_t previousHighlight = highlight;
  highlight = currentPattern;

  displayElement[0] = strdup("Calibration Menu");

  sprintf(buf, "DAC Value: %d", calibrationBuffer);
  displayElement[1] = strdup(buf);

  renderOnce_StringBox(0, highlight, previousHighlight,  0, 16, 32 , 20, false, 1, BLACK, RED);
  renderOnce_StringBox(1, highlight, previousHighlight, 0, 32, 32 , 20, false, 1, BLACK, GREEN);

}


void DisplayModule::globalMenuDisplay(){
  /*
  //nonBlockingRainbow(5);
  gdispFillStringBox(0, 0, width, 10, "ZETAOHM", fontSm , Red, Green, justifyCenter);
  gdispFillStringBox(0, height/2, width, 15,  "GLOBAL", fontSm, Blue, White, justifyCenter);
  gdispFillStringBox(0, height/3, width, 15,  "MENU", fontSm, White, Black, justifyCenter);
  */
}

void DisplayModule::gameOfLifeDisplay(){

  //  for(int row=0; row < LIFELINES; row++){
  //    for(int col=0; col < LIFEWIDTH; col++){
  //
  //      color_t color;
  //
  //      if (col == sequenceArray[selectedChannel].activeStep) {
  //        if (life.grid[row][col] < 1){
  //          color = Yellow;
  //        } else {
  //          color = Magenta;
  //        }
  //   /*     } else if (life.grid[row][col] < 32){
  //          color =  Green;
  //        } else if (life.grid[row][col] < 48){
  //          color =  Red;
  //        } else if ( life.grid[row][col] < 64){
  //          color =  Purple;
  //        } else if ( life.grid[row][col] < 96){
  //          color = Yellow;
  //        } else if (life.grid[row][col] < 112){
  //          color = Yellow;
  //        } else if (life.grid[row][col] < 128){
  //          color = Olive;
  //        }
  //
  //        */
  //      } else {
  //        if (life.grid[row][col] < 1){
  //          color = Black;
  //          } else if (life.grid[row][col] < 32){
  //          color =  Orange;
  //        } else if (life.grid[row][col] < 48){
  //          color =  Yellow;
  //        } else if ( life.grid[row][col] < 64){
  //          color =  Green;
  //        } else if ( life.grid[row][col] < 96){
  //          color = Blue;
  //        } else if (life.grid[row][col] < 112){
  //          color = Navy;
  //        } else if (life.grid[row][col] < 128){
  //          color = Magenta;
  //        }
  //
  //      }
  //
  // //     gdispFillArea( ((col+0)*8), ((row+0)*8), 8, 8,  color);
  //    }
  //  }
}

void DisplayModule::deleteMenuDisplay(){
  /*
  uint8_t previousHighlight = highlight;
  highlight = selectedChannel;
  displayElement[0] = strdup("to clear this pattern");
  renderStringBox(0, highlight, previousHighlight ,0, 60 , 32 , 128, fontSm,  White, Red, justifyCenter);

  displayElement[1] = strdup("press step 16");
  renderStringBox(1, highlight, previousHighlight ,0, 70 , 32 , 128, fontSm, White, Red,  justifyCenter);

  for (int i=2; i< MAX_DISPLAY_ELEMENTS; i++){
    //filling display buffer with dummy data. dirty, but i need to have a reliable buffer size.
    displayElement[i] = strdup("a");
  }

  for( int i=0; i< MAX_DISPLAY_ELEMENTS; i++ ){
    if (displayCache[i]){
      free(displayCache[i]);
    }
    if ( displayElement[i] ){
      displayCache[i] = strdup(displayElement[i]);
      free(displayElement[i]);
    }
  }
  */
};


void DisplayModule::instrumentSelectDisplay(){
  /*
  const char* element;

  element =  "INSTRUMENT SELECT";
  gdispFillStringBox(   0, 31, 128 , 10, element, fontSm, Orange, Red, justifyCenter);

  element = String(instrumentNames[sequenceArray[selectedChannel].instrument]).c_str();
  gdispFillStringBox(   0, 41, 128 , 24, element, fontSm, Red, Orange, justifyCenter);
  element = String("vol: " + String(sequenceArray[selectedChannel].volume)).c_str();
  gdispFillStringBox(   0, 65, 64 , 10, element, fontSm, Black, Orange, justifyCenter);
  element = String("bank: " + String(sequenceArray[selectedChannel].bank)).c_str();
  gdispFillStringBox(   64, 65, 64 , 10, element, fontSm, Black, Orange, justifyCenter);
  */
}

void DisplayModule::timingMenuDisplay(){
  /*
  const char* element;

  element =  "TIMING SELECT";
  gdispFillStringBox(   0, 10, 128 , 20, element, fontMd, Orange, Purple, justifyCenter);

  if (sequenceArray[selectedChannel].stepCount > 1){
    element =  String("play " + String(sequenceArray[selectedChannel].stepCount) + " steps").c_str();
  } else {
    element =  String("play 1 step").c_str();
  }
  gdispFillStringBox(   0, 30, 128 , 10, element, fontSm, Orange, Purple, justifyCenter);

  if (sequenceArray[selectedChannel].stepCount > 1){
    element =  String("over " + String(sequenceArray[selectedChannel].beatCount) + " beats").c_str();
  } else {
    element =  String("over 1 beat").c_str();
  }

  gdispFillStringBox(   0, 40, 128 , 10, element, fontSm, Orange, Purple, justifyCenter);
*/
}
