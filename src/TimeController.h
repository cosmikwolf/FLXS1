#ifndef _TIME_CONTROLLER_H_
#define _TIME_CONTROLLER_H_

#include <Arduino.h>
#include <Encoder.h>
#include <Adafruit_MCP23017.h>

#include "InterfaceController.h"
#include "Sequencer.h"

#include "global.h"

class TimeController {
  public:
    void initialize();
    void runLoopHandler();
    void masterClockHandler();

  private:
    InterfaceController *interfaceControl;
    Sequencer *sequence[4];
};

#endif
