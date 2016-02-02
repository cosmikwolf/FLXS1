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
#define SW_REC 17
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
int32_t knob1Buffer = 0;
int32_t knob1InitValue = 0;
int32_t knob1previousValue = 0;
int16_t stepModeBuffer;
int16_t pitchBuffer;
int16_t gateLengthBuffer;
int8_t  gateTypeBuffer;

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

  unsigned long loopTimer = micros();
  encoderLoop();
    encoderLoopTime = ((micros() - loopTimer) + 9*encoderLoopTime)/10;
    loopTimer = micros();
  smallButtonLoop();
    smallButtonLoopTime = ((micros() - loopTimer) + 9*smallButtonLoopTime)/10;
    loopTimer = micros();
  matrixButtonLoop();
    matrixButtonTime = ((micros() - loopTimer) + 9*matrixButtonTime)/ 10;
    loopTimer = micros();
}

void encoderLoop(){
  knob1Buffer = knob1.read()/-4;
 // knob2Buffer = knob2.read()/-4;

  if (knob1Buffer != knob1previousValue) {
    // knob 1 has changed!
    knob1previousValue = knob1Buffer;
    switch (settingMode) {
      case 0: // step mode
      /*  switch (stepMode) {
          case 0:
          // just change the note
            if (knob1Buffer + sequence[selectedSequence].getStepPitch(selectedStep) < 0){
              // you can turn off a note by turning the value to 0
              // turn off a note by setting gate type and pitch to 0 
              sequence[selectedSequence].stepData[selectedStep].gateType = 0;
              sequence[selectedSequence].setStepPitch(selectedStep, 0);
              knob1.write(4);  
              } else {
              if(sequence[selectedSequence].stepData[selectedStep].gateType == 0){
                // if a note is not active, turn it on.
                sequence[selectedSequence].stepData[selectedStep].gateType = 1; 
              } 
              // and finally set the new step value!
              sequence[selectedSequence].setStepPitch(selectedStep, positive_modulo(stepModeBuffer + knob1Buffer, 127));
            }
            break;

          case 1:
          // change the gate type
            sequence[selectedSequence].setGateLength(selectedStep, positive_modulo(stepModeBuffer + knob1Buffer, 127) );

            break;

          case 2:  
          // change length of gate
            sequence[selectedSequence].setGateType(selectedStep, positive_modulo(stepModeBuffer + knob1Buffer, 3) );
            break;

          case 3:
            break;

          case 4:

            break;
        }
        break;
*/
      case SEQUENCE_SPED: // speed setting
        switch(menuSelection){
          case 0:
            sequence[selectedSequence].setStepCount( positive_modulo(stepModeBuffer + knob1Buffer, 63)+1 );
            break;
          case 1: 
            sequence[selectedSequence].setBeatCount( positive_modulo(stepModeBuffer + knob1Buffer, 127) + 1 );
            break;
        }
        break;
      case SEQUENCE_INST:
        int instBuffer = sequence[selectedSequence].instrument;
        switch(menuSelection){
          case 0:
            sequence[selectedSequence].instrument = positive_modulo(stepModeBuffer+knob1Buffer,127);
            sam2695.programChange(0, selectedSequence, sequence[selectedSequence].instrument);
            break;
          case 1: 
            sequence[selectedSequence].volume = positive_modulo(stepModeBuffer+knob1Buffer, 127) ;
            sam2695.setChannelVolume(selectedSequence, sequence[selectedSequence].volume);
            break;
          case 2: 
            sequence[selectedSequence].bank = positive_modulo(stepModeBuffer+knob1Buffer, 127) ;
            sam2695.setChannelBank(selectedSequence, sequence[selectedSequence].bank);
            break;
        }
        break;
      }
  }

}

void matrixButtonLoop(){
  for (int i=0; i < numSteps; i++){
    if (max7301.fell(i)){
      switch (settingMode) {
        case 0:
          stepModeButtonHandler(i);
          break; 
        case 1:
          sequencerMenuButtonHandler(i);
          break;
        case 2:
          globalMenuButtonHandler(i);
          break;
        default:
          menuItemButtonHandler(settingMode, i);
          break;
      }
    }
    
  }
}

// adding a menu item requires an entry here, in the menuItemButtonHandler, as well as a 
// menuItem case in the display.ino file. all cases should be referenced by a #define that 
// represents the menu item.

void menuItemButtonHandler(uint8_t selectedMode, uint8_t buttonNum){

  switch (selectedMode) {
    case GLOBAL_MIDI:
      switch(buttonNum){
        case 0:
          extClock = !extClock;
          break;
      }
      break;
    case SEQUENCE_SPED:
      switch(buttonNum){
        case 0:
          menuSelection = 0;
          stepModeBuffer = sequence[selectedSequence].stepCount;
          break;
        case 4:
          menuSelection = 1;
          stepModeBuffer = sequence[selectedSequence].beatCount;
          break;
      }
      break;
    case SEQUENCE_GENE:
      switch(buttonNum){
        case 0:
          sequence[selectedSequence].initNewSequence();
          settingMode = 0;
          break;
        case 4:
          Serial.println("Random some notes");
          for(int i=0; i <16; i++){
            sequence[selectedSequence].stepData[i].gateType = random(2);
            sequence[selectedSequence].setStepPitch(i, random(1, 127));
          }
          settingMode = 0;
          break;
        case 8:
          Serial.println("Random all notes");
          for(int i=0; i <16; i++){
            sequence[selectedSequence].stepData[i].gateType = 1;//random(2);
            sequence[selectedSequence].setStepPitch(i, random(1, 127));
          };
          settingMode = 0;
          break;
        case 12:
          deleteSaveFile();
          settingMode = 0;
        break;  
      }
      break;

      case SEQUENCE_QUAN:
        switch(buttonNum){
          case 0:
            sequence[selectedSequence].quantizeKey = 1;
            settingMode = 0;
          break;
          case 4:
            sequence[selectedSequence].quantizeKey = 2;
            settingMode = 0;
          break;
          case 8:
            sequence[selectedSequence].quantizeKey = 0;
            settingMode = 0;
          break;
        }
      break;
    case PATTERN_SELECT:

    //  changePattern(buttonNum, true, true);

      if (max7301.pressed(SW_ALT) == false){
        changePattern(buttonNum, true, true);
      } else {
        changePattern(buttonNum, true, false);
      }

      settingMode = 0;
      break;
   
    case SEQUENCE_SELECT:
      if (buttonNum < sequenceCount){
        selectedSequence = buttonNum;
        Serial.println("setting selectedSequence: " + String(selectedSequence));
        settingMode = 0;
      }
      break;

    case SEQUENCE_INST:
      switch(buttonNum){
        case 0:
          menuSelection = 0;
          stepModeBuffer = sequence[selectedSequence].instrument;
          break;
        case 4:
          menuSelection = 1;
          stepModeBuffer = sequence[selectedSequence].volume;
          break;
        case 8:
          menuSelection = 2;
          stepModeBuffer = sequence[selectedSequence].bank;
          break;
      }
      break;


    case GLOBAL_FILE:
      switch(buttonNum){
          case 0:
            deleteSaveFile();
            loadPattern(0);
            settingMode = 0;
          break;
      }
      break;
   }

};


void sequencerMenuButtonHandler(uint8_t buttonId){
  menuSelection = 127;
  switch (buttonId) {
    case 0:
      settingMode = SEQUENCE_NAME;
      break;
    case 1:
      settingMode = SEQUENCE_SPED;
      break;
    case 2:
      settingMode = SEQUENCE_TRAN;
      break;
    case 3:
      settingMode = SEQUENCE_INST;
      break;
    case 4:
      settingMode = SEQUENCE_QUAN;
      break;
    case 5:
      settingMode = SEQUENCE_EUCL;
      break;
    case 6:
      settingMode = SEQUENCE_GENE;
      break;
    case 7:
      settingMode = SEQUENCE_ORDE;
      break;
    case 8:
      settingMode = SEQUENCE_RAND;
      break;
    case 9:
      settingMode = SEQUENCE_POSI;
      break;
    case 10:
      settingMode = SEQUENCE_GLID;
      break;
    case 11:
      settingMode = 0;
      break;
    case 12:
      settingMode = SEQUENCE_MIDI;
      break;
    case 13:
      settingMode = SEQUENCE_CV;
      break;
    case 14:
      settingMode = SEQUENCE_GATE;
      break;
    case 15:
      settingMode = 0;
      break;
  }

}

void globalMenuButtonHandler(uint8_t buttonId){
    switch (buttonId) {
      case 0:
        settingMode = GLOBAL_MIDI;
        break;
      case 1:
        settingMode = GLOBAL_SAVE;
        break;
      case 2:
        settingMode = GLOBAL_LOAD;
        break;
      case 3:
        settingMode = GLOBAL_FILE;
        break;
      default:
        settingMode = 0;
        break;
  }
}

void stepModeButtonHandler(uint8_t i){
  need2save = true;
  saveTimer = 0;
  if(selectedStep == i && stepMode == 0){
    stepMode = 1; // change the step length
    knob1.write(0);
    stepModeBuffer = sequence[selectedSequence].stepData[i].gateLength;
  } else if (selectedStep == i && stepMode != 0){
    stepMode = positive_modulo(stepMode + 1, 3); // change the step length
    knob1.write(0);
    stepModeBuffer = sequence[selectedSequence].stepData[i].gateType;
  } else {
    stepMode = 0;
    selectedStep = i;
    // since the selected step changed, we need to reset the knob2 value to 0
    knob1.write(0);
    knob1previousValue = 0;
    stepModeBuffer = sequence[selectedSequence].getStepPitch(selectedStep);
  }
}




void smallButtonLoop(){
  for (int i=16; i <27; i++){
    if (max7301.fell(i) ){
      Serial.println("button fell " + String(i));

      switch (i){
        // left row bottom up
        case SW_M0:
          if (settingMode == PATTERN_SELECT){
            stepMode = 0;
            settingMode = 0;
          } else {
            stepMode = 4; 
            settingMode = PATTERN_SELECT;
          }
          break;

        case SW_M1:
          stepMode = 3;
          break;

        case SW_PGDN:
     //     stepMode = 4;
    //      settingMode = TEMPO_SET;
          break;

        case SW_M3:
          settingMode = positive_modulo(settingMode + 1, 3);
          break;

        case SW_STOP:
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

        case SW_M2:
            if (settingMode == SEQUENCE_SELECT){
              stepMode = 0;
              settingMode = 0;
            } else {
              stepMode = 4; 
              settingMode = SEQUENCE_SELECT;
            }  
          break;

      }
    }
  }
}
