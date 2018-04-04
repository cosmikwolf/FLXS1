#include <Arduino.h>
#include "FlashMemory.h"

/*
¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING!
This file contains code that enables saving and loading of patterns. Changing this file could result in an inability to read existing save files.
¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING!
*/
// There is one cache file. Each cache is a 4k segment in the cache file defined
// by the cache index. The CACHEX_DESTINATION is the destination pattern / channel

FlashMemory::FlashMemory(){
  //SerialFlashPrint serialFlashPrint(&file);
};

void FlashMemory::initialize(OutputController * outputControl, Sequencer *sequenceArray, SerialFlashChip *spiFlash, ADC *adc, GlobalVariable *globalObj){
  Serial.println("<</~*^*~\>>Initializing Flash Memory<</~*^*~\>>");
  this->outputControl = outputControl;
  this->sequenceArray = sequenceArray;
  this->spiFlash = spiFlash;
  this->adc = adc;
  this->globalObj = globalObj;

  if(!spiFlash->begin(WINBOND_CS_PIN)){
    Serial.println("SPI FLASH CHIP INITIALIZATION FAILED!");
  }
  spiFlashBusy = 0;
  cacheWriteBusy = 0;
  saveSequenceBusy = 0;
  for(int n = 0; n < CACHE_COUNT; n++){
    cacheStatus[n] = 0;
  }
  noInterrupts();
  this->initializeSaveFile();
  this->initializeGlobalFile();
  this->initializeSongData();
  interrupts();
  int readJsonReturn = this->readGlobalData();
  if(readJsonReturn){
    Serial.println("ERROR READING GLOBAL DATA -- CODE: " + String(readJsonReturn) );
  } else {
    Serial.println("Global Data Loaded" );
  }
  readJsonReturn = this->readSongData();
  if(readJsonReturn){
    Serial.println("ERROR READING SONG DATA -- CODE: " + String(readJsonReturn) );
  } else {
    Serial.println("Song Data Loaded" );
  }


  Serial.println("<</~*^*~\>> Flash Memory Initialization Complete <</~*^*~\>>");
}

void FlashMemory::formatAndInitialize(){

    char* fileName = (char *) malloc(sizeof(char) * 12);
    fileName = strdup("seqData");

    this->formatChip();
    this->initializeCache();

    Serial.println("Creating Data File: " + String(fileName) + "\tsize: " + String(FLASHFILESIZE));
    spiFlash->createErasable(fileName, FLASHFILESIZE);

    for(int pattern=0; pattern < 16; pattern++){
      Serial.println("***----###$$$###---*** *^~^* INITIALIZING PATTERN " + String(pattern) + " *^~^* ***----###$$$###---***");

      for (int channel=0; channel < SEQUENCECOUNT; channel++){
        sequenceArray[channel].initNewSequence(pattern, channel);
      }
      Serial.println("Patterns initialized");
      for (int channel=0; channel < SEQUENCECOUNT; channel++){
        this->saveSequenceData(channel, pattern);
      }
      Serial.println("json sequence saved");
      while(this->cacheWriteSwitch){
        this->cacheWriteLoop();
      };
      Serial.println("***----###$$$###---*** *^~^* PATTERN SAVED " + String(pattern) + " *^~^* ***----###$$$###---***");
      delay(500);
    }
    while(this->cacheWriteSwitch){
      this->cacheWriteLoop();
    }
    delay(100);
    free(fileName);

    Serial.println("Save File Initialization complete.");
}
void FlashMemory::initializeGlobalFile(){
  Serial.println("Initializing Global Settings File");

  char* fileName = (char *) malloc(sizeof(char) * 12);
  fileName =strdup("globData");
  if (!spiFlash->exists(fileName)) {
    Serial.println("globData file does not exist...  creating save file");
    spiFlash->createErasable(fileName, GLOBALFILESIZE);
    globalObj->initGlobals();
    this->saveGlobalData();
  } else {
    Serial.println("Global Settings File found" );

    while(!spiFlash->ready()){

    }
    file = spiFlash->open(fileName);   //open cache file
    if(file){
      Serial.println("Global Settings File Size: " + String(file.size()));
      delay(10);
    } else {
      Serial.println("Could not open global settings file to determine size");
    }
    file.close();
  }
  free(fileName);

}

void FlashMemory::initializeSongData(){
  Serial.println("Initializing Song Data File");

  char* fileName = (char *) malloc(sizeof(char) * 12);
  fileName =strdup("songData");
  if (!spiFlash->exists(fileName)) {
    Serial.println("songData file does not exist...  creating save file");
    spiFlash->createErasable(fileName, GLOBALFILESIZE);
    globalObj->initSongData();
    this->saveSongData();
  } else {
    Serial.println("Song Data File found" );

    while(!spiFlash->ready()){

    }
    file = spiFlash->open(fileName);   //open cache file
    if(file){
      Serial.println("Song Data File Size: " + String(file.size()));
    } else {
      Serial.println("Could not open Song Data file to determine size");
    }
    file.close();
  }
  free(fileName);

}


void FlashMemory::initializeSaveFile(){
  Serial.println("Initializing Save File");

  char* fileName = (char *) malloc(sizeof(char) * 12);
  fileName =strdup("seqData");
  if (!spiFlash->exists(fileName)) {
    Serial.println("seqdata file does not exist... ");
    this->formatAndInitialize();
  } else {
    while(!spiFlash->ready()){
    }
    file = spiFlash->open(fileName);   //open cache file
    if(file){
      Serial.println("Sequence Data File Size: " + String(file.size()));
      delay(10);
    } else {
      Serial.println("Could not open Sequence Data file to determine size");
      delay(10);
    }
    file.close();
  }
  free(fileName);
}

uint8_t FlashMemory::exportSysexData(){
  Serial.println("Beginning SYSEX export of data");
  delay(100);

  const uint8_t sysexheader[2] = {0xF0, 0xFF};
  const uint8_t sysexfooter[1] = {0xF7};

  while (!spiFlash->ready()){
    Serial.println("SPIFLASH NOT READY");
  } ; // wait

  for(uint8_t pattern = 0;pattern<16; pattern++){
    for(uint8_t channel = 0; channel < 4; channel++){
       Serial.println("Sending channel " + String(channel) + " pattern " + String(pattern));

       usbMIDI.sendSysEx(2, sysexheader);
       char* fileName = (char *) malloc(sizeof(char) * 12);
       fileName = strdup("seqData");

       if (spiFlash->exists(fileName)){
         file = spiFlash->open(fileName);
         free(fileName);

         if (file){
           char* fileBuffer = (char*)malloc(SECTORSIZE);  // Allocate memory for the file and a terminating null char.

           file.seek(getSaveAddress(getCacheIndex(channel, pattern)));
           file.read(fileBuffer, SECTORSIZE);
           usbMIDI.sendSysEx(SECTORSIZE,(const uint8_t*) fileBuffer);
           file.close();
           free(fileBuffer);
         } else {
           Serial.println("*&*&*&*&*&*&*&*&* Error, save file exists but cannot open - &*&*&*&*&*&*&*&*&");
           return READ_JSON_ERROR;
         };

         //Serial.println("*&*&*&*&*&*&*&*&* SPI FLASH FILE LOAD END *&*&*&*&*&*&*&*&*&");
       } else {
         free(fileName);
         //Serial.println("*&*&*&*&*&*&*&*&* SPI FLASH FILE DOES NOT EXIST *&*&*&*&*&*&*&*&*&");
         return SAVEFILE_DOES_NOT_EXIST;
       }
       usbMIDI.sendSysEx(1, sysexfooter);
       delay(100);
      // free(fileBuffer);
      // fileBuffer = NULL;
      Serial.println("Channel " + String(channel) + " pattern " + String(pattern) + " sent");

    }
  }

  Serial.println("sending sysex data complete!");
}




void FlashMemory::importSysexData(){

}



bool FlashMemory::validateJson(char* fileBuffer){
  StaticJsonBuffer<16384> jsonBuffer;
  JsonObject& root = jsonBuffer.parse(fileBuffer);
  return root.success();
}

int FlashMemory::getSaveAddress(uint8_t index){
  return index*SECTORSIZE;
};


void FlashMemory::saveCalibrationEEPROM(){
  uint16_t address = 0;
  //setting calibration written flag.
  EEPROMWrite16(address, 0x0000);
  address += 2;

  for (int i = 0; i<4; i++){
    EEPROMWrite16(address, globalObj->adcCalibrationPos[i]);
    address += 2;
  }
  for (int i = 0; i<4; i++){
    EEPROMWrite16(address, globalObj->adcCalibrationNeg[i]);
    address += 2;
  }
  for (int i = 0; i<4; i++){
    EEPROMWrite16(address, globalObj->adcCalibrationOffset[i]);
    address += 2;
  }
  for (int i = 0; i<8; i++){
    EEPROMWrite16(address, globalObj->dacCalibrationNeg[i]);
    address += 2;
  }
  for (int i = 0; i<8; i++){
    EEPROMWrite16(address, globalObj->dacCalibrationPos[i]);
    address += 2;
  }
  Serial.println(" -- CALIBRATION EEPROM WRITE COMPLETE -- ");

};

bool FlashMemory::readCalibrationEEPROM(){
  uint16_t address = 0;
  uint16_t checkVal;

  if(EEPROMRead16(address) == 0xFFFF){
    Serial.println(" -- CALIBRATION HAS NOT BEEN PERFORMED -- ");
    Serial.println(" -- CALIBRATION HAS NOT BEEN PERFORMED -- ");
    Serial.println(" -- CALIBRATION HAS NOT BEEN PERFORMED -- ");
    Serial.println(" -- CALIBRATION HAS NOT BEEN PERFORMED -- ");
    Serial.println(" -- CALIBRATION HAS NOT BEEN PERFORMED -- ");
    Serial.println(" -- CALIBRATION HAS NOT BEEN PERFORMED -- ");
    return 1;
  }
  address += 2;

  for (int i = 0; i<4; i++){
    globalObj->adcCalibrationPos[i] = EEPROMRead16(address);
    address += 2;
  }
  for (int i = 0; i<4; i++){
    globalObj->adcCalibrationNeg[i] = EEPROMRead16(address);
    address += 2;
  }
  for (int i = 0; i<4; i++){
    globalObj->adcCalibrationOffset[i] = EEPROMRead16(address);
    address += 2;
  }
  for (int i = 0; i<8; i++){
    globalObj->dacCalibrationNeg[i] = EEPROMRead16(address);
    address += 2;
  }
  for (int i = 0; i<8; i++){
    globalObj->dacCalibrationPos[i] = EEPROMRead16(address);
    address += 2;
  }


  return 0;
};



void FlashMemory::serializeGlobalSettings(char* fileBuffer){
  StaticJsonBuffer<16384> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  JsonArray& globSettingsArray = root.createNestedArray("settings");

  globSettingsArray.add(18);      // version number           // array index: 0
  globSettingsArray.add(globalObj->dataInputStyle);           // array index: 1
  globSettingsArray.add(globalObj->pageButtonStyle);          // array index: 2
  globSettingsArray.add(globalObj->outputNegOffset[0]);           // array index: 3
  globSettingsArray.add(globalObj->outputNegOffset[1]);           // array index: 4
  globSettingsArray.add(globalObj->outputNegOffset[2]);           // array index: 5
  globSettingsArray.add(globalObj->outputNegOffset[3]);           // array index: 6
  globSettingsArray.add(globalObj->clockMode);           // array index: 7
  globSettingsArray.add(globalObj->tempoX100);           // array index: 8

  Serial.println("Saving Global Settings: " + String(globalObj->dataInputStyle) + "\t" + String(globalObj->pageButtonStyle) + "\t" + String(globalObj->outputNegOffset[0]) + "\t" + String(globalObj->outputNegOffset[1]) + "\t" + String(globalObj->outputNegOffset[2]) + "\t" + String(globalObj->outputNegOffset[3]) + "\t" );

  root.printTo(fileBuffer,4096);
}

bool FlashMemory::deserializeGlobalSettings(char* json){
  StaticJsonBuffer<16384> jsonBuffer;
  JsonObject& jsonReader = jsonBuffer.parseObject(json);
  if(!jsonReader.success()){
    delay(5);
    Serial.println("Deserializing Global Settings Failed, exiting");
    return 0;
  }

  if( jsonReader["settings"][0] != 18) {
    // if global settings are not saved properly, or if it is an older version, initialize globals, re-save and exit.
    // versions prior to 17k are settings version 17
    globalObj->initGlobals();
    this->saveGlobalData();
    return 1;
  }  // global data version is stored here.

  globalObj->dataInputStyle        = jsonReader["settings"][1];
  globalObj->pageButtonStyle       = jsonReader["settings"][2];
  globalObj->outputNegOffset[0]    = jsonReader["settings"][3];
  globalObj->outputNegOffset[1]    = jsonReader["settings"][4];
  globalObj->outputNegOffset[2]    = jsonReader["settings"][5];
  globalObj->outputNegOffset[3]    = jsonReader["settings"][6];
  globalObj->clockMode             = jsonReader["settings"][7];
  globalObj->tempoX100             = jsonReader["settings"][8];

  if(globalObj->clockMode == EXTERNAL_CLOCK_BIDIRECTIONAL_INPUT){
    globalObj->setClockPortDirection(CLOCK_PORT_INPUT);
  } else {
    globalObj->setClockPortDirection(CLOCK_PORT_OUTPUT);
  }
  Serial.println("Reading In Global Settings: " + String(globalObj->dataInputStyle) + "\t" + String(globalObj->pageButtonStyle) + "\t" + String(globalObj->outputNegOffset[0]) + "\t" + String(globalObj->outputNegOffset[1]) + "\t" + String(globalObj->outputNegOffset[2]) + "\t" + String(globalObj->outputNegOffset[3]) + "\t" );
  return 1;
}
void FlashMemory::serializePattern(char* fileBuffer, uint8_t channel, uint8_t pattern){
  StaticJsonBuffer<16384> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["version"] = 18;

  JsonArray& seqSettingsArray = root.createNestedArray("settings"); {}

  seqSettingsArray.add(sequenceArray[channel].stepCount);           // array index: 0
  seqSettingsArray.add(sequenceArray[channel].beatCount);           // array index: 1
  seqSettingsArray.add(sequenceArray[channel].quantizeKey);         // array index: 2
  seqSettingsArray.add(sequenceArray[channel].quantizeMode);        // array index: 3
  seqSettingsArray.add(channel);                                    // array index: 4
  seqSettingsArray.add(pattern);                                    // array index: 5
  seqSettingsArray.add(sequenceArray[channel].clockDivision);       // array index: 6
  seqSettingsArray.add(sequenceArray[channel].gpio_reset);          // array index: 7
  seqSettingsArray.add(sequenceArray[channel].gpio_skipstep);       // array index: 8
  seqSettingsArray.add(sequenceArray[channel].gpio_gatemute);       // array index: 9
  seqSettingsArray.add(sequenceArray[channel].gpio_randompitch);    // array index: 10
  seqSettingsArray.add(sequenceArray[channel].cv_arptypemod);       // array index: 11
  seqSettingsArray.add(sequenceArray[channel].cv_arpspdmod);        // array index: 12
  seqSettingsArray.add(sequenceArray[channel].cv_arpoctmod);        // array index: 13
  seqSettingsArray.add(sequenceArray[channel].cv_arpintmod);        // array index: 14
  seqSettingsArray.add(sequenceArray[channel].cv_pitchmod);         // array index: 15
  seqSettingsArray.add(sequenceArray[channel].cv_gatemod);          // array index: 16
  seqSettingsArray.add(sequenceArray[channel].cv_glidemod);         // array index: 17
  seqSettingsArray.add(sequenceArray[channel].firstStep);           // array index: 18
  seqSettingsArray.add(sequenceArray[channel].swingX100);           // array index: 19
  seqSettingsArray.add(sequenceArray[channel].playMode);            // array index: 20
  seqSettingsArray.add(sequenceArray[channel].skipStepCount);       // array index: 21
  seqSettingsArray.add(sequenceArray[channel].randomLow);           // array index: 22
  seqSettingsArray.add(sequenceArray[channel].randomHigh);          // array index: 23
  seqSettingsArray.add(sequenceArray[channel].quantizeScale);       // array index: 24

  JsonArray& stepDataArray = root.createNestedArray("data");

  //for (int i=0; i< root["stepCount"]; i++){
  for (int i=0; i< MAX_STEPS_PER_SEQUENCE; i++){
    JsonArray& stepDataElement = stepDataArray.createNestedArray();
    stepDataElement.add(i);
    stepDataElement.add(sequenceArray[channel].stepData[i].pitch[0]);
    stepDataElement.add(sequenceArray[channel].stepData[i].pitch[1]);
    stepDataElement.add(sequenceArray[channel].stepData[i].pitch[2]);
    stepDataElement.add(sequenceArray[channel].stepData[i].pitch[3]);
    stepDataElement.add(sequenceArray[channel].stepData[i].chord);
    stepDataElement.add(sequenceArray[channel].stepData[i].beatDiv);
    stepDataElement.add(sequenceArray[channel].stepData[i].gateType);
    stepDataElement.add(sequenceArray[channel].stepData[i].gateLength);
    stepDataElement.add(sequenceArray[channel].stepData[i].arpType);
    stepDataElement.add(sequenceArray[channel].stepData[i].arpOctave);
    stepDataElement.add(sequenceArray[channel].stepData[i].arpSpdNum);
    stepDataElement.add(sequenceArray[channel].stepData[i].arpSpdDen);
    stepDataElement.add(sequenceArray[channel].stepData[i].velocity);
    stepDataElement.add(sequenceArray[channel].stepData[i].velocityType);
    stepDataElement.add(sequenceArray[channel].stepData[i].cv2speed);
    stepDataElement.add(sequenceArray[channel].stepData[i].glide);
  }
  root.printTo(fileBuffer,4096);
}

uint8_t FlashMemory::checkForSavedSequences(){
  char* fileName = (char *) malloc(sizeof(char) * 12);
  fileName = strdup("seqData");
  while (!spiFlash->ready()){
    Serial.println("SPIFLASH NOT READY");
  } ; // wait
  if (spiFlash->exists(fileName)){
    file = spiFlash->open(fileName);
    if (file){
      for(int pattern=0; pattern<16; pattern++){
        for(int channel=0; channel<4; channel++){
          char* fileBuffer = (char*)malloc(SECTORSIZE);  // Allocate memory for the file and a terminating null char.
          file.seek(getSaveAddress(getCacheIndex(channel, pattern)));
          file.read(fileBuffer, SECTORSIZE);
          globalObj->savedSequences[channel][pattern] = this->checkifSequenceHasBeenSaved(fileBuffer);
          file.close();
          free(fileBuffer);
        }
      }
    } else {
      Serial.println("*&*&*&*&*&*&*&*&* Error, save file exists but cannot open - " + String(fileName) + "*&*&*&*&*&*&*&*&*&");
      free(fileName);
      return READ_JSON_ERROR;
    };
    free(fileName);
    return FILE_EXISTS;
  } else {
    free(fileName);
    return SAVEFILE_DOES_NOT_EXIST;
  }
  free(fileName);
}

bool FlashMemory::checkifSequenceHasBeenSaved(char* json){
  StaticJsonBuffer<16384> jsonBuffer;

   JsonObject& jsonReader = jsonBuffer.parseObject(json);

   JsonArray& stepDataArray = jsonReader["data"];

    for (int i=0; i< MAX_STEPS_PER_SEQUENCE; i++){
     StepDatum stepDataBuf;

    if(stepDataArray[i][7] != 0) { // if there are any gate types that are not zero, that means that sequence has been saved
      return true;
    };

    if(stepDataArray[i][1] != 24) { // if there are any pitches that are nto the default 24, that means that sequence has been saved
      return true;
    };
  }
 return false;

}



bool FlashMemory::deserializePattern(uint8_t channel, char* json){
  StaticJsonBuffer<16384> jsonBuffer;

  //Serial.println("jsonBuffer allocated");
   JsonObject& jsonReader = jsonBuffer.parseObject(json);
   //Serial.println("Json Reader Success: " + String(jsonReader.success())) ;
   uint8_t backoutPattern = sequenceArray[channel].pattern;
   //Serial.println("JSON object Parsed");
   sequenceArray[channel].stepCount        = jsonReader["settings"][0];
   sequenceArray[channel].beatCount        = jsonReader["settings"][1];
   sequenceArray[channel].quantizeKey      = jsonReader["settings"][2];
   sequenceArray[channel].quantizeMode     = jsonReader["settings"][3];
   sequenceArray[channel].channel          = jsonReader["settings"][4];
   sequenceArray[channel].pattern          = jsonReader["settings"][5];
   sequenceArray[channel].clockDivision    = jsonReader["settings"][6];
   sequenceArray[channel].gpio_reset       = jsonReader["settings"][7];
   sequenceArray[channel].gpio_skipstep    = jsonReader["settings"][8];
   sequenceArray[channel].gpio_gatemute    = jsonReader["settings"][9];
   sequenceArray[channel].gpio_randompitch = jsonReader["settings"][10];
   sequenceArray[channel].cv_arptypemod    = jsonReader["settings"][11];
   sequenceArray[channel].cv_arpspdmod     = jsonReader["settings"][12];
   sequenceArray[channel].cv_arpoctmod     = jsonReader["settings"][13];
   sequenceArray[channel].cv_arpintmod     = jsonReader["settings"][14];
   sequenceArray[channel].cv_pitchmod      = jsonReader["settings"][15];
   sequenceArray[channel].cv_gatemod       = jsonReader["settings"][16];
   sequenceArray[channel].cv_glidemod      = jsonReader["settings"][17];
   sequenceArray[channel].firstStep        = jsonReader["settings"][18];
   sequenceArray[channel].swingX100        = jsonReader["settings"][19];
   sequenceArray[channel].playMode         = jsonReader["settings"][20];
   sequenceArray[channel].skipStepCount    = jsonReader["settings"][21];
   sequenceArray[channel].randomLow        = jsonReader["settings"][22];
   sequenceArray[channel].randomHigh       = jsonReader["settings"][23];
   sequenceArray[channel].quantizeScale    = jsonReader["settings"][24];
    //.as<uint8_t>();
   //Serial.println("READING IN PATTERN: " + String(sequenceArray[channel].pattern) + " array: "); Serial.println((const char *)jsonReader["pattern"]);
   JsonArray& stepDataArray = jsonReader["data"];
   //Serial.println("Step Data Array Success: " + String(stepDataArray.success())) ;

    //Serial.println("jsonArray declared");
    if (sequenceArray[channel].stepCount == 0){
        for (size_t i = 0; i < 3; i++) {
         Serial.println("Channel " + String(channel) + " -- DATA CORRUPTION ALERT");
        }
      sequenceArray[channel].stepCount = 16;
    }
    for (int i=0; i< MAX_STEPS_PER_SEQUENCE; i++){
     StepDatum stepDataBuf;

     stepDataBuf.pitch[0]    = stepDataArray[i][1];
     stepDataBuf.pitch[1]    = stepDataArray[i][2];
     stepDataBuf.pitch[2]    = stepDataArray[i][3];
     stepDataBuf.pitch[3]    = stepDataArray[i][4];
     stepDataBuf.chord       = stepDataArray[i][5];
     stepDataBuf.beatDiv     = stepDataArray[i][6];
     stepDataBuf.gateType    = stepDataArray[i][7];
     stepDataBuf.gateLength  = stepDataArray[i][8];
     stepDataBuf.arpType     = stepDataArray[i][9];
     stepDataBuf.arpOctave   = stepDataArray[i][10];
     stepDataBuf.arpSpdNum   = stepDataArray[i][11];
     stepDataBuf.arpSpdDen   = stepDataArray[i][12];
     stepDataBuf.velocity    = stepDataArray[i][13];
     stepDataBuf.velocityType = stepDataArray[i][14];
     if(jsonReader["version"] < 18){
      if(stepDataArray[i][14] > 1) stepDataBuf.velocityType += 1 ;
     }
     stepDataBuf.cv2speed    = stepDataArray[i][15];
     stepDataBuf.glide       = stepDataArray[i][16];
     uint8_t index = stepDataArray[i][0];
     sequenceArray[channel].stepData[index] = stepDataBuf;
  }

  bool returnVal = jsonReader.success();

  if(returnVal == false){
    sequenceArray[channel].initNewSequence(backoutPattern, channel);
  }
  return returnVal;
}


void FlashMemory::serializeSongData(char* fileBuffer){
  StaticJsonBuffer<16384> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["version"] = 18;
  JsonArray& songDataArray = root.createNestedArray("songs");
  for(int songNum = 0; songNum <SONG_COUNT_MAX; songNum++){
    JsonArray& songDataSettings = songDataArray.createNestedArray();
    songDataSettings.add(songNum);// song index
    for(int chainNum = 0; chainNum < CHAIN_COUNT_MAX; chainNum++){
      JsonArray& songDataElement = songDataSettings.createNestedArray();
      songDataElement.add(globalObj->chainChannelSelect[0][chainNum]);
      songDataElement.add(globalObj->chainChannelSelect[1][chainNum]);
      songDataElement.add(globalObj->chainChannelSelect[2][chainNum]);
      songDataElement.add(globalObj->chainChannelSelect[3][chainNum]);
      songDataElement.add(globalObj->chainPatternSelect[chainNum]);
      songDataElement.add(globalObj->chainPatternRepeatCount[chainNum]);
      songDataElement.add(globalObj->chainModeMasterChannel[chainNum]);
      songDataElement.add(globalObj->chainChannelMute[0][chainNum]);
      songDataElement.add(globalObj->chainChannelMute[1][chainNum]);
      songDataElement.add(globalObj->chainChannelMute[2][chainNum]);
      songDataElement.add(globalObj->chainChannelMute[3][chainNum]);


      // Serial.println("Saving song data - chainNum: " +String(chainNum) + "\tpattern: " + String(globalObj->chainPatternSelect[chainNum]));
    }
  }
  root.printTo(fileBuffer,4096);
}

bool FlashMemory::deserializeSongData(char* json){
  StaticJsonBuffer<16384> jsonBuffer;
  JsonObject& jsonReader = jsonBuffer.parseObject(json);
  if(!jsonReader.success()){
    delay(5);
    Serial.println("Deserializing Global Settings Failed, exiting");
    return 0;
  }

  if( jsonReader["version"] != 18) {
    // if global settings are not saved properly, or if it is an older version, initialize globals, re-save and exit.
    // versions prior to 17k are settings version 17
    Serial.println("Incorrect song file version!");
    return 0;
  }  // global data version is stored here.
  for(int songNum = 0; songNum <SONG_COUNT_MAX; songNum++){
    globalObj->songIndex =  jsonReader["songs"][songNum][0];
    for(int chainNum= 0; chainNum< CHAIN_COUNT_MAX; chainNum++){
      globalObj->chainChannelSelect[0][chainNum]    = jsonReader["songs"][songNum][chainNum+1][0];
      globalObj->chainChannelSelect[1][chainNum]    = jsonReader["songs"][songNum][chainNum+1][1];
      globalObj->chainChannelSelect[2][chainNum]    = jsonReader["songs"][songNum][chainNum+1][2];
      globalObj->chainChannelSelect[3][chainNum]    = jsonReader["songs"][songNum][chainNum+1][3];
      globalObj->chainPatternSelect[chainNum]       = jsonReader["songs"][songNum][chainNum+1][4];
      globalObj->chainPatternRepeatCount[chainNum]  = jsonReader["songs"][songNum][chainNum+1][5];
      if(jsonReader["songs"][songNum].size() > 6){
        globalObj->chainModeMasterChannel[chainNum] = jsonReader["songs"][songNum][chainNum+1][6];
        globalObj->chainChannelMute[0][chainNum]    = jsonReader["songs"][songNum][chainNum+1][7];
        globalObj->chainChannelMute[1][chainNum]    = jsonReader["songs"][songNum][chainNum+1][8];
        globalObj->chainChannelMute[2][chainNum]    = jsonReader["songs"][songNum][chainNum+1][9];
        globalObj->chainChannelMute[3][chainNum]    = jsonReader["songs"][songNum][chainNum+1][10];
      }
      // Serial.println("loading song data - chainNum: " +String(chainNum) + "\tpattern: " + String(globalObj->chainPatternSelect[chainNum]) + "\trepeat: " + String(globalObj->chainPatternRepeatCount[chainNum]));
    }
  }
  return 1;
}



void FlashMemory::saveSongData(){
  char* fileName = (char *) malloc(sizeof(char) * 12);
  fileName = strdup("songData");
  char * fileBuffer = (char*)calloc(SECTORSIZE, sizeof(char) );

  serializeSongData(fileBuffer);

  while( !(spiFlash->ready()) ){
    //delay(1);
  }
  // Serial.println("Filebuffer: " + String(fileBuffer));
  file = spiFlash->open(fileName);
  if (file){
    file.seek(0);
    file.erase4k();
    file.seek(0);
    file.write(fileBuffer,SECTORSIZE);
    file.close();
  } else {
      Serial.println("##############");
      Serial.println("UNABLE TO OPEN SAVE FILE: "  + String(fileName));
      Serial.println("##############");
  }
  free(fileBuffer);
  fileBuffer = NULL;
}

int FlashMemory::readSongData(){
  char* fileName = (char *) malloc(sizeof(char) * 12);
  fileName = strdup("songData");

  while (!spiFlash->ready()){
    Serial.println("SPIFLASH NOT READY");
  } ; // wait

  if (spiFlash->exists(fileName)){
    //Serial.println("Save file exists... attempting load");
    file = spiFlash->open(fileName);
    if (file){
      char* fileBuffer = (char*)malloc(SECTORSIZE);  // Allocate memory for the file and a terminating null char.
      file.seek(0);
      file.read(fileBuffer, SECTORSIZE);

      if(this->deserializeSongData(fileBuffer) == false ){
        file.close();
        free(fileBuffer);
        file = spiFlash->open(fileName);
        Serial.println("?*?* SONG DATA LOADED BUT DATA CANNOT BE READ. attempting to re-read...");
        file.seek(0);
        file.read(fileBuffer, SECTORSIZE);
        if(this->deserializeSongData(fileBuffer) == false ){
          Serial.println(fileBuffer);
          Serial.println("?*?* SONG DATA LOADED BUT DATA CANNOT BE READ. no more reattempts. data corrupt, re-saving default data");
          globalObj->initSongData();
          this->saveSongData();
        };

      };
      file.close();
      // Serial.println("read Filebuffer: " + String(fileBuffer));
      free(fileBuffer);
    } else {
      Serial.println("*&*&*&*&*&*&*&*&* Error, save file exists but cannot open - " + String(fileName) + "*&*&*&*&*&*&*&*&*&");
      free(fileName);
      return READ_JSON_ERROR;
    };
    free(fileName);
    //Serial.println("*&*&*&*&*&*&*&*&* SPI FLASH FILE LOAD END *&*&*&*&*&*&*&*&*&");
    return FILE_EXISTS;
  } else {
    free(fileName);
    //Serial.println("*&*&*&*&*&*&*&*&* SPI FLASH FILE DOES NOT EXIST *&*&*&*&*&*&*&*&*&");
    return SAVEFILE_DOES_NOT_EXIST;
  }
  free(fileName);
}


void FlashMemory::saveGlobalData(){
  char* fileName = (char *) malloc(sizeof(char) * 12);
  fileName = strdup("globData");
  char * fileBuffer = (char*)calloc(SECTORSIZE, sizeof(char) );

  serializeGlobalSettings(fileBuffer);

  while( !(spiFlash->ready()) ){
    //delay(1);
  }
  // Serial.println("Filebuffer: " + String(fileBuffer));
  file = spiFlash->open(fileName);
  if (file){
    file.seek(0);
    file.erase4k();
    file.seek(0);
    file.write(fileBuffer,SECTORSIZE);
    file.close();
  } else {
      Serial.println("##############");
      Serial.println("UNABLE TO OPEN SAVE FILE: "  + String(fileName));
      Serial.println("##############");
  }
  free(fileBuffer);
  fileBuffer = NULL;
}

int FlashMemory::readGlobalData(){
  char* fileName = (char *) malloc(sizeof(char) * 12);
  fileName = strdup("globData");

  while (!spiFlash->ready()){
    Serial.println("SPIFLASH NOT READY");
  } ; // wait

  if (spiFlash->exists(fileName)){
    //Serial.println("Save file exists... attempting load");
    file = spiFlash->open(fileName);
    if (file){
      char* fileBuffer = (char*)malloc(SECTORSIZE);  // Allocate memory for the file and a terminating null char.
      file.seek(0);
      file.read(fileBuffer, SECTORSIZE);

      if(this->deserializeGlobalSettings(fileBuffer) == false ){
        file.close();
        free(fileBuffer);
        file = spiFlash->open(fileName);
        Serial.println("?*?* GLOBAL DATA LOADED BUT DATA CANNOT BE READ. attempting to re-read...");
        file.seek(0);
        file.read(fileBuffer, SECTORSIZE);
        if(this->deserializeGlobalSettings(fileBuffer) == false ){
          Serial.println(fileBuffer);
          Serial.println("?*?* GLOBAL DATA LOADED BUT DATA CANNOT BE READ. no more reattempts. data corrupt, re-saving default data");
          globalObj->initGlobals();
          this->saveGlobalData();
        };

      };
      file.close();
      // Serial.println("read Filebuffer: " + String(fileBuffer));
      delay(10);
      free(fileBuffer);
    } else {
      Serial.println("*&*&*&*&*&*&*&*&* Error, save file exists but cannot open - " + String(fileName) + "*&*&*&*&*&*&*&*&*&");
      free(fileName);
      return READ_JSON_ERROR;
    };
    free(fileName);
    //Serial.println("*&*&*&*&*&*&*&*&* SPI FLASH FILE LOAD END *&*&*&*&*&*&*&*&*&");
    return FILE_EXISTS;
  } else {
    free(fileName);
    //Serial.println("*&*&*&*&*&*&*&*&* SPI FLASH FILE DOES NOT EXIST *&*&*&*&*&*&*&*&*&");
    return SAVEFILE_DOES_NOT_EXIST;
  }
  free(fileName);
}

void FlashMemory::saveSequenceData(uint8_t channel, uint8_t pattern){
  //http://stackoverflow.com/questions/15179996/how-should-i-allocate-memory-for-c-string-char-array
  globalObj->savedSequences[channel][pattern] = true;
  uint8_t cacheIndex = getCacheIndex(channel, pattern);
  //Serial.println("Saving channel: " + String(channel) + "\tpattern: " + String(pattern) + "\tchannelIndex:" + String(cacheIndex) + "\tsaveAddress: " + String(getSaveAddress(cacheIndex)) );
  while (cacheStatus[cacheIndex] != 0){
    int tempint = cacheWriteLoop();
    //Serial.println("Cache is unavailable upon save attempt...  \tindex:" + String(cacheIndex) + "\tstatus: " + String(cacheStatus[cacheIndex] + " \tcacheWriteReturn: " + String(tempint)));
  }
  saveSequenceBusy = 1;
  cacheWriteTotalTimer = 0;

  char* cacheFileName = (char *) malloc(sizeof(char) * 12);
  cacheFileName = strdup("seqCache");
  char * fileBuffer = (char*)calloc(SECTORSIZE, sizeof(char) );

  serializePattern(fileBuffer, channel, pattern);

  while( !(spiFlash->ready()) ){
    //delay(1);
  }
  file = spiFlash->open(cacheFileName);
  if (file){
    file.seek(getSaveAddress(cacheIndex));
    file.write(fileBuffer,SECTORSIZE);
    file.close();
    setCacheStatus(cacheIndex, SAVING_TO_CACHE_SECTOR);
  } else {
      Serial.println("##############");
      Serial.println("UNABLE TO OPEN SAVE FILE: "  + String(cacheFileName));
      Serial.println("##############");
  }



  free(fileBuffer);
  fileBuffer = NULL;

  free(cacheFileName);
  saveSequenceBusy = 0;
  cacheWriteSwitch = 1;
}

int FlashMemory::readSequenceData(uint8_t channel, uint8_t pattern){
  char* fileName = (char *) malloc(sizeof(char) * 12);
  //strcpy(fileName, String("p" + String(pattern) + ".txt").c_str());
  fileName = strdup("seqData");
  //Serial.println("*&*&*&*&*&*&*&*&* SPI FLASH FILE " + String(fileName) + " LOAD START *&*&*&*&*&*&*&*&*&");

  while (!spiFlash->ready()){
    Serial.println("SPIFLASH NOT READY");
  } ; // wait

  if (spiFlash->exists(fileName)){
    //Serial.println("Save file exists... attempting load");
    file = spiFlash->open(fileName);
    if (file){
      //Serial.println("File opened...");
      //unsigned int fileSize = file.size();  // Get the file size.
      char* fileBuffer = (char*)malloc(SECTORSIZE);  // Allocate memory for the file and a terminating null char.
      //Serial.println(String(SECTORSIZE) + " bytes allocated");
      //Serial.println("SaveAddress: " + String(getSaveAddress(getCacheIndex(channel, pattern))));
      //Serial.println("cacheIndex: " + String(getCacheIndex(channel, pattern)));

      file.seek(getSaveAddress(getCacheIndex(channel, pattern)));
      file.read(fileBuffer, SECTORSIZE);
      //Serial.println("file read");
    //  fileBuffer[SECTORSIZE] = '\0';               // Add the terminating null char.
      //Serial.println(fileBuffer);                // Print the file to the //Serial monitor.
      if(this->deserializePattern(channel, fileBuffer) == false ){

        file.close();
        free(fileBuffer);
        file = spiFlash->open(fileName);
        Serial.println("?*?*?*?*?*?*?*?*?*?*?*?*?*?*?*?*?*?*?*?*? FILE LOADED BUT DATA CANNOT BE READ. attempting to read cache...");
        file.seek(getSaveAddress(getCacheIndex(channel, pattern)));
        file.read(fileBuffer, SECTORSIZE);
        if(this->deserializePattern(channel, fileBuffer) == false ){
          Serial.println(fileBuffer);
          Serial.println("?*?*?*?*?*?*?*?*?*?*?*?*?*?*?*?*?*?*?*?*? FILE LOADED BUT DATA CANNOT BE READ. no more reattempts. data corrupt");
        };

      };
      file.close();
      free(fileBuffer);
    } else {
      Serial.println("*&*&*&*&*&*&*&*&* Error, save file exists but cannot open - " + String(fileName) + "*&*&*&*&*&*&*&*&*&");
      free(fileName);
      return READ_JSON_ERROR;
    };

    free(fileName);
    //Serial.println("*&*&*&*&*&*&*&*&* SPI FLASH FILE LOAD END *&*&*&*&*&*&*&*&*&");
    return FILE_EXISTS;
  } else {
    free(fileName);
    //Serial.println("*&*&*&*&*&*&*&*&* SPI FLASH FILE DOES NOT EXIST *&*&*&*&*&*&*&*&*&");
    return SAVEFILE_DOES_NOT_EXIST;
  }
  free(fileName);

}

void FlashMemory::staggeredLoadLoop(){  //staggers loading of each channel between iterations of masterClock, so that gate has less jitter during load process
  if(globalObj->patternLoadOperationInProgress){
    Serial.println("beginning staggered load " + String(staggerLoadChannelSelector, BIN) );
      for(int i = 0; i<4; i++){
       if( !(staggerLoadChannelSelector & (1 << staggeredLoadCounter) ) ){
        Serial.println("skipping channel " + String(staggeredLoadCounter) );
        staggeredLoadCounter++; // if channel is not selected to be loaded, skip to the next channel!
        if (staggeredLoadCounter >= SEQUENCECOUNT){ goto ENDLOAD; }
       }
      }
    Serial.println(String(millis()) + " \tloading channel " + String(staggeredLoadCounter) );
    this->loadSingleChannel(staggeredLoadCounter, staggeredLoadTargetPattern, true);
    staggeredLoadCounter++;
  };
  ENDLOAD:

  if (staggeredLoadCounter >= SEQUENCECOUNT){
    globalObj->patternLoadOperationInProgress = false;
    staggeredLoadCounter = 0;
    currentPattern = staggeredLoadTargetPattern;
    // Serial.println("save complete: " + String(this->saveTimer));
  }

}

void FlashMemory::loadSingleChannel(uint8_t channel, uint8_t pattern, bool suspendNotesUntilReset){
  int readJsonReturn = this->readSequenceData(channel, pattern);
  if(suspendNotesUntilReset){
  //  sequenceArray[channel].suspendNotesUntilReset = true;
    // for (int stepNum = 0; stepNum < MAX_STEPS_PER_SEQUENCE; stepNum++){
      // if(stepNum == sequenceArray[channel].firstStep ){
      //   sequenceArray[channel].stepData[stepNum].noteStatus = AWAITING_TRIGGER;
      // } else {
        // sequenceArray[channel].stepData[stepNum].noteStatus = NOTE_HAS_BEEN_PLAYED_THIS_ITERATION;
      // }
    // }
  //  Serial.println("setting step " + String(sequenceArray[channel].activeStep) + " on channel " + String(channel) + " to played");
  }

  if ( readJsonReturn == SAVEFILE_DOES_NOT_EXIST )  {
    sequenceArray[channel].initNewSequence(pattern, channel);
  } else if (readJsonReturn == 2) {
    //Serial.("READ JSON ERROR - info above");
  }

}

void FlashMemory::loadPattern(uint8_t pattern, uint8_t channelSelector) {
  // Serial.println("initiating pattern load ");
  globalObj->patternLoadOperationInProgress = true;
  globalObj->waitingToResetAfterPatternLoad = true;
  this->saveTimer = 0;
  this->staggeredLoadTargetPattern = pattern;
  this->staggeredLoadCounter = 0;
  this->staggerLoadChannelSelector = channelSelector;
}

void FlashMemory::savePattern(uint8_t channelSelector,uint8_t *destinationArray){
  for(int i=0; i < SEQUENCECOUNT; i++){
    if( globalObj->patternChannelSelector & (1<<i) ){    //skip the sequences that are masked out
      saveSequenceData(i, destinationArray[i]);
    }
  }
}


void FlashMemory::changePattern(uint8_t pattern, uint8_t channelSelector, uint8_t changeTrigger){
  if (changeTrigger == 0) {
    loadPattern(pattern, channelSelector);
    globalObj->chainModeMasterPulseToGo = 0 ;
  } else {
    globalObj->queuePattern = pattern;
  }
}
