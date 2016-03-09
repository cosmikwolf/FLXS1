
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

 // nonBlockingRainbow(2);
 // delay(100);
 // nonBlockingRainbow(2);
 // delay(100);
 // nonBlockingRainbow(2);
 // delay(100); 
 // nonBlockingRainbow(2);
 // delay(100);
 // nonBlockingRainbow(2);
 // delay(100);
  nonBlockingRainbow(2);
 // delay(100);
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
    case CHANNEL_SELECT:
      channelSelectDisplay();
    break;
    case STEP_DISPLAY:
      stepDisplay();
   // stepDisplayTest();
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


void deleteMenuDisplay(){
  uint8_t previousHighlight = highlight;
  highlight = selectedSequence;
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
    displayElement[0] = strdup("midiNotes[sequence[selectedSequence].stepData[selectedStep].pitch]");  
    //displayElement[0] = strdup(midiNotes[sequence[selectedSequence].stepData[selectedStep].pitch]);  
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

  switch (selectedSequence){
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
  displayElement[1] = strdup(midiNotes[sequence[selectedSequence].stepData[selectedStep].pitch]);
  renderStringBox(1, highlight, previousHighlight, 0, 10, 64 , 22, fontLg , background , foreground, justifyLeft);

  //gate lentgh
  if ( sequence[selectedSequence].stepData[selectedStep].gateType == 0 ){
    displayElement[2] = strdup("L: rest");
  } else if (sequence[selectedSequence].stepData[selectedStep].gateLength == 0){
    displayElement[2] = strdup("L: pulse");
  } else if (sequence[selectedSequence].stepData[selectedStep].gateLength == 1){
    displayElement[2] = strdup("L: 1 step");
  } else {
    sprintf(buf, "L: %d steps", sequence[selectedSequence].stepData[selectedStep].gateLength);
    displayElement[2] = strdup(buf);  
  }
  renderStringBox(2, highlight, previousHighlight, 0, 32, 64 , 10, fontSm ,background , foreground, justifyLeft);

  //velocity
  sprintf(buf, "V: %d", sequence[selectedSequence].stepData[selectedStep].velocity);
  displayElement[3] = strdup(buf); 
  renderStringBox(3, highlight, previousHighlight, 0, 42, 64 , 10, fontSm ,background , foreground, justifyLeft);

  sprintf(buf, "%s", instrumentNames[sequence[selectedSequence].instrument]);  
  displayElement[4] = strdup(buf);
  renderStringBox(4, highlight, previousHighlight, 0, 50, 64 , 10, fontTny ,background , foreground, justifyLeft);

  sprintf(buf, "vol: %d", sequence[selectedSequence].volume);  
  displayElement[5] = strdup(buf);
  renderStringBox(5, highlight, previousHighlight, 0, 60, 64 , 10, fontTny ,background , foreground, justifyLeft);


  displayElement[7] = strdup("tempo");
  renderStringBox(7, highlight, previousHighlight, 64, 0, 64 , 10, fontSm ,Black , White, justifyCenter);

  sprintf(buf, "%d bpm", tempoX100/100 );  
  displayElement[6] = strdup(buf);
  renderStringBox(6, highlight, previousHighlight, 64, 10, 64 , 24, fontMd ,Black , White, justifyCenter);

  sprintf(buf, "%d steps", sequence[selectedSequence].stepCount);  
  displayElement[8] = strdup(buf);
  renderStringBox(8, highlight, previousHighlight, 64, 34, 64 , 10, fontTny ,Black , White, justifyLeft);

  sprintf(buf, "/ %d beats", sequence[selectedSequence].beatCount);  
  displayElement[9] = strdup(buf);
  renderStringBox(9, highlight, previousHighlight, 64, 44, 64 , 10, fontTny ,Black , White, justifyLeft);

  sprintf(buf, "stmd: %d", stepMode);  
  displayElement[10] = strdup(buf);
  renderStringBox(10, highlight, previousHighlight, 64, 60, 64 , 10, fontTny ,Black , White, justifyLeft);


  sprintf(buf, "steps: %d-%d", notePage*16 , (notePage+1)*16 );
  displayElement[11] = strdup(buf);
  renderStringBox(11, highlight, previousHighlight, 0, 70, 64 , 10, fontTny ,background , foreground, justifyLeft);

  sprintf(buf, "state: %d", currentState);
  sprintf(buf, "chan: %d", sequence[selectedSequence].activeStep);
  sprintf(buf, "stmd: %d", stepMode);
  sprintf(buf, "sqnc: %d", selectedSequence);
  sprintf(buf, "step: %d", selectedStep);

  for (int i=12; i< MAX_DISPLAY_ELEMENTS; i++){
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

void channelSelectDisplay() {
  uint8_t previousHighlight = highlight;
  highlight = selectedSequence;
  displayElement[0] = strdup("1");
  renderStringBox(0, highlight, previousHighlight ,0, 64 , 32 , 32, fontLg,  White, Red, justifyCenter);

  displayElement[1] = strdup("2");
  renderStringBox(1, highlight, previousHighlight ,32, 64 , 32 , 32, fontLg, White, Green,  justifyCenter);
 
  displayElement[2] = strdup("3");
  renderStringBox(2, highlight, previousHighlight ,64, 64 , 32 , 32, fontLg, White, Blue,  justifyCenter);

  displayElement[3] = strdup("4");
  renderStringBox(3, highlight, previousHighlight ,96, 64 , 32 , 32, fontLg, White, Black,  justifyCenter);

  for (int i=4; i< MAX_DISPLAY_ELEMENTS; i++){
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

  element = String(instrumentNames[sequence[selectedSequence].instrument]).c_str();
  gdispFillStringBox(   0, 41, 128 , 24, element, fontSm, Red, Orange, justifyCenter);
  element = String("vol: " + String(sequence[selectedSequence].volume)).c_str();
  gdispFillStringBox(   0, 65, 64 , 10, element, fontSm, Black, Orange, justifyCenter);
  element = String("bank: " + String(sequence[selectedSequence].bank)).c_str();
  gdispFillStringBox(   64, 65, 64 , 10, element, fontSm, Black, Orange, justifyCenter);

}

void timingMenuDisplay(){
  const char* element;

  element =  "TIMING SELECT";
  gdispFillStringBox(   0, 10, 128 , 20, element, fontMd, Orange, Purple, justifyCenter);
 
  if (sequence[selectedSequence].stepCount > 1){
    element =  String("play " + String(sequence[selectedSequence].stepCount) + " steps").c_str(); 
  } else {
    element =  String("play 1 step").c_str();
  }
  gdispFillStringBox(   0, 30, 128 , 10, element, fontSm, Orange, Purple, justifyCenter);
 
  if (sequence[selectedSequence].stepCount > 1){
    element =  String("over " + String(sequence[selectedSequence].beatCount) + " beats").c_str(); 
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

/*

void menuItem(uint8_t menuItem){
  uint8_t globalSkip[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  uint8_t spedSkip[] = {1,2,3,5,6,7,8,9,10,11,12,13,14,15};
  uint8_t patternSkip[] = {8,9,10,11,12,13,14,15};
  uint8_t generatorSkip[] = {1,2,3,5,6,7,9,10,11,12,13,14,15};

  switch (menuItem){
    case GLOBAL_MIDI:
      nonBlockingRainbow(5, globalSkip, 15 );;

      display.setTextColor(WHITE)
      display.setCursor(9,1);
      display.setTextSize(2);
      display.println("MIDI MENU" );
      display.setCursor(0,24);

      display.setTextSize(1);
      display.print(" Midi Clock: " );
      if (extClock == true){
         display.println("External");
     } else {
        display.println("Internal");
      };
      break;

    case TIMING_MENU:

      nonBlockingRainbow(5, spedSkip, 14 );

      display.setTextColor(WHITE);
      display.setCursor(0,1);
      display.setTextSize(2);
      display.println("STEP SPEED" );
      display.setCursor(0,24);
      display.setTextSize(1);

      display.println("Speed = Steps/Beats" );

      if (menuSelection == 0){
        display.setTextColor(BLACK, WHITE);
      }
      display.print(" Steps: " );
      display.println(String(sequence[selectedSequence].stepCount));
      display.setTextColor(WHITE);
      if (menuSelection == 1){
        display.setTextColor(BLACK, WHITE);
      }
      display.print(" Beats: " );
      display.println(String(sequence[selectedSequence].beatCount));
      display.setTextColor(WHITE);

      break;
    case GENERATIVE_MENU:
      nonBlockingRainbow(2, generatorSkip, 13 );
      display.setTextColor(WHITE);
      display.setCursor(0,1);
      display.setTextSize(1);
      display.println("Sequence Generator");
      display.setCursor(0,20);
      display.println("Clear All Notes");
      display.println("Random some notes");
      display.println("Random all notes");
      display.println("delete save file");

      break;

    case f_SET:
      nonBlockingRainbow(5, globalSkip, 16 );

      display.setTextColor(WHITE);
      display.setCursor(0,0);
      display.setTextSize(2);
      display.println("TEMPO" );
      display.setCursor(30,20);
      display.println(String(tempo));
      display.setCursor(0,40);
      display.setTextSize(1);
      display.println("Manual Entry");
      display.setCursor(0,50);
      display.println("Tap Tempo");

      break;

    case PATTERN_SELECT:
      nonBlockingRainbow(3, patternSkip, 8 );

      display.setTextColor(WHITE);
      display.setCursor(0,0);
      display.setTextSize(3);
      display.println("PATTERN");
      display.setCursor(0,30);
      display.println("SELECT" );
      break;

    case SEQUENCE_SELECT:
      nonBlockingRainbow(3, patternSkip, 8 );

      display.setTextColor(WHITE);
      display.setCursor(0,0);
      display.setTextSize(3);
      display.println("SEQUENCE");
      display.setCursor(0,30);
      display.println("SELECT" );
      break;

    case SEQUENCE_QUAN:
      nonBlockingRainbow(2, generatorSkip, 13 );
      display.setTextColor(WHITE);
      display.setCursor(0,1);
      display.setTextSize(1);
      display.println("Note Quantizer");
      display.setCursor(0,20);
      display.println("Quantize On - B Minor");
      display.println("Quantize On - D Major");
      display.println("Quantize Off");
      
      break;

    case INSTRUMENT_MENU:
      display.setTextColor(WHITE);
      display.setCursor(0,1);
      display.setTextSize(1);
      display.println("Instrument Select");
      display.setCursor(0,20);
      if (menuSelection == 0){
        display.setTextColor(BLACK, WHITE);
      }
      display.println(instrumentNames[sequence[selectedSequence].instrument] );
      if (menuSelection == 1){
        display.setTextColor(BLACK, WHITE);
      } else {
        display.setTextColor(WHITE, BLACK);
      }
      display.println("volume: " + String(sequence[selectedSequence].volume) );
      if (menuSelection == 2){
        display.setTextColor(BLACK, WHITE);
      } else {
        display.setTextColor(WHITE, BLACK);
      }
      display.println("bank: " + String(sequence[selectedSequence].bank) );
      break;
 //   case SEQUENCE_TRAN:
 //     nonBlockingRainbow(2, generatorSkip, 13 );
 //     display.setTextColor(WHITE);
 //     display.setCursor(0,1);
 //     display.setTextSize(1);
 //     display.println("Note Transposer - NOT IMPLEMENTED YET");
 //     display.setCursor(0,20);
 //     display.println("Transpose 12 notes");
 //     display.println("Transpose Off");
 //     
      break;

    case GLOBAL_FILE:
      display.setTextColor(WHITE);
      display.setCursor(0,1);
      display.setTextSize(1);

      display.println("Delete Save File?");
      break;

    default:
      display.setTextColor(WHITE);
      display.setCursor(0,1);
      display.setTextSize(1);
      display.println("settingMode " + String(menuItem) + " not yet defined");
      break;

  };
}

void sequenceMenuDisplay(){

  uint8_t skip[] = {11,15};
  nonBlockingRainbow(5, skip, 2 );

  display.setTextColor(WHITE);
  display.setCursor(16,1);
  display.setTextSize(2);
  display.println("SEQUENCE");
  display.setTextSize(1);
  display.setCursor(7,21);
  display.print("NAME ");
  display.print("SPED ");
  display.print("TRAN ");
  display.print("INST ");
  display.setCursor(7,33);
  display.print("QUAN ");
  display.print("EUCL ");
  display.print("GENE ");
  display.print("ORDE ");
  display.setCursor(7,45);
  display.print("RAND ");
  display.print("POSI ");
  display.print("GLID ");
  display.print(" ");
  display.setCursor(7,57);
  display.print("MIDI ");
  display.print("{CV} ");
  display.print("GATE ");
  display.print(" ");
}

void globalMenuDisplay(){
  // Second Press: Glbal Menu
//      MIDI  SAVE  LOAD   

  uint8_t skip[] = {4,5,6,7,8,9,10,11,12,13,14,15};
  nonBlockingRainbow(10, skip, 12 );

  display.setTextColor(WHITE);
  display.setCursor(16,1);
  display.setTextSize(2);
  display.println("~GLOBAL~");
  display.setTextSize(1);
  display.setCursor(7,21);
  display.print("MIDI ");
  display.print("SAVE ");
  display.print("LOAD ");
  display.print("FILE ");
  display.setCursor(7,33);
}

void stepDisplay(){
// MAIN STEP DISPLAY 

//  display.drawTriangle( 120,0,
//                          113,9,
//                          127,9, WHITE);
//  if (tempoBlip == true) {
//    display.fillTriangle( 120,0,
//                          113,9,
//                          127,9, WHITE);
//  }
//  if (blipTimer > 20000) {
//    tempoBlip = false;
//  }
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(86,45);
  display.println( String(selectedSequence) + "|" + String(currentPattern) );

  display.setTextSize(1);
  display.setCursor(86,35);
    display.setTextColor(WHITE);
  if (tempoBlip) { display.setTextColor(BLACK, WHITE); };
  if (extClock == true) {
    display.println("EXT");
  } else {
    display.println(String(tempo));
  }
  display.setTextColor(WHITE);

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);


    if (stepMode == 0){
      display.setTextColor(BLACK, WHITE);
    }
    display.setTextSize(3);
    display.print(
      midiNotes[sequence[selectedSequence].stepData[selectedStep].pitch][0]
    );
    display.setTextSize(2);
    display.print(
      midiNotes[sequence[selectedSequence].stepData[selectedStep].pitch][1]
    );
    display.print(
      midiNotes[sequence[selectedSequence].stepData[selectedStep].pitch][2]
    );
        display.print(
      midiNotes[sequence[selectedSequence].stepData[selectedStep].pitch][3]
    );
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(50,0);
    if (stepMode == 2){
      display.setTextColor(BLACK, WHITE);
    }
    if (sequence[selectedSequence].stepData[selectedStep].gateType != 0){
      display.println(" gate: " + String(sequence[selectedSequence].stepData[selectedStep].gateType));
    } else {
      display.println(" ~rest~");
    }
    display.setTextColor(WHITE);
    display.setCursor(50,8);

    display.println(" velo: " + String(sequence[selectedSequence].stepData[selectedStep].velocity));

    if (stepMode == 1){
      display.setTextColor(BLACK, WHITE);
    }
        display.setCursor(50,16);

    display.println(" beat: " + String(sequence[selectedSequence].stepData[selectedStep].gateLength));
   // display.println(String(sequence[selectedSequence].gateLength[selectedStep]) + " Steps"
      display.setTextColor(WHITE);


      if (playing) {
        display.fillTriangle(
          105,5,
          105,25,
          125,15,
          WHITE
          );
      } else {
        display.fillRoundRect(
          105,5,
          8, 20,
          0, WHITE
          );
        display.fillRoundRect(
          117,5,
          8, 20,
          0, WHITE
          );
      }


  display.setCursor(0,24);

  if (stepMode == 3){
    display.setTextColor(BLACK, WHITE);
  }
    display.println("Time: " +String(sequence[selectedSequence].stepCount) + "/" + String(sequence[selectedSequence].beatCount));
  display.setTextColor(WHITE);


 //display.println(F("tempo:") + String(sequence[selectedSequence].tempo));
  display.println( "as: " + String(sequence[selectedSequence].activeStep) + " bt: " + String(sequence[selectedSequence].beatTracker));
  //display.println(F("stepCount: ") + String(sequence[selectedSequence].stepCount));
 // display.println("millis: " + String(millis()));
  //display.println("activeSection: " + activeSection);

//display.println("Playing: " + String(playing));
 // display.println("Peak: " + String(peak1.read()) + " 2: " + String(peak2.read()));
 // display.println("CPU:" + String(AudioProcessorUsage()) + " MAX:" + String(AudioProcessorUsageMax()));
 // display.println("MEM:" + String(AudioMemoryUsage()) + " MAX:" + String(AudioMemoryUsageMax()));
//  display.println(F("avgPeriod:") + String(avgPeriod));
 // display.println(F("avgLoopTime:") + String(avgLoopTime));
//  display.println(F("avgRuntime:") + String(avgRuntime));
  display.println("inst: " + String(sequence[selectedSequence].instrument));
display.println("millis: " + String(millis()));
}

*/
