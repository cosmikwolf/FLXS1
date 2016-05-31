#ifndef _leds_h_
#define _leds_h_
#define FASTLED_ALLOW_INTERRUPTS 0

#include <Arduino.h>
#include <FastLED.h>
#include "global.h"
#include "Sequencer.h"

#define NUM_LEDS 20
#define DATA_PIN 0

class LEDArray
{
  public:
    LEDArray();

    void initialize();
    void loop();
    void fadeall();
    elapsedMicros pixelTimer;

    CRGB leds[NUM_LEDS];
    uint8_t ledMapping[20] = {3,2,1,0,8,7,6,5,13,12,11,10,18,17,16,15,4,9,14,19};

};

#endif
