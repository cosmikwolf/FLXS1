#include <Arduino.h>
#include "Sequencer.h"
#include "midiModule.h"
#include "global.h"

// DAC Mapping for each channel:
#ifndef _masterClock_h_
#define _masterClock_h_

class masterClock {

public:
  masterClock();
  uint8_t gateMap[4]  = {4, 6, 5, 7};
  uint8_t dacCvMap[4] = {0, 4, 2, 6};
  uint8_t dacCcMap[4] = {1, 5, 3, 7};

  void changeTempo(uint32_t newTempoX100);
  void masterClockFunc();
  void internalClockTick();
  void externalClockTick();
  void noteOffSwitch();
  void noteOnSwitch();

};

extern masterClock clock;
#endif
