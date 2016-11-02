#include <Arduino.h>

#ifndef NoteDatum_h
#define NoteDatum_h

// noteStatus indicates the status of the next note
// stepData[activeStep].noteStatus = stepData[activeStep].pitch;
#define NOTPLAYING_NOTQUEUED 								0   // 0 indicates not playing, not queued
#define CURRENTLY_PLAYING 									1		// 1 indicates the note is currently playing
#define CURRENTLY_QUEUED 										2   // 2 indicates the note is currently queued.
#define CURRENTLY_PLAYING_AND_QUEUED				3   // 3 indicates that the note is currently playing and currently queued
#define NOTE_HAS_BEEN_PLAYED_THIS_ITERATION	4   // 4 indicates that the note has been played this iteration
#define CURRENTLY_ARPEGGIATING							5   // 4 indicates that the note has been played this iteration
#define CURRENT_ARP_OFFSET									63	// difference between notestatus value and the current Arpeggiation note

#define MAX_STEPS_PER_SEQUENCE							64

#define GATETYPE_REST				0
#define GATETYPE_STEP				1
#define GATETYPE_ARP				2   // gate types that are greater than 1 are arpeggios.
// the amount greater than 1 - 1 is the number of arpeggios per step. a gateType value of 2 means that the note retriggers every step.
// a gateType of 5 means that the note will retrigger 4 times per step

#define ARPTYPE_OFF   0
#define ARPTYPE_UP		1
#define ARPTYPE_DN		2
#define ARPTYPE_UPDN1 3
#define ARPTYPE_UPDN2 4
#define ARPTYPE_RNDM	5

typedef struct NoteDatum {
	bool      noteOn;             // is a note going to play?
	bool      noteOff;            // does a note need to be shut off?
	uint8_t   noteOnArray[MAX_STEPS_PER_SEQUENCE];   // contains all notes that need to be played
	bool      noteGateArray[MAX_STEPS_PER_SEQUENCE];   // contains all notes that need to be played
	uint8_t   noteVelArray[MAX_STEPS_PER_SEQUENCE];  // contains CC info for notes to be played
	uint8_t   noteVelTypeArray[MAX_STEPS_PER_SEQUENCE];  // contains CC info for notes to be played
	uint8_t   noteLfoSpeed[MAX_STEPS_PER_SEQUENCE];  // contains CC info for notes to be played
	uint8_t   noteGlideArray[MAX_STEPS_PER_SEQUENCE];  // contains CC info for notes to be played
	uint8_t   noteOffArray[MAX_STEPS_PER_SEQUENCE];  // contains all notes that need to be stopped
	uint8_t   channel;            // sequence channel (0-3)
	uint8_t   noteOnStep;         // step number that originated the noteOn message.
	uint8_t   noteOffStep;        // step number that originated the noteOff message.
	uint32_t	offset;
} NoteDatum;

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
	uint8_t			beatDiv;
	uint8_t			velocity;	    // note velocity
	uint8_t			velocityType;
	uint8_t			lfoSpeed;
	//uint8_t			test;

	// utility variables - dont need to be saved.
	uint8_t				noteStatus;		// if note is playing or not
	uint8_t				arpStatus;		// if note is playing or not. Value indicates arp number.
	uint8_t				notePlaying;	// stores the note that is played so it can be turned off.
	uint8_t				stepStatus;		// if note is playing or not
	//uint32_t			stepOffTime;	// timer value when the note should be stopped.

	uint32_t			offset;		    // note start time offset in mcs from the beat start - recalculated each beat
  elapsedMicros	stepTimer;		// timer to compare to the noteOffTimer for noteOff signal

/*
	uint8_t 		arpSpeedNumerator(){

	};

	uint8_t 		arpSpeedNumerator(uint8_t numerator){
		arpSpeed &= 0b00001111;
		arpSpeed |= (numerator & 0b1111 ) << 4
	};

	uint8_t 		arpSpeedDenominator(){

	};

	uint8_t 		arpSpeedDenominator(uint8_t denominator){
	};
*/
/*
	uint32_t	arpLength() {
		//return stepOffTime / gateType;
		if(arpType != 0){
			return stepOffTime / arpSpdDen;
		} else {
			return stepOffTime;
		}
	}*/

} StepDatum;


#endif
