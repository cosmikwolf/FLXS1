#ifndef _OUTPUT_CONTROLLER_H_
#define _OUTPUT_CONTROLLER_H_

#include <Arduino.h>
#include <MIDI.h>
#include "Zetaohm_MAX7301/Zetaohm_MAX7301.h"
#include "Zetaohm_AD5676/Zetaohm_AD5676.h"
#include "Zetaohm_SAM2695/Zetaohm_SAM2695.h"
#include "Zetaohm_MCP4352/Zetaohm_MCP4352.h"
#include "FlashMemory.h"
#include "midiModule.h"
#include "global.h"

#define PROGRAM_CHANGE      1
#define SET_CHANNEL_VOLUME  2
#define SET_CHANNEL_BANK    3

class OutputController {
public:
  void initialize(Zetaohm_MAX7301* backplaneGPIO);
  void noteOn(uint8_t channel, uint8_t note, uint8_t velocity, uint8_t glide);
  void noteOff(uint8_t channel, uint8_t note);
  void allNotesOff(uint8_t channel);
  void samCommand(uint8_t command, uint8_t channel,uint8_t value);
  void dacTestLoop();
  void calibrationRoutine();

private:
  Zetaohm_SAM2695 sam2695;
  Zetaohm_AD5676 ad5676;
  Zetaohm_MCP4352 mcp4352_1;
  Zetaohm_MCP4352 mcp4352_2;

  Zetaohm_MAX7301* backplaneGPIO;

  uint8_t gateMap[4]  = {4,6,5,7};
  uint8_t dacCvMap[4] = {7,0,5,3};
//  uint8_t dacCvMap[4] = {2,3,4,5};
  uint8_t dacCcMap[4] = {1,6,2,4};
  //uint8_t dacCcMap[4] = {1,0,7,6};
  uint8_t rheoMap[8] = {
    0x00 & 3, // 0
    0x00 & 1, // 1
    0x01 & 2, // 2
    0x01 & 1, // 3
    0x01 & 0, // 4
    0x01 & 3, // 5
    0x00 & 2, // 6
    0x00 & 0  // 7
   }
};

#endif
