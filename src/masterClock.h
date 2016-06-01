#include <Arduino.h>
#include "OutputController.h"
#include "Sequencer.h"
#include "midiModule.h"
#include "global.h"

// DAC Mapping for each channel:
#ifndef _MasterClock_h_
#define _MasterClock_h_

class MasterClock {

public:
  void initialize(OutputController* outputControl);
  void changeTempo(uint32_t newTempoX100);
  void masterClockFunc(void);
  void internalClockTick();
  void externalClockTick();
  void noteOffSwitch();
  void noteOnSwitch();

  uint8_t gateMap[4];
  uint8_t dacCvMap[4];
  uint8_t dacCcMap[4];
private:
  OutputController* outputControl;
};

#endif
