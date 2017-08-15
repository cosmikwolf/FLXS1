#include <Arduino.h>
#include "TimeController.h"
#include "midiModule.h"


TimeController::TimeController(){ };

void TimeController::initialize(midi::MidiInterface<HardwareSerial>* serialMidi, MidiModule *midiControl, Sequencer* sequencerArray, ADC *adc, GlobalVariable *globalObj) {

	Serial.println("Initializing TimeController");

	this->serialMidi = serialMidi;
  this->sequencerArray = sequencerArray;
	this->adc = adc;
	this->globalObj = globalObj;

	outputControl.initialize(&backplaneGPIO, serialMidi, adc, globalObj);

	sequencerArray[0].initialize(0, 16, 4, &outputControl, globalObj);
  sequencerArray[1].initialize(1, 16, 4, &outputControl, globalObj);
  sequencerArray[2].initialize(2, 16, 4, &outputControl, globalObj);
  sequencerArray[3].initialize(3, 16, 4, &outputControl, globalObj);

	display.initialize(sequencerArray, &clockMaster, globalObj);

	buttonIo.initialize(&outputControl, &midplaneGPIO, &backplaneGPIO, &saveFile, sequencerArray, &clockMaster, &display, globalObj);

	ledArray.initialize(sequencerArray, globalObj);

	clockMaster.initialize(&outputControl, sequencerArray, serialMidi, midiControl, globalObj);

	saveFile.initialize(&outputControl, sequencerArray, &SerialFlash, adc, globalObj);

	if(eraseAllFlag){
    Serial.println("*&*&*&&**&&&*&*&*& erase all flag set, erasing everything... *&*&*&*&*&*&*&&*");
		saveFile.wipeEEPROM();
		saveFile.formatChip();
    saveFile.initializeCache();

    for(int pattern=0; pattern < 16; pattern++){
      Serial.println("***----###$$$###---*** *^~^* SAVING PATTERN " + String(pattern) + " TO CACHE *^~^* ***----###$$$###---***");

      for (int channel=0; channel < SEQUENCECOUNT; channel++){
        sequencerArray[channel].initNewSequence(pattern, channel);
      }
      Serial.println("Patterns initialized");
      for (int channel=0; channel < SEQUENCECOUNT; channel++){
        saveFile.saveSequenceJSON(channel, pattern);
      }
      Serial.println("json sequence saved");
      while(saveFile.cacheWriteSwitch){
        saveFile.cacheWriteLoop();
      //  Serial.print(".");
      //  delay(10);
      };
      //  Serial.println(" ");
      Serial.println("***----###$$$###---*** *^~^* PATTERN SAVED " + String(pattern) + " TO CACHE *^~^* ***----###$$$###---***");
      delay(500);
    }

    while(saveFile.cacheWriteSwitch){
      saveFile.cacheWriteLoop();
    }
    delay(100);
//    saveFile.loadPattern(0, 0b1111);
    saveFile.listFiles();
  //  changeState(STATE_PITCH0);

  }

//	saveFile.fileSizeTest();
//	saveFile.deleteSaveFile();
	//saveFile.wipeEEPROM();
	//saveFile.deleteTest();
	Serial.println("1"); delay(100);
	saveFile.initializeCache();
	//saveFile.loadPattern(0, 0b1111);
  currentPattern = 0;
  for(int i=0; i<SEQUENCECOUNT; i++){
    sequencerArray[i].initNewSequence(currentPattern, i);
  }
	saveFile.readCalibrationEEPROM();

	buttonIo.changeState(STATE_PITCH0);


//

//	saveFile.listFiles();
//
	//saveFile.deleteSaveFile();
	//saveFile.wipeEEPROM();
	//saveFile.initializeCache();

//	saveFile.deleteSaveFile();
/*
	saveFile.saveSequenceJSON(sequence[0], 0, 0 );
	delay(200);
	saveFile.readSequenceJSON(sequence[0], 0, 0);
	File root = SD.open("/");
  saveFile.printDirectory(root, 2);
	*/

}

void TimeController::runLoopHandler() {
	// digitalWriteFast(PIN_EXT_RX, HIGH);
 	buttonIo.loop(INPUT_INTERVAL);

	if (cacheWriteTimer > 10000 && saveFile.cacheWriteSwitch){
		// digitalWriteFast(PIN_EXT_TX, HIGH);
		saveFile.cacheWriteLoop();
		// digitalWriteFast(PIN_EXT_TX, LOW);
		cacheWriteTimer=0;
	}
	// digitalWriteFast(PIN_EXT_RX, LOW);

  if (clockMaster.displayRunSwitch){
    display.displayLoop(DISPLAY_INTERVAL);
    //clockMaster.displayRunSwitch = false;
  }
}

void TimeController::ledClockHandler(){
  ledArray.loop(0);
}


void TimeController::masterClockHandler(){
	clockMaster.masterClockFunc();
}

void TimeController::sequencerHandler(){
	clockMaster.sequencerFunc();
}

void TimeController::displayClockHandler(){
//  if (clockMaster.displayRunSwitch){
    display.displayLoop(0);
  //  clockMaster.displayRunSwitch = false;
  //}
}



void TimeController::midiClockHandler(){
	midiControl->midiClockSyncFunc(serialMidi);
}

void TimeController::cacheWriteHandler(){
//	saveFile.cacheWriteLoop();
}
