#include <Arduino.h>
#include "DisplayModule.h"

//custom colors
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
  delay(100);
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
/*  delete [] displayCache;
  delete [] displayElement;
  for (int i=0; i< MAX_DISPLAY_ELEMENTS; i++){
    displayCache[i] = NULL;
    displayElement[i] = NULL;
  }
  */
  for (int i=0; i< MAX_DISPLAY_ELEMENTS; i++){
    //clear displaycache so all data redraws.
      free(displayCache[i]);
      displayCache[i] = nullptr;
      free(displayElement[i]);
      displayElement[i] = nullptr;
  }
}

void DisplayModule::cleanupTextBuffers(){
/*  delete [] displayCache;
  for( int i=0; i< MAX_DISPLAY_ELEMENTS; i++ ){
    displayCache[i] = strdup(displayElement[i]);
  }
  delete [] displayElement;
  for (int i=0; i< MAX_DISPLAY_ELEMENTS; i++){
    displayCache[i] = NULL;
    displayElement[i] = NULL;
  }
*/
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

   // if (previousState != currentState) { memset(displayCache, 0, sizeof(displayCache));}

    switch (selectedChannel){
      case 0:
        foreground = GREENYELLOW;
        background = DARKPURPLE;
        break;
      case 1:
        foreground = GREEN;
        background = NAVY;
        break;
      case 2:
        foreground = WHITE;
        background = MAROON;
        break;
      case 3:
        foreground = ORANGE;
        background = NAVY;
        break;
    }
    if (previousState != currentState || previouslySelectedChannel != selectedChannel){
       Serial.println("about to display a new state");
       freeDisplayCache();
//       oled.clearScreen();
       oled.fillScreen(background);
    }

    switch(currentState) {

      case CHANNEL_PITCH_MODE:
        channelPitchMenuDisplay(buf);
      break;
      case CHANNEL_VELOCITY_MODE:
        channelVelocityMenuDisplay(buf);
      break;
      case CHANNEL_ENVELOPE_MODE:
        channelEnvelopeMenuDisplay(buf);
      break;
      case CHANNEL_STEP_MODE:
        channelStepMenuDisplay(buf);
      break;

      case CHANNEL_TUNER_MODE:
        channelTunerDisplay(buf);
      break;

      case PATTERN_SELECT:
        patternSelectDisplay();
      break;

      case SEQUENCE_MENU:
        sequenceMenuDisplay();
      break;

      case GLOBAL_MENU:
        globalMenuDisplay();
      break;

      case TEMPO_MENU:
        tempoMenuDisplay();
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

      case INPUT_DEBUG_MENU:
        inputDebugMenuDisplay();
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
    if (previousState != currentState){
      Serial.println("text buffers cleaned up");
      Serial.println("Freeram: " + String(FreeRam2()));
    }
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
    } else if( textSize == 3){
      oled.setCursor(x+1, y-4);
      oled.setFont(&Font);
      oled.setTextScale(textSize-2, textSize-1);
    } else {
      oled.setCursor(x+1, y );
      oled.setFont(&unborn_small);
      oled.setTextScale(1,2);
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
  //oled.setFont(Arial_8);

  switch (stepMode) {
    case STEPMODE_PITCH0:
      highlight = 1;    break;
    case STEPMODE_GATELENGTH:
      highlight = 6;    break;
    case STEPMODE_CHORD:
      highlight = 7;    break;
    case STEPMODE_STEPCOUNT:
      highlight = 12;   break;
    case STEPMODE_BEATCOUNT:
      highlight = 9;    break;
    case STEPMODE_GATETYPE:
      highlight = 15;   break;
    case STEPMODE_ARPTYPE:
      highlight = 17;   break;
    case STEPMODE_ARPSPEEDNUM:
      highlight = 19;   break;
    case STEPMODE_ARPSPEEDDEN:
      highlight = 25;   break;
    case STEPMODE_ARPOCTAVE:
      highlight = 21;   break;
    case STEPMODE_PITCH1:
      highlight = 2;    break;
    case STEPMODE_PITCH2:
      highlight = 3;    break;
    case STEPMODE_PITCH3:
      highlight = 4;    break;
    case STEPMODE_GLIDE:
      highlight = 8;    break;
  }

  displayElement[1] = strdup(midiNotes[sequenceArray[selectedChannel].stepData[selectedStep].pitch[0]]);

  if (sequenceArray[selectedChannel].stepData[selectedStep].pitch[1] == 255){
    displayElement[2] = strdup("--");
  } else {
    displayElement[2] = strdup(midiNotes[sequenceArray[selectedChannel].stepData[selectedStep].pitch[0] + sequenceArray[selectedChannel].stepData[selectedStep].pitch[1]]);
  }

  if (sequenceArray[selectedChannel].stepData[selectedStep].pitch[2] == 255){
    displayElement[3] = strdup("--");
  } else {
    displayElement[3] = strdup(midiNotes[sequenceArray[selectedChannel].stepData[selectedStep].pitch[0] + sequenceArray[selectedChannel].stepData[selectedStep].pitch[2]]);
  }

  if (sequenceArray[selectedChannel].stepData[selectedStep].pitch[3] == 255){
    displayElement[4] = strdup("--");
  } else {
    displayElement[4] = strdup(midiNotes[sequenceArray[selectedChannel].stepData[selectedStep].pitch[0] + sequenceArray[selectedChannel].stepData[selectedStep].pitch[3]]);
  }

  displayElement[5] = strdup("Gate\nTime:");
  if ( sequenceArray[selectedChannel].stepData[selectedStep].gateType == GATETYPE_REST ){
    displayElement[6] = strdup("RST");
  } else {
    sprintf(buf, "%d.%02d", (sequenceArray[selectedChannel].stepData[selectedStep].gateLength+1)/4, (sequenceArray[selectedChannel].stepData[selectedStep].gateLength+1)%4*100/4  );
    displayElement[6] = strdup(buf);
  }

  const char* chordSelectionArray[] = {
    "unison",
    "maj",
    "min",
    "7th",
    "m7 ",
    "maj7",
    "m/maj7",
    "6th",
    "m6th",
    "aug",
    "flat5",
    "sus",
    "7sus4",
    "add9",
    "7#5",
    "m7#5",
    "maj7#5",
    "7b5",
    "m7b5",
    "maj7b5",
    "sus2",
    "7sus2",
    "dim7",
    "dim",
    "Ø7",
    "5th",
    "7#9"
   };

   //displayElement[7] = strdup(chordSelectionArray[sequenceArray[selectedChannel].stepData[selectedStep].chord].c_str());
displayElement[7] = strdup(chordSelectionArray[sequenceArray[selectedChannel].stepData[selectedStep].chord]);

  //displayElement[7] = strdup(String(sequenceArray[selectedChannel].stepData[selectedStep].chord).c_str());


   displayElement[13] = strdup("Glide:");

  if (sequenceArray[selectedChannel].stepData[selectedStep].glide == 0) {
    sprintf(buf, "off");
  } else {
    sprintf(buf, "%d", sequenceArray[selectedChannel].stepData[selectedStep].glide);
  }
  displayElement[8] = strdup(buf);

  displayElement[14] = strdup("Gate\nTrig:");
  String gateTypeArray[] = { "off", "on", "1hit","hold" };
  displayElement[15] = strdup(gateTypeArray[sequenceArray[selectedChannel].stepData[selectedStep].gateType].c_str() );

  displayElement[16] = strdup("arp");
  displayElement[0]  = strdup("typ:");
  const char*  arpTypeArray[] = { "off","up","dn","ud1","ud2","rndm" };
  displayElement[17] = strdup(arpTypeArray[sequenceArray[selectedChannel].stepData[selectedStep].arpType]);
//========
  displayElement[18] = strdup("arp");
  displayElement[23] = strdup("spd:");
  sprintf(buf, "%d/", sequenceArray[selectedChannel].stepData[selectedStep].arpSpdNum);
  displayElement[19] = strdup(buf);
  sprintf(buf, "%d",  sequenceArray[selectedChannel].stepData[selectedStep].arpSpdDen);
  displayElement[25] = strdup(buf);

  displayElement[20] = strdup("arp");
  displayElement[24] = strdup("oct:");
  sprintf(buf, "%doct", sequenceArray[selectedChannel].stepData[selectedStep].arpOctave);
  displayElement[21] = strdup(buf);

  sprintf(buf, "stpmd:%d", stepMode);
  displayElement[22] = strdup(buf);

  //sprintf(buf, "%s", instrumentNames[sequenceArray[selectedChannel].instrument]);
  //displayElement[4] = strdup(buf);

  //sprintf(buf, "vol: %d", sequenceArray[selectedChannel].volume);
  displayElement[26] = strdup("Step Info:");

  sprintf(buf, "last: %d", sequenceArray[selectedChannel].stepCount, sequenceArray[selectedChannel].beatCount);
  displayElement[12] = strdup(buf);

  if (sequenceArray[selectedChannel].stepData[selectedStep].beatDiv == 1){
    sprintf(buf, "lngt: %d", sequenceArray[selectedChannel].stepData[selectedStep].beatDiv);
  } else if (sequenceArray[selectedChannel].stepData[selectedStep].beatDiv > 0){
    sprintf(buf, "lngt: 1/%d", sequenceArray[selectedChannel].stepData[selectedStep].beatDiv);
  } else {
    sprintf(buf, "lngt: %d", abs(sequenceArray[selectedChannel].stepData[selectedStep].beatDiv)+2 );
  }
  displayElement[9] = strdup(buf);

  sprintf(buf, "%d-%d", notePage*16 , (notePage+1)*16 );
  displayElement[10] = strdup(buf);

  sprintf(buf, "%d:%d", selectedChannel , sequenceArray[selectedChannel].patternIndex );
  displayElement[11] = strdup(buf);

// PITCH INFO
//  renderOnce_StringBox(0,  highlight, previousHighlight, 0,   0, 52, 8, false, 1, background, foreground);

  renderOnce_StringBox(1,  highlight, previousHighlight, 0,  0, 46, 24, true, 3, background , foreground);

  renderOnce_StringBox(2, highlight, previousHighlight, 46,  0,  24, 8, false, 1 ,background , foreground);
  renderOnce_StringBox(3, highlight, previousHighlight, 46,  8,  24, 8, false, 1 ,background , foreground);
  renderOnce_StringBox(4, highlight, previousHighlight, 46,  16, 24, 8, false, 1 ,background , foreground);

  renderOnce_StringBox(7,  highlight, previousHighlight, 0,  25, 64, 12, false, 2, background , foreground);

// col 1
  renderOnce_StringBox(5,  highlight, previousHighlight, 0,   39, 26, 16, false, 1, background , foreground);
  renderOnce_StringBox(6,  highlight, previousHighlight, 26,  39, 38, 16, false, 2, background , foreground);

  renderOnce_StringBox(14,  highlight, previousHighlight, 0,  55, 26, 16, false, 1, background , foreground);

  renderOnce_StringBox(15,  highlight, previousHighlight, 26, 55, 38, 16, false, 2 ,background , foreground);

  renderOnce_StringBox(13,  highlight, previousHighlight, 0, 72, 26, 16, false, 1, background , foreground);
  renderOnce_StringBox(8,  highlight, previousHighlight,  26,  72, 38, 16, false, 2, background , foreground);


  // col 2
  renderOnce_StringBox(16,  highlight, previousHighlight, 64,  39, 26, 8, false, 1, background , foreground);
  renderOnce_StringBox(0,  highlight, previousHighlight, 64,  47, 26, 16, false, 1, background , foreground);
  renderOnce_StringBox(17,  highlight, previousHighlight, 84, 39, 44, 16, false, 2 ,background , foreground);

  renderOnce_StringBox(18,  highlight, previousHighlight, 64,  55, 26, 8, false, 1, background , foreground);
  renderOnce_StringBox(23,  highlight, previousHighlight, 64,  63, 26, 16, false, 1, background , foreground);
  renderOnce_StringBox(19,  highlight, previousHighlight, 84,  55, 22, 16, false, 2 ,background , foreground);
  renderOnce_StringBox(25,  highlight, previousHighlight, 106,  55, 22, 16, false, 2 ,background , foreground);

  renderOnce_StringBox(20,  highlight, previousHighlight,64,  72, 26, 8, false, 1, background , foreground);
  renderOnce_StringBox(24,  highlight, previousHighlight,64,  80, 26, 8, false, 1, background , foreground);
  renderOnce_StringBox(21,  highlight, previousHighlight, 84, 72, 44, 16, false, 2 ,background , foreground);


//renderOnce_StringBox(5,  highlight, previousHighlight, 0,  60, 64, 10, false, 1 ,background , foreground);   // UNUSED

  renderOnce_StringBox(10, highlight, previousHighlight, 72, 0, 28, 8, false, 1 ,background , foreground);
  renderOnce_StringBox(11, highlight, previousHighlight, 100, 0, 28, 8, false, 1 ,background , foreground);

  renderOnce_StringBox(26,  highlight, previousHighlight,72, 8, 56, 8, false, 0 ,background , foreground);
  renderOnce_StringBox(12,  highlight, previousHighlight,72, 16, 56, 8, false, 0 ,background , foreground);
  renderOnce_StringBox(9,  highlight, previousHighlight, 72, 24, 56, 8, false, 0 ,background , foreground);


 renderOnce_StringBox(22,  highlight, previousHighlight, 0, 88, 64, 8, false, 0 ,background , foreground);


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

void DisplayModule::channelVelocityMenuDisplay(char *buf) {
  uint8_t previousHighlight = highlight;

  displayElement[0] = strdup("_-*^*-_ Velocity _-*^*-_");
  displayElement[1] = strdup("velocity level:");
//  char *gateTypeArray[] = { "off", "on", "1-shot","hold" };
  sprintf(buf, "%d", sequenceArray[selectedChannel].stepData[selectedStep].velocity);
  displayElement[2] = strdup(buf);
  displayElement[3] = strdup("velocity type:");
  char *velTypeArray[] = { "none", "voltage", "LFO Sine", "LFO Square", "RoundSq" };
  displayElement[4] = strdup(velTypeArray[sequenceArray[selectedChannel].stepData[selectedStep].velocityType]);
  displayElement[5] = strdup("LFO speed:");
//  char *gateTypeArray[] = { "off", "on", "1-shot","hold" };
  sprintf(buf, "%d", sequenceArray[selectedChannel].stepData[selectedStep].lfoSpeed);
  displayElement[6] = strdup(buf);


  switch (stepMode) {
    case STEPMODE_VELOCITY:
      highlight = 2;
    break;
    case STEPMODE_VELOCITYTYPE:
      highlight = 4;
    break;
    case STEPMODE_LFOSPEED:
      highlight = 6;
    break;
  }

//Title - arp set
  renderOnce_StringBox(0,  highlight, previousHighlight, 0 , 90, 128 , 6, false, 1,  background, foreground);
//velocity
  renderOnce_StringBox(1,  highlight, previousHighlight, 0,  0, 128, 10, false, 0, background, foreground);
  renderOnce_StringBox(2,  highlight, previousHighlight, 0,  6, 128, 15, false, 2, background, foreground);
//vel type
  renderOnce_StringBox(3,  highlight, previousHighlight, 0,  23, 128, 10, false, 0, background, foreground);
  renderOnce_StringBox(4,  highlight, previousHighlight, 0,  29, 128, 15, false, 2, background, foreground);
//lfo speed
  renderOnce_StringBox(5,  highlight, previousHighlight, 0,  46, 64, 10, false, 0, background, foreground);
  renderOnce_StringBox(6,  highlight, previousHighlight, 0,  52, 64, 15, false, 2, background, foreground);
////arp octave
//  renderOnce_StringBox(7,  highlight, previousHighlight, 64,  23, 64, 10, false, 0, background, foreground);
//  renderOnce_StringBox(8,  highlight, previousHighlight, 64,  29, 64, 15, false, 2, background, foreground);
//  //arp count
//  renderOnce_StringBox(9,  highlight, previousHighlight, 64,  46, 64, 10, false, 0, background, foreground);
//  renderOnce_StringBox(10,  highlight, previousHighlight, 64,  52, 64, 15, false, 2, background, foreground);

}

void DisplayModule::channelEnvelopeMenuDisplay(char *buf) {
  uint8_t previousHighlight = highlight;

  displayElement[0] = strdup("ENVELOPE SET");

  switch (stepMode) {
    case STEPMODE_VELOCITY:
      highlight = 1;    break;
  }

  sprintf(buf, "V: %d", sequenceArray[selectedChannel].stepData[selectedStep].velocity);
  displayElement[2] = strdup(buf);

  renderOnce_StringBox(0, highlight, previousHighlight,  0,  0, 128, 16, false, 0,  background, foreground);
  renderOnce_StringBox(1,  highlight, previousHighlight, 0, 16, 64 , 16, false, 2, background, foreground);


}

void DisplayModule::channelTunerDisplay(char *buf){
  uint8_t previousHighlight = highlight;
  highlight = 4;
  int freq1 = frequency;
  int freq2 = trunc(frequency*100)-trunc(freq1*100);
  int prob1 = probability;
  int prob2 = trunc(probability*100)-trunc(prob1*100);

    displayElement[0] = strdup("Tuner");
  sprintf(buf, "INPUT: %d", selectedChannel );
    displayElement[1] = strdup(buf);
  sprintf(buf, "FREQ: %d.%02d", freq1, freq2 );
    displayElement[2] = strdup(buf);
  sprintf(buf, "PROB: %d.%02d", prob1, prob2);
    displayElement[3] = strdup(buf);

    renderOnce_StringBox(0, highlight, previousHighlight,  0,  0, 128, 16, false, 1,  background, foreground);
    renderOnce_StringBox(1, highlight, previousHighlight,  0,  16, 128, 16, false, 2,  background, foreground);
    renderOnce_StringBox(2, highlight, previousHighlight,  0,  32, 128, 16, false, 2,  background, foreground);
    renderOnce_StringBox(3, highlight, previousHighlight,  0,  48, 128, 16, false, 2,  background, foreground);


}

void DisplayModule::channelStepMenuDisplay(char *buf) {
  uint8_t previousHighlight = highlight;
  displayElement[0] = strdup("STEP SET");
  renderOnce_StringBox(0, highlight, previousHighlight, 0 , 0  , 128 , 16, false, 1,  background, foreground);


    switch (stepMode) {
      case STEPMODE_GATETYPE:
        highlight = 0;
      break;
    }

    if (extClock) {
      displayElement[STEPMODE_TEMPO] = strdup("MIDI");
    } else {
      sprintf(buf, "%d bpm", int(tempoX100/100) );
      displayElement[STEPMODE_TEMPO] = strdup(buf);
    };

    renderOnce_StringBox(STEPMODE_TEMPO, highlight, previousHighlight,  0,  0, 128, 16, false, 0,  background, foreground);

}

void DisplayModule::globalMenuDisplay(){

  uint8_t previousHighlight = highlight;
  //uint16_t foreground, background;
  //oled.setFont(Arial_8);

  displayElement[0] = strdup("GLOBAL MENU");
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

void DisplayModule::sequenceMenuDisplay(){
  uint8_t previousHighlight = highlight;

  switch (stepMode) {
    case STEPMODE_QUANTIZEKEY:
      highlight = 2;
    break;
    case STEPMODE_QUANTIZESCALE:
      highlight = 3;
    break;
    default:
      highlight = 2;
    break;
  }

  displayElement[0] = strdup("SEQUENCE MENU");
  renderOnce_StringBox(0, highlight, previousHighlight, 0 , 0  , 128 , 16, false, 1,  BLACK, WHITE);

  displayElement[1] = strdup("Scale Quantization:");
  char *keyArray[] = { "C","C#","D","D#","E","F","F#","G","G#","A","A#","B" };
  char *scaleArray[] = { "OFF", "CHROMATIC", "MAJOR", "MINOR", "MAJORMINOR", "PENTATONIC_MAJOR", "PENTATONIC_MINOR", "PENTATONIC_BLUES", "IONIAN", "AEOLIAN", "DORIAN", "MIXOLYDIAN", "PHRYGIAN", "LYDIAN", "LOCRIAN" };

  displayElement[2] = strdup(keyArray[sequenceArray[selectedChannel].quantizeKey]);
  displayElement[3] = strdup(scaleArray[sequenceArray[selectedChannel].quantizeScale]);

  renderOnce_StringBox(1,  highlight, previousHighlight, 0,  16, 128, 8, false, 0, BLACK, WHITE);
  renderOnce_StringBox(2,  highlight, previousHighlight, 0,  24, 128, 15, false, 2, BLACK, WHITE);
  renderOnce_StringBox(3,  highlight, previousHighlight, 0,  36, 128, 15, false, 2, BLACK, WHITE);
}


void DisplayModule::tempoMenuDisplay(){
  uint8_t previousHighlight = highlight;

  switch (stepMode){
    case STEPMODE_TEMPO:
      highlight = 1;
      break;
    case STEPMODE_EXTCLOCK:
      highlight = 2;
      break;
  }

  displayElement[0] = strdup("TEMPO");
  renderOnce_StringBox(0, highlight, previousHighlight, 0 , 0  , 128 , 16, false, 2,  background, foreground);

  if (extClock) {
    displayElement[1] = strdup("---");
  } else {
    sprintf(buf, "%d bpm", int(tempoX100/100) );
    displayElement[1] = strdup(buf);
  };

  renderOnce_StringBox(1, highlight, previousHighlight,  0, 16, 128, 16, false, 2,  background, foreground);

  if (extClock) {
    displayElement[2] = strdup("external");
  } else {
    displayElement[2] = strdup("internal");
  };

  renderOnce_StringBox(2, highlight, previousHighlight,  0, 32, 128, 16, false, 2,  background, foreground);

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

  sprintf(buf, "PATTENRN SELECT: %d", patternChannelSelector);

  displayElement[16] = strdup(buf);
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

  sprintf(buf, "D0L: %d", dacCalibration[0]);
  displayElement[1] = strdup(buf);
  sprintf(buf, "D0H: %d", dacCalibration[1]);
  displayElement[2] = strdup(buf);
  sprintf(buf, "D1L: %d", dacCalibration[2]);
  displayElement[3] = strdup(buf);
  sprintf(buf, "D1H: %d", dacCalibration[3]);
  displayElement[4] = strdup(buf);
  sprintf(buf, "D2L: %d", dacCalibration[4]);
  displayElement[5] = strdup(buf);
  sprintf(buf, "D2H: %d", dacCalibration[5]);
  displayElement[6] = strdup(buf);
  sprintf(buf, "D3L: %d", dacCalibration[6]);
  displayElement[7] = strdup(buf);
  sprintf(buf, "D3H: %d", dacCalibration[7]);
  displayElement[8] = strdup(buf);
  sprintf(buf, "D4L: %d", dacCalibration[8]);
  displayElement[9] = strdup(buf);
  sprintf(buf, "D4H: %d", dacCalibration[9]);
  displayElement[10] = strdup(buf);
  sprintf(buf, "D5L: %d", dacCalibration[10]);
  displayElement[11] = strdup(buf);
  sprintf(buf, "D5H: %d", dacCalibration[11]);
  displayElement[12] = strdup(buf);
  sprintf(buf, "D6L: %d", dacCalibration[12]);
  displayElement[13] = strdup(buf);
  sprintf(buf, "D6H: %d", dacCalibration[13]);
  displayElement[14] = strdup(buf);
  sprintf(buf, "D7L: %d", dacCalibration[14]);
  displayElement[15] = strdup(buf);
  sprintf(buf, "D7H: %d", dacCalibration[15]);
  displayElement[16] = strdup(buf);
  sprintf(buf, "D8L: %d", dacCalibration[16]);
  displayElement[17] = strdup(buf);
  sprintf(buf, "D8H: %d", dacCalibration[17]);
  displayElement[18] = strdup(buf);

  renderOnce_StringBox(0, highlight, previousHighlight,  0, 0, 32 , 20, false, 1, WHITE, BLACK);

  renderOnce_StringBox(1, highlight, previousHighlight,  0, 8, 32 , 20, false, 1, WHITE, BLACK);
  renderOnce_StringBox(2, highlight, previousHighlight,  0, 16, 32 , 20, false, 1, WHITE, BLACK);

  renderOnce_StringBox(3, highlight, previousHighlight,  0, 24, 32 , 20, false, 1, WHITE, BLACK);
  renderOnce_StringBox(4, highlight, previousHighlight,  0, 32, 32 , 20, false, 1, WHITE, BLACK);

  renderOnce_StringBox(5, highlight, previousHighlight,  0, 40, 32 , 20, false, 1, WHITE, BLACK);
  renderOnce_StringBox(7, highlight, previousHighlight,  0, 48, 32 , 20, false, 1, WHITE, BLACK);

  renderOnce_StringBox(8, highlight, previousHighlight,  0, 56, 32 , 20, false, 1, WHITE, BLACK);
  renderOnce_StringBox(9, highlight, previousHighlight,  0, 62, 32 , 20, false, 1, WHITE, BLACK);

}

void DisplayModule::inputDebugMenuDisplay(){
  uint8_t previousHighlight = highlight;
  highlight = currentPattern;

  displayElement[0] = strdup("Calibration Menu");

  sprintf(buf, "CV IN 1: %d", cvInputRaw[0] );
  displayElement[1] = strdup(buf);
  sprintf(buf, "CV IN 2: %d", cvInputRaw[1] );
  displayElement[2] = strdup(buf);
  sprintf(buf, "CV IN 3: %d", cvInputRaw[2] );
  displayElement[3] = strdup(buf);
  sprintf(buf, "CV IN 4: %d", cvInputRaw[3] );
  displayElement[4] = strdup(buf);

  sprintf(buf, "GT IN 1: %d", gateInputRaw[0]);
  displayElement[5] = strdup(buf);
  sprintf(buf, "GT IN 2: %d", gateInputRaw[1]);
  displayElement[6] = strdup(buf);
  sprintf(buf, "GT IN 3: %d", gateInputRaw[2]);
  displayElement[7] = strdup(buf);
  sprintf(buf, "GT IN 4: %d", gateInputRaw[3]);
  displayElement[8] = strdup(buf);

  renderOnce_StringBox(0, highlight, previousHighlight,  0, 0, 96 , 8, false, 1, WHITE, BLACK);

  renderOnce_StringBox(1, highlight, previousHighlight,  0, 8, 96 , 8, false, 1, WHITE, BLACK);
  renderOnce_StringBox(2, highlight, previousHighlight,  0, 16, 96 , 8, false, 1, WHITE, BLACK);

  renderOnce_StringBox(3, highlight, previousHighlight,  0, 24, 96 , 8, false, 1, WHITE, BLACK);
  renderOnce_StringBox(4, highlight, previousHighlight,  0, 32, 96 , 8, false, 1, WHITE, BLACK);

  renderOnce_StringBox(5, highlight, previousHighlight,  0, 40, 96 , 8, false, 1, WHITE, BLACK);
  renderOnce_StringBox(6, highlight, previousHighlight,  0, 48, 96 , 8, false, 1, WHITE, BLACK);

  renderOnce_StringBox(7, highlight, previousHighlight,  0, 56, 96 , 8, false, 1, WHITE, BLACK);
  renderOnce_StringBox(8, highlight, previousHighlight,  0, 64, 96 , 8, false, 1, WHITE, BLACK);

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
