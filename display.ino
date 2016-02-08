
//static GDisplay* pixmap;
//static pixel_t* surface;

#define PIXMAP_WIDTH  120
#define PIXMAP_HEIGHT 80
    coord_t   i, j;

int color = 0;

void displayStartup(){
 
  gfxInit();

  width = gdispGetWidth();
  height = gdispGetHeight();

  fontTny = gdispOpenFont("fixed_5x8");
  fontSm = gdispOpenFont("DejaVuSans10");
  fontMd = gdispOpenFont("DejaVuSans20");
  fontLg = gdispOpenFont("DejaVuSans32");
  gdispClear(Black);

  nonBlockingRainbow(2);
  delay(100);
  nonBlockingRainbow(2);
  delay(100);
  nonBlockingRainbow(2);
  delay(100); 
  nonBlockingRainbow(2);
  delay(100);
  nonBlockingRainbow(2);
  delay(100);
  nonBlockingRainbow(2);
  delay(100);
  changeState(STEP_DISPLAY);

  gdispFillStringBox( 64,  0, 64 , 10, "TESTING", fontSm , White, Blue, justifyCenter);
  
}

void displayLoop() {
  if (displayTimer > 50000) {
    switch(currentState) {
      case CHANNEL_SELECT:
        channelSelectDisplay();
      break;
      case STEP_DISPLAY:
        stepDisplay();
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

    }
  }
}

// STATE VARIABLE DISPLAY HANDLERS

void globalMenuDisplay(){
  //nonBlockingRainbow(5);
  gdispFillStringBox(0, 0, width, 10, "ZETAOHM", fontSm , Red, Green, justifyCenter);
  gdispFillStringBox(0, height/2, width, 15,  "GLOBAL", fontSm, Blue, White, justifyCenter);
  gdispFillStringBox(0, height/3, width, 15,  "MENU", fontSm, White, Black, justifyCenter);
}

void stepDisplay(){ 
  elapsedMicros timer1 = 0;
  const char* element;
  
  // Pitch Display
  gdispFillStringBox( 64,  0, 64 , 10, "pitch", fontSm , White, Blue, justifyCenter);
  element = String(midiNotes[sequence[selectedSequence].stepData[selectedStep].pitch]).c_str();
  gdispFillStringBox( 64, 10, 64 , 24, element, fontMd ,Blue , White, justifyCenter);

  element = String(midiNotes[sequence[selectedSequence].stepData[selectedStep].pitch]).c_str();
  gdispFillStringBox( 64, 10, 64 , 24, element, fontMd ,Blue , White, justifyCenter);

  if ( sequence[selectedSequence].stepData[selectedStep].gateType == 0 ){
    element = String("Note Off").c_str();
  } else if (sequence[selectedSequence].stepData[selectedStep].gateLength == 0){
    element = String("pulse").c_str();
  } else if (sequence[selectedSequence].stepData[selectedStep].gateLength == 1){
    element = String("1 beat").c_str();
  } else {
    element = String(String(sequence[selectedSequence].stepData[selectedStep].gateLength) + " beats").c_str();
  }
  gdispFillStringBox( 64, 34, 64 , 10, element, fontSm ,Blue , White, justifyCenter);

  // Instrument selection
  element = String(instrumentNames[sequence[selectedSequence].instrument]).c_str();
  gdispFillStringBox( 64, 44, 64 , 10, element, fontTny ,White , Blue, justifyCenter);

  // Selected Sequence Number
  element =  String("ch: " + String(selectedSequence+1)).c_str();
  gdispFillStringBox( 64, 86, 32 , 10, element, fontSm, Blue, White, justifyCenter);

  element = String("pt: " + String(currentPattern)).c_str();
  gdispFillStringBox( 96, 86, 32 , 10, element, fontSm, White, Magenta, justifyCenter);

  element =  String("state: " + String(currentState)).c_str();
  gdispFillStringBox( 0, 0, 64 , 10, element, fontSm, Green, White, justifyCenter);
  
  element =  String("step: " + String(sequence[selectedSequence].activeStep)).c_str();
  gdispFillStringBox( 0, 10, 64 , 10, element, fontSm, Green, White, justifyCenter);

  element =  String("stmd: " + String(stepMode)).c_str();
  gdispFillStringBox( 0, 20, 64 , 10, element, fontSm, Green, White, justifyCenter);
  
  element =  String("sqnc: " + String(selectedSequence)).c_str();
  gdispFillStringBox( 0, 30, 64 , 10, element, fontSm, Green, White, justifyCenter);

  element =  String("step: " + String(selectedStep)).c_str();
  gdispFillStringBox( 0, 40, 64 , 10, element, fontSm, Green, White, justifyCenter);

}

void channelSelectDisplay() {
  const char* element;

  element =  "1";
  gdispFillStringBox(  0, 0, 32 , 96, element, fontLg, Red, White, justifyCenter);
  element =  "2";
  gdispFillStringBox( 32, 0, 32 , 96, element, fontLg, Green, White, justifyCenter);
  element =  "3";
  gdispFillStringBox( 64, 0, 32 , 96, element, fontLg, Blue, White, justifyCenter);
  element =  "4";
  gdispFillStringBox( 96, 0, 32 , 96, element, fontLg, Purple, White, justifyCenter);

}

void patternSelectDisplay(){
 // nonBlockingRainbow(2);
  gdispFillStringBox(0, 0, width, 10, "ZETAOHM", fontSm , Red, Green, justifyCenter);
  gdispFillStringBox(0, height/2, width, 15,  "SEQUENCE", fontSm, Blue, White, justifyCenter);
  gdispFillStringBox(0, height/3, width, 15,  "MEMU", fontSm, White, Black, justifyCenter);
 }

void sequenceMenuDisplay(){ 
  const char* element;

  element =  "INST";
  gdispFillStringBox(   0, 0, 32 , 24, element, fontSm, Orange, Red, justifyCenter);
  element =  "TIME";
  gdispFillStringBox(  32, 0, 32 , 24, element, fontSm, Red, Orange, justifyCenter);
  element =  "1";
  gdispFillStringBox(  64, 0, 32 , 24, element, fontLg, Red, Blue, justifyCenter);
  element =  "1";
  gdispFillStringBox(  96, 0, 32 , 24, element, fontLg, Red, Purple, justifyCenter);

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

    case TEMPO_SET:
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
