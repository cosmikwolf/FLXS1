#include <Arduino.h>
#include "DisplayModule.h"



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
      //oled.setFont(&unborn_small);
      oled.setTextScale(textSize);
    } else if( textSize == 3){
      oled.setCursor(x+1, y-4);
      oled.setFont(&OrbitB_12);
      oled.setTextScale(textSize-2, textSize-1);
    } else {
      oled.setCursor(x+1, y );
      //oled.setFont(&unborn_small);
      oled.setTextScale(1,2);
    }

    oled.setTextColor(color1);
    oled.print(displayElement[index]);
    if (border){
      oled.drawRect(x,y,w,h, color1);
    }
  }
  //clockMaster->clockRunCheck();
}




void DisplayModule::stepMode_common(char *buf){
  uint8_t previousHighlight = highlight;

    sprintf(buf, "%d-%d", notePage*16 , (notePage+1)*16 );
    displayElement[10] = strdup(buf);
    sprintf(buf, "%d:%d", selectedChannel , sequenceArray[selectedChannel].pattern );
    displayElement[11] = strdup(buf);
    renderOnce_StringBox(10, highlight, previousHighlight, 72, 0, 28, 8, false, 1 ,background , foreground);
    renderOnce_StringBox(11, highlight, previousHighlight, 100, 0, 28, 8, false, 1 ,background , foreground);

}



void DisplayModule::channelPitchMenuDisplay(char *buf){

  //  char *buf = new char[51]; // sprintf buffer

  uint8_t previousHighlight = highlight;
  //oled.setFont(Arial_8);

  switch (stepMode) {
    case STATE_PITCH0:
      highlight = 1;
      break;
    case STATE_GATELENGTH:
      highlight = 6;    break;
    case STATE_CHORD:
      highlight = 7;    break;
    case STATE_STEPCOUNT:
      highlight = 12;   break;
    case STATE_BEATCOUNT:
      highlight = 9;    break;
    case STATE_GATETYPE:
      highlight = 15;   break;
    case STATE_ARPTYPE:
      highlight = 17;   break;
    case STATE_ARPSPEEDNUM:
      highlight = 19;   break;
    case STATE_ARPSPEEDDEN:
      highlight = 25;   break;
    case STATE_ARPOCTAVE:
      highlight = 21;   break;
    case STATE_PITCH1:
      highlight = 2;    break;
    case STATE_PITCH2:
      highlight = 3;    break;
    case STATE_PITCH3:
      highlight = 4;    break;
    case STATE_GLIDE:
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

  sprintf(buf, "%d:%d", selectedChannel , sequenceArray[selectedChannel].pattern );
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


  //sprintf(buf, "state: %d", currentMenu);
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


void DisplayModule::channelEnvelopeMenuDisplay(char *buf) {
  uint8_t previousHighlight = highlight;

  displayElement[0] = strdup("ENVELOPE SET");

  switch (stepMode) {
    case STATE_CV2_LEVEL:
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
/*  int freq1 = frequency;
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
*/
  renderOnce_StringBox(0, highlight, previousHighlight,  0,  0, 128, 16, false, 1,  background, foreground);
  renderOnce_StringBox(1, highlight, previousHighlight,  0,  16, 128, 16, false, 2,  background, foreground);
  renderOnce_StringBox(2, highlight, previousHighlight,  0,  32, 128, 16, false, 2,  background, foreground);
  renderOnce_StringBox(3, highlight, previousHighlight,  0,  48, 128, 16, false, 2,  background, foreground);

}

void DisplayModule::channelInputDisplay(char *buf){

  uint8_t previousHighlight = highlight;
  highlight = 4;
  /*
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
*/
  renderOnce_StringBox(0, highlight, previousHighlight,  0,  0, 128, 16, false, 1,  background, foreground);
  renderOnce_StringBox(1, highlight, previousHighlight,  0,  16, 128, 16, false, 2,  background, foreground);
  renderOnce_StringBox(2, highlight, previousHighlight,  0,  32, 128, 16, false, 2,  background, foreground);
  renderOnce_StringBox(3, highlight, previousHighlight,  0,  48, 128, 16, false, 2,  background, foreground);
}
