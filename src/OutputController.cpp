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

  backplaneGPIO->initPort(4,1, OUTPUT); // Gate in 1
  backplaneGPIO->initPort(5,7, OUTPUT); // Gate input 2
  backplaneGPIO->initPort(6,11, OUTPUT); // Gate input 3
  backplaneGPIO->initPort(7,26, OUTPUT); // Gate input 4

  backplaneGPIO->initPort(8,27, OUTPUT); // latch reset

  backplaneGPIO->initPort(9,18, OUTPUT); // slew cap ctrl
  backplaneGPIO->initPort(10,19, OUTPUT); // slew cap ctrl
  backplaneGPIO->initPort(11,20, OUTPUT); // slew cap ctrl
  backplaneGPIO->initPort(12,21, OUTPUT); // slew cap ctrl
  backplaneGPIO->initPort(13,22, OUTPUT); // slew cap ctrl
  backplaneGPIO->initPort(14,23, OUTPUT); // slew cap ctrl
  backplaneGPIO->initPort(15,3, OUTPUT); // slew cap ctrl
  backplaneGPIO->initPort(16,24, OUTPUT); // slew cap ctrl

  backplaneGPIO->initPort(17, 2, OUTPUT);
  backplaneGPIO->initPort(18, 6, OUTPUT);
  backplaneGPIO->initPort(19, 10, OUTPUT);
  backplaneGPIO->initPort(20, 12, OUTPUT);
  backplaneGPIO->initPort(21, 13, OUTPUT);
  backplaneGPIO->initPort(22, 14, OUTPUT);
  backplaneGPIO->initPort(23, 15, OUTPUT);
  backplaneGPIO->initPort(24, 16, OUTPUT);
  backplaneGPIO->initPort(25, 17, OUTPUT);
  backplaneGPIO->initPort(26, 25, OUTPUT);
  backplaneGPIO->initPort(27, 0, OUTPUT);


  backplaneGPIO->updateGpioPinModes(); // send GPIO pin modes to chip


  backplaneGPIO->digitalWrite(0, 0);
  backplaneGPIO->digitalWrite(1, 0);
  backplaneGPIO->digitalWrite(2, 0);
  backplaneGPIO->digitalWrite(3, 0);
  backplaneGPIO->digitalWrite(8, 1);
  backplaneGPIO->digitalWrite(4, 1);
  backplaneGPIO->digitalWrite(5, 1);
  backplaneGPIO->digitalWrite(6, 1);
  backplaneGPIO->digitalWrite(7, 1);
  backplaneGPIO->digitalWrite(8, 1);
  backplaneGPIO->digitalWrite(9, 1);
  backplaneGPIO->digitalWrite(10, 1);
  backplaneGPIO->digitalWrite(11, 1);
  backplaneGPIO->digitalWrite(12, 1);
  backplaneGPIO->digitalWrite(13, 1);
  backplaneGPIO->digitalWrite(18, 1);
  backplaneGPIO->digitalWrite(14, 1);
  backplaneGPIO->digitalWrite(15, 1);
  backplaneGPIO->digitalWrite(16, 1);
  backplaneGPIO->digitalWrite(17, 1);
  backplaneGPIO->digitalWrite(18, 1);
  backplaneGPIO->digitalWrite(19, 1);
  backplaneGPIO->digitalWrite(20, 1);
  backplaneGPIO->digitalWrite(21, 1);
  backplaneGPIO->digitalWrite(22, 1);
  backplaneGPIO->digitalWrite(23, 1);
  backplaneGPIO->digitalWrite(28, 1);
  backplaneGPIO->digitalWrite(24, 1);
  backplaneGPIO->digitalWrite(25, 1);
  backplaneGPIO->digitalWrite(26, 1);
  backplaneGPIO->digitalWrite(27, 1);


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
/*  proto 8 basic calibration
10v - 64240
5v - 48570
0v - 32896
-5v - 17210
-10v  1540
*/
  Serial.println("begin note on");

  if (glide > 0) {
    backplaneGPIO->digitalWrite(outputMap(channel, SLEWSWITCHCV), HIGH);           // turn on switch with cap to ground, enable slew

    if (outputMap(channel, RHEOCHANNELCV) == 0){
      mcp4352_1.setResistance(outputMap(channel, CVRHEO), map(glide, 0,127,0,255) );        // set digipot to correct resistance, set slew rate
    } else {
      mcp4352_2.setResistance(outputMap(channel, CVRHEO), map(glide, 0,127,0,255));           // set digipot to correct resistance, set slew rate
    }

      Serial.println("setting glide to " + String(outputMap(channel, CVRHEO)) + "\ton mcp4352 " +  String(outputMap(channel, RHEOCHANNELCV)) + "\t with slew switch: " + String(outputMap(channel, SLEWSWITCHCV)) );

  } else {

    backplaneGPIO->digitalWrite(outputMap(channel, SLEWSWITCHCV), LOW);        // shut off swich with cap to ground, disable slew

    if (outputMap(channel, RHEOCHANNELCV) == 0){
      mcp4352_1.setResistance(outputMap(channel, CVRHEO), 0);        // set digipot to correct resistance, set slew rate
    } else {
      mcp4352_2.setResistance(outputMap(channel, CVRHEO), 0);           // set digipot to correct resistance, set slew rate
    }

  }
  delayMicroseconds(2);

  ad5676.setVoltage(dacCcMap[channel],  map(velocity, 0,127,1540, 64240 ) );  // set CC voltage
  MIDI.sendNoteOn(note, velocity, channel);                                   // send midi note out
  sam2695.noteOn(channel, note, velocity);                                    // set note on sound chip
  ad5676.setVoltage(dacCvMap[channel],  map(note, 0,127,32896, 64240 ) );    // set CV voltage
  ad5676.setVoltage(dacCvMap[channel],  map(note, 0,127,32896, 64240 ) );    // set CV voltage
  delayMicroseconds(10);

  backplaneGPIO->digitalWrite(channel, HIGH);                                 // open gate voltage
  Serial.println("end note on");
};

uint8_t OutputController::outputMap(uint8_t channel, uint8_t mapType){
  switch(mapType){
    case CVMAP:
      return dacCvMap[channel];
      break;

    case CCMAP:
      return dacCcMap[channel];
      break;

    case SLEWSWITCHCV:
      return slewSwitchMap[dacCvMap[channel]];
      break;

    case SLEWSWITCHCC:
      return slewSwitchMap[dacCcMap[channel]];
      break;

    case RHEOCHANNELCV:
      return rheoMap[dacCvMap[channel]] & 0x10 >> 1;
      break;

    case RHEOCHANNELCC:
      return rheoMap[dacCcMap[channel]] & 0x10 >> 1;
      break;

    case CVRHEO:
      return rheoMap[dacCvMap[channel]] & 0x01;
      break;

    case CCRHEO:
      return rheoMap[dacCcMap[channel]] & 0x01;
      break;
  }
};

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