
//static GDisplay* pixmap;
//static pixel_t* surface;

#define PIXMAP_WIDTH  120
#define PIXMAP_HEIGHT 80

int color = 0;

void displayStartup(){
  gfxInit();
  gdispSetOrientation(GDISP_ROTATE_0);

  width = gdispGetWidth();
  height = gdispGetHeight();

 // pixmap = gdispPixmapCreate(PIXMAP_WIDTH, PIXMAP_HEIGHT);
  //surface = gdispPixmapGetBits(pixmap);


  font = gdispOpenFont("UI1");
  gdispClear(Black);

 // gdispGDrawStringBox(pixmap, 0, 0, width, 50, "ZETAOHM", font, White, justifyCenter);
 // gdispGDrawStringBox(pixmap, 0, height/2, width, 30, "FLXS1 SEQUENCER", font, White, justifyCenter);

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
  changeState(STEP_MODE);
  //gdispBlitArea(0, 0, PIXMAP_WIDTH, PIXMAP_HEIGHT, surface);


}

void displayLoop() {
  //Serial.println(F("displayStart"));
  if (displayTimer > 1000) {
   // runState(STEP_MODE);
stepDisplay();
 //  if (settingMode == 0){
 //    stepDisplay();
 //  } else if (settingMode == 1){
 //    sequenceMenuDisplay();
 //  } else if (settingMode == 2){
 //    globalMenuDisplay();
 //  } else {
 //    menuItem(settingMode);
 //  }
   displayTimer = 0;
  }

 // gdispBlitArea(0, 0, PIXMAP_WIDTH, PIXMAP_HEIGHT, surface);
}

void menuItem(uint8_t menuItem){ 
  nonBlockingRainbow(2);
  gdispDrawStringBox(0, 0, width, 50, "ZETAOHM", font, White, justifyCenter);
  gdispDrawStringBox(0, height/2, width, 30, "MENU MODE", font, White, justifyCenter);
 
}

void sequenceMenuDisplay(){
  nonBlockingRainbow(2);
  gdispFillStringBox(0, 0, width, 10, "ZETAOHM", font , Red, Green, justifyCenter);
  gdispFillStringBox(0, height/2, width, 15,  "SEQUENCE", font, Blue, White, justifyCenter);
  gdispFillStringBox(0, height/3, width, 15,  "MEMU", font, White, Black, justifyCenter);
 }

void globalMenuDisplay(){
  nonBlockingRainbow(5);
  gdispFillStringBox(0, 0, width, 10, "ZETAOHM", font , Red, Green, justifyCenter);
  gdispFillStringBox(0, height/2, width, 15,  "GLOBAL", font, Blue, White, justifyCenter);
  gdispFillStringBox(0, height/3, width, 15,  "MENU", font, White, Black, justifyCenter);

}

void stepDisplay(){ 
  elapsedMicros timer1 = 0;
  //colorWipe(pixels.Color(255, 0, 0), 50); 
  gdispFillStringBox( 0, 0, width, 10, "ZETAOHM", font , Red, Green, justifyCenter);
  gdispFillStringBox( 0, 20, width, 10,  "STEPMODE", font, Blue, White, justifyCenter);
  gdispFillStringBox( 0, 30, width, 20,  "OMG", font, White, Black, justifyCenter);

  //String(selectedSequence) + "|" + String(currentPattern)
}


/*

void menuItem(uint8_t menuItem){
  uint8_t globalSkip[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  uint8_t spedSkip[] = {1,2,3,5,6,7,8,9,10,11,12,13,14,15};
  uint8_t patternSkip[] = {8,9,10,11,12,13,14,15};
  uint8_t generatorSkip[] = {1,2,3,5,6,7,9,10,11,12,13,14,15};

  switch (menuItem){
    case GLOBAL_MIDI:
      nonBlockingRainbow(5, globalSkip, 15 );

      display.setTextColor(WHITE);
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

    case SEQUENCE_SPED:

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
    case SEQUENCE_GENE:
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

    case SEQUENCE_INST:
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
