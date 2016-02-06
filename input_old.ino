/*
void snapDomeLoop(){
  // for matrix buttons
  for (int i=0; i < 16; i++){
    if (max7301.fell(i)){
      switch (currentState) {
        case STEP_DISPLAY:
          stepModeMatrixHandler(i);
          break; 
        case SEQUENCE_MENU:
          sequencerMenuButtonHandler(i);
          break;
        case CHANNEL_SELECT:
     //     globalMenuButtonHandler(i);
          selectedSequence = i % sequenceCount;
          changeState(STEP_DISPLAY);
          break;
        case PATTERN_SELECT:
          break;
        case INSTRUMENT_SELECT:
          instrumentSelectInputHandler(i);
        default:
         // menuItemButtonHandler(currentState, i);
          break;
      }
    }
  }
  // for all other buttons
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
void encoderLoop(){
  knobRead = knob1.read()/-4;
 // knob2Buffer = knob2.read()/-4;

  if (knobRead != knobPrev) {
    // knob 1 has changed!
    knobPrev = knobRead;
    switch (currentState) {
      case STEP_DISPLAY: // step mode
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
                // if a note is not active, turn it on.
                sequence[selectedSequence].stepData[selectedStep].gateType = 1; 
              } 
              // and finally set the new step value!
              sequence[selectedSequence].setStepPitch(selectedStep, positive_modulo(stepModeBuffer + knobRead, 127));
            }
            break;

          case 1:
          // change the gate type
            sequence[selectedSequence].setGateLength(selectedStep, positive_modulo(stepModeBuffer + knobRead, 127) );

            break;

          case 2:  
          // change length of gate
            sequence[selectedSequence].setGateType(selectedStep, positive_modulo(stepModeBuffer + knobRead, 3) );
            break;

          case 3:
            break;

          case 4:

            break;
        }
        break;

      case SEQUENCE_SPED: // speed setting
        switch(menuSelection){
          case 0:
            sequence[selectedSequence].setStepCount( positive_modulo(stepModeBuffer + knobRead, 63)+1 );
            break;
          case 1: 
            sequence[selectedSequence].setBeatCount( positive_modulo(stepModeBuffer + knobRead, 127) + 1 );
            break;
        }
        break;
      case INSTRUMENT_SELECT:
        int instBuffer = sequence[selectedSequence].instrument;
        switch(menuSelection){
          case 0:
            sequence[selectedSequence].instrument = positive_modulo(stepModeBuffer+knobRead,127);
            sam2695.programChange(0, selectedSequence, sequence[selectedSequence].instrument);
            break;
          case 1: 
            sequence[selectedSequence].volume = positive_modulo(stepModeBuffer+knobRead, 127) ;
            sam2695.setChannelVolume(selectedSequence, sequence[selectedSequence].volume);
            break;
          case 2: 
            sequence[selectedSequence].bank = positive_modulo(stepModeBuffer+knobRead, 127) ;
            sam2695.setChannelBank(selectedSequence, sequence[selectedSequence].bank);
            break;
        }
        break;
      }
  }

}
/*
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
}*/



/*


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
   
    case CHANNEL_SELECT:
      if (buttonNum < sequenceCount){
        selectedSequence = buttonNum;
        Serial.println("setting selectedSequence: " + String(selectedSequence));
        settingMode = 0;
      }
      break;

    case INSTRUMENT_SELECT:
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
            if (settingMode == CHANNEL_SELECT){
              stepMode = 0;
              settingMode = 0;
            } else {
              stepMode = 4; 
              settingMode = CHANNEL_SELECT;
            }  
          break;

      }
    }
  }
}


*/