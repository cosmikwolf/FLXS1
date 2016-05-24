/*  -------------------------------------------------------
    Zetaohm_SAM2695.cpp
    A library to facilitate playing music on the Modern Device Zetaohm_SAM2695 board.
    More info is at www.moderndevice.com; refer to "Using the Zetaohm_SAM2695 Arduino 
    Library" in the DocsWiki.
    -------------------------------------
    This version was derived from the library provided by Modern Devices
    . The NewSoftSerial library is used to communicate with the synth
    . The Constructor has been changed to take the rx and tx pin numbers
      (and instantiate the NewSoftSerial library); The default constructtor
      writes to pin 4, and disables receive.
    . A Polymorphic function, samWrite(...) has been added to write thru
      to the synth; it may be used alone, or in combination with existing
      library functions (use of samWrite must leave the synth capable of
      accepting library commands after, however).
    -------------------------------------
    This software is in the public domain.
    Modified 4/2011 R.McGinnis
    ------------------------------------------------------- */

#include "Arduino.h"
#include "Zetaohm_SAM2695.h"
//#include "HardwareSerial.h"
#include <stdio.h>

//#include "NewSoftSerial.h"

// Test TX pin for Serial 3 for Mega
// This may allow for using serial ports but will not use hardware serial support
// In most cases the speed hit will not be great enough to be a large tradeoff
                                                                  
//Zetaohm_SAM2695::Zetaohm_SAM2695(): synth(255, 4) {      // was synth(255, 4)   // 255 -> do not use rx; pin 4 for tx
Zetaohm_SAM2695::Zetaohm_SAM2695() {    
    synthInitialized = 0;                                        // Initialization needs to be done
}
/*
Zetaohm_SAM2695::Zetaohm_SAM2695(HardwareSerial * serial) {
    _HardSerial = serial;
    synthInitialized = 0;                                        // Initialization needs to be done
}
*/
void Zetaohm_SAM2695::begin() {
    if (!(this->synthInitialized)){
    //  this->_HardSerial.begin(31250);                                  // Set MIDI baud rate
 //     this->_HardSerial.begin(31250);                                  // Set MIDI baud rate
        Serial1.begin(31250);
  //    this->Serial.begin(31250);                                  // Set MIDI baud rate
      delay(2);                                                  // let the port settle
      this->synthInitialized = -1;                               // Initialization has been done
    }
}

 size_t Zetaohm_SAM2695::samWrite(byte c) {
    if (!(this->synthInitialized)){
        this->begin();
    }
    Serial1.write(c);
   // this->_HardSerial.write(c);
}

 size_t Zetaohm_SAM2695::samWrite(byte *buf, int cnt) {
    int  i;
    if (!this->synthInitialized){
        this->begin();
    }
    for (i=0; i<cnt; i++) {
        Serial1.write(buf[i]);
        //this->_HardSerial.write(buf[i]);
    }
}

void Zetaohm_SAM2695::noteOn(byte channel, byte pitch, byte velocity) {
    byte command[3] = { 0x90 | (channel & 0x0f), pitch, velocity };
    this->samWrite(command, 3);
}

void Zetaohm_SAM2695::noteOff(byte channel, byte pitch) {
    byte command[3] = { 0x80 | (channel & 0x0f), pitch, byte(0x00) };
    this->samWrite(command, 3);
}

void Zetaohm_SAM2695::programChange(byte bank, byte channel, byte v) {
    // bank is either 0 or 127
    byte command[3] = { 0xB0 | (channel & 0x0f), byte(0x00), bank };
    this->samWrite(command, 3);
    this->samWrite(0xc0 | (channel & 0x0f));
    this->samWrite(v);
}

void Zetaohm_SAM2695::pitchBend(byte channel, int v) {
    // v is a value from 0 to 1023
    // it is mapped to the full range 0 to 0x3fff
    v = map(v, 0, 1023, 0, 0x3fff);
    byte command[3] = { 0xe0 | (channel & 0x0f), byte(v & 0x00ef), byte(v >> 7) };
    this->samWrite(command, 3);
}

void Zetaohm_SAM2695::pitchBendRange(byte channel, byte v) {
    // Also called pitch bend sensitivity
    //BnH 65H 00H 64H 00H 06H vv
    byte command[7] = {0xb0 | (channel & 0x0f), 0x65, 0x00, 0x64, 0x00, 0x06, (v & 0x7f)};
    this->samWrite(command, 7);
}

void Zetaohm_SAM2695::filterCutoff(byte channel, byte v) {
    // Bnh 63h 01h 62h 20h 06h vv
    byte command[7] = {0xb0 | (channel & 0x0f), 0x63, 0x01, 0x62, 0x20, 0x06, (v & 0x7f)};
    this->samWrite(command, 7);
}

void Zetaohm_SAM2695::filterResonance(byte channel, byte v) {
    // Bnh 63h 01h 62h 20h 06h vv
    byte command[7] = {0xb0 | (channel & 0x0f), 0x63, 0x01, 0x62, 0x21, 0x06, (v & 0x7f)};
    this->samWrite(command, 7);
}



void Zetaohm_SAM2695::midiReset() {
    this->samWrite(0xff);
}

void Zetaohm_SAM2695::setChannelVolume(byte channel, byte level) {
    byte command[3] = { (0xb0 | (channel & 0x0f)), 0x07, level };
    this->samWrite(command, 3);
}

void Zetaohm_SAM2695::setChannelBank(byte channel, byte bank) {
    byte command[3] = { (0xb0 | (channel & 0x0f)), 0x00, bank };
    this->samWrite(command, 3);
}

void Zetaohm_SAM2695::allNotesOff(byte channel) {
    // BnH 7BH 00H
    byte command[3] = { (0xb0 | (channel & 0x0f)), 0x7b, 0x00 };
    this->samWrite(command, 3);
}

void Zetaohm_SAM2695::setMasterVolume(byte level) {
    //F0H 7FH 7FH 04H 01H 00H ll F7H
    byte command[8] = { 0xf0, 0x7f, 0x7f, 0x04, 0x01, 0x00, (level & 0x7f), 0xf7 };
    this->samWrite(command, 8);
}

void Zetaohm_SAM2695::setReverb(byte channel, byte program, byte level, byte delayFeedback) {
    // Program 
    // 0: Room1   1: Room2    2: Room3 
    // 3: Hall1   4: Hall2    5: Plate
    // 6: Delay   7: Pan delay
    this->samWrite(0xb0 | (channel & 0x0f));
    this->samWrite(0x50);
    this->samWrite(program & 0x07);
 
    // Set send level
    this->samWrite(0xb0 | (channel & 0x0f));
    this->samWrite(0x5b);
    this->samWrite(level & 0x7f);
  
    if (delayFeedback > 0) {
      //F0H 41H 00H 42H 12H 40H 01H 35H vv xx F7H
      byte command[11] = { 0xf0, 0x41, byte(0x00), 0x42, 0x12, 0x40, 0x01, 0x35, (delayFeedback & 0x7f), 0xf7 };
      this->samWrite(command, 11);
    }
}

void Zetaohm_SAM2695::setChorus(byte channel, byte program, byte level, byte feedback, byte chorusDelay) {
    // Program 
    // 0: Chorus1   1: Chorus2    2: Chorus3 
    // 3: Chorus4   4: Feedback   5: Flanger
    // 6: Short delay   7: FB delay
    this->samWrite(0xb0 | (channel & 0x0f));
    this->samWrite(0x51);
    this->samWrite(program & 0x07);
 
    // Set send level
    this->samWrite(0xb0 | (channel & 0x0f));
    this->samWrite(0x5d);
    this->samWrite(level & 0x7f);
  
    if (feedback > 0) {
    //F0H 41H 00H 42H 12H 40H 01H 3BH vv xx F7H
	byte command[11] = { 0xf0, 0x41, byte(0x00), 0x42, 0x12, 0x40, 0x01, 0x3B, (feedback & 0x7f), 0xf7 };
	this->samWrite(command, 11);
    }
  
    if (chorusDelay > 0) {
    // F0H 41H 00H 42H 12H 40H 01H 3CH vv xx F7H
        byte command[11] = { 0xf0, 0x41, byte(0x00), 0x42, 0x12, 0x40, 0x01, 0x3C, (chorusDelay & 0x7f), 0xf7 };
	this->samWrite(command, 11);
    }
}
