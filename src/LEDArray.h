#ifndef _leds_h_
#define _leds_h_
#define FASTLED_ALLOW_INTERRUPTS 0

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "global.h"
#include "Sequencer.h"

#define NUM_LEDS 23
//#define DATA_PIN 32
#define DATA_PIN 5

class LEDArray
{
  public:
    LEDArray();
    void initialize(Sequencer *sequenceArray);
    void loop(uint16_t frequency);
    void fadeall();
    void rainbowCycle(uint8_t wait);
    uint32_t wheel(byte WheelPos);
    void channelPitchModeLEDHandler();
    void channelSequenceModeLEDHandler();
    void channelGateModeLEDHandler();
    void channelEnvelopeModeLEDHandler();
    void channelStepModeLEDHandler();
    void channelLEDHandler();
  private:
    Sequencer *sequenceArray;
    elapsedMicros pixelTimer;
    elapsedMillis blinkTimer;
    Adafruit_NeoPixel leds = Adafruit_NeoPixel(NUM_LEDS, DATA_PIN, NEO_GRBW + NEO_KHZ800);
  //  Adafruit_NeoPixel leds = Adafruit_NeoPixel(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);


  //  CRGB leds[NUM_LEDS];
    //uint8_t ledMapping[NUM_LEDS] =   {3,2,1,0,8,7,6,5,13,12,11,10,18,17,16,15,4,9,14,19};
    uint8_t ledMapping[NUM_LEDS] = {1,2,3,4,6,7,8,9,11,12,13,14,16,17,18,19,0,5,10,15};
    uint8_t ledMainMatrix[16] = {2,3,4,5,7,8,9,10,12,13,14,15,17,18,19,20};
    uint8_t ledChannelButtons[4] = {1,6,11,16};
    uint8_t ledAuxButtons[3] = {0,21,22};

};

#endif
