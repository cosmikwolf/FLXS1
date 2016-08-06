#ifndef _TIME_CONTROLLER_H_
#define _TIME_CONTROLLER_H_

#include <Arduino.h>
#include <Encoder.h>
#include <Adafruit_MCP23017.h>
#include <Encoder.h>
#include "Zetaohm_MAX7301/Zetaohm_MAX7301.h"

#include "DisplayModule.h"
#include "LEDArray.h"
#include "InputModule.h"
#include "FlashMemory.h"
#include "MasterClock.h"
#include "OutputController.h"
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
    InputModule buttonIo;
    DisplayModule display;
    LEDArray ledArray;
    FlashMemory saveFile;
    NoteDatum noteData[4];
    Zetaohm_MAX7301 midplaneGPIO;
    Zetaohm_MAX7301 backplaneGPIO;
};
extern Sequencer sequence[4];

#endif
