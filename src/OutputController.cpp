#include <Arduino.h>
#include "OutputController.h"

void OutputController::initialize(){
  Serial.println("Initializing SAM2695");
  Zetaohm_SAM2695 sam2695;
  Zetaohm_AD5676 ad5676;
  Adafruit_MCP23017 mcp;

  // PUT STUFF LIKE THIS INSIDE CONSTRUCTORS
  sam2695.begin();
  sam2695.programChange(0, 0, 38);       // give our two channels different voices
  sam2695.programChange(0, 1, 30);
  sam2695.programChange(0, 2, 128);       // give our two channels different voices
  sam2695.programChange(0, 3, 29);
  // PUT STUFF LIKE THIS INSIDE CONSTRUCTORS
  Serial.println("initializing gate outputs");
  mcp.begin(1);      // use default address 0
  mcp.pinMode(9, INPUT);
  mcp.pinMode(4, OUTPUT);
  mcp.pinMode(5, OUTPUT);
  mcp.pinMode(6, OUTPUT);
  mcp.pinMode(7, OUTPUT);
  Serial.println("Initializing DAC");
  // PUT STUFF LIKE THIS INSIDE CONSTRUCTORS
  ad5676.begin(3);
  ad5676.softwareReset();
  delay(1);
  ad5676.internalReferenceEnable(true);
  ad5676.internalReferenceEnable(true);

  Serial.println("Initializing MIDI");
  midiSetup();

  Serial.println("Setting up debug pin");
  pinMode(DEBUG_PIN, OUTPUT);
  pinMode(4, OUTPUT);

  Serial.println("Initializing Flash Memory");
  // PUT STUFF LIKE THIS INSIDE CONSTRUCTORS
  saveFile.initialize();
  Serial.println("Output Controller Initialization Complete");
}

void OutputController::noteOn(uint8_t channel, uint8_t note, uint8_t velocity){
  // proto 6 calibration numbers: 0v: 22180   5v: 43340
  ad5676.setVoltage(dacCvMap[channel],  map(note, 0,127,13716, 58504 ) );
  MIDI.sendNoteOn(note, velocity, channel);
  sam2695.noteOn(channel, note, velocity);
  ad5676.setVoltage(dacCcMap[channel],  map(note, 0,127,0, 43340 ) );
  mcp.digitalWrite(gateMap[channel], HIGH);
}

void OutputController::noteOff(uint8_t channel, uint8_t note){
  mcp.digitalWrite(gateMap[channel], LOW);
  MIDI.sendNoteOff(note, 64, channel);
  sam2695.noteOff(channel, note);
}

void OutputController::allNotesOff(uint8_t channel){
    sam2695.allNotesOff(channel);
    mcp.digitalWrite(channel+4, LOW);
}

void OutputController::samCommand(uint8_t command, uint8_t channel, uint8_t value){
  switch(command){
    case PROGRAM_CHANGE:
      sam2695.programChange(0, channel, value);
    break;

    case SET_CHANNEL_VOLUME:
      sam2695.setChannelVolume(channel, value);
    break;

    case SET_CHANNEL_BANK:
      sam2695.setChannelBank(channel, value);
    break;
  }
}
