/* **********************************
*** ZETAOHM FLXS1 MUSIC SEQUENCER ***
********************************** */

#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <malloc.h>
#include <ADC.h>
#include "TimeController.h"
#include "OutputController.h"
#include "Sequencer.h"
#include "midiModule.h"
#include "global.h"
#include "DisplayModule.h"


TimeController timeControl;
IntervalTimer MasterClockTimer;

MidiModule midiControl;
Sequencer sequence[SEQUENCECOUNT];

AudioInputAnalog              audio_adc(A14);
AudioAnalyzeNoteFrequency     notefreq;
AudioConnection               patchCord0(audio_adc, 0 , notefreq, 0);
elapsedMillis noteFreqTimer;

ADC *adc = new ADC(); // adc object

MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, serialMidi);

uint8_t masterLooptime;
elapsedMicros masterLoopTimer;

void setup() {
  Serial.begin(kSerialSpeed);
  //waiting for serial to begin
//  while (!Serial) ; // wait for serial monitor window to open
  AudioMemory(25);
  notefreq.begin(.15);
  //delay(1500);

  Serial.println("<<<<<----===---==--=-|*+~^~+*|-=--==---===---->>>>> Setup <<<<----===---==--=-|*+~^~+*|-=--==---===---->>>>>");
  Serial.println("Sizeof Sequencer object: " + String(sizeof(sequence[0])));

  SPI.begin();
	SPI.setMOSI(kMosiPin);
	SPI.setSCK(kSpiClockPin);

  serialMidi.begin(MIDI_CHANNEL_OMNI);

  midiControl.midiSetup(sequence);

	serialMidi.setHandleClock( midiClockPulseHandlerWrapper );
  serialMidi.setHandleNoteOn( midiNoteOnHandlerWrapper );
  serialMidi.setHandleNoteOff( midiNoteOffHandlerWrapper );
  serialMidi.setHandleStart( midiStartContinueHandlerWrapper );
  serialMidi.setHandleContinue( midiStartContinueHandlerWrapper );
  serialMidi.setHandleStop(midiStopHandlerWrapper);

  usbMIDI.setHandleRealTimeSystem( usbMidiRealTimeMessageHandler );
  usbMIDI.setHandleNoteOn( midiNoteOnHandlerWrapper );
  usbMIDI.setHandleNoteOff( midiNoteOffHandlerWrapper );

  //usbMIDI.setHandleNoteOff(OnNoteOff)
  //usbMIDI.setHandleNoteOn(usbNoteOn);
  //usbMIDI.setHandleVelocityChange(OnVelocityChange)
  //usbMIDI.setHandleControlChange(OnControlChange)
  //usbMIDI.setHandleProgramChange(OnProgramChange)
  //usbMIDI.setHandleAfterTouch(OnAfterTouch)
  //usbMIDI.setHandlePitchChange(OnPitchChange)
  //usbMIDI.setHandleRealTimeSystem(usbMidiRealTimeMessageHandler);

  timeControl.initialize(&serialMidi, &midiControl, sequence, adc);

  MasterClockTimer.begin(masterLoop,kMasterClockInterval);
  MasterClockTimer.priority(0);

//  MIDITimer.begin(midiTimerLoop,kMidiClockInterval);
//  MIDITimer.priority(0);

//  LEDTimer.begin(ledLoop, kLEDTimerInterval);
//  LEDTimer.priority(1);

  //CacheTimer.begin(cacheLoop,kCacheClockInterval);
  //CacheTimer.priority(2);

  SPI.usingInterrupt(MasterClockTimer);

  Serial.println("<<<--||-->>> Setup Complete <<<--||-->>>");

  pinMode(31, OUTPUT); // debug pin - EXT_TX - exp pin 5
  pinMode(26, OUTPUT); // debug pin - EXT_RX - exp pin 6
  pinMode(3, OUTPUT);
  pinMode(24,OUTPUT);
  digitalWrite(3, LOW);
  digitalWrite(24, LOW);
  Serial.println("Freeram: " + String(FreeRam2()));

  pinMode(1,OUTPUT);
  digitalWrite(1, HIGH);

  adc->enableInterrupts(ADC_0);

  pinMode(A3, INPUT);
  pinMode(A12, INPUT);
  pinMode(A13, INPUT);
  pinMode(A10, INPUT);


//  adc->setConversionSpeed(ADC_LOW_SPEED); // change the conversion speed
  // it can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
  //adc->setSamplingSpeed(ADC_HIGH_SPEED); // change the sampling speed
  delay(100);
}

void loop() {
  timeControl.runLoopHandler();
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
  timeControl.midiClockHandler();
  timeControl.masterClockHandler();
}

void midiTimerLoop(){
//  usbMIDI.read();
  //timeControl.midiClockHandler();
}

void cacheLoop(){
  timeControl.cacheWriteHandler();
}
// global wrappers to create pointers to MidiModule member functions
// https://isocpp.org/wiki/faq/pointers-to-members


void midiClockPulseHandlerWrapper(){
//  timeControl.setDebugPin(3, HIGH);
  midiControl.midiClockPulseHandler();
//  timeControl.setDebugPin(3, LOW);
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
//  Serial.println("realTimeMessage!:\t" + String(realtimebyte));
  switch(realtimebyte){
    case 248:
    midiControl.midiClockPulseHandler();
    break;
    case 250:
    midiControl.midiStartContinueHandler();
    break;
    case 251:
    midiControl.midiStartContinueHandler();
    break;
    case 252:
    midiControl.midiStopHandler();
    break;
  }
  if (realtimebyte == 248) {
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
