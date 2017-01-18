# 1 "/var/folders/m8/cnc3yvx10kvfldn86j24dmkm0000gn/T/tmp4agglS"
#include <Arduino.h>
# 1 "/Users/tenkai/Projects/FLXS1/src/flxs1.ino"




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
#define kMasterClockInterval 750
#define kMidiClockInterval 200
#define kCacheClockInterval 10000
#define kMosiPin 11
#define kSpiClockPin 13

TimeController timeControl;
IntervalTimer MasterClockTimer;
IntervalTimer CacheTimer;
IntervalTimer MIDITimer;

MidiModule midiControl;
NoteDatum noteData[4];
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
void midiTimerLoop();
void cacheLoop();
void midiClockPulseHandlerWrapper();
void midiNoteOnHandlerWrapper(byte channel, byte note, byte velocity);
void midiNoteOffHandlerWrapper(byte channel, byte note, byte velocity);
void midiStartContinueHandlerWrapper();
void midiStopHandlerWrapper();
void usbMidiRealTimeMessageHandler(byte realtimebyte);
#line 44 "/Users/tenkai/Projects/FLXS1/src/flxs1.ino"
void setup() {
  Serial.begin(kSerialSpeed);



  printHeapStats();

  AudioMemory(25);
  notefreq.begin(.15);


  Serial.println("<<<<<----===---==--=-|*+~^~+*|-=--==---===---->>>>> Setup <<<<----===---==--=-|*+~^~+*|-=--==---===---->>>>>");

  Serial.println("Sizeof Sequencer object: " + String(sizeof(sequence[0])));

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

  usbMIDI.setHandleRealTimeSystem( usbMidiRealTimeMessageHandler );
  usbMIDI.setHandleNoteOn( midiNoteOnHandlerWrapper );
  usbMIDI.setHandleNoteOff( midiNoteOffHandlerWrapper );
# 87 "/Users/tenkai/Projects/FLXS1/src/flxs1.ino"
  timeControl.initialize(&serialMidi, &midiControl, noteData, sequence, adc);

  MasterClockTimer.begin(masterLoop,kMasterClockInterval);
  MasterClockTimer.priority(1);

  MIDITimer.begin(midiTimerLoop,kMidiClockInterval);
  MIDITimer.priority(0);







  SPI.usingInterrupt(MasterClockTimer);
  SPI.usingInterrupt(MIDITimer);


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
# 155 "/Users/tenkai/Projects/FLXS1/src/flxs1.ino"
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
  usbMIDI.read();
  timeControl.masterClockHandler();
  usbMIDI.read();
}

void midiTimerLoop(){
  usbMIDI.read();
  timeControl.midiClockHandler();
}

void cacheLoop(){
  timeControl.cacheWriteHandler();
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
# 247 "/Users/tenkai/Projects/FLXS1/src/flxs1.ino"
}