#ifndef _OUTPUT_CONTROLLER_H_
#define _OUTPUT_CONTROLLER_H_

#include <Arduino.h>
#include <MIDI.h>
#include "Adafruit_MCP23017.h"
#include "Zetaohm_AD5676/Zetaohm_AD5676.h"
#include "Zetaohm_SAM2695/Zetaohm_SAM2695.h"
#include "FlashMemory.h"
#include "midiModule.h"
#include "global.h"

#define PROGRAM_CHANGE      1
#define SET_CHANNEL_VOLUME  2
#define SET_CHANNEL_BANK    3

class OutputController {
public:
  void initialize();
  void noteOn(uint8_t channel, uint8_t note, uint8_t velocity);
  void noteOff(uint8_t channel, uint8_t note);
  void allNotesOff(uint8_t channel);
  void samCommand(uint8_t command, uint8_t channel,uint8_t value);

private:
  Zetaohm_SAM2695 sam2695;
  Zetaohm_AD5676 ad5676;
  Adafruit_MCP23017 mcp;

  uint8_t gateMap[4]  = {4, 6, 5, 7};
  uint8_t dacCvMap[4] = {0, 4, 2, 6};
  uint8_t dacCcMap[4] = {1, 5, 3, 7};
};

#endif
