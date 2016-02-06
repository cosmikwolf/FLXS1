//button stuff
#include <SPI.h>
#include <Zetaohm_MAX7301.h>   // import library
#include <Encoder.h>

#define SW_00    0
#define SW_01    1
#define SW_02    2
#define SW_03    3
#define SW_04    4
#define SW_05    5
#define SW_06    6
#define SW_07    7
#define SW_08    8
#define SW_09    9
#define SW_10    10
#define SW_11    11
#define SW_12    12
#define SW_13    13
#define SW_14    14
#define SW_15    15
#define SW_PLAY  16
#define SW_REC   17
#define SW_STOP  18
#define SW_M0    19
#define SW_M1    20
#define SW_M2    21
#define SW_M3    22
#define SW_PGDN  23
#define SW_PGUP  24
#define SW_MENU  25
#define SW_ALT   26

#define ENCODER1LEFTPIN 17
#define ENCODER1RIGHTPIN 16

Encoder knob1(ENCODER1LEFTPIN, ENCODER1RIGHTPIN);
max7301 max7301(5);

// Encoder vars
uint8_t knobRead = 0;
uint8_t knobPrev = 0;
uint8_t knobBuffer = 0;


int16_t stepModeBuffer;
//int16_t pitchBuffer;
//int16_t gateLengthBuffer;
//int8_t  gateTypeBuffer;

int8_t instrumentSelector = 0;
int8_t instBuffer;

unsigned long encoderLoopTime;
unsigned long smallButtonLoopTime;
unsigned long encoderButtonTime;
unsigned long matrixButtonTime;

void buttonSetup() {
  Serial.println("button setup start");  delay(1);
  max7301.begin(0);                      delay(1);
  max7301.gpioPinMode(INPUT_PULLUP);     delay(1);
  max7301.init(0, 14);   // SW_00   
  max7301.init(1, 7);    // SW_01   
  max7301.init(2, 5);    // SW_02   
  max7301.init(3, 26);   // SW_03   
  max7301.init(4, 15);   // SW_04   
  max7301.init(5, 11);   // SW_05   
  max7301.init(6, 9);    // SW_06   
  max7301.init(7, 1);    // SW_07   
  max7301.init(8, 16);   // SW_08   
  max7301.init(9, 12);   // SW_09   
  max7301.init(10, 6);   // SW_10   
  max7301.init(11, 27);  // SW_11   
  max7301.init(12, 17);  // SW_12   
  max7301.init(13, 13);  // SW_13   
  max7301.init(14, 10);  // SW_14   
  max7301.init(15, 0);   // SW_15   
  max7301.init(16, 8);   // SW_PLAY 
  max7301.init(17, 4);   // SW_PAUSE
  max7301.init(18, 2);   // SW_STOP 
  max7301.init(19, 21);  // SW_M0   
  max7301.init(20, 20);  // SW_M1   
  max7301.init(21, 19);  // SW_M2   
  max7301.init(22, 18);  // SW_M3   
  max7301.init(23, 3);   // SW_PGDN 
  max7301.init(24, 25);  // SW_PGUP 
  max7301.init(25, 24);  // SW_MENU 
  max7301.init(26, 23);  // SW_ALT  
  Serial.println("button setup end");
}

void buttonLoop(){
  max7301.update();
  knobRead = knob1.read()/-4;

 // encoderLoop();
 // snapDomeLoop();

  //we always want the alt (non matrix) buttons to behave the same way
  altButtonHandler();

  // now to handle the rest of the buttons.
  switch (currentState) {
    case STEP_DISPLAY:
    stepModeMatrixHandler();
    break; 
    case SEQUENCE_MENU:
    sequencerMenuHandler();
    break;
    case CHANNEL_SELECT:
    channelSelectHandler();
    break;
    case INSTRUMENT_SELECT:
    instrumentSelectInputHandler();
  }
}

void channelSelectHandler(){
  for (int i=0; i < sequenceCount; i++){
    if (max7301.fell(i)){
      selectedSequence = i;
      changeState(STEP_DISPLAY);
    }
  }
}

void altButtonHandler(){
  for (int i=16; i <27; i++){
    if (max7301.fell(i) ){
      switch (i){
        // left row bottom up
        case SW_M0:
        if(currentState == PATTERN_SELECT){
          changeState(STEP_DISPLAY);
        } else {
          changeState(PATTERN_SELECT);
        }

        break;

        case SW_M1:
        if(currentState == CHANNEL_SELECT){
          changeState(STEP_DISPLAY);
        } else {
          changeState(CHANNEL_SELECT);
        }

        break;

        case SW_M2:
        if(currentState == SEQUENCE_MENU){
          changeState(STEP_DISPLAY);
        } else {
          changeState(SEQUENCE_MENU);
        }
        break;

        case SW_M3: //switch M3 toggles the sequence menu
        break;

        case SW_PGDN:
     //     stepMode = 4;
    //      settingMode = TEMPO_SET;
          colorWipe(pixels.Color(255, 0, 0), 50); // Red

          break;

          case SW_STOP:
          if (!playing){ //if the sequence is already paused, stop kills all internal sound.
            for(int s = 0; s < sequenceCount; s++){
              sam2695.allNotesOff(s);
            }
          }
          playing = false;
          for(int s = 0; s < sequenceCount; s++){
            sequence[s].activeStep = 0;
           // sam2695.allNotesOff(s);
          }
          break;

        // right two, bottom up
          case SW_PLAY:
          playing = !playing;
          break;
        }
      }
    }
  }



  void stepModeMatrixHandler(){
//  need2save = true;
//  saveTimer = 0;
    for (int i=0; i < 16; i++){
      if (max7301.fell(i)){
        if(selectedStep == i && stepMode == 0){
        stepMode = 1; // change the step length

        knobBuffer = sequence[selectedSequence].stepData[i].gateLength - knobRead;

       // stepModeBuffer = sequence[selectedSequence].stepData[i].gateLength;
  //    } else if (selectedStep == i && stepMode != 0){
  //      stepMode = positive_modulo(stepMode + 1, 3); // change the step length
  //   //   knob1.write(0);
  //   //   stepModeBuffer = sequence[selectedSequence].stepData[i].gateType;
  //      knobBuffer = sequence[selectedSequence].stepData[i].gateType - knobRead;
      } else {
        stepMode = 0;
        selectedStep = i;
        knobBuffer = sequence[selectedSequence].getStepPitch(selectedStep) - knobRead;
      }
    }
  }


  if (knobRead != knobPrev) {
    knobPrev = knobRead;
    switch (stepMode) {
      case 0:
          // just change the note
      if (knobRead + sequence[selectedSequence].getStepPitch(selectedStep) < 0){
              // you can turn off a note by turning the value to 0
              // turn off a note by setting gate type and pitch to 0 
        sequence[selectedSequence].stepData[selectedStep].gateType = 0;
        sequence[selectedSequence].setStepPitch(selectedStep, 0);
        knob1.write(4);  
      } else {
        if(sequence[selectedSequence].stepData[selectedStep].gateType == 0){
                // if a note is not active, turn it on and give it a length.
          sequence[selectedSequence].stepData[selectedStep].gateType = 1; 
          sequence[selectedSequence].stepData[selectedStep].gateLength = 1; 
        } 
              // and finally set the new step value!
        sequence[selectedSequence].setStepPitch(selectedStep, positive_modulo(knobBuffer + knobRead, 127));
      }
      break;

      case 1:
          // change the gate type
      sequence[selectedSequence].setGateLength(selectedStep, positive_modulo(knobBuffer + knobRead, 127) );

      break;

      case 2:  
          // change length of gate
      sequence[selectedSequence].setGateType(selectedStep, positive_modulo(knobBuffer + knobRead, 3) );
      break;

      case 3:
      break;

      case 4:

      break;
    }
  }
}




void sequencerMenuHandler(){
  menuSelection = 127;

  for (int i=0; i < 16; i++){
    if (max7301.fell(i)){
      switch (i) {
        case 0:
        changeState(INSTRUMENT_SELECT);
        break;
      }
    }
  }
}

void instrumentSelectInputHandler() {

  for (int i=0; i < 16; i++){
    if (max7301.fell(i)){
      instrumentSelector = i;
        switch(instrumentSelector){
        case 0:
          knobBuffer = sequence[selectedSequence].instrument - knobRead;
        break;
        case 4: 
          knobBuffer = sequence[selectedSequence].volume - knobRead;
        break;
        case 8: 
          knobBuffer = sequence[selectedSequence].bank - knobRead;
        break;
      }
    }
  }

  switch(instrumentSelector){
    case 0:
    sequence[selectedSequence].instrument = positive_modulo(knobBuffer + knobRead,127);
    sam2695.programChange(0, selectedSequence, sequence[selectedSequence].instrument);
    break;
    case 4: 
    sequence[selectedSequence].volume = positive_modulo(knobBuffer + knobRead, 127) ;
    sam2695.setChannelVolume(selectedSequence, sequence[selectedSequence].volume);
    break;
    case 8: 
    sequence[selectedSequence].bank = positive_modulo(knobBuffer + knobRead, 127) ;
    sam2695.setChannelBank(selectedSequence, sequence[selectedSequence].bank);
    break;
  }

}

