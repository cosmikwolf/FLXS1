#include <Arduino.h>
#include "FlashMemory.h"

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
    //Serial.println("cache " + String(cacheIndex)+ " status: " + String(cacheStatus[cacheIndex]));
    return cacheStatus[cacheIndex];
  }
};


void FlashMemory::initializeCache(){
  char* fileName = (char *) malloc(sizeof(char) * 12);
  fileName =strdup("seqData");
  if (!spiFlash->exists(fileName)) {
    Serial.println("Creating Data File: " + String(fileName) + "\tsize: " + String(FLASHFILESIZE));
    spiFlash->createErasable(fileName, FLASHFILESIZE);
  }
  free(fileName);

  fileName = strdup( "seqCache");
  if (!spiFlash->exists(fileName)) {
    Serial.println("Creating Cache File: " + String(fileName) );
    spiFlash->createErasable(fileName, FLASHFILESIZE);
  }
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
  Serial.println("*&^%*&^%*&^% New Offset:  \t" + String(newCacheOffset));
  delay(500);

  for(int i=0; i<CACHE_COUNT; i++){
    if (getCacheStatus(cacheOffset, i) != CACHE_READY){
      //copy any cache info over if cache still needs to be written
      Serial.println("!@#$%^&*(*&^%$#@!@#$%^&*()*&^%$#@!@#$%^&*()(*&^%$#@!@#$%^&*(*&^%$#@");
      Serial.println("CACHE STILL NEEDS TO BE WRITTEN PT:" +String(getCachePattern(cacheOffset, i)) + "\tCH:"+ String(getCacheChannel(cacheOffset, i)) );
      Serial.println("!@#$%^&*(*&^%$#@!@#$%^&*()*&^%$#@!@#$%^&*()(*&^%$#@!@#$%^&*(*&^%$#@");
      setCacheChannel(newCacheOffset, i, getCacheChannel(cacheOffset, i));
      setCachePattern(newCacheOffset, i, getCachePattern(cacheOffset, i));
      setCacheStatus( newCacheOffset, i, getCacheStatus(cacheOffset, i));
      delay(500);
    }
  };
  cacheOffset = newCacheOffset;
  EEPROMWrite16(0, cacheOffset); //write the new offset to EEPROM so it will persist to the next reboot.
  listFiles();
};

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
        cacheFileName = strdup("seqCache");
        fileName = strdup("seqData");

        switch (getCacheStatus(cacheOffset, cacheNum) ){

    /*
    save to Cache
    erase save file
    copy Cache to save file
    erase cache
    */

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
