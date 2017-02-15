#include <Arduino.h>
#include "DisplayModule.h"

DisplayModule::DisplayModule(){
};

void DisplayModule::initialize(Sequencer *sequenceArray, MasterClock* clockMaster){

  Serial.println("Initializing Display");
  this->clockMaster = clockMaster;
  this->sequenceArray = sequenceArray;
  currentMenu = 255;
  oled.begin();
  delay(100);
  oled.setBrightness(255);
  //oled.setFont(&Font);//this will load the font
  oled.setFont(&F_Zero_18);//this will load the font
  oled.setTextScale(1);
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
  oled.fillScreen(BLACK);
  oled.setCursor(CENTER,CENTER);
  oled.setTextColor(RED);
  oled.setFont(&LadyRadical_12);//this will load the font
  oled.setTextScale(2);
  oled.println("Zetaohm");
  delay(1000);
  oled.setCursor(CENTER,CENTER);
  oled.setTextColor(BLACK);
  oled.fillScreen(YELLOW);      delay(10);
  oled.setTextScale(1);

  oled.fillScreen(GREEN);
  oled.setCursor(CENTER,CENTER);
  oled.setTextColor(PINK);
  oled.setTextScale(2);
  oled.setTextColor(BLUE);
  oled.println("FLXS1");    delay(500);
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


    if (previousMenu != currentMenu || previouslySelectedChannel != selectedChannel){
       freeDisplayCache();
       oled.fillScreen(background);
       Serial.println("Changing Menu: " + String(currentMenu));
    }

    switch(currentMenu) {
      case PITCH_GATE_MENU:
        stateDisplay_pitch(buf);
      break;

      case ARPEGGIO_MENU:
        stateDisplay_arp(buf);
      break;

      case SEQUENCE_MENU_1:
        sequenceMenuDisplay();
      break;

      case PATTERN_SELECT:
        patternSelectDisplay();
      break;



      case VELOCITY_MENU:
        channelVelocityMenuDisplay(buf);
      break;

      case TUNER_MENU:
        channelTunerDisplay(buf);
      break;

      case CHANNEL_INPUT_MODE:
        channelInputDisplay(buf);
      break;

      case GLOBAL_MENU:
        globalMenuDisplay();
      break;

      case TEMPO_MENU:
        tempoMenuDisplay();
      break;

      case CALIBRATION_MENU:
        calibrationMenuDisplay();
      break;

    }

    if (previousMenu != currentMenu){
      Serial.println("finished first loop of displaying new state");
    }
    cleanupTextBuffers();
    if (previousMenu != currentMenu){
      Serial.println("text buffers cleaned up");
      Serial.println("Freeram: " + String(FreeRam2()));
    }
    previouslySelectedChannel = selectedChannel;
    previousStepMode = stepMode;
    previousMenu = currentMenu;

  };
};

void DisplayModule::renderStringBox(uint8_t index, uint8_t highlight, int16_t x, int16_t y, int16_t w, int16_t h, bool border, uint8_t textSize, uint16_t color, uint16_t bgColor) {
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

    oled.fillRect(x,y,w,h, color2);
    oled.setCursor(x, y);

    switch(textSize){
      case REGULAR1X:
        oled.setFont(&PixelSquare_10);
        oled.setTextScale(1);
        break;
      case REGULAR2X:
        oled.setFont(&PixelSquare_10);
        oled.setTextScale(2);
        break;
      case BOLD1X:
        oled.setFont(&PixelSquareBold_10);
        oled.setTextScale(1);
        break;
      case BOLD2X:
        oled.setFont(&PixelSquareBold_10);
        oled.setTextScale(2);
        break;
      case BOLD4X:
        oled.setCursor(CENTER,y-2);
        oled.setFont(&OrbitBold_14);
        oled.setTextScale(2);
        break;
      case STYLE1X:
        oled.setFont(&OrbitBold_14);
        oled.setTextScale(1);
        break;
    }

    oled.setTextColor(color1);
    oled.print(displayElement[index]);
    if (border){
      oled.drawRect(x,y,w,h, color1);
    }
  }
}


void DisplayModule::stateDisplay_pitch(char*buf){
  uint8_t previousHighlight = highlight;      highlight = 1;

  displayElement[0] = strdup("PITCH+GATE");

  displayElement[1] = strdup(midiNotes[sequenceArray[selectedChannel].stepData[selectedStep].pitch[0]]);
  displayElement[4] = strdup("GATE");

  if ( sequenceArray[selectedChannel].stepData[selectedStep].gateType == GATETYPE_REST ){
    displayElement[5] = strdup("RST");
  } else {
    sprintf(buf, "%d.%02d", (sequenceArray[selectedChannel].stepData[selectedStep].gateLength+1)/4, (sequenceArray[selectedChannel].stepData[selectedStep].gateLength+1)%4*100/4  );
    displayElement[5] = strdup(buf);
  }
  displayElement[6] = strdup("GLIDE");
  if (sequenceArray[selectedChannel].stepData[selectedStep].glide == 0) {
    sprintf(buf, "off");
  } else {
    sprintf(buf, "%d", sequenceArray[selectedChannel].stepData[selectedStep].glide);
  }
  displayElement[9] = strdup(buf);

  displayElement[8] = strdup("Type:");
  String gateTypeArray[] = { "off", "on", "1hit","hold" };
  displayElement[7] = strdup(gateTypeArray[sequenceArray[selectedChannel].stepData[selectedStep].gateType].c_str() );

  //digitalWriteFast(PIN_EXT_RX, HIGH);
  renderStringBox(0,  DISPLAY_LABEL,  0,  0, 128, 15, false, STYLE1X, background , foreground); //  digitalWriteFast(PIN_EXT_RX, LOW);

  renderStringBox(1,  STATE_PITCH0, 0, 15 , 128, 29, false, BOLD4X, background , foreground);     //  digitalWriteFast(PIN_EXT_RX, HIGH);

  renderStringBox(4,  DISPLAY_LABEL, 0,  45,64,17, false, STYLE1X, background , foreground); //  digitalWriteFast(PIN_EXT_RX, LOW);
  renderStringBox(8,  DISPLAY_LABEL, 0,  62,64,17, false, STYLE1X, background , foreground); //  digitalWriteFast(PIN_EXT_RX, HIGH);
  renderStringBox(6,  DISPLAY_LABEL, 0,  79,64,17, false, STYLE1X, background , foreground); //  digitalWriteFast(PIN_EXT_RX, LOW);

  renderStringBox(5,  STATE_GATELENGTH,60, 45,68,17, false, STYLE1X, background , foreground); //  digitalWriteFast(PIN_EXT_RX, HIGH);
  renderStringBox(7,  STATE_GATETYPE,  60, 62,68,17, false, STYLE1X, background , foreground); //  digitalWriteFast(PIN_EXT_RX, LOW);
  renderStringBox(9,  STATE_GLIDE,     60, 79,68,17, false, STYLE1X, background , foreground); //  digitalWriteFast(PIN_EXT_RX, HIGH);
  //digitalWriteFast(PIN_EXT_RX, LOW);
};



void DisplayModule::stateDisplay_arp(char *buf){
  displayElement[0] = strdup("ARPEGGIO");
  displayElement[1]  = strdup("TYPE");
  const char*  arpTypeArray[] = { "off","up","dn","ud1","ud2","rndm" };
  displayElement[2] = strdup(arpTypeArray[sequenceArray[selectedChannel].stepData[selectedStep].arpType]);

  displayElement[4] = strdup("SPEED");
  sprintf(buf, "%d/", sequenceArray[selectedChannel].stepData[selectedStep].arpSpdNum);
  displayElement[5] = strdup(buf);
  sprintf(buf, "%d",  sequenceArray[selectedChannel].stepData[selectedStep].arpSpdDen);
  displayElement[6] = strdup(buf);

  displayElement[8] = strdup("OCTVE");
  sprintf(buf, "%doct", sequenceArray[selectedChannel].stepData[selectedStep].arpOctave);
  displayElement[9] = strdup(buf);


  displayElement[10] = strdup("INTVL");

    const char* chordSelectionArray[] = { "unison", "maj", "min", "7th", "m7 ", "maj7", "m/maj7", "6th", "m6th", "aug", "flat5", "sus", "7sus4", "add9", "7#5", "m7#5", "maj7#5", "7b5", "m7b5", "maj7b5", "sus2", "7sus2",  "dim7", "dim", "Ø7", "5th", "7#9"      };

      //displayElement[7] = strdup(chordSelectionArray[sequenceArray[selectedChannel].stepData[selectedStep].chord].c_str());
   displayElement[11] = strdup(chordSelectionArray[sequenceArray[selectedChannel].stepData[selectedStep].chord]);

   renderStringBox(0,  DISPLAY_LABEL,    0,  0, 128, 15, false, STYLE1X, background , foreground);

   renderStringBox(1,  DISPLAY_LABEL,        0, 20,68,17, false, STYLE1X, background , foreground);
   renderStringBox(2,  STATE_ARPTYPE,     60, 20,68,17, false, STYLE1X, background , foreground);
   renderStringBox(4,  DISPLAY_LABEL,        0, 37,68,17, false, STYLE1X, background , foreground);
   renderStringBox(5,  STATE_ARPSPEEDNUM, 60, 37,34,17, false, STYLE1X, background , foreground);
   renderStringBox(6,  STATE_ARPSPEEDDEN, 94, 37,34,17, false, STYLE1X, background , foreground);

   renderStringBox(8,  DISPLAY_LABEL,        0,  54,68,17, false, STYLE1X, background , foreground);
   renderStringBox(9,  STATE_ARPOCTAVE, 60, 54,68,17, false, STYLE1X, background , foreground);

   renderStringBox(10,  DISPLAY_LABEL,        0, 71,68,17, false, STYLE1X, background , foreground);
   renderStringBox(11,  STATE_CHORD,    60, 71,68,17, false, STYLE1X, background , foreground);

 };


 void DisplayModule::sequenceMenuDisplay(){
   uint8_t previousHighlight = highlight;

   displayElement[0] = strdup("SEQUENCE");

   displayElement[1] = strdup("LAST:");

   sprintf(buf, "%d", sequenceArray[selectedChannel].stepCount);
   displayElement[2] = strdup(buf);

   displayElement[3] = strdup("LENGTH:");

   if (sequenceArray[selectedChannel].stepData[selectedStep].beatDiv == 1){
     sprintf(buf, "%d", sequenceArray[selectedChannel].stepData[selectedStep].beatDiv);
   } else if (sequenceArray[selectedChannel].stepData[selectedStep].beatDiv > 0){
     sprintf(buf, "1/%d", sequenceArray[selectedChannel].stepData[selectedStep].beatDiv);
   } else {
     sprintf(buf, "%d", abs(sequenceArray[selectedChannel].stepData[selectedStep].beatDiv)+2 );
   }
   displayElement[4] = strdup(buf);

   displayElement[5] = strdup("KEY:");
   char *keyArray[] = { "C","C#","D","D#","E","F","F#","G","G#","A","A#","B" };
   char *scaleArray[] = { "OFF", "CHROMATIC", "MAJOR", "MINOR", "MAJORMINOR", "PENTATONIC_MAJOR", "PENTATONIC_MINOR", "PENTATONIC_BLUES", "IONIAN", "AEOLIAN", "DORIAN", "MIXOLYDIAN", "PHRYGIAN", "LYDIAN", "LOCRIAN" };

   displayElement[6] = strdup(keyArray[sequenceArray[selectedChannel].quantizeKey]);
   displayElement[7] = strdup("SCALE:");

   displayElement[8] = strdup(scaleArray[sequenceArray[selectedChannel].quantizeScale]);




   renderStringBox(0,  DISPLAY_LABEL,    0,  0, 128, 15, false, STYLE1X, background , foreground);

   renderStringBox(1,  DISPLAY_LABEL,        0, 20,68,17, false, STYLE1X, background , foreground);
   renderStringBox(2,  STATE_STEPCOUNT,     68, 20,60,17, false, STYLE1X, background , foreground);
   renderStringBox(3,  DISPLAY_LABEL,        0, 37,68,17, false, STYLE1X, background , foreground);
   renderStringBox(4,  STATE_BEATCOUNT,     68, 37,60,17, false, STYLE1X, background , foreground);

   renderStringBox(5,  DISPLAY_LABEL,        0,  54,68,17, false, STYLE1X, background , foreground);
   renderStringBox(6,  STATE_QUANTIZEKEY,     68, 54,60,17, false, STYLE1X, background , foreground);

   renderStringBox(7,  DISPLAY_LABEL,        0, 71,68,17, false, STYLE1X, background , foreground);
   renderStringBox(8,  STATE_QUANTIZESCALE,    68, 71,60,17, false, STYLE1X, background , foreground);

 }
