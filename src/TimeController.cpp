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

	outputControl.initialize(&backplaneGPIO, serialMidi, adc, globalObj, sequencerArray, &saveFile);

	sequencerArray[0].initialize(0, 16, 4, &outputControl, globalObj);
  sequencerArray[1].initialize(1, 16, 4, &outputControl, globalObj);
  sequencerArray[2].initialize(2, 16, 4, &outputControl, globalObj);
  sequencerArray[3].initialize(3, 16, 4, &outputControl, globalObj);

	display.initialize(sequencerArray, &clockMaster, globalObj, midiControl);

	buttonIo.initialize(&outputControl, &midplaneGPIO, &backplaneGPIO, &saveFile, sequencerArray, &clockMaster, &display, globalObj);

	ledArray.initialize(sequencerArray, globalObj);

	clockMaster.initialize(&outputControl, sequencerArray, serialMidi, midiControl, globalObj);

	saveFile.initialize(&outputControl, sequencerArray, &SerialFlash, adc, globalObj);

	if(eraseAllFlag){
    Serial.println("*&*&*&&**&&&*&*&*& erase all flag set, erasing everything... *&*&*&*&*&*&*&&*");
		//saveFile.wipeEEPROM();
		saveFile.formatAndInitialize();
		Serial.println("*&*&*&&**&&&*&*&*& erasing complete!... *&*&*&*&*&*&*&&*");

  }

//	saveFile.fileSizeTest();
//	saveFile.deleteSaveFile();
	//saveFile.wipeEEPROM();
	//saveFile.deleteTest();
	saveFile.initializeCache();
	//saveFile.loadPattern(0, 0b1111);
  currentPattern = 0;
  for(int i=0; i<SEQUENCECOUNT; i++){
    sequencerArray[i].initNewSequence(currentPattern, i);
  }

	if( saveFile.readCalibrationEEPROM() == true ){
		display.calibrationWarning();
	} else {
		Serial.println("Calibration Data Found");
	};

	if (!saveFile.doesSeqDataExist()){
		display.saveFileWarning();
		Serial.println("WARNING! SAVE FILE NOT FOUND");
	} else {
		Serial.println("SAVE FILE FOUND");
		// delay(5);
	}
	//
	// char * fileBuffer = (char*)calloc(SECTORSIZE, sizeof(char) );
	//
	// for(int i=0; i<16; i++){
	// 	saveFile.serializePattern(fileBuffer, 0, i);
	// 	free(fileBuffer);
	// 	fileBuffer = NULL;
	// }
	Serial.println("================ checking to see which sequences have been saved ================");
	saveFile.checkForSavedSequences();
	for(int pattern=0; pattern<16; pattern++){
		for(int channel=0; channel<4; channel++){
			Serial.print(String(globalObj->savedSequences[channel][pattern]));
		}
	}
	Serial.println();

	buttonIo.changeState(STATE_PITCH0);
	midiTestValue = 0;

//

//	saveFile.listFiles();
//
	//saveFile.deleteSaveFile();
	//saveFile.wipeEEPROM();
	//saveFile.initializeCache();

//	saveFile.deleteSaveFile();
/*
	saveFile.saveSequenceData(sequence[0], 0, 0 );
	delay(200);
	saveFile.readSequenceData(sequence[0], 0, 0);
	File root = SD.open("/");
  saveFile.printDirectory(root, 2);
	*/
	Serial.println("===== TIMECONTROLLER INITIALIZATION COMPLETE =====");

}

void TimeController::runLoopHandler() {
	// digitalWriteFast(PIN_EXT_RX, HIGH);

 	buttonIo.loop(INPUT_INTERVAL);

	// saveFile.staggeredLoadLoop();

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

  if(midiTestActive && stepMode == STATE_TEST_MIDI){
//for(int i=0; i<128; i++){
        Serial.println("SENDING MIDI TEST NOTES " + String(midiTestValue));
        serialMidi->sendNoteOn(midiTestValue, 127, 1);
//        delay(100);
				serialMidi->sendNoteOff(midiTestValue, 0,  1);
	//			delay(100);
				serialMidi->read();
				//display.displayLoop(DISPLAY_INTERVAL);
				midiTestValue = (midiTestValue+1)%128;
      //}
  }


//    Serial.println("Sending Midi Test Notes");
//    delay(1000);
  //  for(int i=1; i<10; i++){
      //serialMidi->sendNoteOn(i, 127, 1);
  //    serialMidi->sendNoteOff(i, 0,  1);
      //delay(10);
    //}
//
//
  //  serialMidi->read();
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
