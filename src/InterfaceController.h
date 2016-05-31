#ifndef _INTERFACE_CONTROLLER_H_
#define _INTERFACE_CONTROLLER_H_

#include <Arduino.h>
#include <Encoder.h>
#include <Adafruit_MCP23017.h>

#include "DisplayModule.h"
#include "LEDArray.h"
#include "Zetaohm_AD5676/Zetaohm_AD5676.h"
#include "Zetaohm_SAM2695/Zetaohm_SAM2695.h"
#include "InputModule.h"
//#include "Zetaohm_MAX7301/Zetaohm_MAX7301.h"

#include "global.h"

class InterfaceController {
  public:
    void initialize();
    void displayLoop();
    void inputLoop();
    void ledLoop();

  private:
    Zetaohm_AD5676 *dac;
    Zetaohm_SAM2695 *midiSam;
    Adafruit_MCP23017 *gateIo;
    InputModule *buttonIo;
    Encoder *rotaryKnob;
    DisplayModule *display;
    LEDArray *ledArray;
};

#endif
