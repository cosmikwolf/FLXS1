#include <Arduino.h>
#include "LEDArray.h"

LEDArray::LEDArray(){};

void LEDArray::initialize(Sequencer *sequenceArray, GlobalVariable* globalObj){
  Serial.println("Initializing LED Array");
  this->sequenceArray = sequenceArray;
  this->globalObj = globalObj;

  pinMode(LED_PIN, OUTPUT);

  //LEDS.addLeds<WS2812Controller800Khz,DATA_PIN,GRB>(leds,NUM_LEDS);
  //LEDS.addLeds<NEOPIXEL,DATA_PIN>(leds,NUM_LEDS);
  leds.setBrightness(200);
  leds.begin();
  leds.show(); // Initialize all pixels to 'off'

  // static uint8_t hue = 0;
  // for (int j=0; j < 256; j++){
  //   for (int i=0; i < 16; i++){
  //     leds.setPixelColor(ledMainMatrix[i], j,j,j,j);
  //   }
  //   leds.show();
  //   delayMicroseconds(100);
  // }
  // for (int j=255; j > 0 ; j--){
  //   for (int i=0; i < 16; i++){
  //     leds.setPixelColor(ledMainMatrix[i], j,j,j,j);
  //   }
  //   leds.show();
  //   delayMicroseconds(100);
  // }
  // rainbowCycle(100);
/*

  for(int m=0; m<10; m++ ) {
    for(int i = 0; i < NUM_LEDS; i++) {
      // Set the i'th led to red
      leds[i] = CHSV(hue++, 255, 255);
      // Show the leds

      FastLED.show();
      // now that we've shown the leds, reset the i'th led to black
      // leds[i] = CRGB::Black;
      fadeall();
      // Wait a little bit before we loop around and do it again
      delay(1);
    }

    // Now go in the other direction.
    for(int i = (NUM_LEDS)-1; i >= 0; i--) {
      // Set the i'th led to red
      leds[i] = CHSV(hue++, 255, 255);
      // Show the leds
      FastLED.show();
      // now that we've shown the leds, reset the i'th led to black
      // leds[i] = CRGB::Black;
      fadeall();
      // Wait a little bit before we loop around and do it again
      delay(1);
    };
}
*/
};

void LEDArray::loop(uint16_t interval){

  if (pixelTimer > interval){

    pixelTimer = 0;

    switch (currentMenu ){
      case SEQUENCE_MENU:
      case MOD_MENU_1:
      case MOD_MENU_2:
      case TEMPO_MENU:
      case QUANTIZE_MENU:
          channelSequenceModeLEDHandler();
      break;
      case PITCH_GATE_MENU:
      case ARPEGGIO_MENU:
        if(globalObj->multiSelectSwitch){
          multiSelectLEDHandler();
        } else {
          channelPitchModeLEDHandler();
        }
      break;
      case VELOCITY_MENU:
        if(globalObj->multiSelectSwitch){
          multiSelectLEDHandler();
        } else {
          channelGateModeLEDHandler();
        }
      break;
      case CHANNEL_STEP_MODE:
        if(globalObj->multiSelectSwitch){
          multiSelectLEDHandler();
        } else {
          channelStepModeLEDHandler();
        }
      break;

      case MENU_MULTISELECT:
      //  multiSelectLEDHandler();
      break;

      case INPUT_DEBUG_MENU:
      case PATTERN_SELECT:
      case SAVE_MENU:
        for (int i=0; i < 16; i++){
          //leds[ledMapping[i]] = CHSV(int(millis()/10 +i*64)%255, 255, 255);

          leds.setPixelColor(ledMainMatrix[i], wheel(int(millis()/5 + 18*i)%255));
        }

        for (int i=0; i < 4; i++){
          if( patternChannelSelector & (1<<i) ){
            leds.setPixelColor(ledChannelButtons[i], 0,0,0, 200);
          } else {
            leds.setPixelColor(ledChannelButtons[i], 64,64,64, 0);
          }

/*        if( patternChannelSelector & (1<<i) ){
          //  leds[ledMapping[i+16]] = CHSV(int(millis()/10 +i*64)%255, 255, 255);
          } else {
          //  leds[ledMapping[i+16]] = CHSV(0,0,0);
          }
          */
        }
      break;
    }
    //digitalWriteFast(PIN_EXT_RX, HIGH);
    digitalWriteFast(PIN_EXT_AD_4, HIGH);
    leds.show();
    digitalWriteFast(PIN_EXT_AD_4, LOW);
    //digitalWriteFast(PIN_EXT_RX, LOW);

  }
}

void LEDArray::channelSequenceModeLEDHandler(){
  uint8_t iStep;
  for (int i=0; i < 16; i++){
    iStep = sequenceArray[selectedChannel].getActivePage()*16 + i;
    if (iStep == sequenceArray[selectedChannel].activeStep ){
    //  leds[ledMapping[i]] = CRGB(255, 255, 255);
      leds.setPixelColor(ledMainMatrix[i], 255,255,255,255);

    } else if(sequenceArray[selectedChannel].stepData[iStep].gateType == 0){
        //    leds[ledMapping[i]] = CHSV(0,0,0);
        leds.setPixelColor(ledMainMatrix[i], 1,1,1,1);
    } else {
        //  leds[ledMapping[i]] = CHSV(sequenceArray[selectedChannel].getStepPitch(getNote(i), 0),255,255);
        leds.setPixelColor(ledMainMatrix[i], wheel(sequenceArray[selectedChannel].getStepPitch(iStep, 0)));
    }
  }


  if(playing){
    leds.setPixelColor(0, wheel(64));
  } else {
    leds.setPixelColor(0, wheel(255));
  }

  channelLEDHandler();
}

void LEDArray::multiSelectLEDHandler(){
  for (int i=0; i < 16; i++){
    if (globalObj->multiSelection[getNote(i)]){
      leds.setPixelColor(ledMainMatrix[i], 0,0,0,255);
    } else {
      leds.setPixelColor(ledMainMatrix[i], 64,0,0,0);
    }
  }

  channelLEDHandler();
};


void LEDArray::channelPitchModeLEDHandler(){
  if(blinkTimer > 6000000/tempoX100){
    blinkTimer = 0;
  }

  for (int i=0; i < 16; i++){
    if (getNote(i) == sequenceArray[selectedChannel].activeStep ){
    //  leds[ledMapping[i]] = CRGB(255, 255, 255);
      leds.setPixelColor(ledMainMatrix[i], 127,127,127,255);

    } else if(sequenceArray[selectedChannel].stepData[getNote(i)].gateType == 0){
        //    leds[ledMapping[i]] = CHSV(0,0,0);
        leds.setPixelColor(ledMainMatrix[i], 1,1,1,1);
    } else {
        //  leds[ledMapping[i]] = CHSV(sequenceArray[selectedChannel].getStepPitch(getNote(i), 0),255,255);
        leds.setPixelColor(ledMainMatrix[i], wheel(sequenceArray[selectedChannel].getStepPitch(getNote(i), 0)));
    }

    if (getNote(i) == selectedStep && blinkTimer < 3000000/tempoX100) {
    //  leds[ledMapping[i]] = CHSV(int(millis()/3)%255, 255, 255);
      leds.setPixelColor(ledMainMatrix[i], 255,255,255,0);

    }
  }
  if(playing){
    leds.setPixelColor(0, wheel(64));
  } else {
    leds.setPixelColor(0, wheel(255));
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
  channelLEDHandler();
};

void LEDArray::channelLEDHandler(){
  for (int i=0; i < 4; i++){
    if (selectedChannel == i) {
      if(sequenceArray[i].mute == true){
        leds.setPixelColor(ledChannelButtons[i], 127, 0,0,32);
      } else {
        leds.setPixelColor(ledChannelButtons[i], 0, 0,127,64);
      }
    } else {
      if(sequenceArray[i].mute == true){
        leds.setPixelColor(ledChannelButtons[i], 127, 0,0,0);
      } else {
        leds.setPixelColor(ledChannelButtons[i], 0, 127,0,0);
      }
    }
  }

}
void LEDArray::channelGateModeLEDHandler(){
  for (int i=0; i < 4; i++){
    if (selectedChannel == i) {
    //  leds[ledMapping[i+16]] = CHSV((sequenceArray[selectedChannel].patternIndex * 16) % 255,255,255);
      leds.setPixelColor(ledChannelButtons[i], wheel(64));

    } else {
      leds.setPixelColor(ledChannelButtons[i], 0,0,0,0);
    }
  }
  for (int i=0; i < 16; i++){
    if (getNote(i) == sequenceArray[selectedChannel].activeStep ){
    //  leds[ledMapping[i]] = CRGB(255, 255, 255);
    if (sequenceArray[selectedChannel].stepData[getNote(i)].gateType == 1 ){
      leds.setPixelColor(ledMainMatrix[i], 0,0,255,128);
    } else if (sequenceArray[selectedChannel].stepData[getNote(i)].gateType == 2 ){
      leds.setPixelColor(ledMainMatrix[i], 255,0,0,128);
    } else if (sequenceArray[selectedChannel].stepData[getNote(i)].gateType == 3 ){
      leds.setPixelColor(ledMainMatrix[i], 0,255,0,128);
    } else {
      leds.setPixelColor(ledMainMatrix[i], 255,255,255,255);
    }

    } else if (getNote(i) == selectedStep) {
    //  leds[ledMapping[i]] = CHSV(int(millis()/3)%255, 255, 255);
      leds.setPixelColor(ledMainMatrix[i], wheel(int(millis()/5)%255));

    } else if(sequenceArray[selectedChannel].stepData[getNote(i)].gateType == 0){
        //    leds[ledMapping[i]] = CHSV(0,0,0);
        leds.setPixelColor(ledMainMatrix[i], 0,0,0,0);
    } else {
        //  leds[ledMapping[i]] = CHSV(sequenceArray[selectedChannel].getStepPitch(getNote(i), 0),255,255);
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
  for (int i=0; i < 4; i++){
    if (selectedChannel == i) {
    //  leds[ledMapping[i+16]] = CHSV((sequenceArray[selectedChannel].patternIndex * 16) % 255,255,255);
    leds.setPixelColor(ledChannelButtons[i], wheel(128));

    } else {
      leds.setPixelColor(ledChannelButtons[i], 0,0,0,0);
    }
  }

};

void LEDArray::channelStepModeLEDHandler(){
  for (int i=0; i < 4; i++){
    if (selectedChannel == i) {
    //  leds[ledMapping[i+16]] = CHSV((sequenceArray[selectedChannel].patternIndex * 16) % 255,255,255);
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

/*
void ledSetup(){
  pinMode(0, OUTPUT);
//  digitalWrite(0, LOW);
  pixels.begin();
  pixels.setBrightness(25);
  for (int i=0; i < 16; i++){
    pixels.setPixelColor(ledMapping[i], pixels.Color(255,255,255) );
  }
}



void ledLoop(){
  noInterrupts();
  if (pixelTimer > 20000){
    switch (currentMenu ){
      case STEP_DISPLAY:
        for (int i=0; i < 16; i++){
          if (getNote(i) == sequenceArray[selectedChannel]->activeStep ){
            pixels.setPixelColor(ledMapping[i], pixels.Color(255,255,255) );
          } else if (getNote(i) == selectedStep) {
            pixels.setPixelColor(ledMapping[i], Wheel(int(millis()/3)%255) );
          } else {
            if(sequenceArray[selectedChannel]->stepData[getNote(i)].gateType == 0){
              pixels.setPixelColor(ledMapping[i], pixels.Color(0,0,0));
            } else {
              pixels.setPixelColor(ledMapping[i], Wheel( sequenceArray[selectedChannel]->getStepPitch(getNote(i)) ) );
            }
          }
        }
        for (int i=0; i < 4; i++){
          if (selectedChannel == i) {
            pixels.setPixelColor(ledMapping[i+16], Wheel((sequenceArray[selectedChannel]->patternIndex * 16) % 255));
          } else {
            pixels.setPixelColor(ledMapping[i+16], pixels.Color(0,0,0));
          }
        }

      break;
      case PITCH_GATE_MENU:
        for (int i=0; i < 20; i++){
          if (i%4==0 || i == 3){
            pixels.setPixelColor(ledMapping[i], Wheel((millis()/10 + i*64) % 255));
          } else{
            pixels.setPixelColor(ledMapping[i], pixels.Color(0,0,0));
          }
        }
      break;

      case SEQUENCE_MENU:
        for (int i=0; i < 16; i++){
          if (i<2){
            pixels.setPixelColor(ledMapping[i], Wheel((millis()/10 + i*64) % 255));
          } else if (i == 17) {
            pixels.setPixelColor(ledMapping[i], Wheel((millis()/5) % 255));
          }else{
            pixels.setPixelColor(ledMapping[i], pixels.Color(0,0,0));
          }
        }


      break;

      case PATTERN_SELECT:
        for (int i=0; i < 16; i++){
          pixels.setPixelColor(ledMapping[i], Wheel((millis()/10 + i*64) % 255));
        }

        for (int i=0; i < 4; i++){
          if( patternChannelSelector & (1<<i) ){
            pixels.setPixelColor(ledMapping[i+16], Wheel((millis()/20 + i*64) % 255));
          } else {
            pixels.setPixelColor(ledMapping[i+16], pixels.Color(0,0,0));
          }
        }
      break;
    }
    pixels.show();
    pixelTimer = 0;
  }
  interrupts();
}



// NeoPixel Subs

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<pixels.numPixels(); i++) {
    pixels.setPixelColor(i, c);
    pixels.show();
    delay(wait);
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < pixels.numPixels(); i=i+3) {
          pixels.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        pixels.show();

        delay(wait);

        for (int i=0; i < pixels.numPixels(); i=i+3) {
          pixels.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}

uint32_t freemem(){ // for Teensy 3.0
    uint32_t stackTop;
    uint32_t heapTop;

    // current position of the stack.
    stackTop = (uint32_t) &stackTop;

    // current position of heap.
    void* hTop = malloc(1);
    heapTop = (uint32_t) hTop;
    free(hTop);

    // The difference is the free, available ram.
    return stackTop - heapTop;
}

void nonBlockingRainbow(uint8_t interval) {
  uint16_t i, j;

    j = positive_modulo( millis()/interval, 255 );

    for(i=0; i<pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel((5*i+j) & 255));
    }
    pixels.show();

}

void nonBlockingRainbow(uint8_t interval, uint8_t *skipArr, uint8_t skipArrSize) {
  uint16_t i,n, j;

  j = positive_modulo( millis()/interval, 255 );

  for(i=0; i < pixels.numPixels(); i++) {
    bool skip = false;
    for(n=0; n < skipArrSize; n++){
      if (skipArr[n] == i){
        skip = true;
      }
    }

    if (!skip){
      pixels.setPixelColor(i, Wheel((5*i+j) & 255));
    } else {
      pixels.setPixelColor(i, pixels.Color(10,10,10));
    }
  }

  pixels.show();

}
*/
