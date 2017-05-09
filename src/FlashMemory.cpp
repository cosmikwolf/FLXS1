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

void FlashMemory::initialize(OutputController * outputControl, Sequencer *sequenceArray, SerialFlashChip *spiFlash, ADC *adc){
  debug("<</~*^*~\>>Initializing Flash Memory<</~*^*~\>>");
  this->outputControl = outputControl;
  this->sequenceArray = sequenceArray;
  this->spiFlash = spiFlash;
  this->adc = adc;

  if(!spiFlash->begin(WINBOND_CS_PIN)){
    debug("SPI FLASH CHIP INITIALIZATION FAILED!");
  }
  spiFlashBusy = 0;
  cacheWriteBusy = 0;
  saveSequenceBusy = 0;
  for(int n = 0; n < CACHE_COUNT; n++){
    cacheStatus[n] = 0;
  }
  debug("<</~*^*~\>> Flash Memory Initialization Complete <</~*^*~\>>");
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
    EEPROMWrite16(address, adcCalibrationPos[i]);
    address += 2;
  }
  for (int i = 0; i<4; i++){
    EEPROMWrite16(address, adcCalibrationNeg[i]);
    address += 2;
  }
  for (int i = 0; i<4; i++){
    EEPROMWrite16(address, adcCalibrationOffset[i]);
    address += 2;
  }
  for (int i = 0; i<8; i++){
    EEPROMWrite16(address, dacCalibrationNeg[i]);
    address += 2;
  }
  for (int i = 0; i<8; i++){
    EEPROMWrite16(address, dacCalibrationPos[i]);
    address += 2;
  }
  debug(" -- CALIBRATION EEPROM WRITE COMPLETE -- ");

};

void FlashMemory::readCalibrationEEPROM(){
  uint16_t address = 0;
  uint16_t checkVal;

  if(EEPROMRead16(address) == 0xFFFF){
    debug(" -- CALIBRATION HAS NOT BEEN PERFORMED -- ");
    debug(" -- CALIBRATION HAS NOT BEEN PERFORMED -- ");
    debug(" -- CALIBRATION HAS NOT BEEN PERFORMED -- ");
    debug(" -- CALIBRATION HAS NOT BEEN PERFORMED -- ");
    debug(" -- CALIBRATION HAS NOT BEEN PERFORMED -- ");
    debug(" -- CALIBRATION HAS NOT BEEN PERFORMED -- ");
    return;
  }
  address += 2;


  for (int i = 0; i<4; i++){
    adcCalibrationPos[i] = EEPROMRead16(address);
    address += 2;
  }
  for (int i = 0; i<4; i++){
    adcCalibrationNeg[i] = EEPROMRead16(address);
    address += 2;
  }
  for (int i = 0; i<4; i++){
    adcCalibrationOffset[i] = EEPROMRead16(address);
    address += 2;
  }
  for (int i = 0; i<8; i++){
    dacCalibrationNeg[i] = EEPROMRead16(address);
    address += 2;
  }
  for (int i = 0; i<8; i++){
    dacCalibrationPos[i] = EEPROMRead16(address);
    address += 2;
  }



};

uint8_t FlashMemory::getSaveSector(uint8_t channel, uint8_t pattern){
  return (pattern * 4 + channel) * SECTORSIZE;
};

void FlashMemory::saveSequenceJSON(uint8_t channel, uint8_t pattern){
  //http://stackoverflow.com/questions/15179996/how-should-i-allocate-memory-for-c-string-char-array

  uint8_t cacheIndex = getCacheIndex(channel, pattern);
  debug("Saving channel: " + String(channel) + "\tpattern: " + String(pattern) + "\tchannelIndex:" + String(cacheIndex) + "\tsaveAddress: " + String(getSaveAddress(cacheIndex)) );
  while (cacheStatus[cacheIndex] != 0){
    int tempint = cacheWriteLoop();
    debug("Cache is unavailable upon save attempt...  \tindex:" + String(cacheIndex) + "\tstatus: " + String(cacheStatus[cacheIndex] + " \tcacheWriteReturn: " + String(tempint)));
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
      debug("##############");
      debug("UNABLE TO OPEN SAVE FILE: "  + String(cacheFileName));
      debug("##############");
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

  seqSettingsArray.add(sequenceArray[channel].stepCount);
  seqSettingsArray.add(sequenceArray[channel].beatCount);
  seqSettingsArray.add(sequenceArray[channel].quantizeKey);
  seqSettingsArray.add(sequenceArray[channel].quantizeScale);
  seqSettingsArray.add(sequenceArray[channel].channel);
  seqSettingsArray.add(sequenceArray[channel].pattern);
  seqSettingsArray.add(sequenceArray[channel].clockDivision);
  seqSettingsArray.add(sequenceArray[channel].gpio_reset);
  seqSettingsArray.add(sequenceArray[channel].gpio_yaxis);
  seqSettingsArray.add(sequenceArray[channel].gpio_gateinvert);
  seqSettingsArray.add(sequenceArray[channel].gpio_randompitch);
  seqSettingsArray.add(sequenceArray[channel].cv_arptypemod);
  seqSettingsArray.add(sequenceArray[channel].cv_arpspdmod);
  seqSettingsArray.add(sequenceArray[channel].cv_arpoctmod);
  seqSettingsArray.add(sequenceArray[channel].cv_arpintmod);
  seqSettingsArray.add(sequenceArray[channel].cv_pitchmod);
  seqSettingsArray.add(sequenceArray[channel].cv_gatemod);
  seqSettingsArray.add(sequenceArray[channel].cv_glidemod);

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

  debug("jsonBuffer allocated");
   JsonObject& jsonReader = jsonBuffer.parseObject(json);
   debug("Json Reader Success: " + String(jsonReader.success())) ;

   debug("JSON object Parsed");
   sequenceArray[channel].stepCount    = jsonReader["settings"][0];
   sequenceArray[channel].beatCount    = jsonReader["settings"][1];
   sequenceArray[channel].quantizeKey  = jsonReader["settings"][2];
   sequenceArray[channel].quantizeScale= jsonReader["settings"][3];
   sequenceArray[channel].channel      = jsonReader["settings"][4];
   sequenceArray[channel].pattern      = jsonReader["settings"][5];
   sequenceArray[channel].clockDivision= jsonReader["settings"][6];
   sequenceArray[channel].gpio_reset   = jsonReader["settings"][7];
   sequenceArray[channel].gpio_yaxis   = jsonReader["settings"][8];
   sequenceArray[channel].gpio_gateinvert= jsonReader["settings"][9];
   sequenceArray[channel].gpio_randompitch = jsonReader["settings"][10];
   sequenceArray[channel].cv_arptypemod   = jsonReader["settings"][11];
   sequenceArray[channel].cv_arpspdmod = jsonReader["settings"][12];
   sequenceArray[channel].cv_arpoctmod = jsonReader["settings"][13];
   sequenceArray[channel].cv_arpintmod = jsonReader["settings"][14];
   sequenceArray[channel].cv_pitchmod  = jsonReader["settings"][15];
   sequenceArray[channel].cv_gatemod   = jsonReader["settings"][16];
   sequenceArray[channel].cv_glidemod  = jsonReader["settings"][17];
    //.as<uint8_t>();
   debug("READING IN PATTERN: " + String(sequenceArray[channel].pattern) + " array: "); debug((const char *)jsonReader["pattern"]);
   JsonArray& stepDataArray = jsonReader["data"];
   debug("Step Data Array Success: " + String(stepDataArray.success())) ;

    debug("jsonArray declared");

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

void FlashMemory::serializeHash(char* fileBuffer, uint8_t channel, uint8_t pattern){
  StaticJsonBuffer<16384> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  root["stepCount"]     = sequenceArray[channel].stepCount;
  root["beatCount"]     = sequenceArray[channel].beatCount;
  root["quantizeKey"]   = sequenceArray[channel].quantizeKey;
  root["quantizeScale"] = sequenceArray[channel].quantizeScale;
  root["channel"]       = sequenceArray[channel].channel;
  root["pattern"]       = sequenceArray[channel].pattern;
  JsonArray& stepDataArray = root.createNestedArray("stepData");

  //for (int i=0; i< root["stepCount"]; i++){
  for (int i=0; i< MAX_STEPS_PER_SEQUENCE; i++){
    JsonObject& stepDataObj = jsonBuffer.createObject();
    stepDataObj["i"] = i;
    stepDataObj["p0"] = sequenceArray[channel].stepData[i].pitch[0];
    stepDataObj["p1"] = sequenceArray[channel].stepData[i].pitch[1];
    stepDataObj["p2"] = sequenceArray[channel].stepData[i].pitch[2];
    stepDataObj["p3"] = sequenceArray[channel].stepData[i].pitch[3];
    stepDataObj["ch"] = sequenceArray[channel].stepData[i].chord;
    stepDataObj["bd"] = sequenceArray[channel].stepData[i].beatDiv;
    stepDataObj["gt"] = sequenceArray[channel].stepData[i].gateType;
    stepDataObj["gl"] = sequenceArray[channel].stepData[i].gateLength;
    stepDataObj["at"] = sequenceArray[channel].stepData[i].arpType;
    stepDataObj["ao"] = sequenceArray[channel].stepData[i].arpOctave;
    stepDataObj["an"] = sequenceArray[channel].stepData[i].arpSpdNum;
    stepDataObj["ad"] = sequenceArray[channel].stepData[i].arpSpdDen;
    stepDataObj["v"]  = sequenceArray[channel].stepData[i].velocity ;
    stepDataObj["vt"] = sequenceArray[channel].stepData[i].velocityType;
    stepDataObj["ls"] = sequenceArray[channel].stepData[i].lfoSpeed;
    stepDataObj["g"]  = sequenceArray[channel].stepData[i].glide;
    stepDataArray.add(stepDataObj);
  }
  root.printTo(fileBuffer,SECTORSIZE);
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
      debug("*&*&*&*&*&*&*&*&* SPI FLASH FILE " + String(fileName) + " LOAD START *&*&*&*&*&*&*&*&*&");


      if (spiFlash->exists(fileName)){
        debug("Save file exists... attempting load");
        file = spiFlash->open(fileName);
        if (file){
          debug("File opened...");
          //unsigned int fileSize = file.size();  // Get the file size.
          char* fileBuffer = (char*)malloc(SECTORSIZE);  // Allocate memory for the file and a terminating null char.
          debug(String(SECTORSIZE) + " bytes allocated");
          debug("SaveAddress: " + String(getSaveAddress(getCacheIndex(channel, pattern))));
          debug("cacheIndex: " + String(getCacheIndex(channel, pattern)));

          file.seek(getSaveAddress(getCacheIndex(channel, pattern)));
          file.read(fileBuffer, SECTORSIZE);
          debug("file read");
        //  fileBuffer[SECTORSIZE] = '\0';               // Add the terminating null char.
          debug(fileBuffer);                // Print the file to the serial monitor.
          if(this->deserialize(channel, fileBuffer) ){
            debug("file deserialized.");
          } else {
            debug("?*?*?*?*?*?*?*?*?*?*?*?*?*?*?*?*?*?*?*?*? FILE LOADED BUT DATA CANNOT BE READ");
          };
          file.close();
          free(fileBuffer);
        } else {
          debug("*&*&*&*&*&*&*&*&* Error, save file exists but cannot open - " + String(fileName) + "*&*&*&*&*&*&*&*&*&");
          free(fileName);
          return READ_JSON_ERROR;
        };

        free(fileName);
        debug("*&*&*&*&*&*&*&*&* SPI FLASH FILE LOAD END *&*&*&*&*&*&*&*&*&");
        return FILE_EXISTS;
      } else {
        free(fileName);
        debug("*&*&*&*&*&*&*&*&* SPI FLASH FILE DOES NOT EXIST *&*&*&*&*&*&*&*&*&");
        return SAVEFILE_DOES_NOT_EXIST;
      }
      free(fileName);

}

bool FlashMemory::deserializeHash(uint8_t channel, char* json){
  StaticJsonBuffer<16384> jsonBuffer;

  debug("jsonBuffer allocated");
   JsonObject& jsonReader = jsonBuffer.parseObject(json);
   debug("Json Reader Success: " + String(jsonReader.success())) ;

   debug("JSON object Parsed");
   sequenceArray[channel].stepCount    = jsonReader["stepCount"];
   sequenceArray[channel].beatCount    = jsonReader["beatCount"];
   sequenceArray[channel].quantizeKey  = jsonReader["quantizeKey"];
   sequenceArray[channel].quantizeScale = jsonReader["quantizeScale"];
   sequenceArray[channel].channel      = jsonReader["channel"];
   sequenceArray[channel].pattern = jsonReader["pattern"].as<uint8_t>();
   debug("READING IN PATTERN: " + String(sequenceArray[channel].pattern) + " array: "); debug((const char *)jsonReader["pattern"]);
   JsonArray& stepDataArray = jsonReader["stepData"];
   debug("Step Data Array Success: " + String(stepDataArray.success())) ;

    debug("jsonArray declared");

    for (int i=0; i< jsonReader["stepCount"]; i++){
     if (i != int(stepDataArray[i]["i"]) ) {
       debug("Step Data Index Mismatch Error");
     };
     StepDatum stepDataBuf;

     stepDataBuf.pitch[0]    = stepDataArray[i]["p0"];
     stepDataBuf.pitch[1]    = stepDataArray[i]["p1"];
     stepDataBuf.pitch[2]    = stepDataArray[i]["p2"];
     stepDataBuf.pitch[3]    = stepDataArray[i]["p3"];
     stepDataBuf.chord       = stepDataArray[i]["ch"];
     stepDataBuf.beatDiv     = stepDataArray[i]["bd"];
     stepDataBuf.gateType    = stepDataArray[i]["gt"];
     stepDataBuf.gateLength  = stepDataArray[i]["gl"];
     stepDataBuf.arpType     = stepDataArray[i]["at"];
     stepDataBuf.arpOctave   = stepDataArray[i]["ao"];
     stepDataBuf.arpSpdNum   = stepDataArray[i]["an"];
     stepDataBuf.arpSpdDen   = stepDataArray[i]["ad"];
     stepDataBuf.velocity    = stepDataArray[i]["v"] ;
     stepDataBuf.velocityType= stepDataArray[i]["vt"] ;
     stepDataBuf.lfoSpeed    = stepDataArray[i]["ls"] ;
     stepDataBuf.glide       = stepDataArray[i]["g"] ;

     sequenceArray[channel].stepData[i] = stepDataBuf;
  }

  for(int n = jsonReader["stepCount"]; n< MAX_STEPS_PER_SEQUENCE; n++){
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

  }

  return jsonReader.success();
}

void FlashMemory::loadPattern(uint8_t pattern, uint8_t channelSelector) {
  /*
  for (int i = 0; i<CACHE_COUNT; i++){
    while( getCachePattern(cacheOffset, i) == pattern && getCacheStatus(cacheOffset, i) != 0 ) {
      debug("Delaying because pattern " + String(pattern) + " has pending save operation");
      cacheWriteLoop();
      delay(500);
    }
  }
*/
  debug("[[-]]><{{{--}}}><[[-]] LOADING PATTERN: " + String(pattern) + " [[-]]><{{{--}}}><[[-]]");
//  printPattern();

	for(int i=0; i < SEQUENCECOUNT; i++){

    if ( !(channelSelector & (1 << i) ) ){
      debug("skipping loading channel " + String(i));
      continue; // if channel is not selected to be loaded, don't load the channel!
    } else {
      debug("Loading channel " + String(i));
    };

    debug("About to run readJsonReturn for channel: " + String(i));

    int readJsonReturn = this->readSequenceJSON(i, pattern);

    if ( readJsonReturn == SAVEFILE_DOES_NOT_EXIST )  {
      debug("saveData not available, initializing sequence");
      sequenceArray[i].initNewSequence(pattern, i);
      debug("sequence initialized, saving sequence to JSON");
      //saveSequenceJSON(i, pattern);

      debug("Sequence successfully saved to JSON");

    } else if (readJsonReturn == 2) {
      debug("READ JSON ERROR - info above");
    }

    debug("reading complete!");

  }

  debug("changing current pattern from " + String(currentPattern) + " to " + String(pattern) + " and also this is queuePattern: " + String(queuePattern));
  currentPattern = pattern;
  debug("[[-]]><{{{--}}}><[[-]] PATTERN: " + String(pattern) + " LOAD COMPLETE [[-]]><{{{--}}}><[[-]]");
  //printPattern();
}

void FlashMemory::changePattern(uint8_t pattern, uint8_t channelSelector, boolean saveFirst, boolean instant){
	//debug("currentPattern: " + String(currentPattern) + "\tSEQUENCECOUNT: " + String(SEQUENCECOUNT));
	if(saveFirst){
    for(int i=0; i < SEQUENCECOUNT; i++){
  		//saveChannelPattern(i);
      //saveSequenceJSON(i, currentPattern);
      saveSequenceJSON(i, sequenceArray[i].pattern);
    }
    debug("=*-.-*= Pattern " + String(currentPattern) + " saved. =*-.-*= ");
	}

  if (instant || !playing) {
    debug("Changing pattern instantly: " + String(pattern) + " instant: " + String(instant) + " playing: " + String(playing) );
    loadPattern(pattern, channelSelector);
  } else {
    queuePattern = pattern;
    debug("Queueing pattern: " + String(pattern));
  }
}
