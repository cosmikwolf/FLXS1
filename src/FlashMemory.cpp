#include <Arduino.h>
#include "FlashMemory.h"
#define FILE_EXISTS 0
#define SAVEFILE_DOES_NOT_EXIST 1
#define READ_JSON_ERROR 2
#define WINBOND_CS_PIN 22
#define USE_SPI_FLASH  1
#define FLASHFILESIZE  65536
#define SAVEBLOCKSIZE  4096
#define CACHE_WRITE_DELAY 100

//cacheWriteStatus values
#define AWAITING_FILE_ERASURE 0
#define FILE_ERASED 1
#define CACHE_WRITTEN 2
#define CACHE_ERASED 3

/*
¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING!
This file contains code that enables saving and loading of patterns. Changing this file could result in an inability to read existing save files.
¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING!
*/
FlashMemory::FlashMemory(){
  //SerialFlashPrint serialFlashPrint(&file);
};

void FlashMemory::initialize(Sequencer *sequenceArray, SerialFlashChip *spiFlash){
  Serial.println("<<->> Initializing Flash Memory <<->>");

  this->sequenceArray = sequenceArray;
  this->spiFlash = spiFlash;
  for(int channel = 0; channel < SEQUENCECOUNT; channel++){
    cacheWriteFileStatus[channel] = 0;
  }

  if(USE_SPI_FLASH){
    if(!spiFlash->begin(WINBOND_CS_PIN)){
      Serial.println("SPI FLASH CHIP INITIALIZATION FAILED!");
    }
  } else {
    if (!SD.begin(SD_CS_PIN)){
      Serial.println("SD Card initialization failed!");
      return;
    }
  }
  Serial.println("<<->> Flash Memory Initialization Complete <<->>");
}

void FlashMemory::saveSequenceJSON(uint8_t channel, uint8_t pattern){
  //http://stackoverflow.com/questions/15179996/how-should-i-allocate-memory-for-c-string-char-array

  StaticJsonBuffer<16384> jsonBuffer;

  char *cacheFileName = (char *) malloc(sizeof(char) * 12);
  strcpy(cacheFileName, "saveCache.txt");
  elapsedMillis flashTimer = 0;

  // following ArduinoJSON serialize example: https://githtub.com/bblanchon/ArduinoJson/wiki/FAQ#whats-the-best-way-to-use-the-library
  JsonObject& root = jsonBuffer.createObject();

  root["stepCount"]     = sequenceArray[channel].stepCount;
  root["beatCount"]     = sequenceArray[channel].beatCount;
  root["quantizeKey"]   = sequenceArray[channel].quantizeKey;
  root["quantizeScale"] = sequenceArray[channel].quantizeScale;
  root["channel"]       = sequenceArray[channel].channel;
  root["patternIndex"]  = sequenceArray[channel].patternIndex;

  JsonArray& stepDataArray = root.createNestedArray("stepData");

  for (int i=0; i< root["stepCount"]; i++){
    JsonObject& stepDataObj = jsonBuffer.createObject();
    stepDataObj["i"] = i ;
    stepDataObj["p0"] = sequenceArray[channel].stepData[i].pitch[0];
    stepDataObj["p1"] = sequenceArray[channel].stepData[i].pitch[1];
    stepDataObj["p2"] = sequenceArray[channel].stepData[i].pitch[2];
    stepDataObj["p3"] = sequenceArray[channel].stepData[i].pitch[3];
    stepDataObj["ch"] = sequenceArray[channel].stepData[i].chord ;
    stepDataObj["bd"] = sequenceArray[channel].stepData[i].beatDiv ;
    stepDataObj["gt"] = sequenceArray[channel].stepData[i].gateType ;
    stepDataObj["gl"] = sequenceArray[channel].stepData[i].gateLength ;
    stepDataObj["at"] = sequenceArray[channel].stepData[i].arpType;
    stepDataObj["ao"] = sequenceArray[channel].stepData[i].arpOctave;
    stepDataObj["an"] = sequenceArray[channel].stepData[i].arpSpdNum;
    stepDataObj["ad"] = sequenceArray[channel].stepData[i].arpSpdDen;
    stepDataObj["v"]  = sequenceArray[channel].stepData[i].velocity ;
    stepDataObj["vt"] = sequenceArray[channel].stepData[i].velocityType ;
    stepDataObj["ls"] = sequenceArray[channel].stepData[i].lfoSpeed;
    stepDataObj["g"]  = sequenceArray[channel].stepData[i].glide ;
    stepDataArray.add(stepDataObj);
  }
  Serial.println("==== &*&*&*&*&*&*&*&*&*&*& flash Timer before write: " + String(flashTimer) );

  if(USE_SPI_FLASH){
    Serial.println("Saving file to SPI flash chip: " + String(cacheFileName) + " timer: " + String(flashTimer));
    if (!spiFlash->exists(cacheFileName)) {
      Serial.println("Creating Cache File: " + String(cacheFileName));
      spiFlash->createErasable(cacheFileName, FLASHFILESIZE);
    } else {
      Serial.println("File exists: " + String(cacheFileName));
    }
    file = spiFlash->open(cacheFileName);
    if (file){
      char* charBuffer = (char*)malloc(SAVEBLOCKSIZE);  // Allocate memory for the file and a terminating null char.
      Serial.println("About to prepare... "  + String(flashTimer));
      serialFlashPrint->writePrepare(channel, SAVEBLOCKSIZE);
      Serial.println("About to print... "  + String(flashTimer));
      root.printTo(*serialFlashPrint);
      //root.printTo(charBuffer, SAVEBLOCKSIZE);
      //file.erase();
      //file.seek(0);
      //file.write(charBuffer, SAVEBLOCKSIZE);
      //file.close();
      Serial.println("About to complete... "  + String(flashTimer));

      serialFlashPrint->writeComplete();

      Serial.println("About to free... "  + String(flashTimer));

      free(charBuffer);

      Serial.println("Printed... " + String(flashTimer));
    } else {
      Serial.print("unable to open file: "  + String(cacheFileName));
    }
  } else {
    if (SD.exists(cacheFileName)){
      SD.remove(cacheFileName);
    }
    Serial.println("opening file");
    jsonFile = SD.open(cacheFileName, FILE_WRITE);
    jsonFile.seek(0);
    Serial.println("printing to file handle");
    root.printTo(jsonFile);
    Serial.println("fileSize: " + String(jsonFile.size()));
    jsonFile.close();
  }
  free(cacheFileName);

  cacheWritePattern[channel]=pattern;
  cacheWriteSwitch = 1;

  Serial.println("&*&*&*&*&*&*&*&*&*&*& flash Timer after write: " + String(flashTimer) );

  //Serial.println(String(jsonSaveTimer))

}
/*
AWAITING_FILE_ERASURE
FILE_ERASED
CACHE_WRITTEN
CACHE_ERASED
*/
void FlashMemory::cacheWriteLoop(){

  if(cacheWriteSwitch && cacheWriteTimer > CACHE_WRITE_DELAY){
    for(int channel = 0; channel < SEQUENCECOUNT; channel++){
      char* fileName = (char *) malloc(sizeof(char) * 12);
      String cacheFileName =  "saveCache.txt";
      strcpy(fileName, "P");
      strcat(fileName, String(cacheWritePattern[channel]).c_str());
      strcat(fileName,".TXT");
      if (!spiFlash->exists(fileName)) {
        Serial.println("Creating Save File: " + String(cacheFileName));
        spiFlash->createErasable(fileName, FLASHFILESIZE);
      }

      switch (cacheWriteFileStatus[channel]){
        case AWAITING_FILE_ERASURE: //clear the file where cache will be written to
          if (spiFlash->ready()){
            Serial.println("++ ERASING DESTINATION FILE - CH: " + String(channel));
            file = spiFlash->open(fileName);   //open save file
            if(file){
              file.seek(SAVEBLOCKSIZE*channel);          //seek to address
              file.erase4k();
              file.close();
              cacheWriteFileStatus[channel] = FILE_ERASED;
              cacheWriteTimer = 0;
            } else {
              Serial.println("FILE CLEAR ERROR");
              return;
            }
          }
        break;

        case FILE_ERASED: //write cache to cleared file location

          if (spiFlash->ready()){
            Serial.println("++ WRITING CACHE - CH: " + String(channel));

            char* fileBuffer = (char*)malloc(SAVEBLOCKSIZE);  // Allocate memory for the file and a terminating null char.
            // read cache file
            file = spiFlash->open(cacheFileName.c_str()); // open cache file
            if(file){
              file.seek(4096*channel);
              file.read(fileBuffer, SAVEBLOCKSIZE); // fill buffer with cache file
              file.close();    // close cache file
            } else { Serial.println("CACHE READ ERROR - CH: "+ String(channel)); return; }

            // write to save file
            file = spiFlash->open(fileName);   //open save file
            if (file ){
              file.seek(SAVEBLOCKSIZE*channel);          //seek to address
              file.write(fileBuffer, SAVEBLOCKSIZE);
              file.close();
              cacheWriteTimer = 0;
              cacheWriteFileStatus[channel] = CACHE_WRITTEN;
            } else { Serial.println("CACHE WRITE ERROR - CH: "+ String(channel)); return; }
            free(fileBuffer);
          }
        break;

        case CACHE_WRITTEN:
          if (spiFlash->ready()){
            Serial.println("++ CLEARING CACHE - CH: " + String(channel));

            file = spiFlash->open(cacheFileName.c_str()); // open cache file
            if (file) {
              file.seek(4096*channel);
              file.erase4k();
              file.close();    // close cache file
              cacheWriteFileStatus[channel] = CACHE_ERASED;
              cacheWriteTimer = 0;
            } else { Serial.println("CACHE CLEAR ERROR - CH: "+ String(channel)); return; }
          }
        break;
      };

      free(fileName);

    }
    for(int channel = 0; channel < SEQUENCECOUNT; channel++){
      if (  cacheWriteFileStatus[channel] != CACHE_ERASED) {
        return;
      }
    }
    // reset the write file status for each channel and write switch.
    Serial.println("Cache write operation complete, resetting cacheWriteStatus and CacheWriteSwitch");
    for(int channel = 0; channel < SEQUENCECOUNT; channel++){
      cacheWriteFileStatus[channel] = 0;
    }
    cacheWriteSwitch = 0;

  }
}

int FlashMemory::readSequenceJSON(uint8_t channel, uint8_t pattern){
    char* fileNameChar = (char *) malloc(sizeof(char) * 12);
    strcpy(fileNameChar, "P");
    strcat(fileNameChar, String(pattern).c_str());
  //  strcat(fileNameChar, "CH");
  //  strcat(fileNameChar, String(channel).c_str());
    strcat(fileNameChar,".TXT");

    if(USE_SPI_FLASH){
      Serial.println("*&*&*&*&*&*&*&*&* SPI FLASH FILE " + String(fileNameChar) + " LOAD START *&*&*&*&*&*&*&*&*&");

      if (spiFlash->exists(fileNameChar)){
        Serial.println("Save file exists... attempting load");
        file = spiFlash->open(fileNameChar);
        if (file){
          Serial.println("File opened...");
          unsigned int fileSize = file.size();  // Get the file size.
          char* fileBuffer = (char*)malloc(SAVEBLOCKSIZE + 10);  // Allocate memory for the file and a terminating null char.
          Serial.println(String(fileSize) + " bytes allocated");
          file.seek(4096*channel);
          Serial.println("file seeked");
          file.read(fileBuffer, SAVEBLOCKSIZE);
          Serial.println("file read");
          fileBuffer[SAVEBLOCKSIZE+ 10] = '\0';               // Add the terminating null char.
          Serial.println(fileBuffer);                // Print the file to the serial monitor.
          this->deserialize(channel, fileBuffer);
          Serial.println("file deserialized.");
          file.close();
          free(fileBuffer);
        } else {
          Serial.println("*&*&*&*&*&*&*&*&* Error, save file exists but cannot open - " + String(fileNameChar) + "*&*&*&*&*&*&*&*&*&");
          return READ_JSON_ERROR;
        };

        free(fileNameChar);
        Serial.println("*&*&*&*&*&*&*&*&* SPI FLASH FILE LOAD END *&*&*&*&*&*&*&*&*&");
        return FILE_EXISTS;
      } else {
        free(fileNameChar);
        Serial.println("*&*&*&*&*&*&*&*&* SPI FLASH FILE DOES NOT EXIST *&*&*&*&*&*&*&*&*&");
        return SAVEFILE_DOES_NOT_EXIST;
      }


    } else {
      if (!SD.exists(fileNameChar)){
        Serial.println("save file " + String(fileNameChar) + " does not exist.");
        free(fileNameChar);
        return SAVEFILE_DOES_NOT_EXIST;
      } else {
        Serial.println("reading json file: " + String(fileNameChar));
        jsonFile = SD.open(fileNameChar, FILE_READ);
        Serial.println(" opened json file");
        if (jsonFile) {
          unsigned int fileSize = jsonFile.size();  // Get the file size.
          Serial.println("charBuffer Filesize: " + String(fileSize));
          char* charBuffer = (char*)malloc(fileSize + 10);  // Allocate memory for the file and a terminating null char.
          jsonFile.read(charBuffer, fileSize);         // Read the file into the buffer.
          charBuffer[fileSize] = '\0';               // Add the terminating null char.
          Serial.println(charBuffer);                // Print the file to the serial monitor.
          Serial.println("begin deserialize");
          this->deserialize(channel, charBuffer);
          Serial.println("deserialize complete");
          jsonFile.close();                         // Close the file.
          Serial.println("json File Closed");
          free(charBuffer);
          Serial.println("charBuffer Freed");
        } else {
          Serial.println("jsonFile open error!!!");
        }
        Serial.println("after jsonFileOpened");
        // *** Use the buffer as needed here. ***
      }
      free(fileNameChar);
      Serial.println("JSON file read complete");

      return FILE_EXISTS;
    }
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
   sequenceArray[channel].patternIndex = jsonReader["patternIndex"];

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

  return jsonReader.success();
}

void FlashMemory::loadPattern(uint8_t pattern, uint8_t channelSelector) {

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
      saveSequenceJSON(i, pattern);

      Serial.println("Sequence successfully saved to JSON");

    } else if (readJsonReturn == 2) {
      Serial.println("READ JSON ERROR - info above");
    }

    Serial.println("reading complete!");

  }

  Serial.println("changing current pattern from " + String(currentPattern) + " to " + String(pattern) + " and also this is queuePattern: " + String(queuePattern));
  currentPattern = pattern;
  Serial.println("[[-]]><{{{--}}}><[[-]] PATTERN: " + String(pattern) + " LOAD COMPLETE [[-]]><{{{--}}}><[[-]]");


//  printPattern();
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
  this->deleteAllFiles();
//  for(int i=0; i<16; i++){
//    for(int n=0; n<SEQUENCECOUNT; n++){
//      sequenceArray[n].initNewSequence(i, n);
//    }
//  }
  loadPattern(0, 0b1111);
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
      Serial.print( String(sequenceArray[i].stepData[n].pitch[1]) + "\t" );
      Serial.print( String(sequenceArray[i].stepData[n].pitch[2]) + "\t" );
      Serial.print( String(sequenceArray[i].stepData[n].pitch[3]) + "\t" );
    }
    Serial.println("");
  }
}

void FlashMemory::deleteAllFiles(){
 //File root= SD.open("/");
 //this->rm(root, "/");
 //root.close();

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
