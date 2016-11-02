# 1 "/var/folders/hn/ch6s89_14b1fvg7y7pb4qlyr0000gn/T/tmpQY0lwO"
#include <Arduino.h>
# 1 "/Users/tenkai/Desktop/Projects/flxs1/src/flxs1.ino"




#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <malloc.h>
#include <ADC.h>
#include "TimeController.h"
#include "Sequencer.h"
#include "midiModule.h"
#include "global.h"
#include "DisplayModule.h"

#define kSerialSpeed 115200
#define kClockInterval 600
#define kMosiPin 11
#define kSpiClockPin 13

TimeController timeControl;
IntervalTimer MasterClockTimer;

MidiModule midiControl;
NoteDatum noteData[SEQUENCECOUNT];
Sequencer sequence[SEQUENCECOUNT];
AudioInputAnalog audio_adc(A14);
AudioAnalyzeNoteFrequency notefreq;
AudioConnection patchCord0(audio_adc, 0 , notefreq, 0);
elapsedMillis noteFreqTimer;

ADC *adc = new ADC();

MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, serialMidi);

uint8_t masterLooptime;
elapsedMicros masterLoopTimer;
void setup();
void loop();
void printHeapStats();
void usbNoteOff();
void usbNoteOn(byte channel, byte note, byte velocity);
void masterLoop();
void midiClockPulseHandlerWrapper();
void midiNoteOnHandlerWrapper(byte channel, byte note, byte velocity);
void midiNoteOffHandlerWrapper(byte channel, byte note, byte velocity);
void midiStartContinueHandlerWrapper();
void midiStopHandlerWrapper();
void usbMidiRealTimeMessageHandler(byte realtimebyte);
#line 40 "/Users/tenkai/Desktop/Projects/flxs1/src/flxs1.ino"
void setup() {
  Serial.begin(kSerialSpeed);

  delay(100);

  printHeapStats();




  Serial.println("<<<<<----===---==--=-|*+~^~+*|-=--==---===---->>>>> Setup <<<<----===---==--=-|*+~^~+*|-=--==---===---->>>>>");

  SPI.begin();
 SPI.setMOSI(kMosiPin);
 SPI.setSCK(kSpiClockPin);

  serialMidi.begin(MIDI_CHANNEL_OMNI);

  midiControl.midiSetup(sequence, noteData);

 serialMidi.setHandleClock( midiClockPulseHandlerWrapper );
  serialMidi.setHandleNoteOn( midiNoteOnHandlerWrapper );
  serialMidi.setHandleNoteOff( midiNoteOffHandlerWrapper );
  serialMidi.setHandleStart( midiStartContinueHandlerWrapper );
  serialMidi.setHandleContinue( midiStartContinueHandlerWrapper );
  serialMidi.setHandleStop(midiStopHandlerWrapper);
# 76 "/Users/tenkai/Desktop/Projects/flxs1/src/flxs1.ino"
  timeControl.initialize(&serialMidi, &midiControl, noteData, sequence, adc);
 MasterClockTimer.begin(masterLoop,kClockInterval);
 SPI.usingInterrupt(MasterClockTimer);

  Serial.println("<<<--||-->>> Setup Complete <<<--||-->>>");

  pinMode(31, OUTPUT);
  pinMode(26, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(24,OUTPUT);
  digitalWrite(3, LOW);
  digitalWrite(24, LOW);
  printHeapStats();
  Serial.println("Freeram: " + String(FreeRam2()));

  pinMode(1,OUTPUT);
  digitalWrite(1, HIGH);

  adc->enableInterrupts(ADC_0);

  pinMode(A3, INPUT);
  pinMode(A12, INPUT);
  pinMode(A13, INPUT);
  pinMode(A10, INPUT);





  delay(100);

}

void loop() {

  timeControl.runLoopHandler();


  if (millis()%1000 == 0){
    Serial.println("1:\t" + String(adc->analogRead(A3, ADC_1)) + "\t2:\t" + String(adc->analogRead(A12, ADC_1)) + "\t3:\t" + String(adc->analogRead(A13, ADC_1)) + "\t4:\t" + String(adc->analogRead(A10, ADC_1)));
  };
# 131 "/Users/tenkai/Desktop/Projects/flxs1/src/flxs1.ino"
}

void printHeapStats()
{





}

void usbNoteOff(){

}

void usbNoteOn(byte channel, byte note, byte velocity){
  Serial3.println("note on!:\t" + String(note));
  playing = !playing;
}



void masterLoop(){
  digitalWriteFast(31, HIGH);
  usbMIDI.read();
  timeControl.masterClockHandler();
  digitalWriteFast(31, LOW);
}



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
# 201 "/Users/tenkai/Desktop/Projects/flxs1/src/flxs1.ino"
}