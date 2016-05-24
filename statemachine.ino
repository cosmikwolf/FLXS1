#include <Arduino.h>

// this subroutine initializes the new state. 
void changeState(uint8_t state){
	Serial.print("Changin State from " + String(currentState) + " to " + String(state));
	for (int i=0; i< MAX_DISPLAY_ELEMENTS; i++){
	  //clear displaycache so all data redraws.
      free(displayCache[i]);
	  displayCache[i] = 0;
	}

	currentState = state;
	resetKnobValues();
	//gdispClear(Black);
	oled.clearScreen();
	Serial.println("   - state change complete");

}


void resetKnobValues(){
	knobRead = 0;
	knob1.write(0);
	Serial.println("resetting knob1: " + String(knob1.read()));
};