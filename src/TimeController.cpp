#include <Arduino.h>
#include "TimeController.h"

#define DISPLAY_FREQUENCY 10000
#define INPUT_FREQUENCY 10000
#define LED_FREQUENCY 30000

TimeController::TimeController(){};

void TimeController::initialize() {
	Serial.println("Initializing TimeController");

	sequence[0].initialize(0, 16, 4, (tempoX100/100));
  sequence[1].initialize(1, 16, 4, (tempoX100/100));
  sequence[2].initialize(2, 16, 4, (tempoX100/100));
  sequence[3].initialize(3, 16, 4, (tempoX100/100));

	outputControl.initialize();
	ledArray.initialize(sequence);
	display.initialize(sequence);

	buttonIo.initialize(&outputControl, &saveFile, sequence);
	buttonIo.changeState(STEP_DISPLAY);
	clockMaster.initialize(&outputControl, sequence);
	saveFile.initialize(sequence);
	saveFile.loadPattern(0, 0b1111);

/*
	saveFile.saveSequenceJSON(sequence[0], 0, 0 );
	delay(200);
	saveFile.readSequenceJSON(sequence[0], 0, 0);
	File root = SD.open("/");
  saveFile.printDirectory(root, 2);
	*/
}

void TimeController::runLoopHandler() {
	ledArray.loop(LED_FREQUENCY);
	buttonIo.loop(INPUT_FREQUENCY);
	display.displayLoop(DISPLAY_FREQUENCY);
}

void TimeController::masterClockHandler(){
	clockMaster.masterClockFunc();
}
