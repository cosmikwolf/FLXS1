#include <Arduino.h>
#include "TimeController.h"

void TimeController::initialize() {
  sequence[0]->initialize(0, 16, 4, (tempoX100/100));
  sequence[1]->initialize(1, 16, 4, (tempoX100/100));
  sequence[2]->initialize(2, 16, 4, (tempoX100/100));
  sequence[3]->initialize(3, 16, 4, (tempoX100/100));
}

void TimeController::runLoopHandler() {
	interfaceControl->inputLoop();
	interfaceControl->displayLoop();
	interfaceControl->ledLoop();
}

void TimeController::masterClockHandler(){

}
