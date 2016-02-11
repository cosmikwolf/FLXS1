void changePattern(uint8_t pattern, boolean saveFirst, boolean instant){
	//Serial.println("currentPattern: " + String(currentPattern) + "\tsequenceCount: " + String(sequenceCount));
	if(saveFirst){
		savePattern(currentPattern);
	}

  if (instant || !playing) {
    Serial.println("Changing pattern instantly: " + String(pattern) + " instant: " + String(instant) + " playing: " + String(playing));
    loadPattern(pattern);    
  } else {
    queuePattern = pattern;
    Serial.println("Queueing pattern: " + String(pattern));
  }

}

void initializeFlashMemory(){
 /* pinMode(2, OUTPUT);
  delay(10);
  if (!SD.init(SPI_HALF_SPEED, 2) {
    Serial.println("SD Card initialization failed!");
    //return;
  }
  // THIS IS THE LINE THAT DELETES THE DATAFILE EVERY TIME! 
  // deleteSaveFile();
  if (SD.exists("data.txt")) {
    Serial.println("data.txt exists.");
  } else {
    Serial.println("data.txt does not exist, creating file...");
    saveData = SD.open("data.txt", FILE_WRITE);
    saveData.close();
  } 

  Serial.println("SD Card and save file initialization complete.");
  */
}
 

void deleteSaveFile(){
  SD.remove("data.txt");
  loadPattern(0);
}

void savePattern(uint8_t pattern) {
  Serial.println("Saving to SD Card - " + String(micros()));
  need2save = false;  
  saveData = SD.open("data.txt", FILE_WRITE);

  for(int i=0; i < sequenceCount; i++){

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
    )); 
  
  	Serial.println("seq index: " + String(i) + " " + String(index) 
      + "\tsize"
      + "\tstepData: " + String(sizeof(sequence[0].stepData) )
      + "\tstepCount: " + String(sizeof(sequence[0].stepCount) )
      + "\tbeatCount: " + String(sizeof(sequence[0].beatCount) )
      + "\tquantizeKey: " + String(sizeof(sequence[0].quantizeKey) )
      + "\tinstrument: " + String(sizeof(sequence[0].instrument) )
      + "\tinstType: " + String(sizeof(sequence[0].instType) )
      + "\tvolume: " + String(sizeof(sequence[0].volume) )
      + "\tbank: " + String(sizeof(sequence[0].bank) )
      );

    saveData.seek(index);
    saveData.write( (byte*)&sequence[i].stepData,    sizeof(sequence[i].stepData));
    saveData.write( (byte*)&sequence[i].stepCount,   sizeof(sequence[i].stepCount));
    saveData.write( (byte*)&sequence[i].beatCount,   sizeof(sequence[i].beatCount));
    saveData.write( (byte*)&sequence[i].quantizeKey, sizeof(sequence[i].quantizeKey));
    saveData.write( (byte*)&sequence[i].instrument,  sizeof(sequence[i].instrument));
    saveData.write( (byte*)&sequence[i].instType,    sizeof(sequence[i].instType));
    saveData.write( (byte*)&sequence[i].volume,      sizeof(sequence[i].volume));
    saveData.write( (byte*)&sequence[i].bank,      sizeof(sequence[i].bank));
  }
  saveData.close();
	Serial.println("Done saving to SD..." + String(micros()));

}

void loadPattern(uint8_t pattern) {

  Serial.println("========= LOADING PATTERN: " + String(pattern)); 
  printPattern();

  saveData = SD.open("data.txt");

	for(int i=0; i < sequenceCount; i++){

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
    saveData.read( (byte*)&sequence[i].bank,      sizeof(sequence[i].bank));

    if (sequence[i].instType == 0 || saveData.size() <= index){
      Serial.println("saveData not available, initializing sequence");
      sequence[i].initNewSequence();
    } else {
      Serial.println("saveDataAvailable, loading sequence --- instType: " + String(sequence[i].instType));
    }
    Serial.println("reading complete!");
    
    sam2695.programChange(0, i, sequence[i].instrument);
    sam2695.setChannelVolume(i, sequence[i].volume);

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

void printDirectory(File dir, int numTabs) {
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

void printPattern(){
  Serial.println("Printing Data for pattern: " + String(currentPattern));
  for(int i=0; i < sequenceCount; i++){
    Serial.print("sc:\t"+String(sequence[i].stepCount) +"\tbc:\t"+String(sequence[i].beatCount) +"\tqk:\t"+String(sequence[i].quantizeKey)+"\tinst:\t"+String(sequence[i].instrument)+"\tit:\t"+String(sequence[i].instType) + "\t");
    for(int n=0; n<16; n++){
      Serial.print( String(sequence[i].stepData[n].pitch) + "\t" );
    }
    Serial.println("");
  }
}
