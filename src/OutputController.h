#ifndef _OUTPUT_CONTROLLER_H_
#define _OUTPUT_CONTROLLER_H_

#include <Arduino.h>
#include <MIDI.h>
#include <ADC.h>

#include "Zetaohm_MAX7301/Zetaohm_MAX7301.h"
#include "Zetaohm_AD5676/Zetaohm_AD5676.h"
#include "Zetaohm_MCP4352/Zetaohm_MCP4352.h"
#include "FlashMemory.h"
#include "midiModule.h"
#include "global.h"

#define PROGRAM_CHANGE      1
#define SET_CHANNEL_VOLUME  2
#define SET_CHANNEL_BANK    3

#define GATEMAP             0
#define CVMAP               1
#define CVRHEO              2
#define CCMAP               3
#define CCRHEO              4
#define RHEOCHANNELCV       5
#define RHEOCHANNELCC       6
#define SLEWSWITCHCV        7
#define SLEWSWITCHCC        8

class OutputController {
public:
  void initialize(Zetaohm_MAX7301* backplaneGPIO, midi::MidiInterface<HardwareSerial>* serialMidi, ADC *adc);
  void noteOn(uint8_t channel, uint8_t note, uint8_t velocity, uint8_t velocityType,uint8_t lfoSpeedSetting, uint8_t glide, bool gate);
  void noteOff(uint8_t channel, uint8_t note, bool gateOff);
  void lfoUpdate(uint8_t channel);
  void allNotesOff(uint8_t channel);
  void setClockOutput(bool value);
  void setGateOutputDebug(uint8_t index, bool value);

  uint8_t analogInputTranspose(uint8_t note);

  void dacTestLoop();
  void calibrationRoutine();
  void inputLoopTest();
  void inputRead();
  elapsedMillis clockOutputTimer;
  elapsedMillis lfoTimer;

  elapsedMicros debugTimer1;
  uint8_t lfoType[4];
  uint8_t lfoSpeed[4];
  uint8_t lfoAmplitude[4];
  bool    lfoRheoSet[4];
  bool  clockValue;
  uint8_t outputMap(uint8_t channel, uint8_t mapType);

private:
  Zetaohm_AD5676 ad5676;
  Zetaohm_MCP4352 mcp4352_1;
  Zetaohm_MCP4352 mcp4352_2;
  ADC *adc;
  Zetaohm_MAX7301* backplaneGPIO;
  midi::MidiInterface<HardwareSerial>* serialMidi;

  uint8_t gateMap[4]  = {0,1,2,3};
  uint8_t dacCvMap[4] = {7,0,5,3};
  uint8_t dacCcMap[4] = {1,6,2,4};
  //uint8_t gateMap[4]  = {4,6,5,7};
  //uint8_t dacCvMap[4] = {2,3,4,5};
  //uint8_t dacCcMap[4] = {1,0,7,6};

  // rheoMap contains corresponding rheostate to the dacCcMap and dacCvMap entries.

  //uint8_t slewSwitchMap[8] = {9,10,11,12,13,14,15,16 };
  uint8_t slewSwitchMap[8] = {9 , 10, 16, 14, 13, 15, 11, 12};

  uint8_t rheoMap[8] = {
    0x00 | 3, // 0  // dacchannel
    0x00 | 1, // 1
    0x10 | 2, // 2
    0x10 | 1, // 3
    0x10 | 0, // 4
    0x10 | 3, // 5
    0x00 | 2, // 6
    0x00 | 0  // 7
  };

/* capctrl  rheonet dacchannel  max7301       backplaneGPIO
    0          3       0          19  p22       9
    1          1       1          20  p23       10
    2          0       6          21  p24       11
    3          2       7          22  p25       12
    4          6       4          23  p26       13
    5          5       3          24  p27       14
    6          4       5          26  p7        15
    7          7       2          25  p28       16
*/


};

#endif
