uint8_t ledMapping[] = {3,2,1,0,8,7,6,5,13,12,11,10,18,17,16,15,4,9,14,19};

void ledSetup(){
  pinMode(0, OUTPUT);
  digitalWrite(0, LOW);
  pixels.begin();
  pixels.setBrightness(10);
}

void ledLoop(){
  if (pixelTimer > 20000){
    switch (currentState ){
      case STEP_DISPLAY:
        for (int i=0; i < 16; i++){
          if (i == sequence[selectedSequence].activeStep ){
            pixels.setPixelColor(ledMapping[i], pixels.Color(255,255,255) );      
          } else if ( i == selectedStep) {
            pixels.setPixelColor(ledMapping[selectedStep], Wheel(int(millis()/3)%255) );      
          } else {
            if(sequence[selectedSequence].stepData[i].gateType == 0){
              pixels.setPixelColor(ledMapping[i], pixels.Color(0,0,0));
            } else {
              pixels.setPixelColor(ledMapping[i], Wheel( sequence[selectedSequence].getStepPitch(i) ) );
            }
          }
        }
      break;
      case CHANNEL_SELECT:
        for (int i=0; i < 20; i++){
          if (i<4){
            pixels.setPixelColor(ledMapping[i], Wheel((millis()/10 + i*64) % 255));
          } else if (i == 17) {
            pixels.setPixelColor(ledMapping[i], Wheel((millis()/5) % 255));
          }else{
            pixels.setPixelColor(ledMapping[i], pixels.Color(0,0,0));
          }
        }
      break;
      case PATTERN_SELECT:
        nonBlockingRainbow(10);
      break;
    }
    pixels.show();
    pixelTimer = 0;
  }
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

