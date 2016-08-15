/* **********************************
*** ZETAOHM FLXS1 MUSIC SEQUENCER ***
********************************** */
#include <Arduino.h>
#include <SPI.h>
//#include <i2c_t3.h>
#include "TimeController.h"
#include "Sequencer.h"
#include "midiModule.h"
#include "global.h"
//#include "DisplayModule.h"

#define kSerialSpeed 115200
#define kClockInterval 1000
#define kMosiPin 11
#define kSpiClockPin 13

TimeController timeControl;
IntervalTimer MasterClockTimer;
MidiModule midiControl;
NoteDatum noteData[4];
Sequencer sequencerArray[4];

MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, serialMidi);

void setup() {
  Serial.begin(kSerialSpeed);
  Serial3.begin(31250);
  //waiting for serial to begin
  delay(1500);
  Serial.println("<<<<<----===---==--=-|*+~^~+*|-=--==---===---->>>>> Setup <<<<----===---==--=-|*+~^~+*|-=--==---===---->>>>>");

  SPI.begin();
	SPI.setMOSI(kMosiPin);
	SPI.setSCK(kSpiClockPin);

  midiControl.midiSetup(&serialMidi, sequencerArray, noteData);

	serialMidi.setHandleClock( midiClockPulseHandlerWrapper );
  serialMidi.setHandleNoteOn( midiNoteOnHandlerWrapper );
  serialMidi.setHandleNoteOff( midiNoteOffHandlerWrapper );
  serialMidi.setHandleStart( midiStartContinueHandlerWrapper );
  serialMidi.setHandleContinue( midiStartContinueHandlerWrapper );
  serialMidi.setHandleStop(midiStopHandlerWrapper);

  timeControl.initialize(&serialMidi, &midiControl, noteData, sequencerArray);

	MasterClockTimer.begin(masterLoop,kClockInterval);
	SPI.usingInterrupt(MasterClockTimer);

  Serial.println("<<<--||-->>> Setup Complete <<<--||-->>>");
}

void loop() {
  timeControl.runLoopHandler();
}




// global wrapper to create pointer to ClockMaster member function
// https://isocpp.org/wiki/faq/pointers-to-members
void masterLoop(){
  timeControl.masterClockHandler();
}

// global wrappers to create pointers to MidiModule member functions
// https://isocpp.org/wiki/faq/pointers-to-members
void midiClockPulseHandlerWrapper(){
  midiControl.midiClockPulseHandler();
}

void midiNoteOnHandlerWrapper(byte channel, byte note, byte velocity){
  midiControl.midiNoteOnHandler(channel, note, velocity);
}

void midiNoteOffHandlerWrapper(byte channel, byte note, byte velocity){
  midiControl.midiNoteOffHandler(channel, note, velocity);
}

void midiStartContinueHandlerWrapper(){
  midiControl.midiStartContinueHandler();
}

void midiStopHandlerWrapper(){
  midiControl.midiStopHandler();
}
