#ifndef _TIME_CONTROLLER_H_
#define _TIME_CONTROLLER_H_

#include <Arduino.h>
#include <Encoder.h>
#include "Zetaohm_MAX7301/Zetaohm_MAX7301.h"
#include <ADC.h>

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
    void initialize(midi::MidiInterface<HardwareSerial>* serialMidi, MidiModule *midiControl, NoteDatum *noteData, Sequencer* sequencerArray, ADC *adc);
    void runLoopHandler();
    void masterClockHandler();

  private:
    MasterClock clockMaster;
    OutputController outputControl;
    InputModule buttonIo;
    DisplayModule display;
    LEDArray ledArray;
    FlashMemory saveFile;
    Zetaohm_MAX7301 midplaneGPIO;
    Zetaohm_MAX7301 backplaneGPIO;
    MidiModule *midiControl;
    midi::MidiInterface<HardwareSerial>* serialMidi;
    Sequencer *sequencerArray;
    ADC *adc;

};
//extern Sequencer sequence[4];

#endif
