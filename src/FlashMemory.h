#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <ArduinoJson.h>
#include "serialFlashPrint.h"
#include "global.h"
#include "Sequencer.h"
#include <string>


#ifndef _FlashMemory_h_
#define _FlashMemory_h_

class FlashMemory
{
public:
  FlashMemory();

  void changePattern(uint8_t pattern, uint8_t channelSelector, boolean saveFirst, boolean instant);
  void initialize(Sequencer *sequenceArray,  SerialFlashChip *spiFlash);
  void deleteSaveFile();
  void loadPattern(uint8_t pattern, uint8_t channelSelector) ;
  void printDirectory(File dir, int numTabs);
  void printPattern();
  void cacheWriteLoop();
  void listFiles();
  void saveSequenceJSON(uint8_t channel, uint8_t pattern);
  int  readSequenceJSON(uint8_t channel, uint8_t pattern);
  void deleteAllFiles();
  void rm(File dir, String tempPath);
  bool deserialize(uint8_t channel, char* json);
  bool  cacheWriteSwitch; // is there data in the cache to be saved?
private:
  uint8_t cacheWriteFileStatus[4]; // is the file cleared and ready to be written?
  uint8_t cacheWritePattern[4]; // where should this data be saved?
  elapsedMicros cacheWriteTimer;
  elapsedMicros cacheWriteTotalTimer;

  Sequencer *sequenceArray;
  SerialFlashFile file;
  SerialFlashChip *spiFlash;
  SerialFlashPrint *serialFlashPrint = new SerialFlashPrint(&file);

  File saveData;
  File jsonFile;

};



#endif
