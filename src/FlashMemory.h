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
#include <MD5.h>
#include "globalVariable.h"

#define FILE_EXISTS              0
#define SAVEFILE_DOES_NOT_EXIST  1
#define READ_JSON_ERROR          2
#define WINBOND_CS_PIN           22
#define USE_SPI_FLASH            1
#define FLASHFILESIZE            262144
#define GLOBALFILESIZE           65536
#define SONGFILESIZE             65536
#define SECTORSIZE               4096
#define CACHE_WRITE_DELAY        50000
#define AWAITING_FILE_ERASURE    0

#define CACHE_COUNT              64
#define CACHE_READY              0
#define SAVING_TO_CACHE_SECTOR      1
#define ERASING_SAVE_SECTOR         2
#define COPY_CACHE_TO_SAVE_SECTOR   3
#define ERASING_CACHE_SECTOR        4

#define CACHE_UNAVAILABLE      255   // NO FREE CACHE AVAILABLE

#ifndef _FlashMemory_h_
#define _FlashMemory_h_

class FlashMemory
{
public:
  FlashMemory();

  void changePattern(uint8_t pattern, uint8_t channelSelector, uint8_t changeTrigger);
  void savePattern(uint8_t channelSelector, uint8_t *destinationArray);

  void initialize(OutputController* outputControl, Sequencer *sequenceArray,  SerialFlashChip *spiFlash, ADC *adc, GlobalVariable *globalObj);
  void eraseSaveFile();
  void formatChip();
  void initializeSaveFile();
  void initializeGlobalFile();
  void initializeSongData();
  void formatAndInitialize();
  void printDirectory(File dir, int numTabs);
  void printPattern();
  void initializeCache();
  void fileSizeTest();
  void deleteTest();

  void saveCalibrationEEPROM();
  bool readCalibrationEEPROM();
  bool doesSeqDataExist();
  void setCacheStatus(uint8_t index, uint8_t status);
  int  getCacheStatus(uint8_t index);
  int  getCacheIndex(uint8_t channel,uint8_t pattern);

  int  checkCacheStatus(uint8_t address);
  uint16_t EEPROMRead16(int address);
  void EEPROMWrite16(int address, uint16_t value);
  void wipeEEPROM();

  int cacheWriteLoop();
  void loadPattern(uint8_t pattern, uint8_t channelSelector) ;
  void loadSingleChannel(uint8_t channel, uint8_t pattern, bool suspendNotesUntilReset);
  void staggeredLoadLoop();

  void listFiles();

  int getSaveAddress(uint8_t index);
  void serializePattern(char* fileBuffer, uint8_t channel, uint8_t pattern);
  bool deserializePattern(uint8_t channel, char* json);

  uint8_t checkForSavedSequences();
  bool checkifSequenceHasBeenSaved(char* json);

  void serializeGlobalSettings(char* fileBuffer);
  bool deserializeGlobalSettings(char* json);

  void serializeSongData(char* fileBuffer);
  bool deserializeSongData(char* json);

  void saveSongData();
  int readSongData();

  void saveGlobalData();
  int readGlobalData();

  void saveSequenceData(uint8_t channel, uint8_t pattern);
  int  readSequenceData(uint8_t channel, uint8_t pattern);

  uint8_t exportSysexData();
  void importSysexData();

  bool validateJson(char* fileBuffer);

  void deleteAllFiles();
  void rm(File dir, String tempPath);


  bool  cacheWriteSwitch; // is there data in the cache to be saved?
  bool  spiFlashBusy;
  bool  cacheWriteBusy;
  bool  saveSequenceBusy;

private:
  uint16_t cacheOffset;
  uint8_t cacheStatus[CACHE_COUNT];
  uint8_t cacheNum;
  uint8_t staggeredLoadTargetPattern;
  uint8_t staggerLoadChannelSelector;
  elapsedMicros saveTimer;
  uint8_t staggeredLoadChannelSelector;

  OutputController* outputControl;

  elapsedMicros cacheWriteTimer;
  elapsedMicros cacheWriteTotalTimer;
  GlobalVariable *globalObj;
  Sequencer *sequenceArray;
  SerialFlashFile file;
  SerialFlashChip *spiFlash;
  SerialFlashPrint *serialFlashPrint = new SerialFlashPrint(&file);
  ADC *adc;

  File saveData;
  File jsonFile;

};



#endif
