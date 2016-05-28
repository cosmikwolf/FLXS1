#include <Arduino.h>
#include "fileOps.h"
/*
¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING!
This file contains code that enables saving and loading of patterns. Changing this file could result in an inability to read existing save files.
¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING! ¡WARNING!
*/
fileOps saveFile;
fileOps::fileOps(){};

void fileOps::jsonTest(){
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

void fileOps::initialize(){

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


}

void fileOps::changePattern(uint8_t pattern, uint8_t channelSelector, boolean saveFirst, boolean instant){
	//Serial.println("currentPattern: " + String(currentPattern) + "\tsequenceCount: " + String(sequenceCount));
	if(saveFirst){
    for(int i=0; i < sequenceCount; i++){
  		//saveChannelPattern(i);
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

void fileOps::deleteSaveFile(){
  SD.remove("data.txt");
  for(int i=0; i<16; i++){
    for(int n=0; n<sequenceCount; n++){
      sequence[n].initNewSequence(i, n);
      saveChannelPattern(n);
    }
  }

  loadPattern(0, 0b1111);

}

void fileOps::saveSequenceJSON(Sequencer& sequence){
//elaspedMicros jsonSaveTimer = 0;
  StaticJsonBuffer<16384> jsonBuffer;

  SD.remove("json.txt");
  // following ArduinoJSON serialize example: https://github.com/bblanchon/ArduinoJson/wiki/FAQ#whats-the-best-way-to-use-the-library
  //StaticJsonBuffer<512> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  //JsonArray& data = root.createNestedArray("data");

  root["stepCount"]     = sequence.stepCount;
  root["beatCount"]     = sequence.beatCount;
  root["quantizeKey"]   = sequence.quantizeKey;
  root["instrument"]    = sequence.instrument;
  root["instType"]      = sequence.instType;
  root["volume"]        = sequence.volume;
  root["bank"]          = sequence.bank;
  root["channel"]       = sequence.channel;
  root["patternIndex"]  = sequence.patternIndex;

  JsonArray& stepDataArray = root.createNestedArray("stepData");

  for (int i=0; i< 128; i++){
    JsonObject& stepDataObj = jsonBuffer.createObject();
    stepDataObj["stepnum"] = i ;
    stepDataObj["pitch"] = sequence.stepData[i].pitch ;
    stepDataObj["gateLength"] = sequence.stepData[i].gateLength ;
    stepDataObj["gateType"] = sequence.stepData[i].gateType ;
    stepDataObj["velocity"] = sequence.stepData[i].velocity ;
    stepDataObj["glide"] = sequence.stepData[i].glide ;
    stepDataArray.add(stepDataObj);
  }

  jsonFile = SD.open("json.txt", FILE_WRITE);
  root.prettyPrintTo(jsonFile);
  jsonFile.close();
//  Serial.println(String(jsonSaveTimer))

}
/*
void fileOps::saveChannelPattern_JSON(uint8_t channel) {
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

void fileOps::saveChannelPattern(uint8_t channel) {
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

void fileOps::loadPattern(uint8_t pattern, uint8_t channelSelector) {

  Serial.println("========= LOADING PATTERN: " + String(pattern));
  printPattern();
  saveData = SD.open("data.txt", FILE_READ);

	for(int i=0; i < sequenceCount; i++){

    if ( !(channelSelector & (1 << i) ) ){
      Serial.println("skipping loading channel " + String(i));
      continue; // if channel is not selected to be loaded, don't load the channel!
    } else {
      Serial.println("Loading channel " + String(i));
    }

    int index = int(
    ( i  + pattern * sequenceCount )
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

    Serial.println("seeking to index: " + String(index) + " for sequence " + String(i) + "\t\tfileSize: " + saveData.size());

  //  Serial.println("stepData size: \t" + String(sizeof(sequence[i].stepData)) );
  //  Serial.println("stepCount size: \t" + String(sizeof(sequence[i].stepCount)));
  //  Serial.println("beatCount size: \t" + String(sizeof(sequence[i].beatCount)) );
  //  Serial.println("quantizeKey size: \t" + String(sizeof(sequence[i].quantizeKey) ));
  //  Serial.println("instrument size: \t" + String(sizeof(sequence[i].instrument) ));
  //  Serial.println("instType size: \t" + String(sizeof(sequence[i].instType) ));

    saveData.seek(index);
    Serial.println("loading data");
    saveData.read( (byte*)&sequence[i].stepData,    sizeof(sequence[i].stepData));
    saveData.read( (byte*)&sequence[i].stepCount,   sizeof(sequence[i].stepCount));
    saveData.read( (byte*)&sequence[i].beatCount,   sizeof(sequence[i].beatCount));
    saveData.read( (byte*)&sequence[i].quantizeKey, sizeof(sequence[i].quantizeKey));
    saveData.read( (byte*)&sequence[i].instrument,  sizeof(sequence[i].instrument));
    saveData.read( (byte*)&sequence[i].instType,    sizeof(sequence[i].instType));
    saveData.read( (byte*)&sequence[i].volume,      sizeof(sequence[i].volume));
    saveData.read( (byte*)&sequence[i].bank,        sizeof(sequence[i].bank));
    saveData.read( (byte*)&sequence[i].channel,     sizeof(sequence[i].channel));
    saveData.read( (byte*)&sequence[i].patternIndex,       sizeof(sequence[i].patternIndex));

    if (saveData.size() <= index){
    //if (sequence[i].instType == 0 || saveData.size() <= index){ // original line, not sure why it checked instType?
      Serial.println("saveData not available, initializing sequence");
      sequence[i].initNewSequence(pattern, i);
    } else {
      Serial.println("saveData available, loading sequence --- instType: " + String(sequence[i].instType));
    }
    Serial.println("reading complete!");

    sam2695.programChange(0, i, sequence[i].instrument);
    sam2695.setChannelVolume(i, sequence[i].volume);

    sequence[i].quantizeKey = 1;
    // if no steps are set, it is an empty sequence. initialize a new default sequence.
  // if (sequence[i].instType == 0 ) {
  //   Serial.println("###### INITIALIZING NEW SEQUENCE ######");
  //   sequence[i].initNewSequence();
  // } else {
  //   Serial.println("###### sequence already exists instType: " + String(sequence[i].instType));//
  // }


  }

  Serial.println("closing file handle");

  saveData.close();

  Serial.println("file handle closed");
  Serial.println("changing current pattern from " + String(currentPattern) + " to " + String(pattern) + " and also this is queuePattern: " + String(queuePattern));
  currentPattern = pattern;
  Serial.println("Pattern " + String(pattern) + " loaded");
  printPattern();
}

void fileOps::printDirectory(File dir, int numTabs) {
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

void fileOps::printPattern(){
  Serial.println("Printing Data for pattern: " + String(currentPattern));
  for(int i=0; i < sequenceCount; i++){
    Serial.print("sc:\t"+String(sequence[i].stepCount) +"\tbc:\t"+String(sequence[i].beatCount) +"\tqk:\t"+String(sequence[i].quantizeKey)+"\tinst:\t"+String(sequence[i].instrument)+"\tit:\t"+String(sequence[i].instType) + "\t");
    for(int n=0; n<16; n++){
      Serial.print( String(sequence[i].stepData[n].pitch) + "\t" );
    }
    Serial.println("");
  }
}
