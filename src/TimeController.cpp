#include <Arduino.h>
#include "TimeController.h"
#include "midiModule.h"

#define DISPLAY_FREQUENCY 10000
#define INPUT_FREQUENCY 10000
#define LED_FREQUENCY 30000


TimeController::TimeController(){ };

void TimeController::initialize(midi::MidiInterface<HardwareSerial>* serialMidi, MidiModule *midiControl, NoteDatum *noteData, Sequencer* sequencerArray, ADC *adc) {


	Serial.println("Initializing TimeController");

	this->serialMidi = serialMidi;
  this->sequencerArray = sequencerArray;
	this->adc = adc;
	sequencerArray[0].initialize(0, 16, 4, (tempoX100/100));
  sequencerArray[1].initialize(1, 16, 4, (tempoX100/100));
  sequencerArray[2].initialize(2, 16, 4, (tempoX100/100));
  sequencerArray[3].initialize(3, 16, 4, (tempoX100/100));

	buttonIo.initialize(&outputControl, &midplaneGPIO, &backplaneGPIO, &saveFile, sequencerArray, &clockMaster);
	buttonIo.changeState(CHANNEL_PITCH_MODE);


	display.initialize(sequencerArray);

	ledArray.initialize(sequencerArray);


	outputControl.initialize(&backplaneGPIO, serialMidi, adc);

	clockMaster.initialize(&outputControl, sequencerArray, noteData, serialMidi, midiControl);


	saveFile.initialize(sequencerArray, &SerialFlash);
	saveFile.loadPattern(0, 0b1111);
	//saveFile.deleteSaveFile();
/*
	saveFile.saveSequenceJSON(sequence[0], 0, 0 );
	delay(200);
	saveFile.readSequenceJSON(sequence[0], 0, 0);
	File root = SD.open("/");
  saveFile.printDirectory(root, 2);
	*/
}

void TimeController::runLoopHandler() {
	ledArray.loop(LED_FREQUENCY);
	buttonIo.loop(INPUT_FREQUENCY);
	display.displayLoop(DISPLAY_FREQUENCY);

	outputControl.inputRead();

	if(currentState == CALIBRATION_MENU){
		playing = 0;
		outputControl.dacTestLoop();
		return;
	}

//	outputControl.dacTestLoop();

}

void TimeController::masterClockHandler(){
	clockMaster.masterClockFunc();
}
