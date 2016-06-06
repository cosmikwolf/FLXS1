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
  void initialize(Sequencer *sequenceArray);
  void deleteSaveFile();
  void loadPattern(uint8_t pattern, uint8_t channelSelector) ;
  void printDirectory(File dir, int numTabs);
  void printPattern();
  void saveSequenceJSON(uint8_t channel, uint8_t pattern);
  int readSequenceJSON(uint8_t channel, uint8_t pattern);
  void deleteAllFiles();
  void rm(File dir, String tempPath);
  bool deserialize(uint8_t channel, char* json);
private:
  Sequencer *sequenceArray;
  File saveData;
  File jsonFile;

};



#endif
