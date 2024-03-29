#include <Arduino.h>

#include "midiModule.h"

void MidiModule::midiSetup(Sequencer *sequenceArray, GlobalVariable *globalObj)
{ // backplane is for debug purposes only
  this->sequenceArray = sequenceArray;
  this->globalObj = globalObj;
  //  this->noteData = noteData;
  firstRun = false;
  pulseTimer = 0;
  for (int i = 0; i < 10; i++)
  {
    midiTestArray[i] = false;
  }
}

void MidiModule::midiClockSyncFunc(midi::MidiInterface<HardwareSerial> *serialMidi)
{
  //noInterrupts();
  // if(  ){
  //  Serial.println("1: " + String(serialMidi->getData1()));
  //  Serial.println("2: " + String(serialMidi->getData2()));
  // }
  while(Serial3.available()){
    // digitalWriteFast(PIN_EXT_AD_4, HIGH);
    serialMidi->read();
    // digitalWriteFast(PIN_EXT_AD_4, LOW);
  }
  //interrupts();
}

void MidiModule::midiStopHandler()
{
  if (globalObj->clockMode == EXTERNAL_MIDI_35_CLOCK || globalObj->clockMode == EXTERNAL_MIDI_USB_CLOCK)
  {
    Serial.println("Midi Stop");
    if (globalObj->playing == 0)
    {
      midiClockCount = 0;
      globalObj->midiSetClockOut = false;
      for (int i = 0; i < SEQUENCECOUNT; i++)
      {
        sequenceArray[i].clockReset(true);
      }
    }
    globalObj->playing = 0;
  }
}

void MidiModule::midiNoteOffHandler(byte channel, byte note, byte velocity)
{
  //  Serial.println(String("Note Off: ch=") + channel + ", note=" + note + ", velocity=" + velocity);
}

void MidiModule::midiNoteOnHandler(byte channel, byte note, byte velocity)
{
  bool exitTestLoop;
  if (stepMode == STATE_TEST_MIDI)
  {
    if (midiTestActive == false)
    {
      for (int i = 0; i < 128; i++)
      {
        midiTestArray[i] = false;
      }
    }
    // Serial.println("Recieved test note " + String(note));
    if (note < 128)
    {
      midiTestArray[note] = true;
    }
    exitTestLoop = true;
    for (int n = 0; n < 128; n++)
    {
      if (midiTestArray[n] == false)
      {
        exitTestLoop = false;
      }
    }
    if (exitTestLoop)
    {
      midiTestActive = false;
      // Serial.println("midi Test loop complete");
    }
  }
  else
  {
    if (velocity > 0)
    {
      // Serial.println(String("Note On:  ch=") + channel + ", note=" + note + ", velocity=" + velocity);
    }
    else
    {
      // Serial.println(String("Note Off: ch=") + channel + ", note=" + note);
    }
  }
}

void MidiModule::midiStartContinueHandler()
{
  if (globalObj->clockMode == EXTERNAL_MIDI_35_CLOCK || globalObj->clockMode == EXTERNAL_MIDI_USB_CLOCK)
  {
    Serial.println("Midi Start / Continue");
    globalObj->playing = 1;
  }
}

void MidiModule::midiSongPosition(int songPosition)
{
  if (songPosition == 0)
  {
    midiClockCount = 0;
    globalObj->midiSetClockOut = false;
    for (int i = 0; i < SEQUENCECOUNT; i++)
    {
      sequenceArray[i].clockReset(true);
    }
  }
};

void MidiModule::midiClockPulseHandler()
{
  if (globalObj->clockMode != EXTERNAL_MIDI_35_CLOCK && globalObj->clockMode != EXTERNAL_MIDI_USB_CLOCK)
  {
    return; // no need to run clock pulse handler if using internal clock.
  }

  if (globalObj->playing)
  {
    if (midiClockCount < 4)
    {
      globalObj->midiSetClockOut = true;
    }
    else
    {
      globalObj->midiSetClockOut = false;
    }
    midiClockCount = (midiClockCount + 1) % MIDI_PULSE_COUNT;

    globalObj->chainModeMasterPulseToGo--;
    if ((globalObj->chainModeMasterPulseToGo <= 0) && globalObj->waitingToResetAfterPatternLoad)
    {
      for (int i = 0; i < SEQUENCECOUNT; i++)
      {
        sequenceArray[i].clockReset(true);
      }
      globalObj->waitingToResetAfterPatternLoad = false;
    }

    for (int i = 0; i < SEQUENCECOUNT; i++)
    {
      sequenceArray[i].ppqPulse(MIDI_PULSE_COUNT);
    }
  }
}
