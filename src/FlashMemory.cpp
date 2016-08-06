#include <Arduino.h>
#include "FlashMemory.h"
#define FILE_EXISTS 0
#define SAVEFILE_DOES_NOT_EXIST 1
#define READ_JSON_ERROR 2
/*
¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING!
This file contains code that enables saving and loading of patterns. Changing this file could result in an inability to read existing save files.
¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING!
*/
FlashMemory::FlashMemory(){};

void FlashMemory::initialize(Sequencer *sequenceArray){
  Serial.println("<<->> Initializing Flash Memory <<->>");

  this->sequenceArray = sequenceArray;

  if (!SD.begin(SD_CS_PIN)){
    Serial.println("SD Card initialization failed!");
    return;
  }

  //this->deleteAllFiles();

  Serial.println("<<->> Flash Memory Initialization Complete <<->>");
}

//void FlashMemory::saveSequenceJSON(Sequencer& sequence, char *pattern ){
void FlashMemory::saveSequenceJSON(uint8_t channel, uint8_t pattern){
  //http://stackoverflow.com/questions/15179996/how-should-i-allocate-memory-for-c-string-char-array
  Serial.println("saveSequenceJson FreeMemory 1 : " + String(FreeRam2()));

  StaticJsonBuffer<16384> jsonBuffer;

  char *fileNameChar = (char *) malloc(sizeof(char) * 12);
  strcpy(fileNameChar, "P");
  strcat(fileNameChar, String(pattern).c_str());
  strcat(fileNameChar, "CH");
  strcat(fileNameChar, String(channel).c_str());
  strcat(fileNameChar, ".TXT");

  elapsedMillis flashTimer = 0;

  // following ArduinoJSON serialize example: https://githtub.com/bblanchon/ArduinoJson/wiki/FAQ#whats-the-best-way-to-use-the-library
  JsonObject& root = jsonBuffer.createObject();

  root["stepCount"]     = sequenceArray[channel].stepCount;
  root["beatCount"]     = sequenceArray[channel].beatCount;
  root["quantizeKey"]   = sequenceArray[channel].quantizeKey;
  root["instrument"]    = sequenceArray[channel].instrument;
  root["instType"]      = sequenceArray[channel].instType;
  root["volume"]        = sequenceArray[channel].volume;
  root["bank"]          = sequenceArray[channel].bank;
  root["channel"]       = sequenceArray[channel].channel;
  root["patternIndex"]  = sequenceArray[channel].patternIndex;

  JsonArray& stepDataArray = root.createNestedArray("stepData");
  Serial.println("saveSequenceJson FreeMemory 2 : " + String(FreeRam2()));

  for (int i=0; i< root["stepCount"]; i++){
    JsonObject& stepDataObj = jsonBuffer.createObject();
    stepDataObj["i"] = i ;
    stepDataObj["p0"] = sequenceArray[channel].stepData[i].pitch[0];
    stepDataObj["p1"] = sequenceArray[channel].stepData[i].pitch[1];
    stepDataObj["p2"] = sequenceArray[channel].stepData[i].pitch[2];
    stepDataObj["p3"] = sequenceArray[channel].stepData[i].pitch[3];
    stepDataObj["gl"] = sequenceArray[channel].stepData[i].gateLength ;
    stepDataObj["gt"] = sequenceArray[channel].stepData[i].gateType ;
    stepDataObj["v"] = sequenceArray[channel].stepData[i].velocity ;
    stepDataObj["g"] = sequenceArray[channel].stepData[i].glide ;
    stepDataArray.add(stepDataObj);
  }
  Serial.println("saveSequenceJson FreeMemory 4 : " + String(FreeRam2()));

  if (SD.exists(fileNameChar)){
    SD.remove(fileNameChar);
  }
  Serial.println("opening file");
  jsonFile = SD.open(fileNameChar, FILE_WRITE);
  jsonFile.seek(0);
  Serial.println("printing to file handle");
  root.printTo(jsonFile);
  Serial.println("fileSize: " + String(jsonFile.size()));
  jsonFile.close();
  Serial.print("fileNameChar: ");
  Serial.println(fileNameChar);

  free(fileNameChar);
  Serial.println("saveSequenceJson FreeMemory 5 : " + String(FreeRam2()));

  Serial.println("flash Timer after write: " + String(flashTimer) );

//  Serial.println(String(jsonSaveTimer))

}

int FlashMemory::readSequenceJSON(uint8_t channel, uint8_t pattern){
    char* fileNameChar = (char *) malloc(sizeof(char) * 12);
    strcpy(fileNameChar, "P");
    strcat(fileNameChar, String(pattern).c_str());
    strcat(fileNameChar, "CH");
    strcat(fileNameChar, String(channel).c_str());
    strcat(fileNameChar,".TXT");

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

bool FlashMemory::deserialize(uint8_t channel, char* json){
  StaticJsonBuffer<16384> jsonBuffer;
  Serial.println("deserialize FreeRam 1: " + String(FreeRam2()));

    Serial.println("jsonBuffer allocated");
  JsonObject& jsonReader = jsonBuffer.parseObject(json);
Serial.println("Json Reader Success: " + String(jsonReader.success())) ;
    Serial.println("JSON object Parsed");
   sequenceArray[channel].stepCount    = jsonReader["stepCount"];
   sequenceArray[channel].beatCount    = jsonReader["beatCount"];
   sequenceArray[channel].quantizeKey  = jsonReader["quantizeKey"];
   sequenceArray[channel].instrument   = jsonReader["instrument"];
   sequenceArray[channel].instType     = jsonReader["instType"];
   sequenceArray[channel].volume       = jsonReader["volume"];
   sequenceArray[channel].bank         = jsonReader["bank"];
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
     stepDataBuf.pitch[0]  = stepDataArray[i]["p0"];
     stepDataBuf.pitch[1]  = stepDataArray[i]["p1"];
     stepDataBuf.pitch[2]  = stepDataArray[i]["p2"];
     stepDataBuf.pitch[3]  = stepDataArray[i]["p3"];
     stepDataBuf.gateLength  = stepDataArray[i]["gl"];
     stepDataBuf.gateType  = stepDataArray[i]["gt"];
     stepDataBuf.velocity  = stepDataArray[i]["v"];
     stepDataBuf.glide  = stepDataArray[i]["g"];
     sequenceArray[channel].stepData[i] = stepDataBuf;
  }

  return jsonReader.success();
}

void FlashMemory::loadPattern(uint8_t pattern, uint8_t channelSelector) {

  Serial.println("[[-]]><{{{--}}}><[[-]] LOADING PATTERN: " + String(pattern) + " [[-]]><{{{--}}}><[[-]]");
//  printPattern();

	for(int i=0; i < sequenceCount; i++){
    Serial.println("loadpattern FreeRam 1 : " + String(FreeRam2()));

    if ( !(channelSelector & (1 << i) ) ){
      Serial.println("skipping loading channel " + String(i));
      continue; // if channel is not selected to be loaded, don't load the channel!
    } else {
      Serial.println("Loading channel " + String(i));
    };

    Serial.println("About to run readJsonReturn for channel: " + String(i));
    Serial.println("loadpattern FreeRam 2: " + String(FreeRam2()));

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

    //sam2695.programChange(0, i, sequenceArray[i].instrument);
    //sam2695.setChannelVolume(i, sequenceArray[i].volume);

    sequenceArray[i].quantizeKey = 1;
  }

  Serial.println("changing current pattern from " + String(currentPattern) + " to " + String(pattern) + " and also this is queuePattern: " + String(queuePattern));
  currentPattern = pattern;
  Serial.println("[[-]]><{{{--}}}><[[-]] PATTERN: " + String(pattern) + " LOAD COMPLETE [[-]]><{{{--}}}><[[-]]");


//  printPattern();
}

void FlashMemory::changePattern(uint8_t pattern, uint8_t channelSelector, boolean saveFirst, boolean instant){
	//Serial.println("currentPattern: " + String(currentPattern) + "\tsequenceCount: " + String(sequenceCount));
	if(saveFirst){
    for(int i=0; i < sequenceCount; i++){
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
  for(int i=0; i<16; i++){
    for(int n=0; n<sequenceCount; n++){
      sequenceArray[n].initNewSequence(i, n);
    }
  }
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
  for(int i=0; i < sequenceCount; i++){
    Serial.print("sc:\t"+String(sequenceArray[i].stepCount) +"\tbc:\t"+String(sequenceArray[i].beatCount) +"\tqk:\t"+String(sequenceArray[i].quantizeKey)+"\tinst:\t"+String(sequenceArray[i].instrument)+"\tit:\t"+String(sequenceArray[i].instType) + "\t");
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
 File root= SD.open("/");
 this->rm(root, "/");
 root.close();

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
