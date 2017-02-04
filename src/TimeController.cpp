#include <Arduino.h>
#include "TimeController.h"
#include "midiModule.h"

#define DISPLAY_FREQUENCY 50000
#define INPUT_FREQUENCY 50000
#define LED_FREQUENCY 10000


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

	buttonIo.initialize(&outputControl, &midplaneGPIO, &backplaneGPIO, &saveFile, sequencerArray, &clockMaster);
	buttonIo.changeState(CHANNEL_PITCH_MODE);

	display.initialize(sequencerArray);

	ledArray.initialize(sequencerArray);

	clockMaster.initialize(&outputControl, sequencerArray, serialMidi, midiControl);

	saveFile.initialize(&outputControl, sequencerArray, &SerialFlash, adc);

	if(eraseAllFlag){
    Serial.println("*&*&*&&**&&&*&*&*& erase all flag set, erasing everything... *&*&*&*&*&*&*&&*");
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
    delay(1000);
//    saveFile.loadPattern(0, 0b1111);
    saveFile.listFiles();
  //  changeState(CHANNEL_PITCH_MODE);

  }

//	saveFile.fileSizeTest();
//	saveFile.deleteSaveFile();
	//saveFile.wipeEEPROM();
	//saveFile.deleteTest();

	saveFile.initializeCache();
	saveFile.loadPattern(0, 0b1111);

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

	elapsedMicros timeControlTimer = 0;
	ledArray.loop(LED_FREQUENCY);
	if(timeControlTimer > 10000){	Serial.println("*&*&*&*&*&&*&&*&*&*&*&*&* LED LOOP TOOK MORE THAN 10MS: " + String(timeControlTimer));	}; timeControlTimer = 0;

//	Serial.println("LED Loop timer: " + String(timeControlTimer)); timeControlTimer = 0;
	buttonIo.loop(INPUT_FREQUENCY);
	if(timeControlTimer > 10000){	Serial.println("*&*&*&*&*&&*&&*&*&*&*&*&* BUTTON LOOP TOOK MORE THAN 10MS: " + String(timeControlTimer));	}; timeControlTimer = 0;

	//Serial.println("Button Loop timer: " + String(timeControlTimer)); timeControlTimer = 0;

	if (cacheWriteTimer > 10000 && saveFile.cacheWriteSwitch){
		digitalWriteFast(DEBUG_PIN, HIGH);
		saveFile.cacheWriteLoop();
		digitalWriteFast(DEBUG_PIN, LOW);
		cacheWriteTimer=0;
		if(timeControlTimer > 10000){	Serial.println("*&*&*&*&*&&*&&*&*&*&*&*&* CACHE LOOP TOOK MORE THAN 10MS: " + String(timeControlTimer));	}; timeControlTimer = 0;
	}
	//Serial.println("Cache Loop timer: " + String(timeControlTimer)); timeControlTimer = 0;

	display.displayLoop(DISPLAY_FREQUENCY);
	if(timeControlTimer > 10000){	Serial.println("*&*&*&*&*&&*&&*&*&*&*&*&* DISPLAY LOOP TOOK MORE THAN 10MS: " + String(timeControlTimer));	}; timeControlTimer = 0;

	//Serial.println("Display Loop timer: " + String(timeControlTimer)); timeControlTimer = 0;

	outputControl.inputRead();
	if(timeControlTimer > 10000){	Serial.println("*&*&*&*&*&&*&&*&*&*&*&*&* INPUTREDLOOP TOOK MORE THAN 10MS: " + String(timeControlTimer));	}; timeControlTimer = 0;

	if(currentState == CALIBRATION_MENU){
		playing = 0;
		outputControl.dacTestLoop();
		return;
	}

//	outputControl.dacTestLoop();

}

void TimeController::masterClockHandler(){
//	saveFile.cacheWriteLoop();``
	clockMaster.masterClockFunc();
}

void TimeController::midiClockHandler(){
	midiControl->midiClockSyncFunc(serialMidi);
}

void TimeController::cacheWriteHandler(){
//	saveFile.cacheWriteLoop();
}
