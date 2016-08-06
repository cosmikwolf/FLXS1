#include <Arduino.h>
#include "OutputController.h"

void OutputController::initialize(Zetaohm_MAX7301* backplaneGPIO){
  Serial.println("Initializing SAM2695");

  // PUT STUFF LIKE THIS INSIDE CONSTRUCTORS
  sam2695.begin();
  sam2695.programChange(0, 0, 38);       // give our two channels different voices
  sam2695.programChange(0, 1, 30);
  sam2695.programChange(0, 2, 128);       // give our two channels different voices
  sam2695.programChange(0, 3, 29);
  // PUT STUFF LIKE THIS INSIDE CONSTRUCTORS
  Serial.println("initializing gate outputs");
  this->backplaneGPIO = backplaneGPIO;
  backplaneGPIO->begin(BACKPLANE_MAX7301_CS_PIN);
  backplaneGPIO->initPort(0,8, OUTPUT); // gate out ch 1
  backplaneGPIO->initPort(1,4, OUTPUT); // gate out ch 2
  backplaneGPIO->initPort(2,9, OUTPUT); // gate out ch 3
  backplaneGPIO->initPort(3,5, OUTPUT); // gate out ch 4
  backplaneGPIO->initPort(4,1, INPUT); // Gate in 1
  backplaneGPIO->initPort(5,7, INPUT); // Gate input 2
  backplaneGPIO->initPort(6,11, INPUT); // Gate input 3
  backplaneGPIO->initPort(7,26, INPUT); // Gate input 4
  backplaneGPIO->initPort(8,27, INPUT); // latch reset
  backplaneGPIO->updateGpioPinModes(); //this needs to be fixed for gate inputs. needs mask

  backplaneGPIO->digitalWrite(0, 1);
  backplaneGPIO->digitalWrite(1, 1);
  backplaneGPIO->digitalWrite(2, 1);
  backplaneGPIO->digitalWrite(3, 1);
  backplaneGPIO->digitalWrite(0, 0);
  backplaneGPIO->digitalWrite(1, 0);
  backplaneGPIO->digitalWrite(2, 0);
  backplaneGPIO->digitalWrite(3, 0);

  backplaneGPIO->digitalWrite(8, 1);
  backplaneGPIO->digitalWrite(8, 0);
  backplaneGPIO->digitalWrite(4, 0);
  backplaneGPIO->digitalWrite(5, 0);
  backplaneGPIO->digitalWrite(6, 0);
  backplaneGPIO->digitalWrite(7, 0);

  Serial.println("Initializing DAC");
  // PUT STUFF LIKE THIS INSIDE CONSTRUCTORS
  ad5676.begin(AD5676_CSPIN);
  delay(10);

  mcp4352_1.initialize(19);
  delay(10);

  mcp4352_2.initialize(18);
delay(10);

for (int i=0; i<4; i++){
  Serial.println("Resistance 1: " + String(i) + "- " + String(mcp4352_1.readResistance(i))); delay(1);
  Serial.println("Resistance 2: " + String(i) + "- " + String(mcp4352_2.readResistance(i))); delay(1);
  delay(1);
  mcp4352_1.setResistance(i, 1); delay(1);
  mcp4352_2.setResistance(i, 1); delay(1);
  delay(1);
  Serial.println("Resistance 1: " + String(i) + "- " + String(mcp4352_1.readResistance(i))); delay(1);
  Serial.println("Resistance 2: " + String(i) + "- " + String(mcp4352_2.readResistance(i))); delay(1);
  delay(1);
  mcp4352_1.setResistance(i, 255);  delay(1);
  mcp4352_2.setResistance(i, 255);  delay(1);
  delay(1);
  Serial.println("Resistance 1: " + String(i) + "- " + String(mcp4352_1.readResistance(i))); delay(1);
  Serial.println("Resistance 2: " + String(i) + "- " + String(mcp4352_2.readResistance(i))); delay(1);
}

  Serial.println("Initializing MIDI");
  midiSetup();

  Serial.println("Setting up debug pin");
  pinMode(DEBUG_PIN, OUTPUT);
  pinMode(4, OUTPUT);

  Serial.println("Output Controller Initialization Complete");
}

void OutputController::dacTestLoop(){
  for (int i=0; i<8; i++){
    ad5676.setVoltage(i, 65000 );
    delay(1);
    ad5676.setVoltage(i, 32000 );
    delay(2);
    ad5676.setVoltage(i, 0 );
    delay(2);
  }
}

void OutputController::noteOn(uint8_t channel, uint8_t note, uint8_t velocity, uint8_t glide){
  // proto 6 calibration numbers: 0v: 22180   5v: 43340
//  Serial.println("    OutputController -- on ch:"  + String(channel) + " nt: " + String(note) );
/*
10v - 64240
5v - 48570
0v - 32896
-5v - 17210
-10v  1540
rheostat mapping
mcp4352_1
  0 - 7
  1 - 1
  2 - 6
  3 - 0
mcp4352_2
  0 - 4
  1 - 3
  2 - 2
  3 - 5
*/
  Serial.println("begin note on");

  if (glide > 0) {
    backplaneGPIO->digitalWrite(channel, HIGH);
  } else {
    backplaneGPIO->digitalWrite(channel, LOW);
  }



  mcp4352_1.setResistance(, 255);  delay(1);
  mcp4352_2.setResistance(, 255);  delay(1);

  ad5676.setVoltage(dacCvMap[channel],  map(note, 0,127,32896, 64240 ) );
  MIDI.sendNoteOn(note, velocity, channel);
  sam2695.noteOn(channel, note, velocity);
  ad5676.setVoltage(dacCcMap[channel],  map(velocity, 0,127,1540, 64240 ) );
  backplaneGPIO->digitalWrite(channel, HIGH);
  Serial.println("end note on");
}

void OutputController::noteOff(uint8_t channel, uint8_t note){
//  Serial.println("    OutputController -- off ch:"  + String(channel) + " nt: " + String(note) );
  backplaneGPIO->digitalWrite(channel, LOW);
  MIDI.sendNoteOff(note, 64, channel);
  sam2695.noteOff(channel, note);
}

void OutputController::allNotesOff(uint8_t channel){
    sam2695.allNotesOff(channel);
    backplaneGPIO->digitalWrite(channel, LOW);
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

void OutputController::calibrationRoutine(){
  for (int i=0; i<8; i++){
    ad5676.setVoltage(i,calibrationBuffer);
    delay(1);
  }
}
