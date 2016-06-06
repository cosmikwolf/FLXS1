#include <Arduino.h>
#include "DisplayModule.h"

/*#include "../commonFonts/fonts/font_ArialBold.cpp"
#include "../commonFonts/fonts/font_Arial.cpp"
*/
//    coord_t   i, j;
elapsedMicros displayTimer;
DisplayModule::DisplayModule(){
};

void DisplayModule::initialize(Sequencer (*sequenceArray)[4]){

  Serial.println("Initializing Display");

  this->sequenceArray = sequenceArray;
  oled.begin();
  oled.fillScreen(BLACK);     delay(10);
  oled.fillScreen(RED);     delay(10);
  oled.fillScreen(ORANGE);      delay(10);
  oled.fillScreen(YELLOW);      delay(10);
  oled.fillScreen(GREEN);     delay(10);
  oled.fillScreen(BLUE);      delay(10);
  oled.fillScreen(NAVY);      delay(10);
  oled.fillScreen(PURPLE);      delay(10);
  oled.fillScreen(RED);     delay(10);
  oled.fillScreen(ORANGE);      delay(10);
  oled.fillScreen(YELLOW);      delay(10);
  oled.fillScreen(GREEN);     delay(10);
  oled.fillScreen(BLUE);      delay(10);
  oled.fillScreen(NAVY);      delay(10);
  oled.fillScreen(PURPLE);       delay(100);
  oled.invert(true);          delay(100);
  oled.invert(false);
  oled.fillScreen(RED);
  oled.setCursor(0, 5);
  oled.setTextColor(BLACK);
  oled.setTextSize(4);
  oled.println("ZETAOHM");    delay(100);
  oled.invert(true);    delay(100);
  oled.invert(false);
  oled.fillScreen(GREEN);
  oled.setCursor(0, 5);
  oled.setTextColor(PINK);
  oled.setTextSize(4);
  oled.println("FLXS1");    delay(100);
  oled.invert(true);    delay(100);
  oled.invert(false);
  oled.fillScreen(BLUE);
  oled.setCursor(0, 5);
  oled.setTextColor(ORANGE);
  oled.setTextSize(4);
  oled.println("MUSIKCOMPUTER");    delay(100);
  oled.invert(true);    delay(100);
  oled.invert(false);
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

void DisplayModule::displayLoop() {

  if( displayTimer > 10000){
	  displayTimer = 0;

   // if (previousState != currentState) { memset(displayCache, 0, sizeof(displayCache));}
    if (previousState != currentState || previouslySelectedChannel != selectedChannel){
       Serial.println("about to display a new state");
       freeDisplayCache();
       oled.clearScreen();
    }

    switch(currentState) {
      case CHANNEL_MENU:
        channelMenuDisplay(buf);
      break;
      case STEP_DISPLAY:
      //  if (sequenceArray[selectedChannel]->instType == 1){
       //   gameOfLifeDisplay();
      //  } else {
        stepDisplay(buf);
      //  }
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
    oled.setCursor(x, y);
    oled.setTextSize(textSize);
    oled.fillRect(x,y,w,h, color2);
    oled.setTextColor(color1, color2);
    oled.println(displayElement[index]);
    if (border){
      oled.drawRect(x,y,w,h, color1);
    }
  }
}

void DisplayModule::stepDisplay(char *buf){

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
  if (stepMode == 0) {
    highlight = 1;
  } else if (stepMode == 1){
    highlight = 2;
  } else if (stepMode == 2) {
    highlight = 3;
  } else if (stepMode == 3) {
    highlight = 6;
  } else if (stepMode == 4) {
    highlight = 4;
  } else if (stepMode == 5) {
    highlight = 5;
  } else if (stepMode == 6) {
    highlight = 8;
  } else if (stepMode == 7) {
    highlight = 9;
  };

  displayElement[0] = strdup("step info");
  displayElement[1] = strdup(midiNotes[sequenceArray[selectedChannel]->stepData[selectedStep].pitch]);

  if ( sequenceArray[selectedChannel]->stepData[selectedStep].gateType == 0 ){
    displayElement[2] = strdup("L: rest");
  } else if (sequenceArray[selectedChannel]->stepData[selectedStep].gateLength == 0){
    displayElement[2] = strdup("L: pulse");
  } else if (sequenceArray[selectedChannel]->stepData[selectedStep].gateLength == 1){
    displayElement[2] = strdup("L: 1 step");
  } else {
    sprintf(buf, "L: %d steps", sequenceArray[selectedChannel]->stepData[selectedStep].gateLength);
    sprintf(buf, "L: %d steps", sequenceArray[selectedChannel]->stepData[selectedStep].gateLength);
    displayElement[2] = strdup(buf);
  }

  sprintf(buf, "V: %d", sequenceArray[selectedChannel]->stepData[selectedStep].velocity);
  displayElement[3] = strdup(buf);

  sprintf(buf, "%s", instrumentNames[sequenceArray[selectedChannel]->instrument]);
  displayElement[4] = strdup(buf);

  sprintf(buf, "vol: %d", sequenceArray[selectedChannel]->volume);
  displayElement[5] = strdup(buf);

  displayElement[7] = strdup("TEMPO");

  if (extClock) {
    displayElement[6] = strdup("MIDI");
  } else {
    sprintf(buf, "%d bpm", int(tempoX100/100) );
    displayElement[6] = strdup(buf);
  };

  sprintf(buf, "%d steps", sequenceArray[selectedChannel]->stepCount);
  displayElement[8] = strdup(buf);

  sprintf(buf, "/ %d beats", sequenceArray[selectedChannel]->beatCount);
  displayElement[9] = strdup(buf);

  sprintf(buf, "stmd: %d", stepMode);
  displayElement[10] = strdup(buf);

  sprintf(buf, "steps: %d-%d", notePage*16 , (notePage+1)*16 );
  displayElement[11] = strdup(buf);

  sprintf(buf, "ch/pt: %d/%d", selectedChannel , sequenceArray[selectedChannel]->patternIndex );

  renderOnce_StringBox(0,  highlight, previousHighlight, 0,   0, 64, 10, false, 1, background, foreground);
  renderOnce_StringBox(1,  highlight, previousHighlight, 0,  10, 64, 22, false, 2, background , foreground);
  renderOnce_StringBox(2,  highlight, previousHighlight, 0,  32, 64, 10, false, 1, background , foreground);
  renderOnce_StringBox(3,  highlight, previousHighlight, 0,  42, 64, 10, false, 1 ,background , foreground);
  renderOnce_StringBox(4,  highlight, previousHighlight, 0,  50, 64, 10, false, 1 ,background , foreground);
  renderOnce_StringBox(5,  highlight, previousHighlight, 0,  60, 64, 10, false, 1 ,background , foreground);
  renderOnce_StringBox(6,  highlight, previousHighlight, 64, 10, 64, 24, false, 1 ,BLACK , WHITE);
  renderOnce_StringBox(7,  highlight, previousHighlight, 64,  0, 64, 10, false, 1 ,BLACK , WHITE);
  renderOnce_StringBox(8,  highlight, previousHighlight, 64, 34, 64, 10, false, 0 ,BLACK , WHITE);
  renderOnce_StringBox(9,  highlight, previousHighlight, 64, 44, 64, 10, false, 0 ,BLACK , WHITE);
  renderOnce_StringBox(10, highlight, previousHighlight, 64, 60, 64, 10, false, 1 ,BLACK , WHITE);
  renderOnce_StringBox(11, highlight, previousHighlight, 0,  70, 64, 10, false, 1 ,background , foreground);
  renderOnce_StringBox(12, highlight, previousHighlight, 0,  80, 64, 10, false, 1 ,background , foreground);

  //sprintf(buf, "state: %d", currentState);
  //sprintf(buf, "chan: %d", sequenceArray[selectedChannel]->activeStep);
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

void DisplayModule::channelMenuDisplay(char *buf) {

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

  renderOnce_StringBox(0, highlight, previousHighlight,  0, 16, 32 , 20, false, 1, BLACK, RED);
  renderOnce_StringBox(1, highlight, previousHighlight, 32, 16, 32 , 20, false, 1, BLACK, GREEN);
  renderOnce_StringBox(2, highlight, previousHighlight, 64, 16, 32 , 20, false, 1, BLACK, BLUE);
  renderOnce_StringBox(3, highlight, previousHighlight, 96, 16, 32 , 20, false, 1, BLACK, PURPLE);
  renderOnce_StringBox(4, highlight, previousHighlight,  0, 36, 32 , 20, false, 1, BLACK, RED);
  renderOnce_StringBox(5, highlight, previousHighlight, 32, 36, 32 , 20, false, 1, BLACK, GREEN);
  renderOnce_StringBox(6, highlight, previousHighlight, 64, 36, 32 , 20, false, 1, BLACK, BLUE);
  renderOnce_StringBox(7, highlight, previousHighlight, 96, 36, 32 , 20, false, 1, BLACK, PURPLE);
  renderOnce_StringBox(8, highlight, previousHighlight,  0, 56, 32 , 20, false, 1, BLACK, RED);
  renderOnce_StringBox(9, highlight, previousHighlight, 32, 56, 32 , 20, false, 1, BLACK, GREEN);
  renderOnce_StringBox(10, highlight,previousHighlight, 64, 56, 32 , 20, false, 1, BLACK, BLUE);
  renderOnce_StringBox(11, highlight,previousHighlight, 96, 56, 32 , 20, false, 1, BLACK, PURPLE);
  renderOnce_StringBox(12, highlight,previousHighlight,  0, 76, 32 , 20, false, 1, BLACK, RED);
  renderOnce_StringBox(13, highlight,previousHighlight, 32, 76, 32 , 20, false, 1, BLACK, GREEN);
  renderOnce_StringBox(14, highlight,previousHighlight, 64, 76, 32 , 20, false, 1, BLACK, BLUE);
  renderOnce_StringBox(15, highlight,previousHighlight, 96, 76, 32 , 20, false, 1, BLACK, PURPLE);
  renderOnce_StringBox(16, highlight, previousHighlight, 0, 0, 128 , 16, false, 1, BLACK, PINK);

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
  //      if (col == sequenceArray[selectedChannel]->activeStep) {
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

void DisplayModule::sequenceMenuDisplay(){
  /*
  const char* element;

  element =  "INST";
  gdispFillStringBox(   0, 0, 32 , 24, element, fontSm, Orange, Red, justifyCenter);
  element =  "TIME";
  gdispFillStringBox(  32, 0, 32 , 24, element, fontSm, Red, Orange, justifyCenter);
  */
}

void DisplayModule::instrumentSelectDisplay(){
  /*
  const char* element;

  element =  "INSTRUMENT SELECT";
  gdispFillStringBox(   0, 31, 128 , 10, element, fontSm, Orange, Red, justifyCenter);

  element = String(instrumentNames[sequenceArray[selectedChannel]->instrument]).c_str();
  gdispFillStringBox(   0, 41, 128 , 24, element, fontSm, Red, Orange, justifyCenter);
  element = String("vol: " + String(sequenceArray[selectedChannel]->volume)).c_str();
  gdispFillStringBox(   0, 65, 64 , 10, element, fontSm, Black, Orange, justifyCenter);
  element = String("bank: " + String(sequenceArray[selectedChannel]->bank)).c_str();
  gdispFillStringBox(   64, 65, 64 , 10, element, fontSm, Black, Orange, justifyCenter);
  */
}

void DisplayModule::timingMenuDisplay(){
  /*
  const char* element;

  element =  "TIMING SELECT";
  gdispFillStringBox(   0, 10, 128 , 20, element, fontMd, Orange, Purple, justifyCenter);

  if (sequenceArray[selectedChannel]->stepCount > 1){
    element =  String("play " + String(sequenceArray[selectedChannel]->stepCount) + " steps").c_str();
  } else {
    element =  String("play 1 step").c_str();
  }
  gdispFillStringBox(   0, 30, 128 , 10, element, fontSm, Orange, Purple, justifyCenter);

  if (sequenceArray[selectedChannel]->stepCount > 1){
    element =  String("over " + String(sequenceArray[selectedChannel]->beatCount) + " beats").c_str();
  } else {
    element =  String("over 1 beat").c_str();
  }

  gdispFillStringBox(   0, 40, 128 , 10, element, fontSm, Orange, Purple, justifyCenter);
*/
}
