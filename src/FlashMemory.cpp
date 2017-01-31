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
  cacheFileName = strdup("seqCache");

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
  //  file.read(fileBuffer,SAVEBLOCKSIZE);
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
      fileName = strdup(String("seqData").c_str());
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
  /*
  for (int i = 0; i<CACHE_COUNT; i++){
    while( getCachePattern(cacheOffset, i) == pattern && getCacheStatus(cacheOffset, i) != 0 ) {
      Serial.println("Delaying because pattern " + String(pattern) + " has pending save operation");
      cacheWriteLoop();
      delay(500);
    }
  }
*/
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
