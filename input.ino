//button stuff
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
#define SW_SPARE   27 
#define ENCODER1LEFTPIN 17
#define ENCODER1RIGHTPIN 16

Encoder knob1(ENCODER1LEFTPIN, ENCODER1RIGHTPIN);
max7301 max7301(5);

// Encoder vars
int8_t knobRead = 0;
//int8_t knobPrev = 0;
int8_t knobBuffer = 0;
int8_t knobPrevious = 0;
int8_t knobChange = 0;
uint8_t patternChannelSelector = 0b1111;

int16_t stepModeBuffer;
//int16_t pitchBuffer;
//int16_t gateLengthBuffer;
//int8_t  gateTypeBuffer;

int8_t menuSelector = 0;
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
  max7301.init(27, 22);  // SW_SPARE  
  Serial.println("button setup end");
}

void buttonLoop(){
  max7301.update();
  knobPrevious = knobRead;
  knobRead = knob1.read()/-4  ;
  knobChange = knobRead - knobPrevious;
  //we always want the alt (non matrix) buttons to behave the same way
  altButtonHandler();

  dispSwitch = max7301.pressed(SW_REC);

  // now to handle the rest of the buttons.
  switch (currentState) {
    case STEP_DISPLAY:
    stepModeMatrixHandler();
    break; 
    case SEQUENCE_MENU:
    sequencerMenuHandler();
    break;

   case CHANNEL_MENU:
   channelMenuHandler();
   break;

    case PATTERN_SELECT:
    patternSelectHandler();
    break;

    case INSTRUMENT_MENU:
    instrumentSelectInputHandler();
    break;

    case TIMING_MENU:
    timingMenuInputHandler();
    break;


    case DEBUG_SCREEN:
    debugScreenInputHandler();
    break;
  }
}

// STATE VARIABLE INPUT HANDLERS

void patternSelectHandler(){
  for (int i=0; i < 16; i++){
    if (max7301.fell(i)){
      changePattern(i, patternChannelSelector,  TRUE, TRUE);
      changeState(STEP_DISPLAY);
    }
  }
}

void channelMenuHandler(){
  if (max7301.fell(0)){
    sequence[selectedChannel].initNewSequence(sequence[selectedChannel].patternIndex, selectedChannel);
      changeState(STEP_DISPLAY);

  } else if (max7301.fell(4)){
    for (int i=0; i < 4; i++){
      sequence[i].initNewSequence(sequence[i].patternIndex, i);
    } 
      changeState(STEP_DISPLAY);
 
  } else if (max7301.fell(8)){
    deleteSaveFile();
    changeState(STEP_DISPLAY);

  }
  

}

void channelButtonHandler(uint8_t channel){
  uint8_t previous = patternChannelSelector;
  if (currentState == PATTERN_SELECT) {
    patternChannelSelector =  patternChannelSelector ^ (1 << channel);
    if (patternChannelSelector == 0) {
      patternChannelSelector = previous;
    }
  } else {
    selectedChannel = channel;
    changeState(STEP_DISPLAY);
    resetKnobValues();

  }
}

void altButtonHandler(){
  for (int i=16; i <28; i++){
    if (max7301.fell(i) ){
      switch (i){
        // left row bottom up
        case SW_M0:
          channelButtonHandler(0);
        break;

        case SW_M1:
          channelButtonHandler(1);
        break;

        case SW_M2:
          channelButtonHandler(2);
        break;

        case SW_M3:
          channelButtonHandler(3);
        break;

        case SW_MENU:
          if(currentState == PATTERN_SELECT){
            changeState(STEP_DISPLAY);
          } else {
            changeState(PATTERN_SELECT);
          }
        break;

        case SW_ALT: //switch M3 toggles the sequence menu
      
          if (stepMode < 3 || stepMode > 7) {
            stepMode = 3;
          } else {
            stepMode++;
          }

          if( currentState != STEP_DISPLAY){
            changeState(STEP_DISPLAY);
          }   

        break;

        case SW_SPARE:
          changeState(CHANNEL_MENU);
        break;
        case SW_PGDN:
          notePage = positive_modulo(notePage - 1, 4);     
        break;

        case SW_PGUP:
          notePage = positive_modulo(notePage + 1, 4);     
        break;

        case SW_STOP:
          if (!playing){ //if the sequence is already paused, stop kills all internal sound.
            for(int s = 0; s < sequenceCount; s++){
              sam2695.allNotesOff(s);
              mcp.digitalWrite(s+4, LOW);
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
  //  saveTimer = 0;
    uint8_t instrumentSelectValue;

    for (int i=0; i < 16; i++){
      if (max7301.fell(i)){
        if(selectedStep == getNote(i) && stepMode == 0){
          stepMode = 1; // change the step length

          knobBuffer = sequence[selectedChannel].stepData[getNote(i)].gateLength - knobRead;

         // stepModeBuffer = sequence[selectedChannel].stepData[i].gateLength;
    //    } else if (selectedStep == i && stepMode != 0){
    //      stepMode = positive_modulo(stepMode + 1, 3); // change the step length
    //   //   knob1.write(0);
    //   //   stepModeBuffer = sequence[selectedChannel].stepData[i].gateType;
    //      knobBuffer = sequence[selectedChannel].stepData[i].gateType - knobRead;
        } else if (selectedStep == getNote(i) && stepMode == 1){
          stepMode = 2;
          knobBuffer = sequence[selectedChannel].stepData[getNote(i)].velocity - knobRead;
        }else {
          stepMode = 0;
          selectedStep = getNote(i);
          knobBuffer = sequence[selectedChannel].getStepPitch(selectedStep) - knobRead;
        }
      }
    }


  //  if (knobRead != knobPrev) {
    if (knobChange){
      //knobPrev = knobRead;
      switch (stepMode) {
        case 0:
      // just change the note
        if (knobRead + sequence[selectedChannel].getStepPitch(selectedStep) < 0){
          // you can turn off a note by turning the value to 0
          // turn off a note by setting gate type and pitch to 0 
          sequence[selectedChannel].stepData[selectedStep].gateType = 0;
          sequence[selectedChannel].setStepPitch(selectedStep, 0);
          knob1.write(4);  
        } else {
          if(sequence[selectedChannel].stepData[selectedStep].gateType == 0){
            // if a note is not active, turn it on and give it a length.
            sequence[selectedChannel].stepData[selectedStep].gateType = 1; 
            sequence[selectedChannel].stepData[selectedStep].gateLength = 1; 
          } 
          // and finally set the new step value!
          sequence[selectedChannel].setStepPitch(selectedStep, positive_modulo(sequence[selectedChannel].getStepPitch(selectedStep) + knobChange, 127));

        }
        break;

        case 1:
    // change the gate type
        if ((sequence[selectedChannel].stepData[selectedStep].gateLength == 0) && (knobChange < 0)  ) {
          sequence[selectedChannel].stepData[selectedStep].gateType = 0;
        } else if(knobChange > 0) {
          sequence[selectedChannel].stepData[selectedStep].gateType = 1;
        }
        
        if (sequence[selectedChannel].stepData[selectedStep].gateType > 0){
          sequence[selectedChannel].stepData[selectedStep].gateLength =  positive_modulo(sequence[selectedChannel].stepData[selectedStep].gateLength + knobChange, 127);
        }

        break;

        case 2:  
      // change length of gate
        sequence[selectedChannel].stepData[selectedStep].velocity = positive_modulo(sequence[selectedChannel].stepData[selectedStep].velocity + knobChange, 128 );
        break;

        case 3:
        if (tempoX100 > 100100) {
          tempoX100 = 100100;
        }
        tempoX100 = positive_modulo(tempoX100 + knobChange*100, 100100 );
        if(tempoX100 == 0){
          tempoX100 = 100;
        }
        changeTempo(tempoX100);
        break;

        case 4:
        instrumentSelectValue = positive_modulo(sequence[selectedChannel].instrument + knobChange,128);
        if (sequence[selectedChannel].instrument != instrumentSelectValue){
          sequence[selectedChannel].instrument = instrumentSelectValue;
          sam2695.programChange(0, selectedChannel, sequence[selectedChannel].instrument);
        }
        break;

        case 5:
        instrumentSelectValue = positive_modulo(sequence[selectedChannel].volume + knobChange, 128);
        if (sequence[selectedChannel].volume != instrumentSelectValue ){
          sequence[selectedChannel].volume = instrumentSelectValue;
          sam2695.setChannelVolume(selectedChannel, sequence[selectedChannel].volume);
        }
        break;

        case 6:
        sequence[selectedChannel].stepCount = positive_modulo(sequence[selectedChannel].stepCount + knobChange, 129);
        break;

        case 7:
        sequence[selectedChannel].beatCount = positive_modulo(sequence[selectedChannel].beatCount + knobChange, 129);    
        break;

      }
    }
  }




  void sequencerMenuHandler(){
    for (int i=0; i < 16; i++){
      if (max7301.fell(i)){
        switch (i) {
          case 0:
          changeState(INSTRUMENT_MENU);
          break;
          case 1:
          changeState(TIMING_MENU);
          break;

        }
      }
    }
  }

  void instrumentSelectInputHandler() {

    for (int i=0; i < 16; i++){
      if (max7301.fell(i)){
        menuSelector = i;
      }
    }
    uint8_t instrumentSelectValue;

    switch(menuSelector){
      case 0:
     //changing the instrument every loop causes weird problems with playback during instrument selection
      instrumentSelectValue = positive_modulo(sequence[selectedChannel].instrument + knobChange,128);
      if (sequence[selectedChannel].instrument != instrumentSelectValue){
        sequence[selectedChannel].instrument = instrumentSelectValue;
        sam2695.programChange(0, selectedChannel, sequence[selectedChannel].instrument);
      }
      break;
      case 4: 
      instrumentSelectValue = positive_modulo(sequence[selectedChannel].volume + knobChange, 128);
      if (sequence[selectedChannel].volume != instrumentSelectValue ){
        sequence[selectedChannel].volume = instrumentSelectValue;
        sam2695.setChannelVolume(selectedChannel, sequence[selectedChannel].volume);
      }
      break;
      case 8: 
      instrumentSelectValue = positive_modulo(sequence[selectedChannel].bank + knobChange, 128) ;
      if (sequence[selectedChannel].bank != instrumentSelectValue ){
        sequence[selectedChannel].bank = instrumentSelectValue;
        sam2695.setChannelBank(selectedChannel, sequence[selectedChannel].bank);
      }
      break;
    }

  }

  void timingMenuInputHandler() {
    for (int i=0; i < 16; i++){
      if (max7301.fell(i)){
        menuSelector = i;
      }
    }
    switch(menuSelector){
      case 0:
      sequence[selectedChannel].stepCount = positive_modulo(sequence[selectedChannel].stepCount + knobChange, 129);
      break;
      case 4:
      sequence[selectedChannel].beatCount = positive_modulo(sequence[selectedChannel].beatCount + knobChange, 129);    
      break;
    }
  }


  void debugScreenInputHandler(){
    if(max7301.pressed(SW_ALT)){
      voltManual = positive_modulo(voltManual + 10*knobChange, 65535);

    } else {
      voltManual = positive_modulo(voltManual + knobChange, 65535);

    }
  }