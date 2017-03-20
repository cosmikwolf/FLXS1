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
  backplaneGPIO->initPort(7,12, INPUT_PULLUP); // Gate\ input 4
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
  int32_t sinVal = sin(millis()/10.0)*16384.0;

  if (sin(millis()) > 0 ){
    voltage = 65535;
  } else {
    voltage = 0;
  }

  switch (stepMode){

    case STATE_CALIB_INPUT0_OFFSET:
    case STATE_CALIB_INPUT1_OFFSET:
    case STATE_CALIB_INPUT2_OFFSET:
    case STATE_CALIB_INPUT3_OFFSET:
      for (int i=0; i<8; i++){
        ad5676.setVoltage(dacMap[i], map(0, -16384, 16384, dacCalibrationNeg[dacMap[i]], dacCalibrationPos[dacMap[i]] ) );
        ad5676.setVoltage(dacMap[i], map(0, -16384, 16384, dacCalibrationNeg[dacMap[i]], dacCalibrationPos[dacMap[i]] ) );
      }
    break;
    case STATE_CALIB_INPUT0_LOW:
    case STATE_CALIB_INPUT1_LOW:
    case STATE_CALIB_INPUT2_LOW:
    case STATE_CALIB_INPUT3_LOW:
      for (int i=0; i<8; i++){
        ad5676.setVoltage(dacMap[i], map(8192, -16384, 16384, dacCalibrationNeg[dacMap[i]], dacCalibrationPos[dacMap[i]] ) );
        ad5676.setVoltage(dacMap[i], map(8192, -16384, 16384, dacCalibrationNeg[dacMap[i]], dacCalibrationPos[dacMap[i]] ) );
      }
    break;
    case STATE_CALIB_INPUT1_HIGH:
    case STATE_CALIB_INPUT0_HIGH:
    case STATE_CALIB_INPUT2_HIGH:
    case STATE_CALIB_INPUT3_HIGH:
      for (int i=0; i<8; i++){
        ad5676.setVoltage(dacMap[i], map(-8192, -16384, 16384, dacCalibrationNeg[dacMap[i]], dacCalibrationPos[dacMap[i]] ) );
        ad5676.setVoltage(dacMap[i], map(-8192, -16384, 16384, dacCalibrationNeg[dacMap[i]], dacCalibrationPos[dacMap[i]] ) );
      }
    break;


    case STATE_CALIB_OUTPUT0_LOW:
      ad5676.setVoltage(dacMap[0], map(-8192, -16384, 16384, dacCalibrationNeg[dacMap[0]], dacCalibrationPos[dacMap[0]] ) );
      ad5676.setVoltage(dacMap[0], map(-8192, -16384, 16384, dacCalibrationNeg[dacMap[0]], dacCalibrationPos[dacMap[0]] ) );
    break;
    case STATE_CALIB_OUTPUT0_HIGH:
      ad5676.setVoltage(dacMap[0], map(8192, -16384, 16384, dacCalibrationNeg[dacMap[0]], dacCalibrationPos[dacMap[0]] ) );
      ad5676.setVoltage(dacMap[0], map(8192, -16384, 16384, dacCalibrationNeg[dacMap[0]], dacCalibrationPos[dacMap[0]] ) );
    break;
    case STATE_CALIB_OUTPUT0_TEST:
  //    ad5676.setVoltage(dacMap[0], map(sinVal, -16384, 16384, dacCalibrationNeg[dacMap[0]], dacCalibrationPos[dacMap[0]] ) );
  //    ad5676.setVoltage(dacMap[0], map(sinVal, -16384, 16384, dacCalibrationNeg[dacMap[0]], dacCalibrationPos[dacMap[0]] ) );
      ad5676.setVoltage(dacMap[0], map(0, -16384, 16384, dacCalibrationNeg[dacMap[0]], dacCalibrationPos[dacMap[0]] ) );
      ad5676.setVoltage(dacMap[0], map(0, -16384, 16384, dacCalibrationNeg[dacMap[0]], dacCalibrationPos[dacMap[0]] ) );

    break;
    case STATE_CALIB_OUTPUT1_LOW:
      ad5676.setVoltage(dacMap[1], map(-8192, -16384, 16384, dacCalibrationNeg[dacMap[1]], dacCalibrationPos[dacMap[1]] ) );
      ad5676.setVoltage(dacMap[1], map(-8192, -16384, 16384, dacCalibrationNeg[dacMap[1]], dacCalibrationPos[dacMap[1]] ) );
    break;
    case STATE_CALIB_OUTPUT1_HIGH:
      ad5676.setVoltage(dacMap[1], map(8192, -16384, 16384, dacCalibrationNeg[dacMap[1]], dacCalibrationPos[dacMap[1]] ) );
      ad5676.setVoltage(dacMap[1], map(8192, -16384, 16384, dacCalibrationNeg[dacMap[1]], dacCalibrationPos[dacMap[1]] ) );
    break;
    case STATE_CALIB_OUTPUT1_TEST:
      ad5676.setVoltage(dacMap[1], map(0, -16384, 16384, dacCalibrationNeg[dacMap[1]], dacCalibrationPos[dacMap[1]] ) );
      ad5676.setVoltage(dacMap[1], map(0, -16384, 16384, dacCalibrationNeg[dacMap[1]], dacCalibrationPos[dacMap[1]] ) );
    break;
    case STATE_CALIB_OUTPUT2_LOW:
      ad5676.setVoltage(dacMap[2], map(-8192, -16384, 16384, dacCalibrationNeg[dacMap[2]], dacCalibrationPos[dacMap[2]] ) );
      ad5676.setVoltage(dacMap[2], map(-8192, -16384, 16384, dacCalibrationNeg[dacMap[2]], dacCalibrationPos[dacMap[2]] ) );
    break;
    case STATE_CALIB_OUTPUT2_HIGH:
      ad5676.setVoltage(dacMap[2], map(8192, -16384, 16384, dacCalibrationNeg[dacMap[2]], dacCalibrationPos[dacMap[2]] ) );
      ad5676.setVoltage(dacMap[2], map(8192, -16384, 16384, dacCalibrationNeg[dacMap[2]], dacCalibrationPos[dacMap[2]] ) );
    break;
    case STATE_CALIB_OUTPUT2_TEST:
      ad5676.setVoltage(dacMap[2], map(0, -16384, 16384, dacCalibrationNeg[dacMap[2]], dacCalibrationPos[dacMap[2]] ) );
      ad5676.setVoltage(dacMap[2], map(0, -16384, 16384, dacCalibrationNeg[dacMap[2]], dacCalibrationPos[dacMap[2]] ) );
    break;
    case STATE_CALIB_OUTPUT3_LOW:
      ad5676.setVoltage(dacMap[3], map(-8192, -16384, 16384, dacCalibrationNeg[dacMap[3]], dacCalibrationPos[dacMap[3]] ) );
      ad5676.setVoltage(dacMap[3], map(-8192, -16384, 16384, dacCalibrationNeg[dacMap[3]], dacCalibrationPos[dacMap[3]] ) );
    break;
    case STATE_CALIB_OUTPUT3_HIGH:
      ad5676.setVoltage(dacMap[3], map(8192, -16384, 16384, dacCalibrationNeg[dacMap[3]], dacCalibrationPos[dacMap[3]] ) );
      ad5676.setVoltage(dacMap[3], map(8192, -16384, 16384, dacCalibrationNeg[dacMap[3]], dacCalibrationPos[dacMap[3]] ) );
    break;
    case STATE_CALIB_OUTPUT3_TEST:
      ad5676.setVoltage(dacMap[3], map(0, -16384, 16384, dacCalibrationNeg[dacMap[3]], dacCalibrationPos[dacMap[3]] ) );
      ad5676.setVoltage(dacMap[3], map(0, -16384, 16384, dacCalibrationNeg[dacMap[3]], dacCalibrationPos[dacMap[3]] ) );
    break;
    case STATE_CALIB_OUTPUT4_LOW:
      ad5676.setVoltage(dacMap[4], map(-8192, -16384, 16384, dacCalibrationNeg[dacMap[4]], dacCalibrationPos[dacMap[4]] ) );
      ad5676.setVoltage(dacMap[4], map(-8192, -16384, 16384, dacCalibrationNeg[dacMap[4]], dacCalibrationPos[dacMap[4]] ) );
    break;
    case STATE_CALIB_OUTPUT4_HIGH:
      ad5676.setVoltage(dacMap[4], map(8192, -16384, 16384, dacCalibrationNeg[dacMap[4]], dacCalibrationPos[dacMap[4]] ) );
      ad5676.setVoltage(dacMap[4], map(8192, -16384, 16384, dacCalibrationNeg[dacMap[4]], dacCalibrationPos[dacMap[4]] ) );
    break;
    case STATE_CALIB_OUTPUT4_TEST:
      ad5676.setVoltage(dacMap[4], map(0, -16384, 16384, dacCalibrationNeg[dacMap[4]], dacCalibrationPos[dacMap[4]] ) );
      ad5676.setVoltage(dacMap[4], map(0, -16384, 16384, dacCalibrationNeg[dacMap[4]], dacCalibrationPos[dacMap[4]] ) );
    break;
    case STATE_CALIB_OUTPUT5_LOW:
      ad5676.setVoltage(dacMap[5], map(-8192, -16384, 16384, dacCalibrationNeg[dacMap[5]], dacCalibrationPos[dacMap[5]] ) );
      ad5676.setVoltage(dacMap[5], map(-8192, -16384, 16384, dacCalibrationNeg[dacMap[5]], dacCalibrationPos[dacMap[5]] ) );
    break;
    case STATE_CALIB_OUTPUT5_HIGH:
      ad5676.setVoltage(dacMap[5], map(8192, -16384, 16384, dacCalibrationNeg[dacMap[5]], dacCalibrationPos[dacMap[5]] ) );
      ad5676.setVoltage(dacMap[5], map(8192, -16384, 16384, dacCalibrationNeg[dacMap[5]], dacCalibrationPos[dacMap[5]] ) );
    break;
    case STATE_CALIB_OUTPUT5_TEST:
      ad5676.setVoltage(dacMap[5], map(0, -16384, 16384, dacCalibrationNeg[dacMap[5]], dacCalibrationPos[dacMap[5]] ) );
      ad5676.setVoltage(dacMap[5], map(0, -16384, 16384, dacCalibrationNeg[dacMap[5]], dacCalibrationPos[dacMap[5]] ) );
    break;
    case STATE_CALIB_OUTPUT6_LOW:
      ad5676.setVoltage(dacMap[6], map(-8192, -16384, 16384, dacCalibrationNeg[dacMap[6]], dacCalibrationPos[dacMap[6]] ) );
      ad5676.setVoltage(dacMap[6], map(-8192, -16384, 16384, dacCalibrationNeg[dacMap[6]], dacCalibrationPos[dacMap[6]] ) );
    break;
    case STATE_CALIB_OUTPUT6_HIGH:
      ad5676.setVoltage(dacMap[6], map(8192, -16384, 16384, dacCalibrationNeg[dacMap[6]], dacCalibrationPos[dacMap[6]] ) );
      ad5676.setVoltage(dacMap[6], map(8192, -16384, 16384, dacCalibrationNeg[dacMap[6]], dacCalibrationPos[dacMap[6]] ) );
    break;
    case STATE_CALIB_OUTPUT6_TEST:
      ad5676.setVoltage(dacMap[6], map(0, -16384, 16384, dacCalibrationNeg[dacMap[6]], dacCalibrationPos[dacMap[6]] ) );
      ad5676.setVoltage(dacMap[6], map(0, -16384, 16384, dacCalibrationNeg[dacMap[6]], dacCalibrationPos[dacMap[6]] ) );
    break;
    case STATE_CALIB_OUTPUT7_LOW:
      ad5676.setVoltage(dacMap[7], map(-8192, -16384, 16384, dacCalibrationNeg[dacMap[7]], dacCalibrationPos[dacMap[7]] ) );
      ad5676.setVoltage(dacMap[7], map(-8192, -16384, 16384, dacCalibrationNeg[dacMap[7]], dacCalibrationPos[dacMap[7]] ) );
    break;
    case STATE_CALIB_OUTPUT7_HIGH:
      ad5676.setVoltage(dacMap[7], map(8192, -16384, 16384, dacCalibrationNeg[dacMap[7]], dacCalibrationPos[dacMap[7]] ) );
      ad5676.setVoltage(dacMap[7], map(8192, -16384, 16384, dacCalibrationNeg[dacMap[7]], dacCalibrationPos[dacMap[7]] ) );
    break;
    case STATE_CALIB_OUTPUT7_TEST:
      ad5676.setVoltage(dacMap[7], map(0, -16384, 16384, dacCalibrationNeg[dacMap[7]], dacCalibrationPos[dacMap[7]] ) );
      ad5676.setVoltage(dacMap[7], map(0, -16384, 16384, dacCalibrationNeg[dacMap[7]], dacCalibrationPos[dacMap[7]] ) );
    break;
  }
  //Serial.println("setting voltage all dacs to: " + String(voltage) + " -- starttime:" + String(startTime));

  for (int i=0; i<8; i++){

//    ad5676.setVoltage(i, voltage );
//    delayMicroseconds(1);

//    ad5676.setVoltage(i, voltage );
//    delayMicroseconds(1);
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
  for(int ch=0; ch < SEQUENCECOUNT; ch++){
    //cvInputMapped[ch] = map(cvInputRaw[ch], adcCalibrationNeg[ch], adcCalibrationPos[ch], -60, 60) +1;

    if (cvInputRaw[ch] < adcCalibrationOffset[ch] ){
      cvInputMapped1024[ch] = map( cvInputRaw[ch], adcCalibrationPos[ch], adcCalibrationOffset[ch], 1024, 0);
    }else {
      cvInputMapped1024[ch] = map( cvInputRaw[ch], adcCalibrationOffset[ch], adcCalibrationNeg[ch] , 0, -1024);
    }
    cvInputMapped[ch] = map( cvInputMapped1024[ch], -1024, 1024, -64, 64) ;

  }
  gateInputRaw[0]  = !backplaneGPIO->pressed(4);
  gateInputRaw[1]  = !backplaneGPIO->pressed(5);
  gateInputRaw[2]  = !backplaneGPIO->pressed(6);
  gateInputRaw[3]  = !backplaneGPIO->pressed(7);

  gateInputRose[0]  = backplaneGPIO->rose(4);
  gateInputRose[1]  = backplaneGPIO->rose(5);
  gateInputRose[2]  = backplaneGPIO->rose(6);
  gateInputRose[3]  = backplaneGPIO->rose(7);
  //reset SR Latch
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
  //Serial.println("begin note on ch: " + String(channel) + "\tnote: " + String(note) + "\tvel: "+ String(velocity) + "\tglide: " + String(glide) + "\tgate: " + String(gate));
  if (gate){
    if(backplaneGPIO->cacheCheck(channel) == 1){
      backplaneGPIO->digitalWrite(channel, LOW);                                 // close gate before re opening
    //  delay(1);
      //Serial.println("setting gate low because it was still on.");
    }
  }

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

      ad5676.setVoltage(dacCcMap[channel],  map(velocity, 0,127,dacCalibrationNeg[dacCcMap[channel]], dacCalibrationPos[dacCcMap[channel]] ) );  // set CC voltage
      ad5676.setVoltage(dacCcMap[channel],  map(velocity, 0,127,dacCalibrationNeg[dacCcMap[channel]], dacCalibrationPos[dacCcMap[channel]] ) );  // set CC voltage
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
  //offset = cvInputMapped[channel];
  //serialMidi->sendNoteOn(note, velocity, channel);                                   // send midi note out
  //delayMicroseconds(5);
  ad5676.setVoltage(dacCvMap[channel],  map( (note+offset), 0,120,calibMidscale(dacCvMap[channel]), dacCalibrationPos[dacCvMap[channel]] ) );    // set CV voltage
//delayMicroseconds(5);
  ad5676.setVoltage(dacCvMap[channel],  map( (note+offset), 0,120,calibMidscale(dacCvMap[channel]), dacCalibrationPos[dacCvMap[channel]]) );    // set CV voltage
//  delayMicroseconds(5);
  //Serial.println("Ch " + String(channel) + "\t offset:" + String(offset) + "\traw: " + String(cvInputRaw[channel]));
  if (gate){
    backplaneGPIO->digitalWrite(channel, HIGH);                                 // open gate voltage
  }
  debugTimer1 = 0;

};

uint16_t OutputController::calibMidscale(uint8_t mapAddress){
  return (dacCalibrationNeg[mapAddress]+dacCalibrationPos[mapAddress])/2;
};

uint16_t OutputController::calibLow(uint8_t mapAddress, uint8_t range){

}

uint16_t OutputController::calibHigh(uint8_t mapAddress, uint8_t range){

}

void OutputController::lfoUpdate(uint8_t channel){
  uint8_t rheoStatLevel;
  int16_t voltageLevel;

  //Serial.println("beginning lfoUpdate for channel " + String(channel));
  if (lfoType[channel] < 1){
    return;
  }

  switch(lfoType[channel]){
    case 1:
      voltageLevel = lfoAmplitude[channel]*128;
    break;

    case 2:
      rheoStatLevel = 2;
      voltageLevel = (sin((lfoSpeed[channel]*lfoClockCounter*3.14159*100)/(beatLength*16)))*lfoAmplitude[channel]*128;
      backplaneGPIO->digitalWrite(outputMap(channel, SLEWSWITCHCC), LOW);
    break;

    case 3: // SQUARE WAVE
      rheoStatLevel = 0;
      if ( sin((lfoSpeed[channel]*lfoClockCounter*3.14159*100)/(beatLength*16)) > 0){
        voltageLevel = lfoAmplitude[channel]*128;
      } else {
        voltageLevel = -lfoAmplitude[channel]*128;
      }
      backplaneGPIO->digitalWrite(outputMap(channel, SLEWSWITCHCC), HIGH);        // shut off swich with cap to ground, disable slew
    break;

    case 4: //rounded square wave
      rheoStatLevel = 40;
      if ( sin((lfoSpeed[channel]*lfoClockCounter*3.14159*100)/(beatLength*16)) > 0){
        voltageLevel = lfoAmplitude[channel]*128;
      } else {
        voltageLevel = -lfoAmplitude[channel]*128;
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
    ad5676.setVoltage(dacCcMap[channel],  map(voltageLevel, -16384,16384, dacCalibrationNeg[dacCcMap[channel]], dacCalibrationPos[dacCcMap[channel]] ) );  // set CC voltage
    ad5676.setVoltage(dacCcMap[channel],  map(voltageLevel, -16384,16384, dacCalibrationNeg[dacCcMap[channel]], dacCalibrationPos[dacCcMap[channel]] ) );  // set CC voltage

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
  //Serial.println("    OutputController -- off ch:"  + String(channel) + " nt: " + String(note) + "\timer: " + String(debugTimer1) );

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
    //backplaneGPIO->digitalWrite(17, value);
    digitalWriteFast(PIN_EXT_AD_1, value);
    digitalWrite(CLOCK_PIN, value);

    clockValue = value;
    if (value == HIGH){
      clockOutputTimer = 0;
    }
}

void OutputController::setGateOutputDebug(uint8_t index, bool value){
  backplaneGPIO->digitalWrite(index, value);
}

void OutputController::calibrationRoutine(){
  for (int i=0; i<8; i++){
    ad5676.setVoltage(i,calibrationBuffer);
    delay(1);
  }
}
