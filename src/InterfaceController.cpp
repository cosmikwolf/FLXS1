#include "InterfaceController.h"

InterfaceController::InterfaceController(){

};

void InterfaceController::initialize(OutputController * outputControl) {
    Serial.println("Initializing InterfaceController");
    LEDArray ledArray = LEDArray();
    DisplayModule display = DisplayModule();
    InputModule buttonIo = InputModule();

    ledArray.initialize();
    display.initialize();
  	buttonIo.buttonSetup(outputControl);
};

void InterfaceController::loop(){
  ledArray.loop();
  buttonIo.buttonLoop();
  display.displayLoop();
};
