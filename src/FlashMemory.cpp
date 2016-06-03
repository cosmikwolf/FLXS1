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
FlashMemory saveFile;
FlashMemory::FlashMemory(){};

void FlashMemory::jsonTest(){
  /*
    if (SD.exists("json.txt")) {
    Serial.println("json.txt exists. deleting...");
    SD.remove("json.txt");
  };
  JsonObject& root = jsonBuffer.createObject();
  root["sensor"] = "gps";
  root["time"] = 1351824120;
  JsonArray& data = root.createNestedArray("data");
  data.add(double_with_n_digits(48.756080, 6));
  data.add(double_with_n_digits(2.302038, 6));

  jsonFile = SD.open("json.txt", FILE_WRITE);
  root.printTo(jsonFile);
  jsonFile.seek(0);
  jsonFile.close();

  Serial.println("Parsing json file!");
*/
StaticJsonBuffer<1024> jsonBuffer;

  char* charBuffer;                              // Declare a pointer to your buffer.
  jsonFile = SD.open("json.txt", FILE_READ);
  if (jsonFile)
  {
      unsigned int fileSize = jsonFile.size();  // Get the file size.
      charBuffer = (char*)malloc(fileSize + 1);  // Allocate memory for the file and a terminating null char.
      jsonFile.read(charBuffer, fileSize);         // Read the file into the buffer.
      charBuffer[fileSize] = '\0';               // Add the terminating null char.
      Serial.println(charBuffer);                // Print the file to the serial monitor.
      jsonFile.close();                         // Close the file.
  }
  // *** Use the buffer as needed here. ***
  Serial.println("Sizeof charBuffer: " + String(sizeof(charBuffer)));
  JsonObject& jsonReader = jsonBuffer.parseObject(charBuffer);
  free(charBuffer);                              // Free the memory that was used by the buffer.

  if (!jsonReader.success()) {
    Serial.println("parseObject() failed");
    return;
  }
  /*

  Serial.println("Printing Parsed Data:");
  const char* sensor = jsonReader["sensor"];
  long time = jsonReader["time"];
  double latitude = jsonReader["data"][0];
  double longitude = jsonReader["data"][1];

  // Print values.
  Serial.println(sensor);
  Serial.println(time);
  Serial.println(latitude, 6);
  Serial.println(longitude, 6);
*/
}

void FlashMemory::initialize(){

  if (!SD.begin(SD_CS_PIN)){
    Serial.println("SD Card initialization failed!");
    return;
  }
  // THIS IS THE LINE THAT DELETES THE DATAFILE EVERY TIME!
   //deleteSaveFile();
  if (SD.exists("data.txt")) {
    Serial.println("data.txt exists.");
  } else {
    Serial.println("data.txt does not exist, creating file...");
    saveData = SD.open("data.txt", FILE_WRITE);
    saveData.close();
  }

  Serial.println("SD Card and save file initialization complete.");

  loadPattern(0, 0b1111);

  Serial.println("Flash Memory Initialization Complete");
}

void FlashMemory::changePattern(uint8_t pattern, uint8_t channelSelector, boolean saveFirst, boolean instant){
	//Serial.println("currentPattern: " + String(currentPattern) + "\tsequenceCount: " + String(sequenceCount));
	if(saveFirst){
    for(int i=0; i < sequenceCount; i++){
  		//saveChannelPattern(i);
      saveSequenceJSON(&sequence[i], i, currentPattern);
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
      sequence[n].initNewSequence(i, n);
    }

  }
  loadPattern(0, 0b1111);
}

//void FlashMemory::saveSequenceJSON(Sequencer& sequence, char *pattern ){
void FlashMemory::saveSequenceJSON(Sequencer* sequence, uint8_t channel, uint8_t pattern){
  //http://stackoverflow.com/questions/15179996/how-should-i-allocate-memory-for-c-string-char-array
  StaticJsonBuffer<16384> jsonBuffer;
  char *fileNameChar = (char *) malloc(sizeof(char) * 12);
  strcpy(fileNameChar, "p");
  strcat(fileNameChar, String(pattern).c_str());
  strcat(fileNameChar, "ch");
  strcat(fileNameChar, String(channel).c_str());
  strcat(fileNameChar,".jsn");

  elapsedMillis flashTimer = 0;

  // following ArduinoJSON serialize example: https://githtub.com/bblanchon/ArduinoJson/wiki/FAQ#whats-the-best-way-to-use-the-library
  JsonObject& root = jsonBuffer.createObject();

  root["stepCount"]     = sequence->stepCount;
  root["beatCount"]     = sequence->beatCount;
  root["quantizeKey"]   = sequence->quantizeKey;
  root["instrument"]    = sequence->instrument;
  root["instType"]      = sequence->instType;
  root["volume"]        = sequence->volume;
  root["bank"]          = sequence->bank;
  root["channel"]       = sequence->channel;
  root["patternIndex"]  = sequence->patternIndex;

  JsonArray& stepDataArray = root.createNestedArray("stepData");

  for (int i=0; i< 128; i++){
    JsonObject& stepDataObj = jsonBuffer.createObject();
    stepDataObj["i"] = i ;
    stepDataObj["p"] = sequence->stepData[i].pitch ;
    stepDataObj["gl"] = sequence->stepData[i].gateLength ;
    stepDataObj["gt"] = sequence->stepData[i].gateType ;
    stepDataObj["v"] = sequence->stepData[i].velocity ;
    stepDataObj["g"] = sequence->stepData[i].glide ;
    stepDataArray.add(stepDataObj);
  }

  SD.remove(fileNameChar);
  jsonFile = SD.open(fileNameChar, FILE_WRITE);
  jsonFile.seek(0);
  root.printTo(jsonFile);
  Serial.println("fileSize: " + String(jsonFile.size()));
  jsonFile.close();
  Serial.print("fileNameChar: ");
  Serial.println(fileNameChar);

  free(fileNameChar);

  Serial.println("flash Timer after write: " + String(flashTimer) );

//  Serial.println(String(jsonSaveTimer))

}

int FlashMemory::readSequenceJSON(Sequencer* sequence, uint8_t channel, uint8_t pattern){
    char *fileNameChar = (char *) malloc(sizeof(char) * 12);
    strcpy(fileNameChar, "P");
    strcat(fileNameChar, String(pattern).c_str());
    strcat(fileNameChar, "CH");
    strcat(fileNameChar, String(channel).c_str());
    strcat(fileNameChar,".JSN");
    char* charBuffer;                              // Declare a pointer to your buffer.
    if (!SD.exists(fileNameChar)){
      Serial.println("save file " + String(fileNameChar) + "does not exist.");
      return SAVEFILE_DOES_NOT_EXIST;
    } else {
      Serial.println("reading json file: " + String(fileNameChar));
      StaticJsonBuffer<16384> jsonBuffer;

      jsonFile = SD.open((const char *)fileNameChar, FILE_READ);
      if (jsonFile)
      {
          unsigned int fileSize = jsonFile.size();  // Get the file size.
          charBuffer = (char*)malloc(fileSize + 1);  // Allocate memory for the file and a terminating null char.
          jsonFile.read(charBuffer, fileSize);         // Read the file into the buffer.
          charBuffer[fileSize] = '\0';               // Add the terminating null char.
          //Serial.println(charBuffer);                // Print the file to the serial monitor.
          jsonFile.close();                         // Close the file.
      }
      // *** Use the buffer as needed here. ***
      JsonObject& jsonReader = jsonBuffer.parseObject(charBuffer);
      free(charBuffer);                              // Free the memory that was used by the buffer.
      free(fileNameChar);
      if (!jsonReader.success()) {
        Serial.println("parseObject() failed");
        return READ_JSON_ERROR;
      }
       sequence->stepCount    = jsonReader["stepCount"];
       sequence->beatCount    = jsonReader["beatCount"];
       sequence->quantizeKey  = jsonReader["quantizeKey"];
       sequence->instrument   = jsonReader["instrument"];
       sequence->instType     = jsonReader["instType"];
       sequence->volume       = jsonReader["volume"];
       sequence->bank         = jsonReader["bank"];
       sequence->channel      = jsonReader["channel"];
       sequence->patternIndex = jsonReader["patternIndex"];

       JsonArray& stepDataArray = jsonReader["stepData"];

      for (int i=0; i< 128; i++){
         if (i != int(stepDataArray[i]["i"]) ) {
           Serial.println("Step Data Index Mismatch Error");
           return READ_JSON_ERROR;
         };
         sequence->stepData[i].pitch  = stepDataArray[i]["p"];
         sequence->stepData[i].gateLength  = stepDataArray[i]["gl"];
         sequence->stepData[i].gateType  = stepDataArray[i]["gt"];
         sequence->stepData[i].velocity  = stepDataArray[i]["v"];
         sequence->stepData[i].glide  = stepDataArray[i]["g"];
      }
    }
    Serial.println("JSON file read complete");
    return FILE_EXISTS;
}
/*
void FlashMemory::saveChannelPattern_JSON(uint8_t channel) {
    Serial.println("Saving pattern " + String(sequence[channel].patternIndex) + " channel " + String(channel) + " to SD Card as JSON FILE. time:\t" + String(micros()) );

    root.createNestedObject(String(sequence[channel].patternIndex).concat(String(channel));
// 2 ways this could be done. saving a single file per channel - pattern and saving a single file per pattern. saving per channel pattern would require less memory, and would probably require less seeking.
    sequence[channel].stepData(1-127)

    sequence[channel].stepData[selectedStep].pitch
    sequence[channel].stepData[selectedStep].gateLength
    sequence[channel].stepData[selectedStep].gateType
    sequence[channel].stepData[selectedStep].velocity
    sequence[channel].stepData[selectedStep].glide

    sequence[channel].stepCount
    sequence[channel].beatCount
    sequence[channel].quantizeKey
    sequence[channel].instrument
    sequence[channel].instType
    sequence[channel].volume
    sequence[channel].bank
    sequence[channel].channel
    sequence[channel].patternIndex


}
*/

void FlashMemory::saveChannelPattern(uint8_t channel) {
  Serial.println("Saving pattern " + String(sequence[channel].patternIndex) + " channel " + String(channel) + " to SD Card. time:\t" + String(micros()) );
 // for(int i=0; i < sequenceCount; i++){
 	  int index = int(
    ( channel  + sequence[channel].patternIndex * sequenceCount )
    * ( sizeof(sequence[0].stepData)
      + sizeof(sequence[0].stepCount)
      + sizeof(sequence[0].beatCount)
      + sizeof(sequence[0].quantizeKey)
      + sizeof(sequence[0].instrument)
      + sizeof(sequence[0].instType)
      + sizeof(sequence[0].volume)
      + sizeof(sequence[0].bank)
      + sizeof(sequence[0].channel)
      + sizeof(sequence[0].patternIndex)
    ));

  	Serial.println("channel: " + String(channel) + " " + String(index)
      + "\tsize"
      + "\tstepData: "      + String(sizeof(sequence[0].stepData) )
      + "\tstepCount: "     + String(sizeof(sequence[0].stepCount) )
      + "\tbeatCount: "     + String(sizeof(sequence[0].beatCount) )
      + "\tquantizeKey: "   + String(sizeof(sequence[0].quantizeKey) )
      + "\tinstrument: "    + String(sizeof(sequence[0].instrument) )
      + "\tinstType: "      + String(sizeof(sequence[0].instType) )
      + "\tvolume: "        + String(sizeof(sequence[0].volume) )
      + "\tbank: "          + String(sizeof(sequence[0].bank) )
      + "\tchannel: "       + String(sizeof(sequence[0].channel) )
      + "\tpatternIndex: "  + String(sizeof(sequence[0].patternIndex) )
      );


  saveData = SD.open("data.txt", FILE_WRITE);
  if (!saveData) {
    Serial.println("===***=== SAVE DATA DID NOT WORK, SAVEDATA DID NOT OPEN ===***===");
  } else {
    saveData.seek(index);
    saveData.write( (byte*)&sequence[channel].stepData,     sizeof(sequence[channel].stepData));
    saveData.write( (byte*)&sequence[channel].stepCount,    sizeof(sequence[channel].stepCount));
    saveData.write( (byte*)&sequence[channel].beatCount,    sizeof(sequence[channel].beatCount));
    saveData.write( (byte*)&sequence[channel].quantizeKey,  sizeof(sequence[channel].quantizeKey));
    saveData.write( (byte*)&sequence[channel].instrument,   sizeof(sequence[channel].instrument));
    saveData.write( (byte*)&sequence[channel].instType,     sizeof(sequence[channel].instType));
    saveData.write( (byte*)&sequence[channel].volume,       sizeof(sequence[channel].volume));
    saveData.write( (byte*)&sequence[channel].bank,         sizeof(sequence[channel].bank));
    saveData.write( (byte*)&sequence[channel].channel,      sizeof(sequence[channel].channel));
    saveData.write( (byte*)&sequence[channel].patternIndex, sizeof(sequence[channel].patternIndex));
  //}
    saveData.close();
	  Serial.println("Done saving to SD..." + String(micros()));
  }


}

void FlashMemory::loadPattern(uint8_t pattern, uint8_t channelSelector) {

  Serial.println("========= LOADING PATTERN: " + String(pattern));
  printPattern();

	for(int i=0; i < sequenceCount; i++){

    if ( !(channelSelector & (1 << i) ) ){
      Serial.println("skipping loading channel " + String(i));
      continue; // if channel is not selected to be loaded, don't load the channel!
    } else {
      Serial.println("Loading channel " + String(i));
    };

    int readJsonReturn = this->readSequenceJSON(&sequence[i], i, pattern);

    if ( readJsonReturn == 1 )  {
      Serial.println("saveData not available, initializing sequence");
      sequence[i].initNewSequence(pattern, i);
    } else if (readJsonReturn == 2) {
      Serial.println("READ JSON ERROR - info above");
    }

    Serial.println("reading complete!");

    //sam2695.programChange(0, i, sequence[i].instrument);
    //sam2695.setChannelVolume(i, sequence[i].volume);

    sequence[i].quantizeKey = 1;
  }

  Serial.println("changing current pattern from " + String(currentPattern) + " to " + String(pattern) + " and also this is queuePattern: " + String(queuePattern));
  currentPattern = pattern;
  Serial.println("Pattern " + String(pattern) + " loaded");
  printPattern();
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
    Serial.print("sc:\t"+String(sequence[i].stepCount) +"\tbc:\t"+String(sequence[i].beatCount) +"\tqk:\t"+String(sequence[i].quantizeKey)+"\tinst:\t"+String(sequence[i].instrument)+"\tit:\t"+String(sequence[i].instType) + "\t");
    for(int n=0; n<16; n++){
      Serial.print( String(sequence[i].stepData[n].pitch) + "\t" );
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
