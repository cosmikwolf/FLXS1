#include <Arduino.h>
#include "TimeController.h"
#include "midiModule.h"


TimeController::TimeController(){ };

void TimeController::initialize(midi::MidiInterface<HardwareSerial>* serialMidi, MidiModule *midiControl, Sequencer* sequencerArray, ADC *adc) {

	Serial.println("Initializing TimeController");

	this->serialMidi = serialMidi;
  this->sequencerArray = sequencerArray;
	this->adc = adc;

	outputControl.initialize(&backplaneGPIO, serialMidi, adc);

	sequencerArray[0].initialize(0, 16, 4, (tempoX100/100), &outputControl);
  sequencerArray[1].initialize(1, 16, 4, (tempoX100/100), &outputControl);
  sequencerArray[2].initialize(2, 16, 4, (tempoX100/100), &outputControl);
  sequencerArray[3].initialize(3, 16, 4, (tempoX100/100), &outputControl);


	display.initialize(sequencerArray, &clockMaster);

	buttonIo.initialize(&outputControl, &midplaneGPIO, &backplaneGPIO, &saveFile, sequencerArray, &clockMaster, &display);

	ledArray.initialize(sequencerArray);

	clockMaster.initialize(&outputControl, sequencerArray, serialMidi, midiControl);

	saveFile.initialize(&outputControl, sequencerArray, &SerialFlash, adc);

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

	saveFile.initializeCache();
	saveFile.loadPattern(0, 0b1111);

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

	elapsedMicros timeControlTimer = 0;
	if (clockMaster.ledRunSwitch){
		ledArray.loop(LED_INTERVAL);
		//clockMaster.ledRunSwitch = false;
	}
	if(timeControlTimer > 10000){
   //Serial.println("*&*&*&*&*&&*&&*&*&*&*&*&* LED LOOP TOOK MORE THAN 10MS: " + String(timeControlTimer));
  };
  timeControlTimer = 0;
	// digitalWriteFast(PIN_EXT_RX, LOW);

//	Serial.println("LED Loop timer: " + String(timeControlTimer)); timeControlTimer = 0;
	buttonIo.loop(INPUT_INTERVAL);
	if(timeControlTimer > 10000){
   //Serial.println("*&*&*&*&*&&*&&*&*&*&*&*&* BUTTON LOOP TOOK MORE THAN 10MS: " + String(timeControlTimer));
  };
  timeControlTimer = 0;

	//debug("Button Loop timer: " + String(timeControlTimer)); timeControlTimer = 0;
	// digitalWriteFast(PIN_EXT_RX, HIGH);

	if (cacheWriteTimer > 10000 && saveFile.cacheWriteSwitch){
		// digitalWriteFast(DEBUG_PIN, HIGH);
		saveFile.cacheWriteLoop();
		// digitalWriteFast(DEBUG_PIN, LOW);
		cacheWriteTimer=0;
		if(timeControlTimer > 10000){
     //Serial.println("*&*&*&*&*&&*&&*&*&*&*&*&* CACHE LOOP TOOK MORE THAN 10MS: " + String(timeControlTimer));
    }; timeControlTimer = 0;
	}
	// digitalWriteFast(PIN_EXT_RX, LOW);

	//Serial.println("Cache Loop timer: " + String(timeControlTimer)); timeControlTimer = 0;
	display.displayLoop(DISPLAY_INTERVAL);
	// digitalWriteFast(PIN_EXT_RX, HIGH);

	//if(timeControlTimer > 10000){	Serial.println("*&*&*&*&*&&*&&*&*&*&*&*&* DISPLAY LOOP TOOK MORE THAN 10MS: " + String(timeControlTimer));	}; timeControlTimer = 0;

	//Serial.println("Display Loop timer: " + String(timeControlTimer)); timeControlTimer = 0;


	// digitalWriteFast(PIN_EXT_RX, LOW);

}

void TimeController::masterClockHandler(){
	clockMaster.masterClockFunc();
}

void TimeController::sequencerHandler(){
	clockMaster.sequencerFunc();
}

void TimeController::midiClockHandler(){
	midiControl->midiClockSyncFunc(serialMidi);
}

void TimeController::cacheWriteHandler(){
//	saveFile.cacheWriteLoop();
}
