#include <Arduino.h>

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
	uint8_t			lfoSpeed;
	// utility variables - dont need to be saved.
	uint8_t				noteStatus;		// if note is playing or not
	uint8_t				arpStatus;		// if note is playing or not. Value indicates arp number.
	uint8_t				notePlaying;	// stores the note that is played so it can be turned off.
	uint32_t			offset;		    // note start time offset in mcs from the beat start - recalculated each beat
	elapsedMicros	stepTimer;		// timer to compare to the noteOffTimer for noteOff signal
} StepDatum;

#endif
