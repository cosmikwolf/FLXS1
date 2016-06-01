#ifndef _TIME_CONTROLLER_H_
#define _TIME_CONTROLLER_H_

#include <Arduino.h>
#include <Encoder.h>
#include <Adafruit_MCP23017.h>
#include "MasterClock.h"
#include "OutputController.h"
#include "InterfaceController.h"
#include "Sequencer.h"
#include "global.h"

class TimeController {
  public:
    TimeController();
    void initialize();
    void runLoopHandler();
    void masterClockHandler();

  private:
    MasterClock clockMaster;
    OutputController outputControl;
    InterfaceController interfaceControl;
};

#endif
