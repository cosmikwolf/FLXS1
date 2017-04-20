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
  oled.setFont(&NeueHaasXBlack_28);//this will load the font
  oled.println("RESIST");               delay(50);
  oled.fillScreen(GREEN, ORANGE);
  oled.println("FASISM");      delay(50);
  oled.fillScreen(BLUE, GREEN);      delay(100);
  oled.fillScreen(PURPLE, NAVY);       delay(100);
  oled.fillScreen(NAVY,RED);
  oled.setCursor(CENTER,8);
  oled.setTextColor(ORANGE);
  oled.setFont(&LadyRadical_16);//this will load the font
  oled.setTextScale(2);
  oled.println("Zetaohm");
  delay(100);
  oled.setTextScale(1);
  oled.setCursor(CENTER,48);
  oled.setTextColor(ORANGE);
  oled.setFont(&NeueHaasXBlack_28);//this will load the font
  oled.println("FLXS1");
    delay(1000);
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
        foreground = ORANGE;
        background = BLACK;
        break;
      case 1:
        foreground = GREENYELLOW;
        background = BLACK;
        break;
      case 2:
        foreground = PINK;
        background = BLACK;
        break;
      case 3:
        foreground = RED;
        background = BLACK;
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

      case VELOCITY_MENU:
        stateDisplay_velocity(buf);
      break;

      case SEQUENCE_MENU:
        sequenceMenuDisplay();
      break;

      case INPUT_MENU:
        inputMenuDisplay();
      break;

      case PATTERN_SELECT:
        patternSelectDisplay();
      break;

      case INPUT_DEBUG_MENU:
        inputDebugMenuDisplay();
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
        if (stepMode < STATE_CALIB_OUTPUT0_LOW) {
          inputCalibrationMenuDisplay();
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

      case MENU_MODAL:
        modalPopup();
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
    if (border){
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



void DisplayModule::stateDisplay_pitch(char*buf){

  sprintf(buf, "CV%d PITCH PT:%02d", selectedChannel+1, sequenceArray[selectedChannel].pattern+1);

  displayElement[0] = strdup(buf);

  displayElement[1] = strdup(midiNotes[sequenceArray[selectedChannel].stepData[selectedStep].pitch[0]]);

  sprintf(buf, "GATE%d", selectedChannel+1);

  displayElement[4] = strdup(buf);

  if ( sequenceArray[selectedChannel].stepData[selectedStep].gateType == GATETYPE_REST ){
    displayElement[5] = strdup("REST");
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

  displayElement[8] = strdup("TYPE:");
  String gateTypeArray[] = { "OFF", "ON", "1HIT","HOLD" };
  displayElement[7] = strdup(gateTypeArray[sequenceArray[selectedChannel].stepData[selectedStep].gateType].c_str() );

  //digitalWriteFast(PIN_EXT_RX, HIGH);
  renderStringBox(0,  DISPLAY_LABEL,  0,  0, 128, 15, false, STYLE1X, background , foreground); //  digitalWriteFast(PIN_EXT_RX, LOW);

  renderStringBox(1,  STATE_PITCH0, 0, 15 , 128, 29, false, BOLD4X, background , foreground);     //  digitalWriteFast(PIN_EXT_RX, HIGH);

  renderStringBox(4,  DISPLAY_LABEL, 0,  47,64,16, false, STYLE1X, background , foreground); //  digitalWriteFast(PIN_EXT_RX, LOW);
  renderStringBox(8,  DISPLAY_LABEL, 0,  63,64,16, false, STYLE1X, background , foreground); //  digitalWriteFast(PIN_EXT_RX, HIGH);
  renderStringBox(6,  DISPLAY_LABEL, 0,  79,64,16, false, STYLE1X, background , foreground); //  digitalWriteFast(PIN_EXT_RX, LOW);

  renderStringBox(5,  STATE_GATELENGTH,60, 47,68,16, false, STYLE1X, background , foreground); //  digitalWriteFast(PIN_EXT_RX, HIGH);
  renderStringBox(7,  STATE_GATETYPE,  60, 63,68,16, false, STYLE1X, background , foreground); //  digitalWriteFast(PIN_EXT_RX, LOW);
  renderStringBox(9,  STATE_GLIDE,     60, 79,68,16, false, STYLE1X, background , foreground); //  digitalWriteFast(PIN_EXT_RX, HIGH);
  //digitalWriteFast(PIN_EXT_RX, LOW);
};



void DisplayModule::stateDisplay_arp(char *buf){

  sprintf(buf, "CV%dA Arp PT:%02d", selectedChannel+1, sequenceArray[selectedChannel].pattern+1 );
  displayElement[0] = strdup(buf);
  displayElement[1]  = strdup("TYPE");
  const char*  arpTypeArray[] = { "OFF","UP","DOWN","UP DN 1","UP DN 2","RANDOM" };
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


 void DisplayModule::stateDisplay_velocity(char *buf) {
   sprintf(buf, "CV%dB LFO  PT:%02d", selectedChannel+1, sequenceArray[selectedChannel].pattern+1 );
   displayElement[0] = strdup(buf);
   displayElement[1] = strdup("LEVEL:");

   sprintf(buf, "%d", sequenceArray[selectedChannel].stepData[selectedStep].velocity);
   displayElement[2] = strdup(buf);
   displayElement[3] = strdup("TYPE:");
   char *velTypeArray[] = { "none", "voltage", "LFO Sine", "LFO Square", "RoundSq" };
   displayElement[4] = strdup(velTypeArray[sequenceArray[selectedChannel].stepData[selectedStep].velocityType]);
   displayElement[5] = strdup("LFO SPD:");

   sprintf(buf, "%d", sequenceArray[selectedChannel].stepData[selectedStep].lfoSpeed);
   displayElement[6] = strdup(buf);


   renderStringBox(0,  DISPLAY_LABEL,    0,  0, 128, 15, false, STYLE1X, background , foreground);

   renderStringBox(1,  DISPLAY_LABEL,        0, 20,68,17, false, STYLE1X, background , foreground);
   renderStringBox(2,  STATE_VELOCITY,     60, 20,68,17, false, STYLE1X, background , foreground);
   renderStringBox(3,  DISPLAY_LABEL,        0, 37,68,17, false, STYLE1X, background , foreground);
   renderStringBox(4,  STATE_VELOCITYTYPE, 50, 37,78,17, false, STYLE1X, background , foreground);

   renderStringBox(5,  DISPLAY_LABEL,        0,  54,68,17, false, STYLE1X, background , foreground);
   renderStringBox(6,  STATE_LFOSPEED, 80, 54,47,17, false, STYLE1X, background , foreground);

   renderStringBox(10,  DISPLAY_LABEL,        0, 71,68,17, false, STYLE1X, background , foreground);
   renderStringBox(11,  STATE_CHORD,    60, 71,68,17, false, STYLE1X, background , foreground);

}


 void DisplayModule::sequenceMenuDisplay(){

   sprintf(buf, "CH%d SETTINGS", selectedChannel+1);

   displayElement[0] = strdup(buf);

   displayElement[1] = strdup("LAST:");

   sprintf(buf, "%d", sequenceArray[selectedChannel].stepCount);
   displayElement[2] = strdup(buf);

   displayElement[3] = strdup("LENGTH:");

   if (sequenceArray[selectedChannel].clockDivision == 1){
     sprintf(buf, "%d", sequenceArray[selectedChannel].clockDivision);
   } else if (sequenceArray[selectedChannel].clockDivision > 0){
     sprintf(buf, "1/%d", sequenceArray[selectedChannel].clockDivision);
   } else {
     sprintf(buf, "%d", abs(sequenceArray[selectedChannel].clockDivision)+2 );
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

 renderStringBox(0,  DISPLAY_LABEL,     0,    0, 128, 15, false, STYLE1X, background , foreground);
 renderStringBox(1,  DISPLAY_LABEL,     0,   16,  64,16, false, STYLE1X, background , foreground);
 //renderStringBox(2,  STATE_XAXISINPUT,  74,  16,  54,16, false, STYLE1X, background , foreground);
 renderStringBox(3,  DISPLAY_LABEL,     0,   32,  64,16, false, STYLE1X, background , foreground);
 //renderStringBox(4,  STATE_YAXISINPUT,  74,  32,  54,16, false, STYLE1X, background , foreground);

}



 void DisplayModule::tempoMenuDisplay(){

   displayElement[0] = strdup("TEMPO");
   displayElement[2] = strdup("SYNC");
   displayElement[4] = strdup("RESET:");

   if (sequenceArray[selectedChannel].gpio_reset < 4){
    sprintf(buf, "GT%d", sequenceArray[selectedChannel].gpio_reset +1 );
    displayElement[5] = strdup(buf);
  } else {
    displayElement[5] = strdup("NONE");
  }

  displayElement[6] = strdup("Y-AXIS:");
  if (sequenceArray[selectedChannel].gpio_yaxis < 4){
   sprintf(buf, "GT%d", sequenceArray[selectedChannel].gpio_yaxis +1 );
   displayElement[7] = strdup(buf);
 } else {
   displayElement[7] = strdup("NONE");
 }

   switch(clockMode){
     case INTERNAL_CLOCK:
       sprintf(buf, "%dbpm", int(tempoX100/100) );
       displayElement[1] = strdup(buf);
       displayElement[3] = strdup("INTERNAL CLOCK");
     break;
     case EXTERNAL_MIDI_CLOCK:
       displayElement[1] = strdup("MIDI");
       displayElement[3] = strdup("MIDI SYNC");
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
       displayElement[3] = strdup("CLOCK INPUT");
     break;


   }
   renderStringBox(0,  DISPLAY_LABEL, 0, 0 , 128, 15, false, STYLE1X, background , foreground);

   renderStringBox(1,  STATE_TEMPO, 0, 16 , 128, 29, false, BOLD4X, background , foreground);

   renderStringBox(3,  STATE_EXTCLOCK, 0,  45,128,17, false, STYLE1X, background , foreground);
   ///renderStringBox(3,  STATE_EXTCLOCK,60, 45,68,17, false, STYLE1X, background , foreground);

   renderStringBox(4,  DISPLAY_LABEL, 0,  62,74,17, false, STYLE1X, background , foreground);

   renderStringBox(5,  STATE_RESETINPUT,  74, 62,54,17, false, STYLE1X, background , foreground);

   renderStringBox(6,  DISPLAY_LABEL,    0,  79,64,16, false, STYLE1X, background , foreground);
   renderStringBox(7,  STATE_YAXISINPUT,     74,  79,54,16, false, STYLE1X, background , foreground);

 }

 void DisplayModule::modMenu1_DisplayHandler(){
   sprintf(buf, "CH%d MOD CONFG", selectedChannel+1);
   displayElement[0] = strdup(buf);

   displayElement[1] = strdup("GATELNGTH:");

   if (sequenceArray[selectedChannel].cv_gatemod < 4){
    sprintf(buf, "CV%d", sequenceArray[selectedChannel].cv_gatemod +1 );
    displayElement[2] = strdup(buf);
  } else {
    displayElement[2] = strdup("OFF");
  }
  displayElement[3] = strdup("GATEINVRT:");

  if (sequenceArray[selectedChannel].gpio_gateinvert < 4){
   sprintf(buf, "GT%d", sequenceArray[selectedChannel].gpio_gateinvert +1 );
   displayElement[4] = strdup(buf);
 } else {
   displayElement[4] = strdup("OFF");
 }
 displayElement[5] = strdup("RNDMPTCH:");

 if (sequenceArray[selectedChannel].gpio_randompitch < 4){
  sprintf(buf, "GT%d", sequenceArray[selectedChannel].gpio_randompitch +1 );
  displayElement[6] = strdup(buf);
} else {
  displayElement[6] = strdup("OFF");
}
displayElement[7] = strdup("TRNSPOSE:");

if (sequenceArray[selectedChannel].cv_pitchmod < 4){
 sprintf(buf, "CV%d", sequenceArray[selectedChannel].cv_pitchmod +1 );
 displayElement[8] = strdup(buf);
} else {
 displayElement[8] = strdup("OFF");
}

displayElement[9] = strdup("GLIDETIME:");

if (sequenceArray[selectedChannel].cv_glidemod < 4){
 sprintf(buf, "CV%d", sequenceArray[selectedChannel].cv_glidemod +1 );
 displayElement[10] = strdup(buf);
} else {
 displayElement[10] = strdup("OFF");
}

   renderStringBox(0,  DISPLAY_LABEL,      0,    0, 128, 15, false, STYLE1X, background , foreground);
   renderStringBox(1,  DISPLAY_LABEL,      0,   15,  64,16, false, STYLE1X, background , foreground);
   renderStringBox(2,  STATE_GATEMOD,     96,   15,  32,16, false, STYLE1X, background , foreground);
   renderStringBox(3,  DISPLAY_LABEL,      0,   31,  64,16, false, STYLE1X, background , foreground);
   renderStringBox(4,  STATE_GATEINVERT,  96,   31,  32,16, false, STYLE1X, background , foreground);
   renderStringBox(5,  DISPLAY_LABEL,      0,   47,  64,16, false, STYLE1X, background , foreground);
   renderStringBox(6,  STATE_RANDOMPITCH, 96,   47,  32,16, false, STYLE1X, background , foreground);
   renderStringBox(7,  DISPLAY_LABEL,      0,   63,  64,16, false, STYLE1X, background , foreground);
   renderStringBox(8,  STATE_PITCHMOD,    96,   63,  32,16, false, STYLE1X, background , foreground);
   renderStringBox(9,  DISPLAY_LABEL,      0,   79,  64,16, false, STYLE1X, background , foreground);
   renderStringBox(10,  STATE_GLIDEMOD,   96,   79,  32,16, false, STYLE1X, background , foreground);

 };

 void DisplayModule::modMenu2_DisplayHandler(){
   sprintf(buf, "CH%d MOD CONFG", selectedChannel+1);
   displayElement[0] = strdup(buf);

   displayElement[1] = strdup("ARP TYPE:");

   if (sequenceArray[selectedChannel].cv_arptypemod < 4){
    sprintf(buf, "CV%d", sequenceArray[selectedChannel].cv_arptypemod +1 );
    displayElement[2] = strdup(buf);
  } else {
    displayElement[2] = strdup("OFF");
  }
  displayElement[3] = strdup("ARP SPEED:");

  if (sequenceArray[selectedChannel].cv_arpspdmod < 4){
   sprintf(buf, "CV%d", sequenceArray[selectedChannel].cv_arpspdmod +1 );
   displayElement[4] = strdup(buf);
 } else {
   displayElement[4] = strdup("OFF");
 }
 displayElement[5] = strdup("ARP OCTVE:");

 if (sequenceArray[selectedChannel].cv_arpoctmod < 4){
  sprintf(buf, "CV%d", sequenceArray[selectedChannel].cv_arpoctmod +1 );
  displayElement[6] = strdup(buf);
} else {
  displayElement[6] = strdup("OFF");
}
displayElement[7] = strdup("ARP INTVL:");

if (sequenceArray[selectedChannel].cv_arpintmod < 4){
 sprintf(buf, "CV%d", sequenceArray[selectedChannel].cv_arpintmod +1 );
 displayElement[8] = strdup(buf);
} else {
 displayElement[8] = strdup("OFF");
}
/*
displayElement[9] = strdup("GLIDETIME:");

if (sequenceArray[selectedChannel].cv_glidemod < 4){
 sprintf(buf, "CV%d", sequenceArray[selectedChannel].cv_glidemod +1 );
 displayElement[10] = strdup(buf);
} else {
 displayElement[10] = strdup("OFF");
}
*/
   renderStringBox(0,  DISPLAY_LABEL,      0,    0, 128, 15, false, STYLE1X, background , foreground);
   renderStringBox(1,  DISPLAY_LABEL,      0,   15,  64,16, false, STYLE1X, background , foreground);
   renderStringBox(2,  STATE_ARPTYPEMOD,     96,   15,  32,16, false, STYLE1X, background , foreground);
   renderStringBox(3,  DISPLAY_LABEL,      0,   31,  64,16, false, STYLE1X, background , foreground);
   renderStringBox(4,  STATE_ARPSPDMOD,  96,   31,  32,16, false, STYLE1X, background , foreground);
   renderStringBox(5,  DISPLAY_LABEL,      0,   47,  64,16, false, STYLE1X, background , foreground);
   renderStringBox(6,  STATE_ARPOCTMOD, 96,   47,  32,16, false, STYLE1X, background , foreground);
   renderStringBox(7,  DISPLAY_LABEL,      0,   63,  64,16, false, STYLE1X, background , foreground);
   renderStringBox(8,  STATE_ARPINTMOD,    96,   63,  32,16, false, STYLE1X, background , foreground);
   //renderStringBox(9,  DISPLAY_LABEL,      0,   79,  64,16, false, STYLE1X, background , foreground);
   //renderStringBox(10,  STATE_GLIDEMOD,   96,   79,  32,16, false, STYLE1X, background , foreground);


 };



 void DisplayModule::patternSelectDisplay(){

   uint8_t previousHighlight = highlight;
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
   sprintf(buf, "Current: %02d", currentPattern );
   sprintf(buf, "CH1:%02d CH2:%02d CH3:%02d CH4:%02d ", sequenceArray[0].pattern+1,sequenceArray[1].pattern+1, sequenceArray[2].pattern+1, sequenceArray[3].pattern+1 );
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


void DisplayModule::modalPopup(){
  uint8_t previousHighlight = highlight;
  highlight = currentPattern;

  sprintf(buf, "Calibration Saved!");
  displayElement[0] = strdup(buf);

  renderStringBox(0, DISPLAY_LABEL,  0, 0, 128 , 8, false, REGULAR3X, BLACK, WHITE);
}

 void DisplayModule::inputCalibrationMenuDisplay(){
   uint8_t previousHighlight = highlight;
   highlight = currentPattern;
   oled.setTextWrap(true);

   sprintf(buf, "Input Calibration: SHIFT-CH1 to save");
   displayElement[0] = strdup(buf);


   sprintf(buf, "CV IN 1: %d %d.%02dv %d", cvInputMapped[0],    intFloatHundreds(cvInputMapped1024[0]*500/1024),    intFloatTensOnes(cvInputMapped1024[0]*500/1024), cvInputMapped1024[0] );
   displayElement[1] = strdup(buf);
   sprintf(buf, "O:%d", adcCalibrationOffset[0]);
   displayElement[2] = strdup(buf);
   sprintf(buf, "L:%d", adcCalibrationPos[0]);
   displayElement[3] = strdup(buf);
   sprintf(buf, "H:%d", adcCalibrationNeg[0]);
   displayElement[4] = strdup(buf);

   sprintf(buf, "CV IN 2: %d %d.%02dv %d", cvInputMapped[1],    intFloatHundreds(cvInputMapped1024[1]*500/1024),    intFloatTensOnes(cvInputMapped1024[1]*500/1024), cvInputMapped1024[1] );
   displayElement[5] = strdup(buf);
   sprintf(buf, "O:%d", adcCalibrationOffset[1]);
   displayElement[6] = strdup(buf);
   sprintf(buf, "L: %d", adcCalibrationPos[1]);
   displayElement[7] = strdup(buf);
   sprintf(buf, "H: %d", adcCalibrationNeg[1]);
   displayElement[8] = strdup(buf);

   sprintf(buf, "CV IN 3: %d %d.%02dv %d", cvInputMapped[2],    intFloatHundreds(cvInputMapped1024[2]*500/1024),    intFloatTensOnes(cvInputMapped1024[2]*500/1024), cvInputMapped1024[2] );
   displayElement[9] = strdup(buf);
   sprintf(buf, "O:%d", adcCalibrationOffset[2]);
   displayElement[10] = strdup(buf);
   sprintf(buf, "L: %d", adcCalibrationPos[2]);
   displayElement[11] = strdup(buf);
   sprintf(buf, "H: %d", adcCalibrationNeg[2]);
   displayElement[12] = strdup(buf);

   sprintf(buf, "CV IN 4: %d %d.%02dv %d", cvInputMapped[3],    intFloatHundreds(cvInputMapped1024[3]*500/1024),    intFloatTensOnes(cvInputMapped1024[3]*500/1024), cvInputMapped1024[3] );
   displayElement[13] = strdup(buf);
   sprintf(buf, "O:%d", adcCalibrationOffset[3]);
   displayElement[14] = strdup(buf);
   sprintf(buf, "L: %d", adcCalibrationPos[3]);
   displayElement[15] = strdup(buf);
   sprintf(buf, "H: %d", adcCalibrationNeg[3]);
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


   sprintf(buf, "CV1A -5v:%d", dacCalibrationNeg[dacMap[0]]);
   displayElement[1] = strdup(buf);
   sprintf(buf, "+5v :%d", dacCalibrationPos[dacMap[0]]);
   displayElement[2] = strdup(buf);
   sprintf(buf, "0v");
   displayElement[3] = strdup(buf);

   sprintf(buf, "CV1B -5v:%d", dacCalibrationNeg[dacMap[1]]);
   displayElement[4] = strdup(buf);
   sprintf(buf, "+5v :%d", dacCalibrationPos[dacMap[1]]);
   displayElement[5] = strdup(buf);
   sprintf(buf, "0v");
   displayElement[6] = strdup(buf);

   sprintf(buf, "CV2A -5v:%d", dacCalibrationNeg[dacMap[2]]);
   displayElement[7] = strdup(buf);
   sprintf(buf, "+5v :%d", dacCalibrationPos[dacMap[2]]);
   displayElement[8] = strdup(buf);
   sprintf(buf, "0v");
   displayElement[9] = strdup(buf);

   sprintf(buf, "CV2B -5v:%d", dacCalibrationNeg[dacMap[3]]);
   displayElement[10] = strdup(buf);
   sprintf(buf, "+5v :%d", dacCalibrationPos[dacMap[3]]);
   displayElement[11] = strdup(buf);
   sprintf(buf, "0v");
   displayElement[12] = strdup(buf);

   sprintf(buf, "CV3A -5v:%d", dacCalibrationNeg[dacMap[4]]);
   displayElement[13] = strdup(buf);
   sprintf(buf, "+5v :%d", dacCalibrationPos[dacMap[4]]);
   displayElement[14] = strdup(buf);
   sprintf(buf, "0v");
   displayElement[15] = strdup(buf);

   sprintf(buf, "CV3B -5v:%d", dacCalibrationNeg[dacMap[5]]);
   displayElement[16] = strdup(buf);
   sprintf(buf, "+5v :%d", dacCalibrationPos[dacMap[5]]);
   displayElement[17] = strdup(buf);
   sprintf(buf, "0v");
   displayElement[18] = strdup(buf);

   sprintf(buf, "CV4A -5v:%d", dacCalibrationNeg[dacMap[6]]);
   displayElement[19] = strdup(buf);
   sprintf(buf, "+5v :%d", dacCalibrationPos[dacMap[6]]);
   displayElement[20] = strdup(buf);
   sprintf(buf, "0v");
   displayElement[21] = strdup(buf);

   sprintf(buf, "CV4B -5v:%d", dacCalibrationNeg[dacMap[7]]);
   displayElement[22] = strdup(buf);
   sprintf(buf, "+5v :%d", dacCalibrationPos[dacMap[7]]);
   displayElement[23] = strdup(buf);
   sprintf(buf, "0v");
   displayElement[24] = strdup(buf);

   renderStringBox(0, DISPLAY_LABEL,  0, 0, 128 , 8, false, REGULAR1X, BLACK, WHITE);

   renderStringBox(1, STATE_CALIB_OUTPUT0_LOW,      0, 16, 64 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(2, STATE_CALIB_OUTPUT0_HIGH,    63, 16, 45 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(3, STATE_CALIB_OUTPUT0_TEST,    108, 16, 20 , 8, false, REGULAR1X, BLACK, WHITE);

   renderStringBox(4, STATE_CALIB_OUTPUT1_LOW,      0, 24, 64 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(5, STATE_CALIB_OUTPUT1_HIGH,    63, 24, 45 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(6, STATE_CALIB_OUTPUT1_TEST,    108, 24, 20 , 8, false, REGULAR1X, BLACK, WHITE);

   renderStringBox(7, STATE_CALIB_OUTPUT2_LOW,      0, 32, 64 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(8, STATE_CALIB_OUTPUT2_HIGH,    63, 32, 45 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(9, STATE_CALIB_OUTPUT2_TEST,    108, 32, 20 , 8, false, REGULAR1X, BLACK, WHITE);

   renderStringBox(10, STATE_CALIB_OUTPUT3_LOW,      0, 40, 64 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(11, STATE_CALIB_OUTPUT3_HIGH,    63, 40, 45 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(12, STATE_CALIB_OUTPUT3_TEST,    108, 40, 20 , 8, false, REGULAR1X, BLACK, WHITE);

   renderStringBox(13, STATE_CALIB_OUTPUT4_LOW,      0, 48, 64 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(14, STATE_CALIB_OUTPUT4_HIGH,    63, 48, 45 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(15, STATE_CALIB_OUTPUT4_TEST,    108, 48, 20 , 8, false, REGULAR1X, BLACK, WHITE);

   renderStringBox(16, STATE_CALIB_OUTPUT5_LOW,      0, 56, 64 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(17, STATE_CALIB_OUTPUT5_HIGH,    63, 56, 45 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(18, STATE_CALIB_OUTPUT5_TEST,    108, 56, 20 , 8, false, REGULAR1X, BLACK, WHITE);

   renderStringBox(19, STATE_CALIB_OUTPUT6_LOW,      0, 64, 64 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(20, STATE_CALIB_OUTPUT6_HIGH,    63, 64, 45 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(21, STATE_CALIB_OUTPUT6_TEST,    108, 64, 20 , 8, false, REGULAR1X, BLACK, WHITE);

   renderStringBox(22, STATE_CALIB_OUTPUT7_LOW,      0, 72, 64 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(23, STATE_CALIB_OUTPUT7_HIGH,    63, 72, 45 , 8, false, REGULAR1X, BLACK, WHITE);
   renderStringBox(24, STATE_CALIB_OUTPUT7_TEST,    108, 72, 20 , 8, false, REGULAR1X, BLACK, WHITE);
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

   displayElement[0] = strdup("GLOBAL MENU");
   displayElement[1] = strdup("INIT CH");
   displayElement[2] = strdup("INIT PAT");
   displayElement[3] = strdup("DELETE SAVE FILE");
   displayElement[4] = strdup("TOGGLE GAME OF LIFE");
   displayElement[5] = strdup("");

   renderStringBox(0, DISPLAY_LABEL, 0 , 0  , 128 , 16, false, STYLE1X,  WHITE, RED);
   renderStringBox(1, DISPLAY_LABEL, 0 , 16 , 64 , 16, false, STYLE1X, WHITE, GREEN);
   renderStringBox(2, DISPLAY_LABEL, 0 , 32 , 64 , 16, false, STYLE1X, WHITE, BLUE);
   renderStringBox(3, DISPLAY_LABEL ,0, 48 , 128 , 16, false, STYLE1X, WHITE, BLACK);
   renderStringBox(4, DISPLAY_LABEL ,0, 64 , 128 , 16, false, STYLE1X, WHITE, BLACK);
   renderStringBox(5, DISPLAY_LABEL, 64 , 16 , 64 , 16, false, STYLE1X, WHITE, GREEN);
   renderStringBox(6, DISPLAY_LABEL, 64 , 32 , 64 , 16, false, STYLE1X, WHITE, BLUE);

   sprintf(buf, "step: %d", selectedStep);

 }
