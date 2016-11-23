/* **********************************
*** ZETAOHM FLXS1 MUSIC SEQUENCER ***
********************************** */
//#include <Arduino.h>
#include <Audio.h>
#include <SPI.h>
#include <Wire.h>
/*#include "TimeController.h"
#include "Sequencer.h"
#include "midiModule.h"
#include "global.h"
#include "DisplayModule.h"
*/
#define kSerialSpeed 115200
#define kClockInterval 500
#define kMosiPin 11
#define kSpiClockPin 13

/*TimeController timeControl;
IntervalTimer MasterClockTimer;
IntervalTimer frequencyAnalysisTimer;

MidiModule midiControl;
NoteDatum noteData[4];
*/

AudioInputAnalog              adc(A7);
AudioAnalyzeNoteFrequency     notefreq;
AudioMixer4                   mixer;
AudioAnalyzePeak              peak;
AudioOutputAnalog             dac;
AudioConnection               patchCord1(adc, 0, mixer, 0);
AudioConnection               patchCord2(mixer, 0 , notefreq, 0);
AudioConnection               patchCord3(mixer, 0, peak, 0);
//AudioConnection               patchCord3(mixer,0, peak,0);
AudioConnection               patchCord4(mixer, 0, dac, 0);

AudioAnalyzeFFT1024    myFFT;
AudioConnection patchCord5(adc, 0, myFFT, 0);


//MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, serialMidi);

void setup() {
  AudioMemory(30);
  //notefreq.begin(.15);
  mixer.gain(0, 1.0);
  //adc.setAnalogReference(EXTERNAL);
  dac.analogReference(EXTERNAL);
  Serial.begin(kSerialSpeed);
  //Serial3.begin(31250);
  //waiting for serial to begin
//  delay(1500);
//  Serial.println("<<<<<----===---==--=-|*+~^~+*|-=--==---===---->>>>> Setup <<<<----===---==--=-|*+~^~+*|-=--==---===---->>>>>");
/*
  SPI.begin();
	SPI.setMOSI(kMosiPin);
	SPI.setSCK(kSpiClockPin);

midiControl.midiSetup(&serialMidi, sequence, noteData);

  serialMidi.setHandleNoteOn( midiNoteOnHandlerWrapper );
  serialMidi.setHandleNoteOff( midiNoteOffHandlerWrapper );
  serialMidi.setHandleClock( midiClockPulseHandlerWrapper );
  serialMidi.setHandleStart( midiStartContinueHandlerWrapper );
  serialMidi.setHandleContinue( midiStartContinueHandlerWrapper );
  serialMidi.setHandleStop( midiStopHandlerWrapper );
*/
  //usbMIDI.setHandleNoteOff(OnNoteOff)
  //usbMIDI.setHandleNoteOn(OnNoteOn)
  //usbMIDI.setHandleVelocityChange(OnVelocityChange)
  //usbMIDI.setHandleControlChange(OnControlChange)
  //usbMIDI.setHandleProgramChange(OnProgramChange)
  //usbMIDI.setHandleAfterTouch(OnAfterTouch)
  //usbMIDI.setHandlePitchChange(OnPitchChange)
  //usbMIDI.setHandleRealTimeSystem(usbMidiRealTimeMessageHandler);


//  timeControl.initialize(&serialMidi, &midiControl, noteData);
//	MasterClockTimer.begin(masterLoop,kClockInterval);
//  frequencyAnalysisTimer.begin(frequencyAnalysisLoop, 20000);
//	SPI.usingInterrupt(MasterClockTimer);

Serial.println("<<<--||-->>> Setup Complete <<<--||-->>>");
}

void loop() {
//  timeControl.runLoopHandler();
float n;
int i;


if (myFFT.available()) {
  // each time new FFT data is available
  // print it all to the Arduino Serial Monitor
  Serial.print("FFT: ");
  for (i=0; i<40; i++) {
    n = myFFT.read(i);
    if (n >= 0.01) {
      Serial.print(n);
      Serial.print(" ");
    } else {
      Serial.print("  -  "); // don't print "0.00"
    }
  }
  Serial.println();
}
/*
if (notefreq.available()) {
    float note = notefreq.read();
    float prob = notefreq.probability();
    Serial.printf("Note: %3.2f | Probability: %.2f\n", note, prob);
}


if (peak.available()) {
  int monoPeak = peak.read() * 30.0;
  Serial.print("|");
  for (int cnt=0; cnt<monoPeak; cnt++) {
    Serial.print(">");
  }
  Serial.println();
}
*/
}


void frequencyAnalysisLoop(){
/*
  if (peak.available()) {
    int monoPeak = peak.read() * 30.0;
    Serial.print("|");
    for (int cnt=0; cnt<monoPeak; cnt++) {
      Serial.print(">");
    }
    Serial.println();
  }
*/
}
/*
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

void usbMidiRealTimeMessageHandler(byte realtimebyte) {
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
*/
