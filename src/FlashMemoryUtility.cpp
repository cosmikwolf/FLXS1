#include <Arduino.h>
#include "FlashMemory.h"

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



void FlashMemory::wipeEEPROM(){
  for (int i=0; i<2048; i++){
    Serial.print("writing EEProm: " + String(i));
    EEPROM.update(i, 0);
    Serial.println("\t" + String(EEPROM.read(i) ));
  }
  delay(500);
}


void FlashMemory::fileSizeTest(){

  uint16_t maxFileLimit = 1024;
  Serial.println("****^*^^^*****^^*^*^*^^** BEGINNING FILE SIZE TEST ***^^*^*^^*^^*^*^**^*^^**^*^*");
  delay(1000);

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
      } else {
        break; // no more files
      }
    }

    Serial.println("****^*^^^*****^^*^*^*^^** BEGIN FILE ERASE ***^^*^*^^*^^*^*^**^*^^**^*^*");
    delay(1000);
    this->deleteSaveFile();
    Serial.println("****^*^^^*****^^*^*^*^^** BEGIN TEST FILE CREATION ***^^*^*^^*^^*^*^**^*^^**^*^*");

    delay(1000);
    char* fileName = (char *) malloc(sizeof(char) * 12);


    for (int i =0; i< 16; i++) {
      for(int t=0; t< 4; t++){
        fileName = strdup(String("seq" + String(i) + "_t" + String(t) ).c_str() );
        if (!spiFlash->exists(fileName)) {
          Serial.println("Creating Test File: " + String(fileName) + "\tsize: " + String(FLASHFILESIZE));
          spiFlash->createErasable(fileName, FLASHFILESIZE);
        }
        free(fileName);
      }
    }
    for (int i =0; i< 16; i++) {
      for(int t=0; t< 4; t++){
        fileName = strdup(String("cache" + String(i) + "_t" + String(t) ).c_str() );
        if (!spiFlash->exists(fileName)) {
          Serial.println("Creating Test File: " + String(fileName) + "\tsize: " + String(FLASHFILESIZE));
          spiFlash->createErasable(fileName, FLASHFILESIZE);
        }
        free(fileName);
      }
    }
  Serial.println("****^*^^^*****^^*^*^*^^** BEGIN LISTING NEW FILES ***^^*^*^^*^^*^*^**^*^^**^*^*");

  delay(1000);

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
    } else {
      break; // no more files
    }
  }

  Serial.println(" *(*(*(*(*(*(**(  FILESIZE TEST IS COMPLETE!!! *)*)*)*)*)*)*)*)*");
  delay(10000);
}
