#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <ArduinoJson.h>
#include "global.h"
#include "Sequencer.h"

#ifndef _FlashMemory_h_
#define _FlashMemory_h_

class FlashMemory
{
public:
  FlashMemory();

  void changePattern(uint8_t pattern, uint8_t channelSelector, boolean saveFirst, boolean instant);
  void initialize();
  void deleteSaveFile();
  void saveChannelPattern(uint8_t channel) ;
  void loadPattern(uint8_t pattern, uint8_t channelSelector) ;
  void printDirectory(File dir, int numTabs);
  void printPattern();
  void saveSequenceJSON(Sequencer&);
  void jsonTest();
  File saveData;
  File jsonFile;

};

extern FlashMemory saveFile;

#endif
