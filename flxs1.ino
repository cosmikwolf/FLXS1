/* **********************************
*** ZETAOHM FLXS1 MUSIC SEQUENCER ***
********************************** */
#include <Arduino.h>
#include <Wire.h>
//#include <Audio.h>
//#include <analyze_notefreq.h>
#include "inputModule.h"
#include "displayModule.h"
#include "masterClock.h"
#include "midiModule.h"
#include "fileOps.h"
#include "LEDArray.h"
#include "Sequencer.h"
#include "global.h"

/* MIGHT NOT NEED ANY OF THESE VARIABLES? */
uint8_t numSteps = 16;
uint8_t settingMode = 0;

unsigned long lastMicros;
unsigned long avgDelta;
unsigned long lastPulseLength;
unsigned long avgPulseLength;
unsigned long avgPulseJitter;
unsigned long pulseLength;
unsigned long avgInterval;
unsigned long lastAvgInterval;
unsigned long intervalJitter;
unsigned long avgIntervalJitter;
unsigned long timerAvg;

elapsedMicros printTimer;
elapsedMicros saveTimer;
elapsedMicros pixelTimer;
elapsedMicros displayTimer;
elapsedMicros displayDebugTimer;
elapsedMicros inputDebugTimer;
/*  MIGHT NOT NEED ANY OF THESE VARIABLES! */


//end afx-01a
//AudioInputAnalog			adc1(A9);           //xy=579.2000274658203,275.2000274658203
//AudioAnalyzeNoteFrequency	notefreq;      //xy=716.2000274658203,275.2000274658203
//AudioConnection				patchCord1(adc1, notefreq);

//coord_t height, width;
//font_t fontTny;
//font_t fontSm;
//font_t fontMd;
//font_t fontLg;
IntervalTimer masterClockTimer;
uint32_t masterClockInterval = 500;
uint8_t masterPulseCount =24;


void setup() {
	Serial.begin(115200);

	Serial.println("Initializing SPI");
	//AudioMemory(25);


	SPI.begin();
	SPI.setMOSI(11);
	SPI.setSCK(13);

	Serial.println("Initializing Display");
	display.initialize();

	delay(500);
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
//
	Serial.println("Initializing Flash Memory");
	saveFile.initializeFlashMemory();

	Serial.println("Initializing Neopixels");
	leds.initialize();
	Serial.println("Beginning Master Clock");
	masterClockTimer.begin(masterLoop,masterClockInterval);
	SPI.usingInterrupt(masterClockTimer);
	//notefreq.begin(.1);

	//currentState = DEBUG_SCREEN;

}

void loop() {
//	leds.loop();
	interface.buttonLoop();

	if( displayTimer > 10000){
//	  		display.displayLoop();
	   		displayTimer = 0;
	}

	for (int i=0; i<8; i++){
//	    ad5676.setVoltage(i,  voltManual );
	  //  ad5676.setVoltage(i,  positive_modulo(10*millis(), 65535) );
	}

}

void masterLoop(){
  clockMaster.masterClockFunc();
}
