
//static GDisplay* pixmap;
//static pixel_t* surface;


#define MAX_DISPLAY_ELEMENTS 17

    coord_t   i, j;

int color = 0;
uint32_t runcount;

char *displayCache[MAX_DISPLAY_ELEMENTS];
char *displayElement[MAX_DISPLAY_ELEMENTS];
uint8_t highlight;

void displayStartup(){
 
  gfxInit();

  width = gdispGetWidth();
  height = gdispGetHeight();

  fontTny = gdispOpenFont("fixed_5x8");
  fontSm = gdispOpenFont("DejaVuSans10");
  fontMd = gdispOpenFont("DejaVuSans12");
  fontLg = gdispOpenFont("DejaVuSans20");
  gdispClear(Black);

  nonBlockingRainbow(2);

  changeState(STEP_DISPLAY);
  delay(100);

  gdispFillStringBox( 0,  0, 128 , 32, "ZETAOHM",           fontMd , Red ,   Yellow, justifyCenter);
    nonBlockingRainbow(2);
  delay(500);
    nonBlockingRainbow(2);

  gdispFillStringBox( 0,  32, 128 , 32, "FLXS1" ,           fontLg , Green,  Black, justifyCenter);
    nonBlockingRainbow(2);

  delay(500);
    nonBlockingRainbow(2);

  gdispFillStringBox( 0,  64, 128 , 32, "MUSIC COMPUTER",   fontMd , Navy,   Orange, justifyCenter);
  nonBlockingRainbow(2);

  delay(500);
  nonBlockingRainbow(2);

  gdispClear(Black);

}

void displayLoop() {

 // if (previousState != currentState) { memset(displayCache, 0, sizeof(displayCache));}
  switch(currentState) {
    case CHANNEL_MENU:
      channelMenuDisplay();
    break;
    case STEP_DISPLAY:
      if (sequence[selectedChannel].instType == 1){
        gameOfLifeDisplay();
      } else {
        stepDisplay();
      }
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


    case DEBUG_SCREEN:
      debugScreenDisplay();
    break;
  }
  previousState = currentState;
}

// STATE VARIABLE DISPLAY HANDLERS

void globalMenuDisplay(){
  //nonBlockingRainbow(5);
  gdispFillStringBox(0, 0, width, 10, "ZETAOHM", fontSm , Red, Green, justifyCenter);
  gdispFillStringBox(0, height/2, width, 15,  "GLOBAL", fontSm, Blue, White, justifyCenter);
  gdispFillStringBox(0, height/3, width, 15,  "MENU", fontSm, White, Black, justifyCenter);
}

void gameOfLifeDisplay(){

  for(int row=0; row < LIFELINES; row++){
    for(int col=0; col < LIFEWIDTH; col++){

      color_t color;

      if (col == sequence[selectedChannel].activeStep) {
        if (life.grid[row][col] < 1){
          color = Yellow;
        } else {
          color = Magenta;
        }
   /*     } else if (life.grid[row][col] < 32){
          color =  Green;
        } else if (life.grid[row][col] < 48){
          color =  Red;
        } else if ( life.grid[row][col] < 64){
          color =  Purple;
        } else if ( life.grid[row][col] < 96){
          color = Yellow;
        } else if (life.grid[row][col] < 112){
          color = Yellow;
        } else if (life.grid[row][col] < 128){
          color = Olive;
        }

        */
      } else {
        if (life.grid[row][col] < 1){
          color = Black;
          } else if (life.grid[row][col] < 32){
          color =  Orange;
        } else if (life.grid[row][col] < 48){
          color =  Yellow;
        } else if ( life.grid[row][col] < 64){
          color =  Green;
        } else if ( life.grid[row][col] < 96){
          color = Blue;
        } else if (life.grid[row][col] < 112){
          color = Navy;
        } else if (life.grid[row][col] < 128){
          color = Magenta;
        }
      
      }

      gdispFillArea( ((col+0)*8), ((row+0)*8), 8, 8,  color);
    }
  }
}

void deleteMenuDisplay(){
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

};

void stepDisplayTest(){
  Serial.println(String(runcount) + "\t" + String(millis()) + "\tbegin stepdisplay test");

  char *buf = new char[255]; // sprintf buffer
  //displayElement[0] = "pitch";
  for (int i=0; i< 10000;i++){
    displayElement[0] = strdup("midiNotes[sequence[selectedChannel].stepData[selectedStep].pitch]");  
    //displayElement[0] = strdup(midiNotes[sequence[selectedChannel].stepData[selectedStep].pitch]);  
    //gdispFillStringBox( 64,  0, 64 , 10, displayElement[0], fontSm , White, Blue, justifyCenter);
    //free(displayElement[0]);

  }

  delete buf;
  ++runcount;

}


void renderStringBox(uint8_t index, uint8_t highlight, uint8_t previousHighlight, coord_t x, coord_t y, coord_t cx, coord_t cy, font_t font, color_t color, color_t bgColor, justify_t justify) {
  if ( strcmp(displayElement[index], displayCache[index]) != 0  || previousHighlight != highlight ) {  
    if (highlight == index) {
      gdispFillStringBox( x, y, cx , cy, displayElement[index], font , color , bgColor, justify);    
    } else {
      gdispFillStringBox( x, y, cx , cy, displayElement[index], font , bgColor , color, justify);    
    }
  }
}



void stepDisplay(){ 

  char *buf = new char[51]; // sprintf buffer
  uint8_t previousHighlight = highlight;
  bool overrideArray[MAX_DISPLAY_ELEMENTS];

  color_t foreground, background;

  switch (selectedChannel){
    case 0:
      foreground = Red;
      background = Black;

    break;
    case 1:
      foreground = Lime;
      background = Black;
    break;
    case 2:
      foreground = SkyBlue;
      background = Black;
    break;
    case 3:
      foreground = Silver;
      background = Black;
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

  // Pitch Display
  displayElement[0] = strdup("step info");
  renderStringBox(0, highlight, previousHighlight, 0,  0, 64 , 10, fontSm , background, foreground, justifyLeft);
  displayElement[1] = strdup(midiNotes[sequence[selectedChannel].stepData[selectedStep].pitch]);
  renderStringBox(1, highlight, previousHighlight, 0, 10, 64 , 22, fontLg , background , foreground, justifyLeft);

  //gate lentgh
  if ( sequence[selectedChannel].stepData[selectedStep].gateType == 0 ){
    displayElement[2] = strdup("L: rest");
  } else if (sequence[selectedChannel].stepData[selectedStep].gateLength == 0){
    displayElement[2] = strdup("L: pulse");
  } else if (sequence[selectedChannel].stepData[selectedStep].gateLength == 1){
    displayElement[2] = strdup("L: 1 step");
  } else {
    sprintf(buf, "L: %d steps", sequence[selectedChannel].stepData[selectedStep].gateLength);
    displayElement[2] = strdup(buf);  
  }
  renderStringBox(2, highlight, previousHighlight, 0, 32, 64 , 10, fontSm ,background , foreground, justifyLeft);

  //velocity
  sprintf(buf, "V: %d", sequence[selectedChannel].stepData[selectedStep].velocity);
  displayElement[3] = strdup(buf); 
  renderStringBox(3, highlight, previousHighlight, 0, 42, 64 , 10, fontSm ,background , foreground, justifyLeft);

  sprintf(buf, "%s", instrumentNames[sequence[selectedChannel].instrument]);  
  displayElement[4] = strdup(buf);
  renderStringBox(4, highlight, previousHighlight, 0, 50, 64 , 10, fontTny ,background , foreground, justifyLeft);

  sprintf(buf, "vol: %d", sequence[selectedChannel].volume);  
  displayElement[5] = strdup(buf);
  renderStringBox(5, highlight, previousHighlight, 0, 60, 64 , 10, fontTny ,background , foreground, justifyLeft);

  displayElement[7] = strdup("tempo");
  renderStringBox(7, highlight, previousHighlight, 64, 0, 64 , 10, fontSm ,Black , White, justifyCenter);

  if (extClock) {
    displayElement[6] = strdup("MIDI");
  } else {
    sprintf(buf, "%d bpm", tempoX100/100 );  
    displayElement[6] = strdup(buf);
  }
  renderStringBox(6, highlight, previousHighlight, 64, 10, 64 , 24, fontMd ,Black , White, justifyCenter);

  sprintf(buf, "%d steps", sequence[selectedChannel].stepCount);  
  displayElement[8] = strdup(buf);
  renderStringBox(8, highlight, previousHighlight, 64, 34, 64 , 10, fontTny ,Black , White, justifyLeft);

  sprintf(buf, "/ %d beats", sequence[selectedChannel].beatCount);  
  displayElement[9] = strdup(buf);
  renderStringBox(9, highlight, previousHighlight, 64, 44, 64 , 10, fontTny ,Black , White, justifyLeft);

  sprintf(buf, "stmd: %d", stepMode);  
  displayElement[10] = strdup(buf);
  renderStringBox(10, highlight, previousHighlight, 64, 60, 64 , 10, fontTny ,Black , White, justifyLeft);


  sprintf(buf, "steps: %d-%d", notePage*16 , (notePage+1)*16 );
  displayElement[11] = strdup(buf);
  renderStringBox(11, highlight, previousHighlight, 0, 70, 64 , 10, fontTny ,background , foreground, justifyLeft);

  sprintf(buf, "ch/pt: %d/%d", selectedChannel , sequence[selectedChannel].patternIndex );
  displayElement[12] = strdup(buf);
  renderStringBox(12, highlight, previousHighlight, 0, 80, 64 , 10, fontTny ,background , foreground, justifyLeft);


  sprintf(buf, "state: %d", currentState);
  sprintf(buf, "chan: %d", sequence[selectedChannel].activeStep);
  sprintf(buf, "stmd: %d", stepMode);
  sprintf(buf, "sqnc: %d", selectedChannel);
  sprintf(buf, "step: %d", selectedStep);

  for (int i=13; i< MAX_DISPLAY_ELEMENTS; i++){
    //filling display buffer with dummy data. dirty but i need to have a reliable buffer size.
    displayElement[i] = strdup("a");
  }

  for( int i=0; i< MAX_DISPLAY_ELEMENTS; i++ ){
      free(displayCache[i]);
      displayCache[i] = strdup(displayElement[i]);      
      free(displayElement[i]);
  }

  delete buf;  
}

void channelMenuDisplay() {
  uint8_t previousHighlight = highlight;
  highlight = selectedChannel;
  displayElement[0] = strdup("CHANNEL MENU");
  renderStringBox(0, highlight, previousHighlight, 0 , 0  , 128 , 16, fontSm,  White, Red, justifyCenter);

  displayElement[1] = strdup("INIT CH");
  renderStringBox(1, highlight, previousHighlight, 0 , 16 , 64 , 16, fontSm, White, Green,  justifyLeft);
 
  displayElement[2] = strdup("INIT PAT");
  renderStringBox(2, highlight, previousHighlight, 0 , 32 , 64 , 16, fontSm, White, Blue,  justifyLeft);

  displayElement[3] = strdup("DELETE SAVE FILE");
  renderStringBox(3, highlight, previousHighlight ,0, 48 , 128 , 16, fontSm, White, Black,  justifyCenter);

  displayElement[4] = strdup("TOGGLE GAME OF LIFE");
  renderStringBox(4, highlight, previousHighlight ,0, 64 , 128 , 16, fontSm, White, Black,  justifyCenter);


  displayElement[5] = strdup("EXTCLOCK");
  renderStringBox(5, highlight, previousHighlight, 64 , 16 , 64 , 16, fontSm, White, Green,  justifyLeft);
 
  displayElement[6] = strdup("--------");
  renderStringBox(6, highlight, previousHighlight, 64 , 32 , 64 , 16, fontSm, White, Blue,  justifyLeft);



  for (int i=7; i< MAX_DISPLAY_ELEMENTS; i++){
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
}

void patternSelectDisplay(){
    uint8_t previousHighlight = highlight;

  highlight = currentPattern;
  //nonBlockingRainbow(2);

  displayElement[16] = "PATTERN SELECT";
  renderStringBox(16, highlight, previousHighlight, 0, 0, 128 , 16, fontSm, Black, Pink, justifyCenter);
  displayElement[0] =  "0";
  renderStringBox(0, highlight, previousHighlight,  0, 16, 32 , 20, fontLg, Black, Red, justifyCenter);
  displayElement[1] =  "1";
  renderStringBox(1, highlight, previousHighlight, 32, 16, 32 , 20, fontLg, Black, Green, justifyCenter);
  displayElement[2] =  "2";
  renderStringBox(2, highlight, previousHighlight, 64, 16, 32 , 20, fontLg, Black, Blue, justifyCenter);
  displayElement[3] =  "3";
  renderStringBox(3, highlight, previousHighlight, 96, 16, 32 , 20, fontLg, Black, Purple, justifyCenter);

  displayElement[4] =  "4";
  renderStringBox(4, highlight, previousHighlight,  0, 36, 32 , 20, fontLg, Black, Red, justifyCenter);
  displayElement[5] =  "5";
  renderStringBox(5, highlight, previousHighlight, 32, 36, 32 , 20, fontLg, Black, Green, justifyCenter);
  displayElement[6] =  "6";
  renderStringBox(6, highlight, previousHighlight, 64, 36, 32 , 20, fontLg, Black, Blue, justifyCenter);
  displayElement[7] =  "7";
  renderStringBox(7, highlight, previousHighlight, 96, 36, 32 , 20, fontLg, Black, Purple, justifyCenter);

  displayElement[8] =  "8";
  renderStringBox(8, highlight, previousHighlight,  0, 56, 32 , 20, fontLg, Black, Red, justifyCenter);
  displayElement[9] =  "9";
  renderStringBox(9, highlight, previousHighlight, 32, 56, 32 , 20, fontLg, Black, Green, justifyCenter);
  displayElement[10] =  "A";
  renderStringBox(10, highlight,previousHighlight, 64, 56, 32 , 20, fontLg, Black, Blue, justifyCenter);
  displayElement[11] =  "B";
  renderStringBox(11, highlight,previousHighlight, 96, 56, 32 , 20, fontLg, Black, Purple, justifyCenter);

  displayElement[12] =  "C";
  renderStringBox(12, highlight,previousHighlight,  0, 76, 32 , 20, fontLg, Black, Red, justifyCenter);
  displayElement[13] =  "D";
  renderStringBox(13, highlight,previousHighlight, 32, 76, 32 , 20, fontLg, Black, Green, justifyCenter);
  displayElement[14] =  "E";
  renderStringBox(14, highlight,previousHighlight, 64, 76, 32 , 20, fontLg, Black, Blue, justifyCenter);
  displayElement[15] =  "F";
  renderStringBox(15, highlight,previousHighlight, 96, 76, 32 , 20, fontLg, Black, Purple, justifyCenter);

  for( int i=0; i< 17; i++ ){
    if (displayCache[i]){
      free(displayCache[i]);
    }
    if ( displayElement[i] ){
      displayCache[i] = strdup(displayElement[i]);      
     // free(displayElement[i]);
    }
  }

}

void sequenceMenuDisplay(){ 
  const char* element;

  element =  "INST";
  gdispFillStringBox(   0, 0, 32 , 24, element, fontSm, Orange, Red, justifyCenter);
  element =  "TIME";
  gdispFillStringBox(  32, 0, 32 , 24, element, fontSm, Red, Orange, justifyCenter);

}

void instrumentSelectDisplay(){
  const char* element;

  element =  "INSTRUMENT SELECT";
  gdispFillStringBox(   0, 31, 128 , 10, element, fontSm, Orange, Red, justifyCenter);

  element = String(instrumentNames[sequence[selectedChannel].instrument]).c_str();
  gdispFillStringBox(   0, 41, 128 , 24, element, fontSm, Red, Orange, justifyCenter);
  element = String("vol: " + String(sequence[selectedChannel].volume)).c_str();
  gdispFillStringBox(   0, 65, 64 , 10, element, fontSm, Black, Orange, justifyCenter);
  element = String("bank: " + String(sequence[selectedChannel].bank)).c_str();
  gdispFillStringBox(   64, 65, 64 , 10, element, fontSm, Black, Orange, justifyCenter);

}

void timingMenuDisplay(){
  const char* element;

  element =  "TIMING SELECT";
  gdispFillStringBox(   0, 10, 128 , 20, element, fontMd, Orange, Purple, justifyCenter);
 
  if (sequence[selectedChannel].stepCount > 1){
    element =  String("play " + String(sequence[selectedChannel].stepCount) + " steps").c_str(); 
  } else {
    element =  String("play 1 step").c_str();
  }
  gdispFillStringBox(   0, 30, 128 , 10, element, fontSm, Orange, Purple, justifyCenter);
 
  if (sequence[selectedChannel].stepCount > 1){
    element =  String("over " + String(sequence[selectedChannel].beatCount) + " beats").c_str(); 
  } else {
    element =  String("over 1 beat").c_str();
  }

  gdispFillStringBox(   0, 40, 128 , 10, element, fontSm, Orange, Purple, justifyCenter);

}


void debugScreenDisplay(){
  const char* element;

  element =  String(voltManual).c_str();
  gdispFillStringBox(   0, 30, 128 , 20, element, fontMd, White, Black, justifyCenter);

}
