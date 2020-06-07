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
#include "globalVariable.h"

#define CPU_RESET_CYCLECOUNTER              \
  do                                        \
  {                                         \
    ARM_DEMCR |= ARM_DEMCR_TRCENA;          \
    ARM_DWT_CTRL |= ARM_DWT_CTRL_CYCCNTENA; \
    ARM_DWT_CYCCNT = 0;                     \
  } while (0)

//#define NUMLEDS  23

//DMAMEM int displayMemory[NUMLEDS*8];
//int drawingMemory[NUMLEDS*8];

//OctoSK6812 octoLeds(NUMLEDS, displayMemory, drawingMemory, SK6812_GRBW);
Sequencer sequence[SEQUENCECOUNT];

TimeController timeControl;

IntervalTimer MasterClockTimer;
IntervalTimer LEDClockTimer;
IntervalTimer SequencerTimer;
IntervalTimer midiClockTimer;
//IntervalTimer DisplayLoopTimer;
//IntervalTimer PeripheralLoopTimer;

MidiModule midiControl;

ADC *adc = new ADC(); // adc object

//AudioInputAnalog              adc0(A14);
//AudioAnalyzeNoteFrequency notefreq;

////      fft1024;        //xy=467,147
//AudioConnection               patchCord1(adc0 , notefreq);

// unsigned long    cyclesLast;
elapsedMicros cyclesTimer;
unsigned long clockCycles;
GlobalVariable globalObj;
uint8_t cycleIntervalCount;
MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, serialMidi);
char *sysexBuffer;
uint8_t sysexCounter;
bool sysexImportWriting = false;
elapsedMicros sysexTimer;

void setup()
{
  Serial.begin(kSerialSpeed);
  //waiting for serial to begin
  //  while (!Serial) ; // wait for serial monitor window to open
  //AudioMemory(24);
  //notefreq.begin(.15);
  // begin cycle counter
  CPU_RESET_CYCLECOUNTER;
  cycleIntervalCount = 0;
  cyclesTimer = 0;
  delay(500);

  Serial.println("<<<<<----===---==--=-|*+~^~+*|-=--==---===---->>>>> Setup <<<<----===---==--=-|*+~^~+*|-=--==---===---->>>>>");
  Serial.println("Sizeof Sequencer object: " + String(sizeof(sequence[0])));

  SPI.begin();
  SPI.setMOSI(kMosiPin);
  SPI.setSCK(kSpiClockPin);

  pinMode(7, INPUT);  //MIDI SERIAL PIN CONFIG
  pinMode(8, OUTPUT); //MIDI SERIAL PIN CONFIG
  Serial3.begin(31250);
  Serial3.clear();
  Serial3.setTX(8);
  Serial3.setRX(7);
  delay(500);
  serialMidi.begin(MIDI_CHANNEL_OMNI);
  serialMidi.turnThruOff();
  globalObj.initialize(adc);

  for (int i = 0; i < 1000; i++)
  {
    // Serial.println(globalObj.generateRandomNumber(0, 65535));
  }

  midiControl.midiSetup(sequence, &globalObj);

  serialMidi.setHandleClock(midiClockPulseHandlerWrapper);
  serialMidi.setHandleNoteOn(midiNoteOnHandlerWrapper);
  serialMidi.setHandleNoteOff(midiNoteOffHandlerWrapper);
  serialMidi.setHandleStart(midiStartContinueHandlerWrapper);
  serialMidi.setHandleContinue(midiStartContinueHandlerWrapper);
  serialMidi.setHandleStop(midiStopHandlerWrapper);

  usbMIDI.setHandleRealTimeSystem(usbMidiRealTimeMessageHandler);
  usbMIDI.setHandleNoteOn(midiNoteOnHandlerWrapper);
  usbMIDI.setHandleNoteOff(midiNoteOffHandlerWrapper);
  usbMIDI.setHandleSongPosition(midiSongPositionPointerWrapper);
  usbMIDI.setHandleTimeCodeQuarterFrame(midiTimeCodePointerWrapper);
  usbMIDI.setHandleSysEx(midiSysexHandlerWrapper);

  //usbMIDI.setHandleNoteOff(OnNoteOff)
  //usbMIDI.setHandleNoteOn(usbNoteOn);
  //usbMIDI.setHandleVelocityChange(OnVelocityChange)
  //usbMIDI.setHandleControlChange(OnControlChange)
  //usbMIDI.setHandleProgramChange(OnProgramChange)
  //usbMIDI.setHandleAfterTouch(OnAfterTouch)
  //usbMIDI.setHandlePitchChange(OnPitchChange)
  //usbMIDI.setHandleRealTimeSystem(usbMidiRealTimeMessageHandler);

  timeControl.initialize(&serialMidi, &midiControl, sequence, adc, &globalObj);

  //PeripheralLoopTimer.begin(peripheralLoop, kPeripheralLoopTimer);
  //PeripheralLoopTimer.priority(64);

  LEDClockTimer.begin(LEDLoop, kLedClockInterval);
  LEDClockTimer.priority(2);

  MasterClockTimer.begin(masterLoop, kMasterClockInterval);
  MasterClockTimer.priority(0);

  //  DisplayLoopTimer.begin(displayLoop,DISPLAY_INTERVAL);
  //  DisplayLoopTimer.priority(3);
  SequencerTimer.begin(sequencerLoop, kSequenceTimerInterval);
  SequencerTimer.priority(4);

  //SPI.usingInterrupt(PeripheralLoopTimer);
  SPI.usingInterrupt(SequencerTimer);
  //SPI.usingInterrupt(LEDClockTimer);

  midiClockTimer.begin(midiTimerLoop, kMidiClockInterval);
  //  MIDITimer.priority(0);

  //  LEDTimer.begin(ledLoop, kLEDTimerInterval);
  //  LEDTimer.priority(1);

  //CacheTimer.begin(cacheLoop,kCacheClockInterval);
  //CacheTimer.priority(2);

  pinMode(PIN_EXT_TX, OUTPUT); // debug pin - EXT_TX - exp pin 5
  pinMode(PIN_EXT_AD_1, OUTPUT);
  pinMode(PIN_EXT_AD_2, OUTPUT);
  pinMode(PIN_EXT_AD_3, OUTPUT);
  pinMode(PIN_EXT_AD_4, OUTPUT);
  pinMode(PIN_EXT_AD_5, OUTPUT);
  pinMode(PIN_EXT_AD_6, OUTPUT);
  pinMode(PIN_EXT_RX, OUTPUT);

  pinMode(3, OUTPUT);
  pinMode(24, OUTPUT);
  //digitalWrite(3, HIGH);  //INPUT 1 for HIHG HIGH
  //digitalWrite(24, HIGH);
  //pinMode(1,OUTPUT);
  //digitalWrite(1, LOW);

  //adc->enableInterrupts(ADC_1);
  adc->setAveraging(4, ADC_1);   // set number of averages
  adc->setResolution(16, ADC_1); // set bits of resolution
  //adc->setAveraging(8, ADC_0); // set number of averages
  //adc->setResolution(16, ADC_0); // set bits of resolution
  adc->setReference(ADC_REFERENCE::REF_3V3, ADC_1);
  //adc->setReference(ADC_REF_3V3, ADC_1);

  // it can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED_16BITS, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
  // see the documentation for more information
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED, ADC_1); // change the conversion speed
  // it can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED, ADC_1); // change the sampling speed
  // CLOCK PIN SETUP

  globalObj.setClockPortDirection(CLOCK_PORT_OUTPUT);

  pinMode(A3, INPUT);
  pinMode(A12, INPUT);
  pinMode(A13, INPUT);
  pinMode(A14, INPUT);
  pinMode(A10, INPUT);
  Serial.println("<<<--||-->>> Setup Complete <<<--||-->>>");
}

void loop()
{
  timeControl.runLoopHandler();
}

void usbNoteOff()
{
  //  Serial.println("note off!:\t" + String(note));
}

void usbNoteOn(byte channel, byte note, byte velocity)
{
  //Serial3.println("note on!:\t" + String(note));
  globalObj.playing = !globalObj.playing;
}

// global wrapper to create pointer to ClockMaster member function
// https://isocpp.org/wiki/faq/pointers-to-members
void sequencerLoop()
{
  usbMIDI.read();
  timeControl.sequencerHandler();
}

void masterLoop()
{
  timeControl.masterClockHandler();
}

void LEDLoop()
{
  timeControl.ledClockHandler();
}

void midiTimerLoop()
{
  //  usbMIDI.read();
  while (Serial3.available())
  {
    timeControl.midiClockHandler();
  }
}

void cacheLoop()
{
  timeControl.cacheWriteHandler();
}
// global wrappers to create pointers to MidiModule member functions
// https://isocpp.org/wiki/faq/pointers-to-members

void midiSysexHandlerWrapper(const byte *data, uint16_t length, bool last)
{
  if (!((globalObj.sysex_status == SYSEX_IMPORTING) || (globalObj.sysex_status == SYSEX_READYFORDATA)))
  {
    return;
  }

  if (globalObj.sysex_status == SYSEX_READYFORDATA)
  {
    globalObj.sysex_status = SYSEX_IMPORTING;
    globalObj.sysex_channel = 0;
    globalObj.sysex_pattern = 0;
    timeControl.runDisplayLoop();
  }

  if (sysexImportWriting == true)
  {
    Serial.println("------ ----- ---  Sysex messages are arriving too fast! --- ---- --- ----");
  }

  if (millis() < 6000)
  {
    return;
  }
  if (sysexCounter == 0)
  {
    sysexTimer = 0;
    timeControl.flashMemoryControl(1);
    // Serial.println(" -- ");
    // Serial.print(String(millis() ) + "Sysex Message. Buffer Allocated - Part 1 ");
    sysexBuffer = (char *)malloc(SECTORSIZE);
    if (data[0] == 0xF0)
    {
      *data++;
    }
    strcpy(sysexBuffer, (char *)data);
  }
  else
  {
    // Serial.print(String(sysexCounter +1) + " -" + String(sysexTimer) + "- ");
    strncat(sysexBuffer, (char *)data, length);
  }

  sysexCounter++;

  if (last)
  {
    // Serial.println(sysexBuffer);
    sysexTimer = 0;
    sysexImportWriting = true;
    timeControl.sysexMessageHandler(sysexBuffer);
    sysexImportWriting = false;
    sysexCounter = 0;
    // Serial.println(" Sysex Message Complete -- freeing buffer. took " + String(sysexTimer) + "micros" );
    free(sysexBuffer);
    // sysexBuffer = NULL;
  }
}
// void midiSysexHandlerWrapper(uint8_t *data, unsigned int size){
//
// }

void midiClockPulseHandlerWrapper()
{
  //  timeControl.setDebugPin(3, HIGH);
  if (globalObj.clockMode != EXTERNAL_MIDI_35_CLOCK)
    return;
  midiControl.midiClockPulseHandler();
  //  timeControl.setDebugPin(3, LOW);
}

void midiNoteOnHandlerWrapper(byte channel, byte note, byte velocity)
{
  midiControl.midiNoteOnHandler(channel, note, velocity);
}

void midiNoteOffHandlerWrapper(byte channel, byte note, byte velocity)
{
  midiControl.midiNoteOffHandler(channel, note, velocity);
}

void midiStartContinueHandlerWrapper()
{
  if (globalObj.clockMode != EXTERNAL_MIDI_35_CLOCK)
    return;
  midiControl.midiStartContinueHandler();
}

void midiStopHandlerWrapper()
{
  if (globalObj.clockMode != EXTERNAL_MIDI_35_CLOCK)
    return;
  midiControl.midiStopHandler();
}

void midiTimeCodePointerWrapper(uint8_t data)
{
  //Serial.println("MIDI TIME CODE: " + String(data));
}

void midiSongPositionPointerWrapper(uint16_t songPosition)
{
  //Serial.println("Song position Pointer: " + String(songPosition));
  midiControl.midiSongPosition(songPosition);
}

void usbMidiRealTimeMessageHandler(byte realtimebyte)
{
  //Serial.println("realTimeMessage!:\t" + String(realtimebyte));
  if (globalObj.clockMode != EXTERNAL_MIDI_USB_CLOCK)
    return;
  switch (realtimebyte)
  {
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

  if (realtimebyte == 248)
  {
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
