#include <Arduino.h>
#include "LEDArray.h"

LEDArray::LEDArray(){};

void LEDArray::initialize(Sequencer *sequenceArray, GlobalVariable* globalObj){
  Serial.println("Initializing LED Array");
  this->sequenceArray = sequenceArray;
  this->globalObj = globalObj;

  pinMode(LED_PIN, OUTPUT);

  leds.setBrightness(200);
  leds.begin();
  leds.show(); // Initialize all pixels to 'off'

};

void LEDArray::playPauseHandler(){
  if(globalObj->playing){
    if(globalObj->chainModeActive){
      leds.setPixelColor(0, wheel(192));
    } else {
      leds.setPixelColor(0, wheel(64));
    }
  } else {
    if(globalObj->chainModeActive){
      leds.setPixelColor(0, wheel(255, 32));
    } else {
      leds.setPixelColor(0, wheel(255));
    }
  }
}

void LEDArray::loop(uint16_t interval){

  if (pixelTimer > interval){

    pixelTimer = 0;

    if(blinkTimer > 6000000/globalObj->tempoX100){
      blinkTimer = 0;
    }

    switch (currentMenu ){
      case SEQUENCE_MENU:
      case QUANTIZE_MENU:
      case MOD_MENU_1:
      case MOD_MENU_2:
      case TEMPO_MENU:
      case GLOBAL_MENU_1:
      case GLOBAL_MENU_2:
      case SYSEX_MENU:
        if(stepMode == STATE_QUANTIZEMODE){
          quantizeModeLEDHandler();
        } else {
          channelSequenceModeLEDHandler();
        }
      break;
      case PITCH_GATE_MENU:
      case MENU_RANDOM:
      case ARPEGGIO_MENU:
        if(globalObj->multiSelectSwitch){
          multiSelectLEDHandler();
        } else {
          channelPitchModeLEDHandler(globalObj->selectedStep);
        }
      break;
      case VELOCITY_MENU:
        if(globalObj->multiSelectSwitch){
          multiSelectLEDHandler();
        } else {
          channelGateModeLEDHandler();
        }
      break;

      case INPUT_DEBUG_MENU:
      case PATTERN_SELECT:
      case SAVE_MENU:
      patternSelectSaveHandler();
      break;
      case MENU_PATTERN_CHAIN:
      patternChainLEDHandler();
      break;
    }
    leds.show();
  }
}
void LEDArray::patternChainLEDHandler(){
  this->playPauseHandler();
  for (int chainNum=0; chainNum < 16; chainNum++){
    if(globalObj->chainPatternSelect[globalObj->chainSelectedPattern] == chainNum){
      leds.setPixelColor(ledMainMatrix[chainNum], wheel(int(millis()/5 + 18*chainNum)%255, 255));
    } else if(globalObj->savedSequences[0][chainNum] || globalObj->savedSequences[1][chainNum] || globalObj->savedSequences[2][chainNum] || globalObj->savedSequences[3][chainNum] ){
        leds.setPixelColor(ledMainMatrix[chainNum],dim( wheel(int(millis()/5 + 18*chainNum)%255, 0), 64) );
      } else {
        leds.setPixelColor(ledMainMatrix[chainNum], 0,0,0,10);
      }
  }

  for (int channel=0; channel < 4; channel++){
    if(globalObj->chainChannelSelect[channel][globalObj->chainSelectedPattern] == true){
      leds.setPixelColor(ledChannelButtons[channel], 0, 0,0,255);
    } else {
      leds.setPixelColor(ledChannelButtons[channel], 0, 0,0,10);
    }
  }

};

void LEDArray::patternSelectSaveHandler(){
  this->playPauseHandler();

  for (int i=0; i < 16; i++){
    if(globalObj->savedSequences[0][i] || globalObj->savedSequences[1][i] || globalObj->savedSequences[2][i] || globalObj->savedSequences[3][i] ){
      leds.setPixelColor(ledMainMatrix[i], wheel(int(millis()/5 + 18*i)%255));
    } else {
      leds.setPixelColor(ledMainMatrix[i], 0,0,0,10);
    }
  }

  for (int i=0; i < 4; i++){
    if( globalObj->patternChannelSelector & (1<<i) ){
      leds.setPixelColor(ledChannelButtons[i], wheel(int(millis()/10 + 18*i)%255, 200));
    } else {
      leds.setPixelColor(ledChannelButtons[i], 16,16,16, 0);
    }
  }

}

void LEDArray::quantizeModeLEDHandler(){
  for (int i=0; i < 12; i++){
    if (sequenceArray[selectedChannel].quantizeMode & (1 << i) ){
      leds.setPixelColor(ledMainMatrix[i], 0,255,0,255);
    } else {
      leds.setPixelColor(ledMainMatrix[i], 255,0,0,1);
    }
  }
  for (int i=12; i < 16; i++){
    leds.setPixelColor(ledMainMatrix[i], 0,0,0,0);
  }
  playPauseHandler();
  channelLEDHandler();
}


void LEDArray::channelSequenceModeLEDHandler(){
  uint8_t iStep;
  for (int i=0; i < 16; i++){
    iStep = sequenceArray[selectedChannel].getActivePage()*16 + i;
    if (iStep == sequenceArray[selectedChannel].activeStep ){
      leds.setPixelColor(ledMainMatrix[i], 255,255,255,255);

    } else if(sequenceArray[selectedChannel].stepData[iStep].gateType == 0){
      leds.setPixelColor(ledMainMatrix[i], 1,1,1,1);
    } else {
      leds.setPixelColor(ledMainMatrix[i], wheel(sequenceArray[selectedChannel].getStepPitch(iStep, 0)));
    }
  }

  playPauseHandler();
  channelLEDHandler();
}

void LEDArray::multiSelectLEDHandler(){
  for (int i=0; i < 16; i++){
    if (globalObj->multiSelection[getNote(i)] && blinkTimer < 3000000 / globalObj->tempoX100){
      leds.setPixelColor(ledMainMatrix[i], 0,0,0,255);
    } else if(sequenceArray[selectedChannel].stepData[getNote(i)].gateType != 0){
      leds.setPixelColor(ledMainMatrix[i], wheel(sequenceArray[selectedChannel].getStepPitch(getNote(i), 0)));
    } else {
      leds.setPixelColor(ledMainMatrix[i], 16,0,16,0);
    }
  }
  this->playPauseHandler();
  this->channelLEDHandler();
};


void LEDArray::channelPitchModeLEDHandler(uint8_t stepSelect){

  for (int i=0; i < 16; i++){
    if (getNote(i) == sequenceArray[selectedChannel].activeStep ){
      leds.setPixelColor(ledMainMatrix[i], 127,127,127,255);
    } else if(sequenceArray[selectedChannel].stepData[getNote(i)].gateType == 0){
      leds.setPixelColor(ledMainMatrix[i], 1,1,1,1);
    } else {
      leds.setPixelColor(ledMainMatrix[i], wheel(sequenceArray[selectedChannel].getStepPitch(getNote(i), 0)));
    }

    if (getNote(i) == stepSelect && blinkTimer < 3000000/globalObj->tempoX100) {
      leds.setPixelColor(ledMainMatrix[i], 255,255,255,0);
    }
  }

  switch(notePage){
    case 0:
    leds.setPixelColor(21, wheel(255));
    leds.setPixelColor(22, wheel(127));
    break;
    case 1:
    leds.setPixelColor(21, wheel(191));
    leds.setPixelColor(22, wheel(127));
    break;
    case 2:
    leds.setPixelColor(21, wheel(127));
    leds.setPixelColor(22, wheel(191));
    break;
    case 3:
    leds.setPixelColor(21, wheel(127));
    leds.setPixelColor(22, wheel(255));
    break;
  }
  playPauseHandler();
  channelLEDHandler();
};

void LEDArray::channelLEDHandler(){
  for (int i=0; i < 4; i++){
    if (selectedChannel == i) {
      if(sequenceArray[i].muteGate == true){
        leds.setPixelColor(ledChannelButtons[i], 127, 0,0,32);
      } else {
        leds.setPixelColor(ledChannelButtons[i], 0, 0,127,64);
      }
    } else {
      if(sequenceArray[i].muteGate == true){
        leds.setPixelColor(ledChannelButtons[i], 127, 0,0,0);
      } else {
        leds.setPixelColor(ledChannelButtons[i], 0, 127,0,0);
      }
    }
  }

}
void LEDArray::channelGateModeLEDHandler(){
  channelLEDHandler();
  playPauseHandler();

  for (int i=0; i < 16; i++){
    if (getNote(i) == sequenceArray[selectedChannel].activeStep ){
      if (sequenceArray[selectedChannel].stepData[getNote(i)].gateType == 1 ){
        leds.setPixelColor(ledMainMatrix[i], 0,0,255,128);
      } else if (sequenceArray[selectedChannel].stepData[getNote(i)].gateType == 2 ){
        leds.setPixelColor(ledMainMatrix[i], 255,0,0,128);
      } else if (sequenceArray[selectedChannel].stepData[getNote(i)].gateType == 3 ){
        leds.setPixelColor(ledMainMatrix[i], 0,255,0,128);
      } else {
        leds.setPixelColor(ledMainMatrix[i], 255,255,255,255);
      }

    } else if (getNote(i) == globalObj->selectedStep) {
      leds.setPixelColor(ledMainMatrix[i], wheel(int(millis()/5)%255));

    } else if(sequenceArray[selectedChannel].stepData[getNote(i)].gateType == 0){
      leds.setPixelColor(ledMainMatrix[i], 0,0,0,0);
    } else {
      if (sequenceArray[selectedChannel].stepData[getNote(i)].gateType == 1 ){
        leds.setPixelColor(ledMainMatrix[i], 0,0,255,0);
      } else if (sequenceArray[selectedChannel].stepData[getNote(i)].gateType == 2 ){
        leds.setPixelColor(ledMainMatrix[i], 255,0,0,0);
      } else if (sequenceArray[selectedChannel].stepData[getNote(i)].gateType == 3 ){
        leds.setPixelColor(ledMainMatrix[i], 0,255,0,0);
      }
    }
  }
};


void LEDArray::channelEnvelopeModeLEDHandler(){

  playPauseHandler();

  for (int i=0; i < 4; i++){
    if (selectedChannel == i) {
      leds.setPixelColor(ledChannelButtons[i], wheel(128));

    } else {
      leds.setPixelColor(ledChannelButtons[i], 0,0,0,0);
    }
  }

};

void LEDArray::channelStepModeLEDHandler(){

  playPauseHandler();

  for (int i=0; i < 4; i++){
    if (selectedChannel == i) {
      leds.setPixelColor(ledChannelButtons[i], wheel(196));
    } else {
      leds.setPixelColor(ledChannelButtons[i], 0,0,0,0);
    }
  }
};

void LEDArray::fadeall() {
  //   for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); }
}

void LEDArray::rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256 * 1; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< 16; i++) {
      leds.setPixelColor(ledMainMatrix[i], wheel(((i * 256 / 16) + j) & 255));
      leds.setPixelColor(ledChannelButtons[i%4], wheel(((i * 256 / 16) + j) & 255));
    }
    leds.show();
    delayMicroseconds(wait);
  }
}

uint32_t LEDArray::wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return leds.Color(255 - WheelPos * 3, 0, WheelPos * 3,0);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return leds.Color(0, WheelPos * 3, 255 - WheelPos * 3,0);
  }
  WheelPos -= 170;
  return leds.Color(WheelPos * 3, 255 - WheelPos * 3, 0,0);
}

uint32_t LEDArray::dim(uint32_t color, uint8_t brightness){
  uint32_t dimmedColor = 0;

  for(int i=0; i<4; i++){
    dimmedColor |= (( ((color & (0xFF << i*8)) >> i*8 ) * brightness ) >> 8 ) << (i*8);
  }
 return dimmedColor;

}

uint32_t LEDArray::wheel(byte WheelPos, uint8_t whiteVal) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return leds.Color(255 - WheelPos * 3, 0, WheelPos * 3,whiteVal);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return leds.Color(0, WheelPos * 3, 255 - WheelPos * 3,whiteVal);
  }
  WheelPos -= 170;
  return leds.Color(WheelPos * 3, 255 - WheelPos * 3, 0,whiteVal);
}
