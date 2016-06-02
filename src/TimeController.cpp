#include <Arduino.h>
#include "TimeController.h"

TimeController::TimeController(){};

void TimeController::initialize() {
	Serial.println("Initializing TimeController");

	outputControl.initialize();

  ledArray.initialize();
  display.initialize();
	buttonIo.buttonSetup(&outputControl);
	clockMaster.initialize(&outputControl);

  sequence[0].initialize(0, 16, 4, (tempoX100/100));
  sequence[1].initialize(1, 16, 4, (tempoX100/100));
  sequence[2].initialize(2, 16, 4, (tempoX100/100));
  sequence[3].initialize(3, 16, 4, (tempoX100/100));

	saveFile.saveSequenceJSON(sequence[0]);
	saveFile.readSequenceJSON(sequence[0]);
	
}

void TimeController::runLoopHandler() {
	ledArray.loop();
	buttonIo.buttonLoop();
	display.displayLoop();
}

void TimeController::masterClockHandler(){
	clockMaster.masterClockFunc();
}
