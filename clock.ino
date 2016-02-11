
void changeTempo(uint16_t newTempo){
	tempo = newTempo;
	beatLength = 60000000/tempo;
}

void masterClockFunc(){ 

elapsedMicros loopTimer = 0;


if (inputTimer > 10000){
  inputTimer = 0;
}
//  avgInterval =((micros() - lastMicros) + 9* avgInterval) / 10;
//  timerAvg = (lastTimer + 9*timerAvg) /10;
//  lastMicros = micros();
//
//  intervalJitter = (abs(int(avgInterval) - int(lastAvgInterval)));
//  avgIntervalJitter = (intervalJitter * 9 + avgIntervalJitter) / 10;
//  lastAvgInterval = avgInterval;

  if(playing){
    if( extClock == false ){ 
      internalClockTick();
    } else {
      externalClockTick();
    }

    noteOffSwitch();
    noteOnSwitch();
  }

  wasPlaying = playing;
  lastTimer = loopTimer;

//if (pixelTimer > 2000){
//  pixelTimer = 0;
//}
//
}


void internalClockTick(){
        // int clock
  if (wasPlaying == false){
    Serial.println("TestTimer: " + String(testTimer));
        // if playing has just re-started, the master tempo timer and the master beat count must be reset
   // MIDI.send(Start, 0, 0, 1);  // MIDI.sendSongPosition(0);
    masterTempoTimer = 0;
    masterPulseCount = 0;
    internalClockTimer = 0;
    startTime = 0;

    for (int i=0; i< sequenceCount; i++){
      sequence[i].clockStart(startTime);
      sequence[i].beatPulse(beatLength);
      sequence[i].runSequence(&noteData[i]);
    }
  } else  if (internalClockTimer > 60000000/tempo){
       // Serial.print(" b4 ");
    if (queuePattern != currentPattern) {
      //changePattern(queuePattern, true, true);
    }
    for (int i=0; i< sequenceCount; i++){
      digitalWriteFast(DEBUG_PIN, HIGH);
      sequence[i].runSequence(&noteData[i]);
      sequence[i].beatPulse(beatLength); 

      digitalWriteFast(DEBUG_PIN, LOW);

    }
    tempoBlip = !tempoBlip;
    internalClockTimer = 0;

       // Serial.print(" b5 ");
  }  else { 
    for (int i=0; i< sequenceCount; i++){
      sequence[i].runSequence(&noteData[i]);
    } 
  }
}

void externalClockTick(){
  // ext clock sync
  for (int i=0; i< sequenceCount; i++){
    sequence[i].runSequence(&noteData[i]);
  }
}

void noteOffSwitch(){
  for (int i=0; i< sequenceCount; i++){
    if (noteData[i].noteOff == true){
      for (int n=0; n< 16; n++){
        if (noteData[i].noteOffArray[n] == NULL){
          continue;
        }
         // noteOn(noteData[i].channel,noteData[i].noteOffArray[n]);
        mcp.digitalWrite(noteData[i].channel+4, LOW);
        MIDI.sendNoteOff(noteData[i].noteOffArray[n], 64, noteData[i].channel);
        sam2695.noteOff(noteData[i].channel, noteData[i].noteOffArray[n]);
         // usbMIDI.sendNoteOff(noteData[i].noteOffArray[n], 64, noteData[i].channel);
          //Serial.println("noteOff: " + String(noteData[i].noteOffArray[n]) + "\tbt: " + String(sequence[selectedSequence].beatTracker) ) ;
      }
    }
  }
}

void noteOnSwitch(){
  for (int i=0; i< sequenceCount; i++){
    if (noteData[i].noteOn == true){
      for (int n=0; n< 128; n++){
        if (noteData[i].noteOnArray[n] == NULL){
          continue;
        }

        ad5676.setVoltage(noteData[i].channel,  map(noteData[i].noteOnArray[n], 0,127,0, 65535 ) );
      //  Serial.println("Note Data: " + String(noteData[i].noteOnArray[n]) + " \t\tsetting voltage to: " + String( map(noteData[i].noteOnArray[n], 0,127,0, 65535 )));
        MIDI.sendNoteOn(noteData[i].noteOnArray[n], noteData[i].noteVelArray[n], noteData[i].channel);
        sam2695.noteOn(noteData[i].channel, noteData[i].noteOnArray[n], 127);
        mcp.digitalWrite(noteData[i].channel+4, HIGH);
       // Serial.println("triggering Note");
        //Serial.println( "noteOn: " + String(noteData[i].noteOnArray[n]) 
        // + "\tbt: " + String(sequence[selectedSequence].beatTracker) 
        // + "\tch: " + String(noteData[i].channel)
        // + "\tseq: " + String(noteData[i].sequenceTime)
        // + "\toff: " + String(noteData[i].offset)
        // + "\tstartTime: " + String(startTime));
      }
    }
  }
}

