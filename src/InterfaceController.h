#ifndef _INTERFACE_CONTROLLER_H_
#define _INTERFACE_CONTROLLER_H_

#include <Arduino.h>
#include <Encoder.h>
#include <Adafruit_MCP23017.h>
#include "DisplayModule.h"
#include "LEDArray.h"
#include "InputModule.h"
#include "OutputController.h"
#include "global.h"

class InterfaceController {
  public:
    InterfaceController();
    void initialize(OutputController * outputControl);
    void loop();
  private:
    InputModule buttonIo;
    DisplayModule display;
    LEDArray ledArray;
};

#endif
