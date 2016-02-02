// this subroutine initializes the new state. 
void changeState(uint8_t state){
	switch(state){
		case STEP_DISPLAY:
			initializeStepMode();
		break;
	}
}

// runState runs when something needs to be updated.
void runState(uint8_t state){
	// run display subroutine, 
	switch(state){
		case STEP_DISPLAY:

		break;
	}
}



void initializeStepMode(){
  gdispClear(Black);

}

void runStepMode(){
// DATA THAT NEEDS TO BE DISPLAYED:
	// Note pitch
	// Mod 	value
	// 
}