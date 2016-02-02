// this subroutine initializes the new state. 
void changeState(uint8_t state){
	switch(state){
		case STEP_MODE:
			initializeStepMode();
		break;
	}
}

// runState runs when something needs to be updated.
void runState(uint8_t state){
	// run display subroutine, 
	switch(state){
		case STEP_MODE:

		break;
	}
}




void initializeStepMode(){
  gdispClear(Black);
}

void runStepMode(){
	
}