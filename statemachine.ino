// this subroutine initializes the new state. 
void changeState(uint8_t state){
	Serial.print("Changin State from " + String(currentState) + " to " + String(state));
	currentState = state;
	knob1.write(0);
	gdispClear(Black);
	Serial.println("   - state change complete");
}
