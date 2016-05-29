/* **********************************
*** ZETAOHM FLXS1 MUSIC SEQUENCER ***
********************************** */
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#include "inputModule.h"
#include "DisplayModule.h"
#include "masterClock.h"
#include "midiModule.h"
#include "fileOps.h"
#include "LEDArray.h"
#include "Sequencer.h"
#include "global.h"

#define kClockInterval 500
#define kSerialSpeed 115200
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
masterClock clockMaster;

void setup() {
  IntervalTimer masterClockTimer;

	Serial.begin(kSerialSpeed);

	Serial.println("Initializing SPI");
	SPI.begin();
	SPI.setMOSI(kMosiPin);
	SPI.setSCK(kSpiClockPin);

	Serial.println("Initializing Display");
	display.initialize();

	Serial.println("Initializing Sequence Objects");
	sequence[0].initialize(0, 16, 4, (tempoX100/100));
	sequence[1].initialize(1, 16, 4, (tempoX100/100));
	sequence[2].initialize(2, 16, 4, (tempoX100/100));
	sequence[3].initialize(3, 16, 4, (tempoX100/100));

	Serial.println("Freeram: " + String(FreeRam2()));
	Serial.println("Initializing SAM2695");
	sam2695.begin();
	sam2695.programChange(0, 0, 38);       // give our two channels different voices
	sam2695.programChange(0, 1, 30);
	sam2695.programChange(0, 2, 128);       // give our two channels different voices
	sam2695.programChange(0, 3, 29);

	Serial.println("Initializing Button Array");
	interface.buttonSetup();

	Serial.println("Initializing MIDI");
	midiSetup();

	Serial.println("Initializing DAC");
	ad5676.begin(3);
	ad5676.softwareReset();
	delay(1);
	ad5676.internalReferenceEnable(true);
	ad5676.internalReferenceEnable(true);

	Serial.println("Setting up debug pin");
	pinMode(DEBUG_PIN, OUTPUT);
	pinMode(4, OUTPUT);

	Serial.println("initializing gate outputs");
	mcp.begin(1);      // use default address 0
	mcp.pinMode(9, INPUT);
	mcp.pinMode(4, OUTPUT);
	mcp.pinMode(5, OUTPUT);
	mcp.pinMode(6, OUTPUT);
	mcp.pinMode(7, OUTPUT);

 	Serial.println("Initializing Flash Memory");
	saveFile.initialize();

	Serial.println("Initializing Neopixels");
	leds.initialize();

	Serial.println("Beginning Master Clock");
	masterClockTimer.begin(masterLoop,kClockInterval);
	SPI.usingInterrupt(masterClockTimer);
}

void loop() {
	leds.loop();
	interface.buttonLoop();
	display.displayLoop();
}

void masterLoop(){
  clockMaster.masterClockFunc();
}
