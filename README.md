Notes!


Timer:

Needs to run at 300 BPM

@300 BPM: each beat takes up 200ms

during this time, display needs to update roughly 4-6 times

notes need to be triggered based upon minimum beat division timing... meaning at 300 bpm, and 16 steps per beat... 

200ms / 16 notes/beat = clock need to be triggered every 12.5 ms minimum.

this means everything needs to happen within this grid. 

currently, clock takes about 2ms per trigger.



List of loops from most frequent to least:

clock,
LED loop,
button loop,
display loop.
save loop.





OVERCLOCK Notes:


168		Neopixels don't work at this speed
		SSD.351 doesn't work at this speed
144		SSD.351 - SLOWWWWWWWWWWWW @ SPI_CLOCK_DIV2 
144		SSD.351 - 515k pixels/sec @ SPI_CLOCK_DIV2 
							 SPI_CLOCK_DIV4

120		SSD.351 - 562k pixels/sec @ SPI_CLOCK_DIV2
96 		SSD.351 - 449k pixels/sec @ SPI_CLOCK_DIV2




Stringbox 2 is rendered and is not selected
stepMode = 0



stepMode is changed to 1
stringbox 2 is selected











void stepDisplay(){ 
  elapsedMicros timer1 = 0;
   char *buf = new char[101]; // sprintf buffer
   char *displayElement[12];
   for (int i; i<12; i++){
    displayElement[i] = new char[51];
   }
  Serial.print("A"); delay(1);

  // Pitch Display
  displayElement[0] = "pitch";
  if ( strcmp(displayElement[0], displayCache[0]) != 0 ) {
    gdispFillStringBox( 64,  0, 64 , 10, displayElement[0], fontSm , White, Blue, justifyCenter);
  }
  displayElement[1] = strdup(midiNotes[sequence[selectedSequence].stepData[selectedStep].pitch]);
  if ( strcmp(displayElement[1], displayCache[1]) != 0 ) {
    if (stepMode == 0) {
        gdispFillStringBox( 64, 10, 64 , 24, displayElement[1], fontLg , White , Blue, justifyCenter);
    } else {
        gdispFillStringBox( 64, 10, 64 , 24, displayElement[1], fontLg , Blue , White, justifyCenter);
    }
  }
Serial.print("B"); delay(1);


  if ( sequence[selectedSequence].stepData[selectedStep].gateType == 0 ){
    displayElement[2] = "Note Off";
  } else if (sequence[selectedSequence].stepData[selectedStep].gateLength == 0){
    displayElement[2] = "pulse";
  } else if (sequence[selectedSequence].stepData[selectedStep].gateLength == 1){
    displayElement[2] = "1 beat";
  } else {
    sprintf(buf, "%d beats", sequence[selectedSequence].stepData[selectedStep].gateLength);
    displayElement[2] = strdup(buf);
  }

  if ( strcmp(displayElement[2], displayCache[2]) != 0 ) {
    Serial.println(String(millis()) + " printing displaycache2!");
    Serial.println("1: displayElement[2]: " + String(displayElement[2]) + "\tdisplayCache[2]: " + String(displayCache[2]));

    if (stepMode == 1){
      gdispFillStringBox( 64, 34, 64 , 10, displayElement[2], fontSm ,White , Blue, justifyCenter);    
    } else {
      gdispFillStringBox( 64, 34, 64 , 10, displayElement[2], fontSm ,Blue , White, justifyCenter);
    }
  }

Serial.print("C"); delay(1);

  sprintf(buf, "Vel: %d", sequence[selectedSequence].stepData[selectedStep].velocity);
  displayElement[3] = strdup(buf);

  if ( strcmp(displayElement[3], displayCache[3]) != 0 ) {
    if (stepMode == 2){
      gdispFillStringBox( 64, 44, 64 , 10, displayElement[3], fontSm ,White , Blue, justifyCenter);
    } else {
      gdispFillStringBox( 64, 44, 64 , 10, displayElement[3], fontSm ,Blue , White, justifyCenter);
    }

  }  

  Serial.print("c"); delay(1);

  // Instrument selection
  sprintf(buf, "%s", instrumentNames[sequence[selectedSequence].instrument]);
  displayElement[4] = strdup(buf);
  if ( strcmp(displayElement[4], displayCache[4]) != 0 ) {
    gdispFillStringBox( 64, 54, 64 , 10, displayElement[4], fontTny ,White , Blue, justifyCenter);
  }
  Serial.println("D");

  // Selected Sequence Number
  sprintf(buf, "ch: %d", selectedSequence+1);
  displayElement[5] = strdup(buf);
  if ( strcmp(displayElement[5], displayCache[5]) != 0 ) {
    gdispFillStringBox( 64, 86, 32 , 10, displayElement[5], fontSm, Blue, White, justifyCenter);
  }
  sprintf(buf, "pt: %d", currentPattern);
  displayElement[6] = strdup(buf);
  if ( strcmp(displayElement[6], displayCache[6]) != 0 ) {
    gdispFillStringBox( 96, 86, 32 , 10, displayElement[6], fontSm, White, Magenta, justifyCenter);
  }
  sprintf(buf, "state: %d", currentState);
  displayElement[7] = strdup(buf);
  if ( strcmp(displayElement[7], displayCache[7]) != 0 ) {
    gdispFillStringBox( 0, 0, 64 , 10, displayElement[7], fontSm, Green, White, justifyCenter);
  }
  sprintf(buf, "step: %d", sequence[selectedSequence].activeStep);
  displayElement[8] = strdup(buf);
  if ( strcmp(displayElement[8], displayCache[8]) != 0 ) {
    gdispFillStringBox( 0, 10, 64 , 10, displayElement[8], fontSm, Green, White, justifyCenter);
  }
  sprintf(buf, "stmd: %d", stepMode);
  displayElement[9] = strdup(buf);
  if ( strcmp(displayElement[9], displayCache[9]) != 0 ) {
    gdispFillStringBox( 0, 20, 64 , 10, displayElement[9], fontSm, Green, White, justifyCenter);
  }
  sprintf(buf, "sqnc: %d", selectedSequence);
  displayElement[10] = strdup(buf);
  if ( strcmp(displayElement[10], displayCache[10]) != 0 ) {
    gdispFillStringBox( 0, 30, 64 , 10, displayElement[10], fontSm, Green, White, justifyCenter);
  }
  sprintf(buf, "step: %d", selectedStep);
  displayElement[11] = strdup(buf);
  if ( strcmp(displayElement[11], displayCache[11]) != 0 ) {
    gdispFillStringBox( 0, 40, 64 , 10, displayElement[11], fontSm, Green, White, justifyCenter);
  }

Serial.println("begin strcpy");

  for (int i=0;i<12;i++){
    Serial.print(String(i) + " ");
    if ( displayElement[i] ){
    //  displayCache[i] = strdup(displayElement[i]);      
        Serial.printf("%p\t%d\t", displayElement[i], sizeof(displayElement[i]) );
        free(displayElement[i]);
    }
  }
    Serial.println();

  delete[] buf;
//  delete[] displayElement;
Serial.println("sizeof: " + String(sizeof(displayElement)));

Serial.println("strcpy done\t\t" + String(millis()));

 // Serial.println("setting memcpy");
 // delay(10);
 // memcpy(displayCache, displayElement, sizeof(displayCache));
 // delay(10);
 // Serial.println("done setting memcpy");
  
}


