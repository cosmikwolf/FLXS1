#include "Arduino.h"
#include "Sequencer.h"

Sequencer::Sequencer() {

};

void Sequencer::initialize(uint8_t ch, uint8_t stepCount, uint8_t beatCount, float tempo){
	this->channel = ch;
	this->stepCount = stepCount;
  this->beatCount = beatCount;
	this->tempo = tempo;
	this->stepTimer = 0;
	this->sequenceTimer = 0;
  this->instrument = 0;
  this->instType = 0;
  this->volume = 100;
	for (int i=0; i < stepCount; i++){
   // stepData[i].gateLength = 1;
  //  stepData[i].velocity = 127;
    stepData[i].pitch = 24;
	};
  beatLength = 60000000/tempo;
  calculateStepTimers();
  monophonic = true;
};	


void Sequencer::initNewSequence(){
  stepCount = stepCount;
  beatCount = 4;
  quantizeKey = 0;
  instrument = 0;
  volume = 100;
  bank = 0;
  instType = 2; //initialized regular instrument
  for(int n=0; n < stepCount; n++){
    stepData[n].pitch      = 24;
    stepData[n].gateLength = 1;
    stepData[n].gateType   = 0;
    stepData[n].velocity   = 127;
    stepData[n].glide      = 0;
  }
};


void Sequencer::setTempo(uint16_t tempo){
  this->tempo = tempo;
  beatLength = 60000000/tempo;
  calculateStepTimers();
}

void Sequencer::setStepPitch(uint8_t step, uint8_t pitch){
	stepData[step].pitch = pitch;
};

void Sequencer::setGateLength(uint8_t step, uint8_t length){
	stepData[step].gateLength = length;
  calculateStepTimers();
};

void Sequencer::setStepCount(uint8_t stepCountNew){
  stepCount = stepCountNew;
  calculateStepTimers();
};

void Sequencer::setBeatCount(uint16_t beatCountNew){
  beatCount = beatCountNew;
  calculateStepTimers();
};

void Sequencer::setGateType(uint8_t step, uint8_t gate){
  stepData[step].gateType = gate;
}

void Sequencer::setStepVelocity(uint8_t step, uint8_t velocity){
	stepData[step].velocity = velocity;
};

void Sequencer::setStepGlide(uint8_t step, uint8_t glideTime){
	stepData[step].glide = glideTime;
}

void Sequencer::calculateStepTimers(){
  uint32_t noteTimerMcsCounter = 0;
  stepLength = beatLength*beatCount/stepCount;

 // Serial.println(" stepCount: " + String(stepCount) + " stepLength: " + String(stepLength) + " beatLength: " + String(beatLength) + " tempo: " + String(tempo));
  for (int stepNum = 0; stepNum < stepCount; stepNum++){
    stepUtil[stepNum].noteTimerMcs = (stepData[stepNum].gateLength*stepLength);
    stepUtil[stepNum].beat = floor(noteTimerMcsCounter / beatLength);
    stepUtil[stepNum].offset = stepNum*stepLength;
    noteTimerMcsCounter = noteTimerMcsCounter + stepUtil[stepNum].noteTimerMcs;
  /*
    Serial.println( String(channel) + " " + String(stepNum) + " " +
      "ntm: " + String(stepData[stepNum].noteTimerMcs) +
      "\tbt: " + String(stepData[stepNum].beat) +
      "\toff: " + String(stepData[stepNum].offset)  + 
      "\tgaL: " + String(stepData[stepNum].gateLength) + 
      "\tgaT: " + String(stepData[stepNum].gateType) +
      "\tptch: " + String(stepData[stepNum].pitch) +
      "\tOC: " + String(noteTimerMcsCounter) 
    );
  */
  }


  
}


void Sequencer::clockStart(elapsedMicros startTime){
  firstBeat = true;
  stepTimer = startTime;
};

void Sequencer::beatPulse(uint32_t beatLength){
  // this is sent every 24 pulses received from midi clock
  // and also when a play or continue command is received.
  this->beatLength = beatLength;
  calculateStepTimers();
  beatTimer = 0;
  tempoPulse = true;

  if(firstBeat){  
    activeStep = 0;
    beatTracker = 0;
    firstBeat = false; 
  } else {
    //beatTracker= positive_modulo(beatTracker + 1, beatCount); 
    beatTracker = (beatTracker + 1) % beatCount;
  }

  if (beatTracker == 0) {
    for(int i = 0; i < stepCount; i++){
      // reset the note status for notes that have been played.
      // leave notes that have not been turned off yet.
    //  if (stepData[i].noteStatus == 4){
    //    stepData[i].noteStatus = 0;
    //  }
    }
    activeStep = 0;
    stepTimer = 0;
    if(channel == 1){
     // Serial.println("Resetting Sequence Timer " + String(sequenceTimer));
    }
    sequenceTimer = 0;
    // We are resetting the note status when the sequenceTimer is no longer larger than 
    // the offset value. This means that runSequence needs to run at least once before
    // it can reset the note statuses. For the first note, that means it wont ever reset
    // so we must reset it manually here.
    stepUtil[0].noteStatus = 0;
  } 
};

void Sequencer::runSequence(NoteDatum *noteData){
  // clear noteData from last iteration.
  // if (firstBeat){
  //   Serial.println("firstBeat - runSequenceStart: " + String(sequenceTimer) );
  // }

  //unsigned long timer = micros();

  noteData->noteOff = false;
  noteData->noteOn = false;
  for(int i = 0; i < stepCount; i++){
    noteData->noteOnArray[i] = NULL;
    noteData->noteVelArray[i] = NULL;
    noteData->noteOffArray[i] = NULL;
  }
  noteData->channel = 0;         
  noteData->noteOnStep = 0;      
  noteData->noteOffStep = 0;     

  if (stepTimer > stepLength) {
    activeStep = positive_modulo(activeStep + 1, stepCount);
    stepTimer = 0;
  }

  for (int stepNum = 0; stepNum < stepCount; stepNum++){
    // set notes to be stopped, and marked as played.
    if ( (stepUtil[stepNum].stepTimer > stepUtil[stepNum].noteTimerMcs) && (stepUtil[stepNum].noteStatus == 1) ){
      // if the note is playing and has played out the gate length, end the note.
      noteData->noteOff = true;
      noteData->channel = channel;
      noteData->noteOffStep = stepNum;
      int n = 0;
      for (int f=0; f<stepCount; f++){
        if (noteData->noteOffArray[f] == NULL){
          noteData->noteOffArray[f] = stepUtil[stepNum].notePlaying;
          break;
        }
      }
      stepUtil[stepNum].noteStatus = 4;
    }

    // set notes to be played
    if((stepData[stepNum].gateType != 0 ) ){
//      if ( sequenceTimer >= stepData[stepNum].offset || firstBeat ) {

      if ( sequenceTimer + 15000>= stepUtil[stepNum].offset ) {
        if (stepUtil[stepNum].noteStatus == 0)  {
         //   if (stepNum > activeStep){
         //     activeStep = stepNum;
         //   }
         /* if(stepData[stepNum].pitch > 0){
             Serial.println("third condition: " + String(stepNum) + "\tst:" + String(sequenceTimer) + "   slsn:" +String(stepLength*stepNum) + "\tste:" + String(stepTimer) + "\tof:" + String(stepData[stepNum].offset) + "\tns:" + String(stepData[stepNum].noteStatus) + "\tgt:" + String(stepData[stepNum].gateType) + "\tnt:" + String(stepData[stepNum].noteTimerMcs) );
            Serial.println("timekeeper: " + String(timekeeper));
          } */

          // Serial.println("Note should be played!" + String(stepNum));


          //shut off any other notes that might still be playing.
          for (int stepNum = 0; stepNum < stepCount; stepNum++){
            if(stepUtil[stepNum].noteStatus == 1){
              noteData->noteOff = true;
              noteData->channel = channel;
              noteData->noteOffStep = stepNum;
              for (int f=0; f<stepCount; f++){
                if (noteData->noteOffArray[f] == NULL){
                  noteData->noteOffArray[f] = stepUtil[stepNum].notePlaying;
                  break;
                }
              }
              stepUtil[stepNum].noteStatus = 4;
            }
          }
          noteData->noteOn = true;
          noteData->channel = channel;
          noteData->noteOnStep = stepNum;
          
          noteData->triggerTime = micros();
          noteData->sequenceTime = sequenceTimer;
          noteData->offset = stepUtil[stepNum].offset;
          stepUtil[stepNum].stepTimer = 0;
          stepUtil[stepNum].noteStatus = 1;

          if (quantizeKey == 1){
            stepUtil[stepNum].notePlaying = quantizePitch(stepData[stepNum].pitch, aminor, 1);
            Serial.println("quantized note: " + String(stepData[stepNum].pitch) + " -> " + String(stepUtil[stepNum].notePlaying));
          } else {
            stepUtil[stepNum].notePlaying = stepData[stepNum].pitch;          
          }

          for (int i=0; i< stepCount; i++){
            if (noteData->noteOnArray[i] == NULL){
              noteData->noteOnArray[i] = stepUtil[stepNum].notePlaying;                
              noteData->noteVelArray[i] = stepData[stepNum].velocity;
              break;
            }
          }

          // noteStatus indicates the status of the next note
          // 0 indicates not playing, not queued
          // 1 indicates the note is currently playing
          // 2 indicates the note is currently queued.
          // 3 indicates that the note is currently playing and currently queued
          // 4 indicates that the note has been played this iteration
          // stepData[activeStep].noteStatus = stepData[activeStep].pitch;   
        }
      } else {
        if (stepUtil[stepNum].noteStatus == 4){
          stepUtil[stepNum].noteStatus = 0;
        }
      }
    }
  }
  //timekeeper = ((micros() - timer)+9*timekeeper)/10;
}

uint8_t Sequencer::quantizePitch(uint8_t note, uint32_t scale, bool direction){
  uint8_t count = 0;
  while ( (0b100000000000 >> (note % 12) ) & ~scale ) {
    if (direction){
      note += 1;
    } else {
      note -= 1;
    }
    count += 1;
    if (count > 12) {
      break; // emergency break if while loop goes OOC
    }
  }
  return note;
}

uint8_t Sequencer::getStepPitch(uint8_t step){
	return stepData[step].pitch;
};

int Sequencer::positive_modulo(int i, int n) {
    return (i % n + n) % n;
}


