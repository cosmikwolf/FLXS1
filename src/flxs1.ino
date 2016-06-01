/* **********************************
*** ZETAOHM FLXS1 MUSIC SEQUENCER ***
********************************** */
#include <Arduino.h>
#include <SPI.h>
#include <i2c_t3.h>
#include "TimeController.h"
#include "Sequencer.h"

#include "global.h"

//#include "InputModule.h"
//#include "DisplayModule.h"
//#include "midiModule.h"
//#include "FlashMemory.h"
//#include "LEDArray.h"


#define kSerialSpeed 115200
#define kClockInterval 1000
#define kMosiPin 11
#define kSpiClockPin 13

/*
  Globals to refactor (in this file)
  - display
  - sequence
  - sam2695
  - tempoX100
  - interface
  - ad5676
  - mcp
  - saveFile
  - leds
*/

TimeController timeControl;
IntervalTimer MasterClockTimer;

void setup() {
  Serial.begin(kSerialSpeed);
  while (!Serial){
      ;
  };
  Serial.println("Setup Start");
  SPI.begin();
	SPI.setMOSI(kMosiPin);
	SPI.setSCK(kSpiClockPin);

  timeControl.initialize();
	MasterClockTimer.begin(masterLoop,kClockInterval);
	SPI.usingInterrupt(MasterClockTimer);
  Serial.println("Setup Complete");
}

void loop() {
  timeControl.runLoopHandler();
}

void masterLoop(){
  timeControl.masterClockHandler();
}
