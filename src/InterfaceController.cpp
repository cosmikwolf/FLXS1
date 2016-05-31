#include "InterfaceController.h"

#define kSerialSpeed 115200
#define kMosiPin 11
#define kSpiClockPin 13

void InterfaceController::initialize() {
  	Serial.begin(kSerialSpeed);

  	Serial.println("Initializing SPI");
  	SPI.begin();
  	SPI.setMOSI(kMosiPin);
  	SPI.setSCK(kSpiClockPin);

  	Serial.println("Initializing Display");
    // PUT STUFF LIKE THIS INSIDE CONSTRUCTORS
  	display->initialize();

  	Serial.println("Initializing SAM2695");
    // PUT STUFF LIKE THIS INSIDE CONSTRUCTORS
  	sam2695.begin();
  	sam2695.programChange(0, 0, 38);       // give our two channels different voices
  	sam2695.programChange(0, 1, 30);
  	sam2695.programChange(0, 2, 128);       // give our two channels different voices
  	sam2695.programChange(0, 3, 29);

  	Serial.println("Initializing Button Array");
  	buttonIo->buttonSetup();

  	Serial.println("Initializing MIDI");
  	midiSetup();

  	Serial.println("Initializing DAC");
    // PUT STUFF LIKE THIS INSIDE CONSTRUCTORS
  	ad5676.begin(3);
  	ad5676.softwareReset();
  	delay(1);
  	ad5676.internalReferenceEnable(true);
  	ad5676.internalReferenceEnable(true);

  	Serial.println("Setting up debug pin");
  	pinMode(DEBUG_PIN, OUTPUT);
  	pinMode(4, OUTPUT);

    // PUT STUFF LIKE THIS INSIDE CONSTRUCTORS
  	Serial.println("initializing gate outputs");
  	mcp.begin(1);      // use default address 0
  	mcp.pinMode(9, INPUT);
  	mcp.pinMode(4, OUTPUT);
  	mcp.pinMode(5, OUTPUT);
  	mcp.pinMode(6, OUTPUT);
  	mcp.pinMode(7, OUTPUT);

   	Serial.println("Initializing Flash Memory");
    // PUT STUFF LIKE THIS INSIDE CONSTRUCTORS
    saveFile.initialize();

  	Serial.println("Initializing Neopixels");
    // PUT STUFF LIKE THIS INSIDE CONSTRUCTORS
    ledArray->initialize();

  	Serial.println("Beginning Master Clock");
}

void InterfaceController::displayLoop(){

}

void InterfaceController::inputLoop(){
  ledArray->loop();
  buttonIo->buttonLoop();
}

void InterfaceController::ledLoop(){

}
