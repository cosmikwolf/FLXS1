#include <Arduino.h>
#include "OutputController.h"

void OutputController::initialize(Zetaohm_MAX7301* backplaneGPIO, midi::MidiInterface<HardwareSerial>* serialMidi, ADC *adc){
  Serial.println("Initializing MIDI");

  this->serialMidi = serialMidi;
  this->adc = adc;
  this->lfoType[0] = 0;
  this->lfoType[1] = 0;
  this->lfoType[2] = 0;
  this->lfoType[3] = 0;
  this->lfoSpeed[0] = 1;
  this->lfoSpeed[1] = 1;
  this->lfoSpeed[2] = 1;
  this->lfoSpeed[3] = 1;
  this->lfoAmplitude[0] = 0;
  this->lfoAmplitude[1] = 0;
  this->lfoAmplitude[2] = 0;
  this->lfoAmplitude[3] = 0;
  this->lfoRheoSet[0] = 1;
  this->lfoRheoSet[1] = 1;
  this->lfoRheoSet[2] = 1;
  this->lfoRheoSet[3] = 1;

  Serial.println("initializing gate outputs");
  this->backplaneGPIO = backplaneGPIO;
  backplaneGPIO->begin(BACKPLANE_MAX7301_CS_PIN);
  backplaneGPIO->initPort(0,8, OUTPUT); // gate out ch 1
  backplaneGPIO->initPort(1,4, OUTPUT); // gate out ch 2
  backplaneGPIO->initPort(2,9, OUTPUT); // gate out ch 3
  backplaneGPIO->initPort(3,5, OUTPUT); // gate out ch 4

  backplaneGPIO->initPort(17, 6, OUTPUT); // clock output

  backplaneGPIO->initPort(4,10, INPUT_PULLUP); // Gate in 1
  backplaneGPIO->initPort(5,7,  INPUT_PULLUP); // Gate input 2
  backplaneGPIO->initPort(6,11, INPUT_PULLUP); // Gate input 3
  backplaneGPIO->initPort(7,12, INPUT_PULLUP); // Gate input 4
/*
10  P14  10
11  P11   7
12  P15  11
13  P16  12
*/
  backplaneGPIO->initPort(8,27, OUTPUT); // latch reset

  backplaneGPIO->initPort(9,18, OUTPUT); // slew cap ctrl
  backplaneGPIO->initPort(10,19, OUTPUT); // slew cap ctrl
  backplaneGPIO->initPort(11,20, OUTPUT); // slew cap ctrl
  backplaneGPIO->initPort(12,21, OUTPUT); // slew cap ctrl
  backplaneGPIO->initPort(13,22, OUTPUT); // slew cap ctrl
  backplaneGPIO->initPort(14,23, OUTPUT); // slew cap ctrl
  backplaneGPIO->initPort(15,3, OUTPUT); // slew cap ctrl
  backplaneGPIO->initPort(16,24, OUTPUT); // slew cap ctrl

  backplaneGPIO->initPort(22, 14, INPUT_PULLUP); //Encoder Switch

  backplaneGPIO->initPort(18, 2, OUTPUT);

  backplaneGPIO->initPort(19, 1, INPUT);
  backplaneGPIO->initPort(20, 26, INPUT);
  backplaneGPIO->initPort(21, 13, INPUT);
  backplaneGPIO->initPort(23, 15, INPUT);
  backplaneGPIO->initPort(24, 16, INPUT);
  backplaneGPIO->initPort(25, 17, INPUT);
  backplaneGPIO->initPort(26, 25, INPUT);
  backplaneGPIO->initPort(27, 0, INPUT);

  backplaneGPIO->updateGpioPinModes(); // send GPIO pin modes to chip


  //backplaneGPIO->digitalWrite(0, 0);
  //backplaneGPIO->digitalWrite(1, 0);
  //backplaneGPIO->digitalWrite(2, 0);
  //backplaneGPIO->digitalWrite(3, 0);
  //backplaneGPIO->digitalWrite(8, 0);
  //backplaneGPIO->digitalWrite(4, 0);
  //backplaneGPIO->digitalWrite(5, 0);
  //backplaneGPIO->digitalWrite(6, 0);
  //backplaneGPIO->digitalWrite(7, 0);
  //backplaneGPIO->digitalWrite(8, 0);
  //backplaneGPIO->digitalWrite(9, 0);
  //backplaneGPIO->digitalWrite(10, 0);
  //backplaneGPIO->digitalWrite(11, 0);
  //backplaneGPIO->digitalWrite(12, 0);
  //backplaneGPIO->digitalWrite(13, 0);
  //backplaneGPIO->digitalWrite(18, 0);
  //backplaneGPIO->digitalWrite(14, 0);
  //backplaneGPIO->digitalWrite(15, 0);
  //backplaneGPIO->digitalWrite(16, 0);
  //backplaneGPIO->digitalWrite(17, 0);
  //backplaneGPIO->digitalWrite(18, 0);
  //backplaneGPIO->digitalWrite(19, 0);
  //backplaneGPIO->digitalWrite(20, 0);
  //backplaneGPIO->digitalWrite(21, 0);
  //backplaneGPIO->digitalWrite(22, 0);
  //backplaneGPIO->digitalWrite(23, 0);
  //backplaneGPIO->digitalWrite(28, 0);
  //backplaneGPIO->digitalWrite(24, 0);
  //backplaneGPIO->digitalWrite(25, 0);
  //backplaneGPIO->digitalWrite(26, 0);
  //backplaneGPIO->digitalWrite(27, 0);


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

for (int i=0; i<4; i++){
  backplaneGPIO->digitalWrite(outputMap(i, SLEWSWITCHCV), HIGH);        // shut off swich with cap to ground, disable slew
  backplaneGPIO->digitalWrite(outputMap(i, SLEWSWITCHCC), HIGH);        // shut off swich with cap to ground, disable slew
}


  Serial.println("Setting up debug pin");
  pinMode(DEBUG_PIN, OUTPUT);
  pinMode(4, OUTPUT);

  Serial.println("Output Controller Initialization Complete");

  backplaneGPIO->update();
  for (int i=0; i<28; i++){
    Serial.println("Reading port: " + String(i) + "\t" + String(backplaneGPIO->readAddress(i)));
  }
}

void OutputController::dacTestLoop(){
  int voltage; // = 65535*(sin(millis()/10.0)+1)/2;

  if (sin(millis()) > 0 ){
    voltage = 65535;
  } else {
    voltage = 0;
  }

  Serial.println("setting voltage all dacs to: " + String(voltage) + " -- starttime:" + String(startTime));

  for (int i=0; i<8; i++){

    ad5676.setVoltage(i, voltage );
    delayMicroseconds(1);

    ad5676.setVoltage(i, voltage );
    delayMicroseconds(1);
    //ad5676.setVoltage(i, 65535sin(millis()/2) );
  }
}

void OutputController::inputLoopTest(){
//  backplaneGPIO->update();
  bool trig = false;
  for (int i =0; i<28; i++){
    if (backplaneGPIO->fell(i)){
    //  Serial.println("*+_*+_*+*+_*+_*+*+*+_*  Backplane input fell: " + String(i));
      trig = true;
    }
  }

  if (trig) {
    Serial.println("CV1: " + String(analogRead(22)) + "\tCV2: " + String(analogRead(21)) + "\tCV3: " + String(analogRead(20)) + "\tCV4: " + String(analogRead(23)));
  }
  //backplaneGPIO->digitalWrite(8, 1);
  delay(1);
  //backplaneGPIO->digitalWrite(8, 0);
}

void OutputController::inputRead(){

  backplaneGPIO->update();

  cvInputRaw[0] = adc->analogRead(A3, ADC_1);
  cvInputRaw[1] = adc->analogRead(A12, ADC_1);
  cvInputRaw[2] = adc->analogRead(A13, ADC_1);
  cvInputRaw[3] = adc->analogRead(A10, ADC_1);
  gateInputRaw[0]  = backplaneGPIO->pressed(4);
  gateInputRaw[1]  = backplaneGPIO->pressed(5);
  gateInputRaw[2]  = backplaneGPIO->pressed(6);
  gateInputRaw[3]  = backplaneGPIO->pressed(7);
  //  for (int i=0; i<28; i++){
  //    Serial.println("Backplane input: " + String(i) + "--" + String(backplaneGPIO->digitalRead(i)));
  //  }
/*    for (int i=0; i<4; i++){
      Serial.println("GATE input: " + String(i) + "--" + String(gateInputRaw[i]));
    }
    for (int i=0; i<4; i++){
      Serial.println("CV input: " + String(i) + "--" + String(cvInputRaw[i]));
    } */
    backplaneGPIO->digitalWrite(8, 1);
    backplaneGPIO->digitalWrite(8, 0);


}

void OutputController::noteOn(uint8_t channel, uint8_t note, uint8_t velocity, uint8_t velocityType, uint8_t lfoSpeedSetting, uint8_t glide, bool gate){
  // proto 6 calibration numbers: 0v: 22180   5v: 43340
//  Serial.println("    OutputController -- on ch:"  + String(channel) + " nt: " + String(note) );
/*  proto 8 basic calibration
10v - 64240
5v - 48570
0v - 32896
-5v - 17210
-10v  1540
*/
  //Serial3.println("begin note on ch: " + String(channel) + "\tnote: " + String(note) + "\tvel: "+ String(velocity) + "\tglide: " + String(glide));

  if (glide > 0) {

    backplaneGPIO->digitalWrite(outputMap(channel, SLEWSWITCHCV), LOW);           // turn on switch with cap to ground, enable slew

    if (outputMap(channel, RHEOCHANNELCV) == 0){
      mcp4352_1.setResistance(outputMap(channel, CVRHEO), map(glide, 0,127,0,255) );        // set digipot to correct resistance, set slew rate
    } else {
      mcp4352_2.setResistance(outputMap(channel, CVRHEO), map(glide, 0,127,0,255));           // set digipot to correct resistance, set slew rate
    }


    //  Serial.println("glide  ch: " + String(channel) + "\ton dacCh: " + String(dacCvMap[channel]) + "\tCVrheo: " + String(outputMap(channel, CVRHEO)) + "\ton mcp4352 " +  String(outputMap(channel, RHEOCHANNELCV)) + "\t with slew switch: " + String(outputMap(channel, SLEWSWITCHCV)) + "\tslewSetting: " + String(map(glide, 0,127,0,255)) );

  } else {

    backplaneGPIO->digitalWrite(outputMap(channel, SLEWSWITCHCV), HIGH);        // shut off swich with cap to ground, disable slew

    if(velocityType == 1){
        Serial.println("velocitytype == 1 on channel " + String(channel));
      if (outputMap(channel, RHEOCHANNELCC) == 0){
        mcp4352_1.setResistance(outputMap(channel, CCRHEO), 0);        // set digipot to 0
      } else {
        mcp4352_2.setResistance(outputMap(channel, CCRHEO), 0);        // set digipot to 0
      }
      ad5676.setVoltage(dacCcMap[channel],  map(velocity, 0,127,1540, 64240 ) );  // set CC voltage
      ad5676.setVoltage(dacCcMap[channel],  map(velocity, 0,127,1540, 64240 ) );  // set CC voltage
      lfoRheoSet[channel] = 1;
    } else if (velocityType > 1){
      Serial.println("velocitytype > 1 on channel " + String(channel) + "type: " + String(velocityType));
      lfoAmplitude[channel] = velocity;
      lfoSpeed[channel] = lfoSpeedSetting;
      lfoType[channel] = velocityType;
      lfoRheoSet[channel] = 1;
    }

    if (outputMap(channel, RHEOCHANNELCV) == 0){
      mcp4352_1.setResistance(outputMap(channel, CVRHEO), 0);        // set digipot to 0
    } else {
      mcp4352_2.setResistance(outputMap(channel, CVRHEO), 0);        // set digipot to 0
    }

    //Serial.println("NO glide  ch: " + String(channel) + "\ton dacCh: " + String(dacCvMap[channel]) + "\tCVrheo: " + String(outputMap(channel, CVRHEO)) + "\ton mcp4352 " +  String(outputMap(channel, RHEOCHANNELCV)) + "\t with slew switch: " + String(outputMap(channel, SLEWSWITCHCV)) + "\tslewSetting: " + String(map(glide, 0,127,0,255)) );

  }
  int offset = 0;
switch (channel){
  case 1:
  offset = map(adc->analogRead(A12, ADC_1), 0, 1023, 60, -60) ;
  //Serial.println("ch1 offset: "+ String(offset));
  break;
  case 2:
  offset = map(adc->analogRead(A13, ADC_1), 0, 1023, 60, -60) ;

  break;
  case 3:
  offset = map(adc->analogRead(A10, ADC_1), 0, 1023, 60, -60) ;
  break;
}

  serialMidi->sendNoteOn(note, velocity, channel);                                   // send midi note out
  delayMicroseconds(5);
  ad5676.setVoltage(dacCvMap[channel],  map( (note+offset), 0,127,32896, 64240 ) );    // set CV voltage
  ad5676.setVoltage(dacCvMap[channel],  map( (note+offset), 0,127,32896, 64240 ) );    // set CV voltage
  delayMicroseconds(5);

  if (gate){
    backplaneGPIO->digitalWrite(channel, HIGH);                                 // open gate voltage
  }


};


void OutputController::lfoUpdate(uint8_t channel){
  uint8_t rheoStatLevel;
  int8_t voltageLevel;

  //Serial.println("beginning lfoUpdate for channel " + String(channel));
  if (lfoType[channel] < 2){
    return;
  }

  switch(lfoType[channel]){
    case 2:
      rheoStatLevel = 0;
      voltageLevel = (sin((lfoSpeed[channel]*startTime*3.14159)/(beatLength*16)))*lfoAmplitude[channel];
    break;

    case 3: // SQUARE WAVE
      rheoStatLevel = 0;
      if ( sin((lfoSpeed[channel]*startTime*3.14159)/(beatLength*16)) > 0){
        voltageLevel = lfoAmplitude[channel];
      } else {
        voltageLevel = -lfoAmplitude[channel];
      }
      backplaneGPIO->digitalWrite(outputMap(channel, SLEWSWITCHCC), HIGH);        // shut off swich with cap to ground, disable slew

    break;

    case 4: //rounded square wave
      rheoStatLevel = 40;
      if ( sin((lfoSpeed[channel]*startTime*3.14159)/(beatLength*16)) > 0){
        voltageLevel = lfoAmplitude[channel];
      } else {
        voltageLevel = -lfoAmplitude[channel];
      }
      backplaneGPIO->digitalWrite(outputMap(channel, SLEWSWITCHCC), LOW);        // shut off swich with cap to ground, disable slew

    break;

    default:
//      return;
    break;
  }

  //lfoAmplitude[channel]
  //lfoRheoSet[channel]

  //if (lfoRheoSet[channel] == 1){
    if (outputMap(channel, RHEOCHANNELCC) == 0){
      mcp4352_1.setResistance(outputMap(channel, CCRHEO), rheoStatLevel);        // set digipot to 0
    } else {
      mcp4352_2.setResistance(outputMap(channel, CCRHEO), rheoStatLevel);        // set digipot to 0
    }
    //Serial.println("setting rheo");
    lfoRheoSet[channel] = 0;
  //}
    ad5676.setVoltage(dacCcMap[channel],  map(voltageLevel, -127,127,1, 65535 ) );  // set CC voltage
    ad5676.setVoltage(dacCcMap[channel],  map(voltageLevel, -127,127,1, 65535 ) );  // set CC voltage

//  Serial.println("Setting velocity-ch:" + String(channel) + "\tVL: " + String(voltageLevel) + "\trheo: " + String(rheoStatLevel) + "\ttype: " + String(lfoType[channel]) + "\tstartTime: " + String(startTime) + "\tbeatLength:" + String(beatLength) + "\tamp: " + String(lfoAmplitude[channel]) + "\tsinResult:" + String(sin((startTime*3.14159)/(beatLength) )) + "\tdivide: " + String(startTime/beatLength) +"\tendVolt: " + String(map(voltageLevel, -127,127,0, 65535 )));

}

uint8_t OutputController::analogInputTranspose(uint8_t note){

  uint16_t input = analogRead(22);

  return (note + map(input, 0, 1023, 0, 127) );
}

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
      return rheoMap[dacCvMap[channel]] >> 4;
      break;

    case RHEOCHANNELCC:
      return rheoMap[dacCcMap[channel]] >> 4;
      break;

    case CVRHEO:
      return rheoMap[dacCvMap[channel]] & 0x0F;
      break;

    case CCRHEO:
      return rheoMap[dacCcMap[channel]] & 0x0F;
      break;
  }
};




void OutputController::noteOff(uint8_t channel, uint8_t note, bool gateOff){
//  Serial.println("    OutputController -- off ch:"  + String(channel) + " nt: " + String(note) );

  if (gateOff){
    backplaneGPIO->digitalWrite(channel, LOW);
  }
  serialMidi->sendNoteOff(note, 64, channel);

}

void OutputController::allNotesOff(uint8_t channel){
    backplaneGPIO->digitalWrite(channel, LOW);
    this->lfoType[channel] = 0;
    this->lfoAmplitude[channel] = 0;
    this->lfoRheoSet[channel] = 1;
}

void OutputController::setClockOutput(bool value){
    backplaneGPIO->digitalWrite(17, value);
    if (value == HIGH){
      clockOutputTimer = 0;
    }
}

void OutputController::calibrationRoutine(){
  for (int i=0; i<8; i++){
    ad5676.setVoltage(i,calibrationBuffer);
    delay(1);
  }
}
