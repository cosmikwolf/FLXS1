/* **********************************
*** ZETAOHM FLXS1 MUSIC SEQUENCER ***
********************************** */
#include <Arduino.h>
#include <SPI.h>
#include <i2c_t3.h>
#include "inputModule.h"
#include "DisplayModule.h"
#include "masterClock.h"
#include "midiModule.h"
#include "fileOps.h"
#include "LEDArray.h"
#include "Sequencer.h"
#include "global.h"
#include "TimeController.h"


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

TimeController controller;
masterClock clockMaster;

void setup() {
  IntervalTimer masterClockTimer;
  contoller.initialize();
	masterClockTimer.begin(masterLoop,kClockInterval);
	SPI.usingInterrupt(masterClockTimer);
}

void loop() {
  controller.runLoopHandler();
}

void masterLoop(){
  clockMaster.masterClockFunc();
}
