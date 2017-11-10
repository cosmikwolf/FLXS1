#include <Arduino.h>
#include "global.h"
#ifndef StepDatum_h
#define StepDatum_h

typedef	struct StepDatum {
	// data that needs to be stored
	uint8_t			pitch[4];		    // note pitch
	uint8_t			chord; //to select what chord to set
	uint8_t 		gateType;		// gate type (hold, repeat, arpeggio)
	uint8_t			gateLength;		// gate length becomes arpCount if
	uint8_t 		arpType;		// off, up, down, up & down etc
	uint8_t 		arpOctave;		// off, up, down, up & down etc
	uint8_t 		arpSpdNum;	//
	uint8_t 		arpSpdDen;	//
	uint8_t			glide;			// portamento time - to be implemented.
	int8_t			beatDiv;
	uint8_t			velocity;	    // note velocity
	uint8_t			velocityType;
	uint8_t			cv2offset;
	uint8_t			lfoSpeed;
	// utility variables - dont need to be saved.
	uint8_t			noteStatus;		// if note is playing or not
	uint8_t			arpStatus;		// if note is playing or not. Value indicates arp number.
	uint8_t			notePlaying;	// stores the note that is played so it can be turned off.
	//int32_t			offset;		    // note start time offset in mcs from the beat start - recalculated each beat
	int32_t 	  framesRemaining;  // number of frames to continue playing.
	int32_t 	  arpLastFrame;  // When to end current arp.

  uint8_t     test1;
  uint8_t     test2;
  uint8_t     test3;
  uint8_t     test4;
  uint8_t     test5;
  uint8_t     test6;
  uint8_t     test7;
  uint8_t     test8;
  uint8_t     test9;
  uint8_t     test10;
  uint8_t     test11;
  uint8_t     test12;
  uint8_t     test13;
  uint8_t     test14;
  uint8_t     test15;
  uint8_t     test16;
  uint8_t     test17;
  uint8_t     test18;
  uint8_t     test19;
  uint8_t     test20;
  uint8_t     test21;
  uint8_t     test22;
  uint8_t     test23;
  uint8_t     test24;

  uint8_t     test2_1;
  uint8_t     test2_2;
  uint8_t     test2_3;
  uint8_t     test2_4;
  uint8_t     test2_5;
  uint8_t     test2_6;
  uint8_t     test2_7;
  uint8_t     test2_8;
  uint8_t     test2_9;
  uint8_t     test2_10;
  uint8_t     test2_11;
  uint8_t     test2_12;
  uint8_t     test2_13;
  uint8_t     test2_14;
  uint8_t     test2_15;
  uint8_t     test2_16;
  uint8_t     test2_17;
  uint8_t     test2_18;
  uint8_t     test2_19;
  uint8_t     test2_20;
  uint8_t     test2_21;
  uint8_t     test2_22;
  uint8_t     test2_23;
  uint8_t     test2_24;

	bool gateTrig(){ //should the gate be turned on this step?
		switch(gateType){
			case GATETYPE_REST:
				return false;
			break;
			case GATETYPE_STEP:
				return true;
			break;
      case GATETYPE_TIE:
        return true;
      break;
      case GATETYPE_1HIT:
        if (arpStatus > 0){
          return false;
        } else {
          return true;
        }
      break;
		}
	}
	bool gateOff(){ // should the gate be turned off this step?
		switch(gateType){
			case GATETYPE_REST:
				return true;
			break;
			case GATETYPE_STEP:
				return true;
			break;
      case GATETYPE_TIE:
        return false;
      break;
      case GATETYPE_1HIT:
        return true;
      break;
		}
	}
} StepDatum;

#endif
