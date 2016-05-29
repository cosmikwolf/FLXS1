#include <Adafruit_NeoPixel.h>

#include <SPI.h>
//#include <spi4teensy3.h>
//#include <Zetaohm_AD5676_spi4teensy.h>
#include <Zetaohm_AD5676.h>
#include <i2c_t3.h>
#include "Adafruit_MCP23017.h"

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(20, 4, NEO_RGBW + NEO_KHZ800);

Adafruit_MCP23017 mcp;

Zetaohm_AD5676 ad5676;
elapsedMillis counter;

IntervalTimer masterClock;
IntervalTimer gateClock;
uint32_t masterClockInterval = 700;
uint32_t gateFuncInterval = 5000;
boolean gateFunc;

void setup(){
  Serial.begin(115200);
  Serial.println("Initializing AD5676 Octal DAC");
  ad5676.begin(3,14);
  ad5676.softwareReset();
  //ad5676.internalReferenceEnable(true);
  masterClock.begin(masterClockFunc,masterClockInterval);
  gateClock.begin(gateNeopixelFunc,gateFuncInterval);
//pinMode(2, OUTPUT);
  pixels.begin();
  pixels.setBrightness(100);
  SPI.usingInterrupt(masterClock);
  mcp.begin(1);      // use default address 0

  mcp.pinMode(0, OUTPUT);
  mcp.pinMode(1, OUTPUT);
  mcp.pinMode(2, OUTPUT);
  mcp.pinMode(3, OUTPUT);
  mcp.pinMode(4, OUTPUT);
  mcp.pinMode(5, OUTPUT);
  mcp.pinMode(6, OUTPUT);
  mcp.pinMode(7, OUTPUT);
  mcp.pinMode(8, OUTPUT);
  mcp.pinMode(9, OUTPUT);
  mcp.pinMode(10, OUTPUT);
  mcp.pinMode(11, OUTPUT);
  mcp.pinMode(12, OUTPUT);
  mcp.pinMode(13, OUTPUT);
  mcp.pinMode(14, OUTPUT);
  mcp.pinMode(15, OUTPUT);
  mcp.pullUp(0,  LOW);
  mcp.pullUp(1,  LOW);
  mcp.pullUp(2,  LOW);
  mcp.pullUp(3,  LOW);
  mcp.pullUp(4,  LOW);
  mcp.pullUp(5,  LOW);
  mcp.pullUp(6,  LOW);
  mcp.pullUp(7,  LOW);
  mcp.pullUp(8,  LOW);
  mcp.pullUp(9,  LOW);
  mcp.pullUp(10, LOW);
  mcp.pullUp(11, LOW);
  mcp.pullUp(12, LOW);
  mcp.pullUp(13, LOW);
  mcp.pullUp(14, LOW);
  mcp.pullUp(15, LOW);
}

void loop(){
//  digitalWrite(2,HIGH);
  //digitalWrite(2,LOW);
  //  ad5676.setVoltage(5, n);

/*	
  if (counter < 1000) {
    ad5676.setVoltage(65535, 0);
    ad5676.setVoltage(65535, 1);
    ad5676.setVoltage(65535, 2);
    ad5676.setVoltage(65535, 3);
    ad5676.setVoltage(65535, 4);
    ad5676.setVoltage(65535, 5);
    ad5676.setVoltage(65535, 6);
    ad5676.setVoltage(65535, 7);
  } else if (counter > 2000) {
    ad5676.setVoltage(0,  0);    
    ad5676.setVoltage(0,  1);    
    ad5676.setVoltage(0,  2);    
    ad5676.setVoltage(0,  3);    
    ad5676.setVoltage(0,  4);    
    ad5676.setVoltage(0,  5);    
    ad5676.setVoltage(0,  6);    
    ad5676.setVoltage(0,  7);    
    counter = 0;
      Serial.println("resetting...\t" + String(millis()));
  } else {
    ad5676.setVoltage(0,  0);    
    ad5676.setVoltage(0,  1);    
    ad5676.setVoltage(0,  2);    
    ad5676.setVoltage(0,  3);    
    ad5676.setVoltage(0,  4);    
    ad5676.setVoltage(0,  5);    
    ad5676.setVoltage(0,  6);    
    ad5676.setVoltage(0,  7);    
  }
  */
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

void gateNeopixelFunc(){
    Serial.println("gate neo func");

  if (gateFunc){
    mcp.digitalWrite(0,  HIGH);
    mcp.digitalWrite(1,  HIGH);
    mcp.digitalWrite(2,  HIGH);
    mcp.digitalWrite(3,  HIGH);
    mcp.digitalWrite(4,  HIGH);
    mcp.digitalWrite(5,  HIGH);
    mcp.digitalWrite(6,  HIGH);
    mcp.digitalWrite(7,  HIGH);
    mcp.digitalWrite(8,  HIGH);
    mcp.digitalWrite(9,  HIGH);
    mcp.digitalWrite(10, HIGH);
    mcp.digitalWrite(11, HIGH);
    mcp.digitalWrite(12, HIGH);
    mcp.digitalWrite(13, HIGH);
    mcp.digitalWrite(14, HIGH);
    mcp.digitalWrite(15, HIGH);
    for (int i=0; i<20; i++){
      pixels.setPixelColor(i, pixels.Color(255,255,255)); // Moderately bright green color.
      pixels.show();
    }
    gateFunc = false;
  } else {
      for (int i=0; i<20; i++){
      pixels.setPixelColor(i, pixels.Color(0,0,0)); // Moderately bright green color.
      pixels.show();
    }
    mcp.digitalWrite(0,  LOW);
    mcp.digitalWrite(1,  LOW);
    mcp.digitalWrite(2,  LOW);
    mcp.digitalWrite(3,  LOW);
    mcp.digitalWrite(4,  LOW);
    mcp.digitalWrite(5,  LOW);
    mcp.digitalWrite(6,  LOW);
    mcp.digitalWrite(7,  LOW);
    mcp.digitalWrite(8,  LOW);
    mcp.digitalWrite(9,  LOW);
    mcp.digitalWrite(10, LOW);
    mcp.digitalWrite(11, LOW);
    mcp.digitalWrite(12, LOW);
    mcp.digitalWrite(13, LOW);
    mcp.digitalWrite(14, LOW);
    mcp.digitalWrite(15, LOW);
    gateFunc = true;
  }
}
void masterClockFunc(){
  Serial.println("master clock func");
 float a = sin(micros()/50000.0)+1.0;
 uint16_t n = a * 32767;

 for (int i=0; i<8; i++){
  ad5676.setVoltage(i, n);
}

}