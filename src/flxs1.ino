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
NoteDatum noteData[4];
Sequencer sequence[4];
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
  delay(100);

  printHeapStats();

  //AudioMemory(25);
  //notefreq.begin(.15);

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

  //usbMIDI.setHandleNoteOff(OnNoteOff)
  //usbMIDI.setHandleNoteOn(usbNoteOn);
  //usbMIDI.setHandleVelocityChange(OnVelocityChange)
  //usbMIDI.setHandleControlChange(OnControlChange)
  //usbMIDI.setHandleProgramChange(OnProgramChange)
  //usbMIDI.setHandleAfterTouch(OnAfterTouch)
  //usbMIDI.setHandlePitchChange(OnPitchChange)
  //usbMIDI.setHandleRealTimeSystem(usbMidiRealTimeMessageHandler);

  timeControl.initialize(&serialMidi, &midiControl, noteData, sequence, adc);
	MasterClockTimer.begin(masterLoop,kClockInterval);
	SPI.usingInterrupt(MasterClockTimer);

  Serial.println("<<<--||-->>> Setup Complete <<<--||-->>>");

  pinMode(31, OUTPUT); // debug pin - EXT_TX - exp pin 5
  pinMode(26, OUTPUT); // debug pin - EXT_RX - exp pin 6
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


//  adc->setConversionSpeed(ADC_LOW_SPEED); // change the conversion speed
  // it can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
  //adc->setSamplingSpeed(ADC_HIGH_SPEED); // change the sampling speed
  delay(100);

}

void loop() {
  //digitalWriteFast(26, HIGH);
  timeControl.runLoopHandler();

  //digitalWriteFast(26, LOW);
  if (millis()%1000 == 0){
    Serial.println("1:\t" + String(adc->analogRead(A3, ADC_1)) + "\t2:\t" + String(adc->analogRead(A12, ADC_1)) + "\t3:\t" + String(adc->analogRead(A13, ADC_1)) + "\t4:\t" + String(adc->analogRead(A10, ADC_1)));
  };

/*  if (!playing){
    if (notefreq.available()) {
    //  Serial
              frequency = notefreq.read();
              probability = notefreq.probability();
        //      Serial.println("Note: "+ String(frequency) + " | Probability: " + String(probability) + " mem use max: " + String(AudioMemoryUsageMax()));
    }
  }
  */
  //if (noteFreqTimer > 10000){
    //noteFreqTimer = 0;
//  }

}

void printHeapStats()
{
//  Serial.print("                  arena: ");Serial.println(mallinfo().arena);
//  Serial.print("  total allocated space: ");Serial.println(mallinfo().uordblks);
//  Serial.print("  total non-inuse space: ");Serial.println(mallinfo().fordblks);
//  Serial.print("   top releasable space: ");Serial.println(mallinfo().keepcost);
//  Serial.println("");
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
  digitalWriteFast(31, HIGH);
  usbMIDI.read();
  timeControl.masterClockHandler();
  digitalWriteFast(31, LOW);
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
