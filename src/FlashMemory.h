#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <ArduinoJson.h>
#include "serialFlashPrint.h"
#include "global.h"
#include "Sequencer.h"
#include <EEPROM.h>
#include <string>
#include <ADC.h>

#define FILE_EXISTS              0
#define SAVEFILE_DOES_NOT_EXIST  1
#define READ_JSON_ERROR          2
#define WINBOND_CS_PIN           22
#define USE_SPI_FLASH            1
#define FLASHFILESIZE            65536
#define SAVEBLOCKSIZE            4096
#define CACHE_WRITE_DELAY        500
#define AWAITING_FILE_ERASURE    0

#define CACHE_COUNT              16
#define CACHE_READY              0   // CACHE IS CLEARED AND READY
#define CACHE_SET                1   // CACHE SET, AWAITING WRITE
#define CACHE_WRITING            2   // WRITING TO CACHE IN PROGRESS
#define FILE_ERASING             3   // DEST FILE HAS BEEN ERASED
#define FILE_COPYING             4   // CACHE HAS BEEN COPIED
#define CACHE_ERASING            5   // CACHE FILE IS ERASING

#define CACHE_UNAVAILABLE      255   // NO FREE CACHE AVAILABLE

#ifndef _FlashMemory_h_
#define _FlashMemory_h_

class FlashMemory
{
public:
  FlashMemory();

  void changePattern(uint8_t pattern, uint8_t channelSelector, boolean saveFirst, boolean instant);
  void initialize(Sequencer *sequenceArray,  SerialFlashChip *spiFlash, ADC *adc);
  void deleteSaveFile();
  void loadPattern(uint8_t pattern, uint8_t channelSelector) ;
  void printDirectory(File dir, int numTabs);
  void printPattern();
  void initializeCache();
  int  findFreeCache();
  void fileSizeTest();
  void deleteTest();

  void setCacheStatus(uint16_t cacheOffset, uint8_t cacheIndex, uint8_t status);
  void setCacheChannel(uint16_t cacheOffset, uint8_t cacheIndex, uint8_t channel);
  void setCachePattern(uint16_t cacheOffset, uint8_t cacheIndex, uint8_t pattern);

  int  getCacheStatus(uint16_t cacheOffset, uint8_t cacheIndex);
  int  getCacheChannel(uint16_t cacheOffset, uint8_t cacheIndex);
  int  getCachePattern(uint16_t cacheOffset, uint8_t cacheIndex);

  int  checkCacheStatus(uint8_t address);
  uint16_t EEPROMReadlong(int address);
  void EEPROMWrite16(int address, uint16_t value);
  void wipeEEPROM();

  void cacheWriteLoop();

  void listFiles();
  void saveSequenceJSON(uint8_t channel, uint8_t pattern);
  int  readSequenceJSON(uint8_t channel, uint8_t pattern);
  int getSaveAddress(uint8_t channel, uint8_t pattern);
  void serialize(char* fileBuffer, uint8_t channel, uint8_t pattern);
  bool validateJson(char* fileBuffer);

  void deleteAllFiles();
  void rm(File dir, String tempPath);
  bool deserialize(uint8_t channel, char* json);
  bool  cacheWriteSwitch; // is there data in the cache to be saved?
  bool  spiFlashBusy;
  bool  cacheWriteBusy;
  bool  saveSequenceBusy;

private:
  uint16_t cacheOffset;
  uint8_t cacheStatus[CACHE_COUNT];
  uint8_t cacheNum;

  elapsedMicros cacheWriteTimer;
  elapsedMicros cacheWriteTotalTimer;

  Sequencer *sequenceArray;
  SerialFlashFile file;
  SerialFlashChip *spiFlash;
  SerialFlashPrint *serialFlashPrint = new SerialFlashPrint(&file);
  ADC *adc;

  File saveData;
  File jsonFile;

};



#endif
