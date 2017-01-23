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

void FlashMemory::initialize(Sequencer *sequenceArray, SerialFlashChip *spiFlash, ADC *adc){
  Serial.println("<</~*^*~\>>Initializing Flash Memory<</~*^*~\>>");

  this->sequenceArray = sequenceArray;
  this->spiFlash = spiFlash;
  this->adc = adc;

//  wipeEEPROM();
  if(!spiFlash->begin(WINBOND_CS_PIN)){
    Serial.println("SPI FLASH CHIP INITIALIZATION FAILED!");
  }
  spiFlashBusy = 0;
  cacheWriteBusy = 0;
  saveSequenceBusy = 0;

  Serial.println("<</~*^*~\>> Flash Memory Initialization Complete <</~*^*~\>>");
}

void FlashMemory::wipeEEPROM(){
  for (int i=0; i<2048; i++){
    Serial.print("writing EEProm: " + String(i));
    EEPROM.update(i, 0);
    Serial.println("\t" + String(EEPROM.read(i) ));
  }
  delay(1000);
}

void FlashMemory::initializeCache(){
  char *cacheFileName = (char *) malloc(sizeof(char) * 12);
  strcpy(cacheFileName, "cache.txt");
  char* fileName = (char *) malloc(sizeof(char) * 12);
  strcpy(fileName, "data.txt");

  if (!spiFlash->exists(cacheFileName)) {
    Serial.println("Creating Cache File: " + String(cacheFileName) + "\tsize: " + String(FLASHFILESIZE));
    spiFlash->createErasable(cacheFileName, FLASHFILESIZE);
  }
  if (!spiFlash->exists(fileName)) {
    Serial.println("Creating Save File: " + String(cacheFileName) );
    spiFlash->createErasable(fileName, FLASHFILESIZE);
  }
  free(cacheFileName);
  free(fileName);

  //Randomize a cache offset so that EEPROM is used evenly across all bytes
  //EEPROM will begin to degrade after 100000 writes, so this will increase longevity of the device.
  //Identify previous cache offset value:
  cacheNum = 0;
  cacheOffset = EEPROMReadlong(0);
  spiFlashBusy = false;
  randomSeed(adc->analogRead(A7, ADC_1) + micros());
  uint16_t newCacheOffset = random(2,2000);
  Serial.println("*&^%*&^%*&^% Cache Offset:\t" + String(cacheOffset));
  Serial.println("*&^%*&^%*&^% new Cache Offset:\t" + String(newCacheOffset));

  delay(1000);
  for(int i=0; i<CACHE_COUNT; i++){
    if (getCacheStatus(cacheOffset, i) != CACHE_READY){
      //copy any cache info over if cache still needs to be written
      Serial.println("!@#$%^&*(*&^%$#@!@#$%^&*()*&^%$#@!@#$%^&*()(*&^%$#@!@#$%^&*(*&^%$#@");
      Serial.println("CACHE STILL NEEDS TO BE WRITTEN PT:" +String(getCachePattern(cacheOffset, i)) + "\tCH:"+ String(getCacheChannel(cacheOffset, i)) );
      Serial.println("!@#$%^&*(*&^%$#@!@#$%^&*()*&^%$#@!@#$%^&*()(*&^%$#@!@#$%^&*(*&^%$#@");
      setCacheChannel(newCacheOffset, i, getCacheChannel(cacheOffset, i));
      setCachePattern(newCacheOffset, i, getCachePattern(cacheOffset, i));
      setCacheStatus( newCacheOffset, i, getCacheStatus(cacheOffset, i));
      delay(2000);
    }
  };
  cacheOffset = newCacheOffset;
  EEPROMWrite16(0, cacheOffset); //write the new offset to EEPROM so it will persist to the next reboot.

  listFiles();
}

int FlashMemory::findFreeCache(){
    for (int i=0; i<8; i++){
      if (getCacheStatus(cacheOffset, i) == CACHE_READY){
        return i;
      }
    }
    //Serial.println("-+-+-+-= NO FREE CACHE FOUND! =-+-+-+-");
    return 255; //error - CACHE_UNAVAILABLE
}

void FlashMemory::setCacheChannel(uint16_t _cacheOffset, uint8_t cacheIndex, uint8_t channel){
  EEPROM.update(cacheIndex+_cacheOffset+CACHE_COUNT, channel);
};

void FlashMemory::setCachePattern(uint16_t _cacheOffset, uint8_t cacheIndex, uint8_t pattern){
  EEPROM.update(cacheIndex+_cacheOffset+2*CACHE_COUNT, pattern);
};

void FlashMemory::setCacheStatus(uint16_t _cacheOffset, uint8_t cacheIndex, uint8_t status){

  //Serial.println("Setting Cache Status:\t" + String(cacheIndex) + "\tch: " + String(getCacheChannel(_cacheOffset, cacheIndex)) +"\tpa: "+ String(getCachePattern(_cacheOffset, cacheIndex)) + "\tstatus: " + String(getCacheStatus(_cacheOffset, cacheIndex)) + "\tms: " + String(millis()) );

  cacheStatus[cacheIndex] = status;
  if (status == FILE_COPYING || status == FILE_ERASING){
    EEPROM.update(cacheIndex+_cacheOffset, CACHE_WRITING);
    // If device is reboot during save, must go back to before FILE_ERASING
  } else {
    EEPROM.update(cacheIndex+_cacheOffset, CACHE_READY);
  }
};

int  FlashMemory::getCacheChannel(uint16_t _cacheOffset, uint8_t cacheIndex){
  return  EEPROM.read(cacheIndex+_cacheOffset+CACHE_COUNT);
};

int  FlashMemory::getCachePattern(uint16_t _cacheOffset, uint8_t cacheIndex){
  return  EEPROM.read(cacheIndex+_cacheOffset+2*CACHE_COUNT);
};

int  FlashMemory::getCacheStatus(uint16_t _cacheOffset, uint8_t cacheIndex){
  if (cacheStatus[cacheIndex] == 0){
    uint8_t status = EEPROM.read(cacheIndex+_cacheOffset);
    //Serial.println("checking EEPROM: " + String(status));
    return  status; // if 0, check EEPROM;
  } else {
  //  Serial.println("cache " + String(cacheIndex)+ " status: " + String(cacheStatus[cacheIndex]));
    return cacheStatus[cacheIndex];
  }
};

void FlashMemory::saveSequenceJSON(uint8_t channel, uint8_t pattern){
  //http://stackoverflow.com/questions/15179996/how-should-i-allocate-memory-for-c-string-char-array

  uint8_t selectedCacheIndex = findFreeCache();
  while (selectedCacheIndex == CACHE_UNAVAILABLE){  // probably need some kind of timeout here
    cacheWriteLoop();
    selectedCacheIndex = findFreeCache();
  }
  saveSequenceBusy = 1;

  setCachePattern(cacheOffset, selectedCacheIndex, pattern);
  setCacheChannel(cacheOffset, selectedCacheIndex, channel);
  setCacheStatus(cacheOffset, selectedCacheIndex, CACHE_SET);

  char* cacheFileName = (char *) malloc(sizeof(char) * 12);
  strcpy(cacheFileName, "cache.txt");

  char * fileBuffer = (char*)malloc(SAVEBLOCKSIZE);
  bool validation = 0;

  serialize(fileBuffer,  channel, pattern);

  while(!(spiFlash->ready()|| !spiFlashBusy) ){
    delay(1);
  }
  spiFlashBusy = true;
  file = spiFlash->open(cacheFileName);
  if (file){
    file.seek(getSaveAddress(channel, pattern));
    file.write(fileBuffer,SAVEBLOCKSIZE);
    file.read(fileBuffer,SAVEBLOCKSIZE);

    setCacheStatus(cacheOffset, selectedCacheIndex, CACHE_WRITING);
    file.close();
  } else {
      Serial.println("##############");
      Serial.println("##############");
      Serial.println("##############");
      Serial.println("##############");
      Serial.println("##############");
      Serial.println("##############");
      Serial.println("UNABLE TO OPEN SAVE FILE: "  + String(cacheFileName));
      Serial.println("##############");
      Serial.println("##############");
      Serial.println("##############");
      Serial.println("##############");
      Serial.println("##############");
      Serial.println("##############");
  }


  free(fileBuffer);
  free(cacheFileName);
  saveSequenceBusy = 0;
  spiFlashBusy = 0;
  cacheWriteSwitch = 1;
  cacheWriteTotalTimer = 0;

}

void FlashMemory::serialize(char* fileBuffer, uint8_t channel, uint8_t pattern){
  StaticJsonBuffer<16384> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  root["stepCount"]     = sequenceArray[channel].stepCount;
  root["beatCount"]     = sequenceArray[channel].beatCount;
  root["quantizeKey"]   = sequenceArray[channel].quantizeKey;
  root["quantizeScale"] = sequenceArray[channel].quantizeScale;
  root["channel"]       = sequenceArray[channel].channel;
  root["pattern"]       = sequenceArray[channel].pattern;
  JsonArray& stepDataArray = root.createNestedArray("stepData");

  for (int i=0; i< root["stepCount"]; i++){
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
  root.printTo(fileBuffer,SAVEBLOCKSIZE);
}

bool FlashMemory::validateJson(char* fileBuffer){
  StaticJsonBuffer<16384> jsonBuffer;
  JsonObject& root = jsonBuffer.parse(fileBuffer);
  return root.success();
}

void FlashMemory::cacheWriteLoop(){
  if(cacheWriteBusy || saveSequenceBusy){
    return;
  }
  cacheWriteBusy = true;
  cacheNum = (cacheNum+1)%CACHE_COUNT;
    uint8_t channel = getCacheChannel(cacheOffset, cacheNum);
    uint8_t pattern = getCachePattern(cacheOffset, cacheNum);
    if( (getCacheStatus(cacheOffset, cacheNum) >= CACHE_SET)
         && cacheWriteTimer > CACHE_WRITE_DELAY   ){
        //Serial.print("A+");
        // Serial.println("Running Cache Write Loop:\t" + String(cacheNum) + "\tch: " + String(channel) +"\tpa: "+ String(getCachePattern(cacheOffset, cacheNum)) + "\tstatus: " + String(getCacheStatus(cacheOffset, cacheNum)) + "\tms: " + String(millis()) );
        char* fileName = (char *) malloc(sizeof(char) * 12);
        char* cacheFileName = (char *) malloc(sizeof(char) * 12);
        strcpy(cacheFileName, "cache.txt");
        strcpy(fileName, "data.txt");

        switch (getCacheStatus(cacheOffset, cacheNum) ){

          case CACHE_SET:
            if(spiFlashBusy){break;};
            if (spiFlash->ready()){
               spiFlashBusy = true;
              file = spiFlash->open(cacheFileName);
              if (file){
                char * fileBuffer = (char*)malloc(SAVEBLOCKSIZE);
                file.seek(getSaveAddress(channel, pattern));
                serialize(fileBuffer,  channel, pattern);
                file.write(fileBuffer,SAVEBLOCKSIZE);
                setCacheStatus(cacheOffset, cacheNum, CACHE_WRITING);
                free(fileBuffer);
              } else {
                Serial.print("unable to open file: "  + String(cacheFileName));
              }
              file.close();
              spiFlashBusy = false;
            }

          break;
          case CACHE_WRITING: //clear the file where cache will be written to
            if(spiFlashBusy){break;};
            if (spiFlash->ready()){
              spiFlashBusy = true;

            //  Serial.println(":::{{{--->>><<<---}}}:::CACHELOOP " + String(cacheNum) + " :::{{{--->>><<<---}}}::: ERASING DESTINATION - " + String(fileName) + " CH: " + String(channel) + "\tpat:" + String(pattern) +" timer:" + String(cacheWriteTotalTimer));
              file = spiFlash->open(fileName);   //open cache file
              if(file){
                file.seek(getSaveAddress(channel, pattern)); //seek to address
                file.erase4k();
                setCacheStatus(cacheOffset, cacheNum, FILE_ERASING);
                cacheWriteTimer = 0;
              } else {
                Serial.println(":::{{{--->>><<<---}}}:::CACHELOOP " + String(cacheNum) + " :::{{{--->>><<<---}}}::: FILE ERASE ERROR - " + String(fileName));
              }
              file.close();
              spiFlashBusy = false;
            }
          break;

          case FILE_ERASING: //write cache to cleared file location
            if(spiFlashBusy){break;};
            if (spiFlash->ready()){
               spiFlashBusy = true;

            //  Serial.println(":::{{{--->>><<<---}}}:::CACHELOOP " + String(cacheNum) + " :::{{{--->>><<<---}}}::: WRITING CACHE - CH: " + String(channel));
              char* fileBuffer = (char*)malloc(SAVEBLOCKSIZE);  // Allocate memory
              file = spiFlash->open(cacheFileName); // open cache file
              if(file){
                file.seek(getSaveAddress(channel, pattern));
                file.read(fileBuffer, SAVEBLOCKSIZE); // fill buffer with cache file
                file.close();    // close cache file
              } else { Serial.println(":::{{{--->>><<<---}}}:::CACHELOOP " + String(cacheNum) + " :::{{{--->>><<<---}}}::: CACHE READ ERROR - CH: "+ String(channel)); }
              file.close();    // close cache file

              // write to save file
              file = spiFlash->open(fileName);   //open save file
              if (file){
                file.seek(getSaveAddress(channel, pattern)); //seek to address
                file.write(fileBuffer, SAVEBLOCKSIZE);
                cacheWriteTimer = 0;
                Serial.print("Writing CH:" + String(channel) + "\tPT:" + String(pattern) + "\t" );
                Serial.println(fileBuffer);
              } else { Serial.println(":::{{{--->>><<<---}}}:::CACHELOOP " + String(cacheNum) + " :::{{{--->>><<<---}}}::: CACHE WRITE ERROR - CH: "+ String(channel)); }
              file.close();    // close cache file
              setCacheStatus(cacheOffset, cacheNum, FILE_COPYING);
              free(fileBuffer);
              spiFlashBusy = false;

            }
          break;

          case FILE_COPYING:
            if(spiFlashBusy){break;};
            if (spiFlash->ready()){
               spiFlashBusy = true;

            //  Serial.println(":::{{{--->>><<<---}}}:::CACHELOOP " + String(cacheNum) + " :::{{{--->>><<<---}}}::: CLEARING CACHE - CH: " + String(channel)  );
              file = spiFlash->open(cacheFileName); // open cache file
              if (file) {
                file.seek(getSaveAddress(channel, pattern));
                file.erase4k();
                cacheWriteTimer = 0;
                setCacheStatus(cacheOffset, cacheNum, CACHE_ERASING);
              } else { Serial.println(":::{{{--->>><<<---}}}:::CACHELOOP " + String(cacheNum) + " :::{{{--->>><<<---}}}::: CACHE CLEAR ERROR - CH: "+ String(channel));}

              file.close();    // close cache file
          //    Serial.println("@@@@@@--***%%***--@@@@@@ ^ __ ^ <<<???>>> FILE SAVED SUCCESSFULLY: " + String(fileName) + "<<<???>>>  ^ __ ^  @@@@@@--***%%***--@@@@@@ \t\tPT:" + String(pattern)+ "\tCH: " + String(channel)  +" timer:" + String(cacheWriteTotalTimer));
            spiFlashBusy = false;
            }
          break;

          case CACHE_ERASING:
            if(spiFlashBusy){break;};
            if (spiFlash->ready()){
              spiFlashBusy = true;
              setCacheStatus(cacheOffset, cacheNum, CACHE_READY);
              spiFlashBusy = false;
            }
          break;
        };

        free(fileName);
        free(cacheFileName);
      }


    cacheWriteBusy = false;

    for(int _cacheNum = 0; _cacheNum < CACHE_COUNT; _cacheNum++){
      if (getCacheStatus(cacheOffset, _cacheNum) != CACHE_READY) {
        break;
      }
      if(cacheWriteSwitch){
        Serial.println("SAVE OPERATION COMPLETE " + String(cacheWriteTotalTimer));
      }
      cacheWriteSwitch = 0;
    }

}

int FlashMemory::getSaveAddress(uint8_t channel, uint8_t pattern){
  return ((pattern*4)+channel)*SAVEBLOCKSIZE;
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
      strcpy(fileName, String("data.txt").c_str());
      Serial.println("*&*&*&*&*&*&*&*&* SPI FLASH FILE " + String(fileName) + " LOAD START *&*&*&*&*&*&*&*&*&");


      if (spiFlash->exists(fileName)){

        Serial.println("Save file exists... attempting load");
        file = spiFlash->open(fileName);
        if (file){
          Serial.println("File opened...");
          //unsigned int fileSize = file.size();  // Get the file size.
          char* fileBuffer = (char*)malloc(SAVEBLOCKSIZE);  // Allocate memory for the file and a terminating null char.
          Serial.println(String(SAVEBLOCKSIZE) + " bytes allocated");
          file.seek(getSaveAddress(channel, pattern));
          Serial.println("file seeked to; " + String(getSaveAddress(channel, pattern)));
          file.read(fileBuffer, SAVEBLOCKSIZE);
          Serial.println("file read");
        //  fileBuffer[SAVEBLOCKSIZE] = '\0';               // Add the terminating null char.
          Serial.println(fileBuffer);                // Print the file to the serial monitor.
          if(this->deserialize(channel, fileBuffer) ){
            Serial.println("file deserialized.");
          } else {
            Serial.println("?*?*?*?*?*?*?*?*?*?*?*?*?*?*?*?*?*?*?*?*? FILE LOADED BUT DATA CANNOT BE READ");
          };
          file.close();
          free(fileBuffer);
        } else {
          Serial.println("*&*&*&*&*&*&*&*&* Error, save file exists but cannot open - " + String(fileName) + "*&*&*&*&*&*&*&*&*&");
          free(fileName);
          return READ_JSON_ERROR;
        };

        free(fileName);
        Serial.println("*&*&*&*&*&*&*&*&* SPI FLASH FILE LOAD END *&*&*&*&*&*&*&*&*&");
        return FILE_EXISTS;
      } else {
        free(fileName);
        Serial.println("*&*&*&*&*&*&*&*&* SPI FLASH FILE DOES NOT EXIST *&*&*&*&*&*&*&*&*&");
        return SAVEFILE_DOES_NOT_EXIST;
      }
      free(fileName);

}

bool FlashMemory::deserialize(uint8_t channel, char* json){
  StaticJsonBuffer<16384> jsonBuffer;

  Serial.println("jsonBuffer allocated");
   JsonObject& jsonReader = jsonBuffer.parseObject(json);
   Serial.println("Json Reader Success: " + String(jsonReader.success())) ;

   Serial.println("JSON object Parsed");
   sequenceArray[channel].stepCount    = jsonReader["stepCount"];
   sequenceArray[channel].beatCount    = jsonReader["beatCount"];
   sequenceArray[channel].quantizeKey  = jsonReader["quantizeKey"];
   sequenceArray[channel].quantizeScale = jsonReader["quantizeScale"];
   sequenceArray[channel].channel      = jsonReader["channel"];
   sequenceArray[channel].pattern = jsonReader["pattern"].as<uint8_t>();
   Serial.print("READING IN PATTERN: " + String(sequenceArray[channel].pattern) + " array: "); Serial.println((const char *)jsonReader["pattern"]);
   JsonArray& stepDataArray = jsonReader["stepData"];
   Serial.println("Step Data Array Success: " + String(stepDataArray.success())) ;

    Serial.println("jsonArray declared");

    for (int i=0; i< jsonReader["stepCount"]; i++){
     if (i != int(stepDataArray[i]["i"]) ) {
       Serial.println("Step Data Index Mismatch Error");
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
    stepDataBuf.noteStatus   =  NOTPLAYING_NOTQUEUED;
    stepDataBuf.notePlaying  =   0;
    stepDataBuf.stepTimer    =   0;
    sequenceArray[channel].stepData[n] = stepDataBuf;

  }

  return jsonReader.success();
}

void FlashMemory::loadPattern(uint8_t pattern, uint8_t channelSelector) {
  for (int i = 0; i<CACHE_COUNT; i++){
    while( getCachePattern(cacheOffset, i) == pattern && getCacheStatus(cacheOffset, i) != 0 ) {
      Serial.println("Delaying because pattern " + String(pattern) + " has pending save operation");
      cacheWriteLoop();
    }
  }

  Serial.println("[[-]]><{{{--}}}><[[-]] LOADING PATTERN: " + String(pattern) + " [[-]]><{{{--}}}><[[-]]");
//  printPattern();

	for(int i=0; i < SEQUENCECOUNT; i++){

    if ( !(channelSelector & (1 << i) ) ){
      Serial.println("skipping loading channel " + String(i));
      continue; // if channel is not selected to be loaded, don't load the channel!
    } else {
      Serial.println("Loading channel " + String(i));
    };

    Serial.println("About to run readJsonReturn for channel: " + String(i));

    int readJsonReturn = this->readSequenceJSON(i, pattern);

    if ( readJsonReturn == SAVEFILE_DOES_NOT_EXIST )  {
      Serial.println("saveData not available, initializing sequence");
      sequenceArray[i].initNewSequence(pattern, i);
      Serial.println("sequence initialized, saving sequence to JSON");
      //saveSequenceJSON(i, pattern);

      Serial.println("Sequence successfully saved to JSON");

    } else if (readJsonReturn == 2) {
      Serial.println("READ JSON ERROR - info above");
    }

    Serial.println("reading complete!");

  }

  Serial.println("changing current pattern from " + String(currentPattern) + " to " + String(pattern) + " and also this is queuePattern: " + String(queuePattern));
  currentPattern = pattern;
  Serial.println("[[-]]><{{{--}}}><[[-]] PATTERN: " + String(pattern) + " LOAD COMPLETE [[-]]><{{{--}}}><[[-]]");
  printPattern();
}

void FlashMemory::changePattern(uint8_t pattern, uint8_t channelSelector, boolean saveFirst, boolean instant){
	//Serial.println("currentPattern: " + String(currentPattern) + "\tSEQUENCECOUNT: " + String(SEQUENCECOUNT));
	if(saveFirst){
    for(int i=0; i < SEQUENCECOUNT; i++){
  		//saveChannelPattern(i);
      saveSequenceJSON(i, currentPattern);
    }
    Serial.println("=*-.-*= Pattern " + String(currentPattern) + " saved. =*-.-*= ");
	}

  if (instant || !playing) {
    Serial.println("Changing pattern instantly: " + String(pattern) + " instant: " + String(instant) + " playing: " + String(playing) );
    loadPattern(pattern, channelSelector);
  } else {
    queuePattern = pattern;
    Serial.println("Queueing pattern: " + String(pattern));
  }
}

void FlashMemory::deleteSaveFile(){
  //this->deleteAllFiles();
//  for(int i=0; i<16; i++){
//    for(int n=0; n<SEQUENCECOUNT; n++){
//      sequenceArray[n].initNewSequence(i, n);
//    }
//  }
//  loadPattern(0, 0b1111);
unsigned char id[5];
spiFlash->readID(id);
unsigned long size = spiFlash->capacity(id);
unsigned long startMillis = millis();

if (size > 0) {
    Serial.print("Flash Memory has ");
    Serial.print(size);
    Serial.println(" bytes.");
    Serial.println("Erasing ALL Flash Memory:");
    Serial.println("  Takes 40-60 seconds");
    spiFlash->eraseAll();
    unsigned long dotMillis = millis();
    unsigned char dotcount = 0;
    while (SerialFlash.ready() == false) {
      if (millis() - dotMillis > 1000) {
        dotMillis = dotMillis + 1000;
        Serial.print(".");
        dotcount = dotcount + 1;
        if (dotcount >= 60) {
          Serial.println();
          dotcount = 0;
        }
      }
    }
    if (dotcount > 0) Serial.println();
    Serial.println("Erase completed");
    unsigned long elapsed = millis() - startMillis;
    Serial.print("  actual wait: ");
    Serial.print(elapsed / 1000ul);
    Serial.println(" seconds.");
  }
  delay(1000);
}



void FlashMemory::printDirectory(File dir, int numTabs) {
   while(true) {
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());

     String temp = String(entry.name());

     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }

     entry.close();

   }
}

void FlashMemory::printPattern(){
  Serial.println("Printing Data for pattern: " + String(currentPattern));
  for(int i=0; i < SEQUENCECOUNT; i++){
    Serial.print("sc:\t"+String(sequenceArray[i].stepCount) +"\tbc:\t"+String(sequenceArray[i].beatCount) +"\tqk:\t"+String(sequenceArray[i].quantizeKey)+"\t");
    for(int n=0; n<16; n++){
      Serial.print( String(sequenceArray[i].stepData[n].pitch[0]) + "\t" );
      //Serial.print( String(sequenceArray[i].stepData[n].pitch[1]) + "\t" );
      //Serial.print( String(sequenceArray[i].stepData[n].pitch[2]) + "\t" );
      //Serial.print( String(sequenceArray[i].stepData[n].pitch[3]) + "\t" );
    }
    Serial.println("");
  }
}

void FlashMemory::deleteAllFiles(){
 //File root= SD.open("/");
 //this->rm(root, "/");
 //root.close();

}


void FlashMemory::listFiles(){
  spiFlash->opendir();
  while (1) {
    char filename[64];
    uint32_t filesize;

    if (spiFlash->readdir(filename, sizeof(filename), filesize)) {
      Serial.print("  ");
      Serial.print(filename);
      for (int i=0; i < (20 - strlen(filename)); i++) {
        Serial.print(" ");
      }
      Serial.print("  ");
      Serial.print(filesize);
      Serial.print(" bytes");
      Serial.println();
      for(int i=0; i<64; i++){
        char* fileBuffer = (char*)malloc(SAVEBLOCKSIZE);  // Allocate memory for the file and a terminating null char.
        file.seek(SAVEBLOCKSIZE*i);
        file.read(fileBuffer, SAVEBLOCKSIZE);
        fileBuffer[SAVEBLOCKSIZE] = '\0';               // Add the terminating null char.
        Serial.println(String(i) + ":\t" + String(fileBuffer));                // Print the file to the serial monitor.
        free(fileBuffer);
      }

    } else {
      break; // no more files
    }
  }
  for(int i=0; i<CACHE_COUNT; i++){
    Serial.print(String(i) + "\t");
    Serial.print("pt:" + String(getCachePattern(cacheOffset, i)) + "\t");
    Serial.print("ch:" + String(getCacheChannel(cacheOffset, i)) + "\t");
    Serial.print("st:" + String(getCacheStatus(cacheOffset, i))  + "\t");
    Serial.println(" ");
  }
}


void FlashMemory::rm(File dir, String tempPath) {
  int DeletedCount = 0;
  int FolderDeleteCount = 0;
  int FailCount = 0;
  String rootpath = "/";

  while(true) {
    File entry =  dir.openNextFile();
    String localPath;

    Serial.println("");
    if (entry) {
      if ( entry.isDirectory() )
      {
        localPath = tempPath + entry.name() + rootpath + '\0';
        char folderBuf[localPath.length()];
        localPath.toCharArray(folderBuf, localPath.length() );
        rm(entry, folderBuf);


        if( SD.rmdir( folderBuf ) )
        {
          Serial.print("Deleted folder ");
          Serial.println(folderBuf);
          FolderDeleteCount++;
        }
        else
        {
          Serial.print("Unable to delete folder ");
          Serial.println(folderBuf);
          FailCount++;
        }
      }
      else
      {
        localPath = tempPath + entry.name() + '\0';
        char charBuf[localPath.length()];
        localPath.toCharArray(charBuf, localPath.length() );

        if( SD.remove( charBuf ) )
        {
          Serial.print("Deleted ");
          Serial.println(localPath);
          DeletedCount++;
        }
        else
        {
          Serial.print("Failed to delete ");
          Serial.println(localPath);
          FailCount++;
        }

      }
    }
    else {
      // break out of recursion
      break;
    }
  }
}


//This function will write a 2 byte (16bit) int to the eeprom at
//the specified address to address + 1
void FlashMemory::EEPROMWrite16(int address, uint16_t value) {
  //Decomposition from a uint16_t to 2 bytes by using bitshift.
  //One = Most significant -> Two = Least significant byte
  byte two = (value & 0xFF);
  byte one = ((value >> 8) & 0xFF);
  //Write the 2 bytes into the eeprom memory.
  EEPROM.update(address, two);
  EEPROM.update(address + 1, one);
}

uint16_t FlashMemory::EEPROMReadlong(int address)  {
  //Read the 2 bytes from the eeprom memory.
  long two = EEPROM.read(address);
  long one = EEPROM.read(address + 1);

  //Return the recomposed long by using bitshift.
  return ((two << 0) & 0xFF) + ((one << 8) & 0xFFFF);
}
