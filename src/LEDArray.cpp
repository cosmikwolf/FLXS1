#include <Arduino.h>
#include "LEDArray.h"

LEDArray::LEDArray(){};

void LEDArray::initialize(Sequencer *sequenceArray){
  Serial.println("Initializing LED Array");
  this->sequenceArray = sequenceArray;

  pinMode(LED_PIN, OUTPUT);

  LEDS.addLeds<WS2812Controller800Khz,DATA_PIN,GRB>(leds,NUM_LEDS);
  //LEDS.addLeds<NEOPIXEL,DATA_PIN>(leds,NUM_LEDS);
	LEDS.setBrightness(100);

  static uint8_t hue = 0;
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
};

void LEDArray::loop(uint16_t frequency){

  if (pixelTimer > frequency){
    pixelTimer = 0;

    switch (currentState ){
      case STEP_DISPLAY:
        for (int i=0; i < 16; i++){
          if (getNote(i) == sequenceArray[selectedChannel].activeStep ){
            leds[ledMapping[i]] = CRGB(255, 255, 255);
          } else if (getNote(i) == selectedStep) {
            leds[ledMapping[i]] = CHSV(int(millis()/3)%255, 255, 255);
          } else {
            if(sequenceArray[selectedChannel].stepData[getNote(i)].gateType == 0){
              leds[ledMapping[i]] = CHSV(0,0,0);
            } else {
              leds[ledMapping[i]] = CHSV(sequenceArray[selectedChannel].getStepPitch(getNote(i), 0),255,255);
            }
          }
        }
        for (int i=0; i < 4; i++){
          if (selectedChannel == i) {
            leds[ledMapping[i+16]] = CHSV((sequenceArray[selectedChannel].patternIndex * 16) % 255,255,255);
          } else {
            leds[ledMapping[i+16]] = CHSV(0,0,0);
          }
        }
      break;

      case CHANNEL_MENU:
        for (int i=0; i < 20; i++){
          if (i%4==0 || i == 3){
            leds[ledMapping[i]] = CHSV(int(millis()/3)%255, 255, 255);
          } else{
            leds[ledMapping[i]] = CHSV(0,0,0);
          }
        }
      break;

      case SEQUENCE_MENU:
        for (int i=0; i < 16; i++){
          if (i<2){
            leds[ledMapping[i]] = CHSV(int(millis()/10 +i*64)%255, 255, 255);
          } else if (i == 17) {
            leds[ledMapping[i]] = CHSV(int(millis()/5)%255, 255, 255);
          }else{
            leds[ledMapping[i]] = CHSV(0,0,0);
          }
        }
      break;

      case PATTERN_SELECT:
        for (int i=0; i < 16; i++){
          leds[ledMapping[i]] = CHSV(int(millis()/10 +i*64)%255, 255, 255);
        }

        for (int i=0; i < 4; i++){
          if( patternChannelSelector & (1<<i) ){
            leds[ledMapping[i+16]] = CHSV(int(millis()/10 +i*64)%255, 255, 255);
          } else {
            leds[ledMapping[i+16]] = CHSV(0,0,0);
          }
        }
      break;
    }
    noInterrupts();
    FastLED.show();
    interrupts();
  }
}


void LEDArray::fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); } }

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
    switch (currentState ){
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
      case CHANNEL_MENU:
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
