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
  Serial.println("<</~*^*~\>> Flash Memory Initialization Complete <</~*^*~\>>");
}

// BRAIN DUMP:
// always use dedicated cache for each file save.
// CACHE STATUSES
// SAVING TO CACHESECTOR
// ERASING SAVESECTOR
// COPYING CACHESECTOR TO SAVESECTOR
// ERASING CACHESECTOR
//
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

uint8_t FlashMemory::getSaveSector(uint8_t channel, uint8_t pattern){
  return (pattern * 4 + channel) * SECTORSIZE;
};

void FlashMemory::saveSequenceJSON(uint8_t channel, uint8_t pattern){
  //http://stackoverflow.com/questions/15179996/how-should-i-allocate-memory-for-c-string-char-array

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

  serialize(fileBuffer, channel, pattern);

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


void FlashMemory::serialize(char* fileBuffer, uint8_t channel, uint8_t pattern){
  StaticJsonBuffer<16384> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  JsonArray& seqSettingsArray = root.createNestedArray("settings");

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
  seqSettingsArray.add(sequenceArray[channel].playDirection);       // array index: 20
  seqSettingsArray.add(sequenceArray[channel].skipStepCount);       // array index: 21
  seqSettingsArray.add(sequenceArray[channel].randomLow);           // array index: 22
  seqSettingsArray.add(sequenceArray[channel].randomHigh);          // array index: 23

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
    stepDataElement.add(sequenceArray[channel].stepData[i].lfoSpeed);
    stepDataElement.add(sequenceArray[channel].stepData[i].glide);
  }
  root.printTo(fileBuffer,4096);
}

bool FlashMemory::deserialize(uint8_t channel, char* json){
  StaticJsonBuffer<16384> jsonBuffer;

  //Serial.println("jsonBuffer allocated");
   JsonObject& jsonReader = jsonBuffer.parseObject(json);
   //Serial.println("Json Reader Success: " + String(jsonReader.success())) ;

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
   sequenceArray[channel].playDirection    = jsonReader["settings"][20];
   sequenceArray[channel].skipStepCount    = jsonReader["settings"][21];
   sequenceArray[channel].randomLow        = jsonReader["settings"][22];
   sequenceArray[channel].randomHigh       = jsonReader["settings"][23];
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
     stepDataBuf.velocityType= stepDataArray[i][14];
     stepDataBuf.lfoSpeed    = stepDataArray[i][15];
     stepDataBuf.glide       = stepDataArray[i][16];
     uint8_t index = stepDataArray[i][0];
     sequenceArray[channel].stepData[index] = stepDataBuf;
  }

/*  for(int n = sequenceArray[channel].stepCount; n< MAX_STEPS_PER_SEQUENCE; n++){
    // fill up the remaining steps with zeroed data so if stepCount is increased, it will work
    StepDatum stepDataBuf;

    stepDataBuf.pitch[0]   = 24;
    for (int i=1; i<4; i++){
      stepDataBuf.pitch[i] = 0;
    }
    stepDataBuf.chord	   		 =	 0;
    stepDataBuf.gateType		 =	 0;
    stepDataBuf.gateLength	 =	 1;
    stepDataBuf.arpType			 =	 0;
    stepDataBuf.arpOctave		 =   1;
    stepDataBuf.arpSpdNum		 =   1;
    stepDataBuf.arpSpdDen		 =   4;
    stepDataBuf.glide				 =   0;
    stepDataBuf.beatDiv			 =   4;
    stepDataBuf.velocity		 =  67;
    stepDataBuf.velocityType =   0;
    stepDataBuf.lfoSpeed		 =  16;
    stepDataBuf.offset       =   0;
    stepDataBuf.noteStatus   =  AWAITING_TRIGGER;
    stepDataBuf.notePlaying  =   0;
    sequenceArray[channel].stepData[n] = stepDataBuf;

  }*/

  return jsonReader.success();
}


bool FlashMemory::validateJson(char* fileBuffer){
  StaticJsonBuffer<16384> jsonBuffer;
  JsonObject& root = jsonBuffer.parse(fileBuffer);
  return root.success();
}

int FlashMemory::getSaveAddress(uint8_t index){
  return index*SECTORSIZE;
};

int FlashMemory::readSequenceJSON(uint8_t channel, uint8_t pattern){
    /*char* fileNameChar = (char *) malloc(sizeof(char) * 12);
    strcpy(fileNameChar, "P");
    strcat(fileNameChar, String(pattern).c_str());
  //  strcat(fileNameChar, "CH");
  //  strcat(fileNameChar, String(channel).c_str());
    strcat(fileNameChar,".TXT");
*/
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
          if(this->deserialize(channel, fileBuffer) == false ){

            file.close();
            free(fileBuffer);
            file = spiFlash->open(fileName);
            Serial.println("?*?*?*?*?*?*?*?*?*?*?*?*?*?*?*?*?*?*?*?*? FILE LOADED BUT DATA CANNOT BE READ. attempting to read cache...");
            file.seek(getSaveAddress(getCacheIndex(channel, pattern)));
            file.read(fileBuffer, SECTORSIZE);
            if(this->deserialize(channel, fileBuffer) == false ){
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


void FlashMemory::loadPattern(uint8_t pattern, uint8_t channelSelector) {
  /*
  for (int i = 0; i<CACHE_COUNT; i++){
    while( getCachePattern(cacheOffset, i) == pattern && getCacheStatus(cacheOffset, i) != 0 ) {
      Serial.println("Delaying because pattern " + String(pattern) + " has pending save operation");
      cacheWriteLoop();
      delay(500);
    }
  }
*/
  //Serial.println("[[-]]><{{{--}}}><[[-]] LOADING PATTERN: " + String(pattern) + " [[-]]><{{{--}}}><[[-]]");
//  printPattern();

	for(int i=0; i < SEQUENCECOUNT; i++){

    if ( !(channelSelector & (1 << i) ) ){
      //Serial.println("skipping loading channel " + String(i));
      continue; // if channel is not selected to be loaded, don't load the channel!
    } else {
      //Serial.println("Loading channel " + String(i));
    };

    //Serial.println("About to run readJsonReturn for channel: " + String(i));

    int readJsonReturn = this->readSequenceJSON(i, pattern);

    if ( readJsonReturn == SAVEFILE_DOES_NOT_EXIST )  {
      //Serial.println("saveData not available, initializing sequence");
      sequenceArray[i].initNewSequence(pattern, i);
      //Serial.println("sequence initialized, saving sequence to JSON");
      //saveSequenceJSON(i, pattern);

      //Serial.println("Sequence successfully saved to JSON");

    } else if (readJsonReturn == 2) {
      //Serial.println("READ JSON ERROR - info above");
    }

    //Serial.println("reading complete!");

  }

  //Serial.println("changing current pattern from " + String(currentPattern) + " to " + String(pattern) + " and also this is queuePattern: " + String(queuePattern));
  currentPattern = pattern;
  //Serial.println("[[-]]><{{{--}}}><[[-]] PATTERN: " + String(pattern) + " LOAD COMPLETE [[-]]><{{{--}}}><[[-]]");
  //printPattern();
}

void FlashMemory::savePattern(uint8_t channelSelector,uint8_t *destinationArray){
  for(int i=0; i < SEQUENCECOUNT; i++){
    //need to skip the sequences that are masked out
    if( patternChannelSelector & (1<<i) ){
      saveSequenceJSON(i, destinationArray[i]);
    }
  }
}


void FlashMemory::changePattern(uint8_t pattern, uint8_t channelSelector, boolean instant){
	//Serial.println("currentPattern: " + String(currentPattern) + "\tSEQUENCECOUNT: " + String(SEQUENCECOUNT));
	//if(saveFirst){
//    for(int i=0; i < SEQUENCECOUNT; i++){
  		//saveChannelPattern(i);
      //saveSequenceJSON(i, currentPattern);
      //saveSequenceJSON(i, sequenceArray[i].pattern);
  //  }
    //Serial.println("=*-.-*= Pattern " + String(currentPattern) + " saved. =*-.-*= ");
	//}

  if (instant || !playing) {
    //Serial.println("Changing pattern instantly: " + String(pattern) + " instant: " + String(instant) + " playing: " + String(playing) );
    loadPattern(pattern, channelSelector);
  } else {
    queuePattern = pattern;
    //Serial.println("Queueing pattern: " + String(pattern));
  }
}
