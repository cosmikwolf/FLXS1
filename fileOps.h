#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <ArduinoJson.h>
#include "global.h"
#include "Sequencer.h"

#ifndef _fileOps_h_
#define _fileOps_h_

class fileOps
{
public:
  fileOps();

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

extern fileOps saveFile;

#endif
