/* **********************************
*** ZETAOHM FLXS1 MUSIC SEQUENCER ***
********************************** */
#include <Arduino.h>
#include <SPI.h>
#include <i2c_t3.h>
#include "InputModule.h"
#include "DisplayModule.h"
#include "masterClock.h"
#include "midiModule.h"
#include "fileOps.h"
#include "LEDArray.h"
#include "Sequencer.h"
#include "global.h"
#include "TimeController.h"
#define kClockInterval 500

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
masterClock clockMaster;

void setup() {
  IntervalTimer masterClockTimer;
  timeControl.initialize();
	masterClockTimer.begin(masterLoop,kClockInterval);
	SPI.usingInterrupt(masterClockTimer);
}

void loop() {
  timeControl.runLoopHandler();
}

void masterLoop(){
  clockMaster.masterClockFunc();
}
