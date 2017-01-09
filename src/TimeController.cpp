#include <Arduino.h>
#include "TimeController.h"
#include "midiModule.h"

#define DISPLAY_FREQUENCY 10000
#define INPUT_FREQUENCY 10000
#define LED_FREQUENCY 7000


TimeController::TimeController(){ };

void TimeController::initialize(midi::MidiInterface<HardwareSerial>* serialMidi, MidiModule *midiControl, NoteDatum *noteData, Sequencer* sequencerArray, ADC *adc) {

	Serial.println("Initializing TimeController");

	this->serialMidi = serialMidi;
  this->sequencerArray = sequencerArray;
	this->adc = adc;

	outputControl.initialize(&backplaneGPIO, serialMidi, adc);

	sequencerArray[0].initialize(0, 16, 4, (tempoX100/100));
  sequencerArray[1].initialize(1, 16, 4, (tempoX100/100));
  sequencerArray[2].initialize(2, 16, 4, (tempoX100/100));
  sequencerArray[3].initialize(3, 16, 4, (tempoX100/100));

	buttonIo.initialize(&outputControl, &midplaneGPIO, &backplaneGPIO, &saveFile, sequencerArray, &clockMaster);
	buttonIo.changeState(CHANNEL_PITCH_MODE);


	display.initialize(sequencerArray);

	ledArray.initialize(sequencerArray);


	clockMaster.initialize(&outputControl, sequencerArray, noteData, serialMidi, midiControl);
	saveFile.initialize(sequencerArray, &SerialFlash, adc);


	if(eraseAllFlag){
    Serial.println("*&*&*&&**&&&*&*&*& erase all flag set, erasing everything... *&*&*&*&*&*&*&&*");
    saveFile.deleteSaveFile();
    saveFile.wipeEEPROM();
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
//        delay(10);
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

	saveFile.initializeCache();

	saveFile.loadPattern(0, 0b1111);

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

//	Serial.println("LED Loop timer: " + String(timeControlTimer)); timeControlTimer = 0;
	buttonIo.loop(INPUT_FREQUENCY);
	//Serial.println("Button Loop timer: " + String(timeControlTimer)); timeControlTimer = 0;

	if (cacheWriteTimer>1000){
		digitalWriteFast(DEBUG_PIN, HIGH);
		saveFile.cacheWriteLoop();
		digitalWriteFast(DEBUG_PIN, LOW);
		cacheWriteTimer=0;
	}
	//Serial.println("Cache Loop timer: " + String(timeControlTimer)); timeControlTimer = 0;

	display.displayLoop(DISPLAY_FREQUENCY);
	//Serial.println("Display Loop timer: " + String(timeControlTimer)); timeControlTimer = 0;

	outputControl.inputRead();


	if(currentState == CALIBRATION_MENU){
		playing = 0;
		outputControl.dacTestLoop();
		return;
	}

//	outputControl.dacTestLoop();

}

void TimeController::masterClockHandler(){
//	saveFile.cacheWriteLoop();

	clockMaster.masterClockFunc();
}

void TimeController::cacheWriteHandler(){
//	saveFile.cacheWriteLoop();
}
