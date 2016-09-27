# 1 "/var/folders/hn/ch6s89_14b1fvg7y7pb4qlyr0000gn/T/tmpJ_YC_7"
#include <Arduino.h>
# 1 "/Users/tenkai/Desktop/Projects/flxs1/src/flxs1.ino"




#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <malloc.h>

#include "TimeController.h"
#include "Sequencer.h"
#include "midiModule.h"
#include "global.h"
#include "DisplayModule.h"

#define kSerialSpeed 115200
#define kClockInterval 400
#define kMosiPin 11
#define kSpiClockPin 13

TimeController timeControl;
IntervalTimer MasterClockTimer;

MidiModule midiControl;
NoteDatum noteData[4];
Sequencer sequence[4];
AudioInputAnalog adc(A14);
AudioAnalyzeNoteFrequency notefreq;
AudioConnection patchCord0(adc, 0 , notefreq, 0);
elapsedMillis noteFreqTimer;

MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, serialMidi);

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
#line 38 "/Users/tenkai/Desktop/Projects/flxs1/src/flxs1.ino"
void setup() {
  Serial.begin(9600);

  delay(1500);

  printHeapStats();
  AudioMemory(27);
  notefreq.begin(.15);

  Serial.println("<<<<<----===---==--=-|*+~^~+*|-=--==---===---->>>>> Setup <<<<----===---==--=-|*+~^~+*|-=--==---===---->>>>>");

  SPI.begin();
 SPI.setMOSI(kMosiPin);
 SPI.setSCK(kSpiClockPin);

  midiControl.midiSetup(&serialMidi, sequence, noteData);
# 71 "/Users/tenkai/Desktop/Projects/flxs1/src/flxs1.ino"
  timeControl.initialize(&serialMidi, &midiControl, noteData, sequence);
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
}

void loop() {
  digitalWriteFast(26, HIGH);
  timeControl.runLoopHandler();
  digitalWriteFast(26, LOW);
# 104 "/Users/tenkai/Desktop/Projects/flxs1/src/flxs1.ino"
}

void printHeapStats()
{
  Serial.print("                  arena: ");Serial.println(mallinfo().arena);
  Serial.print("  total allocated space: ");Serial.println(mallinfo().uordblks);
  Serial.print("  total non-inuse space: ");Serial.println(mallinfo().fordblks);
  Serial.print("   top releasable space: ");Serial.println(mallinfo().keepcost);
  Serial.println("");
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
# 174 "/Users/tenkai/Desktop/Projects/flxs1/src/flxs1.ino"
}