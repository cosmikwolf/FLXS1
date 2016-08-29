/* **********************************
*** ZETAOHM FLXS1 MUSIC SEQUENCER ***
********************************** */
#include <Arduino.h>
#include <SPI.h>
#include <i2c_t3.h>
#include <Audio.h>

#include "TimeController.h"
#include "Sequencer.h"
#include "midiModule.h"
#include "global.h"

#include "DisplayModule.h"

#define kSerialSpeed 115200
#define kClockInterval 1000
#define kMosiPin 11
#define kSpiClockPin 13

TimeController timeControl;
IntervalTimer MasterClockTimer;

MidiModule midiControl;
NoteDatum noteData[4];

AudioInputAnalog              adc(A7);
AudioAnalyzeNoteFrequency     notefreq;
AudioConnection               patchCord2(adc, 0 , notefreq, 0);
elapsedMillis noteFreqTimer;

MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, serialMidi);

void setup() {
  AudioMemory(30);
  notefreq.begin(.15);

  Serial.begin(kSerialSpeed);
  Serial3.begin(115200);
  //waiting for serial to begin
  delay(1500);
  Serial.println("<<<<<----===---==--=-|*+~^~+*|-=--==---===---->>>>> Setup <<<<----===---==--=-|*+~^~+*|-=--==---===---->>>>>");

  SPI.begin();
	SPI.setMOSI(kMosiPin);
	SPI.setSCK(kSpiClockPin);

  midiControl.midiSetup(&serialMidi, sequence, noteData);

//	serialMidi.setHandleClock( midiClockPulseHandlerWrapper );
//  serialMidi.setHandleNoteOn( midiNoteOnHandlerWrapper );
//  serialMidi.setHandleNoteOff( midiNoteOffHandlerWrapper );
//  serialMidi.setHandleStart( midiStartContinueHandlerWrapper );
//  serialMidi.setHandleContinue( midiStartContinueHandlerWrapper );
//  serialMidi.setHandleStop(midiStopHandlerWrapper);

  //usbMIDI.setHandleNoteOff(OnNoteOff)
  usbMIDI.setHandleNoteOn(usbNoteOn);
  //usbMIDI.setHandleVelocityChange(OnVelocityChange)
  //usbMIDI.setHandleControlChange(OnControlChange)
  //usbMIDI.setHandleProgramChange(OnProgramChange)
  //usbMIDI.setHandleAfterTouch(OnAfterTouch)
  //usbMIDI.setHandlePitchChange(OnPitchChange)
  usbMIDI.setHandleRealTimeSystem(usbMidiRealTimeMessageHandler);

  timeControl.initialize(&serialMidi, &midiControl, noteData);
	MasterClockTimer.begin(masterLoop,kClockInterval);
	SPI.usingInterrupt(MasterClockTimer);

  Serial.println("<<<--||-->>> Setup Complete <<<--||-->>>");
}

void loop() {
  timeControl.runLoopHandler();
/*  if (noteFreqTimer > 10000){
    if (notefreq.available()) {
        float note = notefreq.read();
        float prob = notefreq.probability();
        Serial.printf("Note: %3.2f | Probability: %.2f\n", note, prob);
    }
    noteFreqTimer = 0;
  }
  */
}


void usbNoteOff(){
//  Serial.println("note off!:\t" + String(note));
}

void usbNoteOn(byte channel, byte note, byte velocity){
  Serial3.println("note on!:\t" + String(note));
  playing = !playing;
}

// global wrapper to create pointer to ClockMaster member function
// https://isocpp.org/wiki/faq/pointers-to-members
void masterLoop(){
  usbMIDI.read();
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

void usbMidiRealTimeMessageHandler(byte realtimebyte) {
  Serial.println("realTimeMessage!:\t" + String(realtimebyte));
  if (realtimebyte == 248) {
    midiControl.midiClockPulseHandler();
  };
  //switch(realtimebyte){
  //  case MIDI_CLOCK:
  //    midiControl.midiClockPulseHandler();
  //    break;
  //  case MIDI_START:
  //    midiControl.midiStartContinueHandler();
  //    break;
  //  case MIDI_CONTINE:
  //    midiControl.midiStartContinueHandler();
  //    break;
  //  case MIDI_STOP:
  //    midiControl.midiStopHandler();
  //    break;
  //}
}
