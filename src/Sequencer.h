#include <Arduino.h>
#include "OutputController.h"
#include "StepDatum.h"
#include "global.h"

#ifndef _Sequencer_h_
#define _Sequencer_h_

class OutputController;

class Sequencer
{
	public:
		//Sequencer();

	//	void 		initialize(uint8_t ch, uint8_t stepCount, uint8_t beatCount, uint32_t tempoX100);
		void 		initialize(uint8_t ch, uint8_t stepCount, uint8_t beatCount, uint32_t tempoX100, OutputController* outputControl);
		void 		runSequence();
		// Sequencing Modes
		void 		sequenceModeStandardStep();
		// Note Trigger Utilities
	//	void  	clearNoteData(NoteDatum *noteData);
		void 		incrementActiveStep();
		void    noteTrigger(uint8_t stepNum, bool gateTrig);
		void    noteShutOff(uint8_t stepNum, bool gateOff);

		void 		calculateStepTimers();
		void 		beatPulse(uint32_t beatLength);
		void 		clockStart(elapsedMicros startTime);
		int  		positive_modulo(int i, int n);
		void 		initNewSequence(uint8_t pattern, uint8_t ch);

		void 		setTempo(uint32_t tempoX100);
		void 		setStepPitch(uint8_t step, uint8_t pitch, uint8_t index);
		void 		setGateLength(uint8_t step, uint8_t length);
		void 		setGateType(uint8_t step, uint8_t gate);
		void 		setStepVelocity(uint8_t step, uint8_t velocity);
		void 		setStepGlide(uint8_t step, uint8_t glideTime);

		void 		setStepCount(uint8_t stepCountNew);
		void 		setBeatCount(uint16_t beatCountNew);

		void 		ppqPulse(uint8_t maxPulseCount);
		uint8_t   quantizePitch(uint8_t note, uint8_t key, uint8_t scale, bool direction);
		uint8_t  	getStepPitch(uint8_t step, uint8_t index);
		uint8_t   getArpCount(uint8_t stepNum);
		uint32_t  getStepLength(uint8_t stepNum);
		uint32_t  ppqSequenceTime();

		boolean  	monophonic;
		boolean  	beatPulseResyncFlag;
		boolean	 	firstBeat;		// this signal is sent when midi clock starts.

		uint8_t	 	activeStep;
		uint8_t	 	zeroBeat;
		uint8_t		zeroBeatIndex;
		uint8_t		ppqPulseIndex;
		uint8_t 	stepCount;
		uint8_t		quantizeKey;
		uint8_t		quantizeScale;
		uint8_t 	pattern;
		uint8_t		channel;
		uint8_t 	maxPulseCount;

		uint8_t 	lowEndStep;
		uint8_t 	tempVar1;  //temp vars that i have to keep around because data corruption
		uint8_t 	tempVar2;  //temp vars that i have to keep around because data corruption
		uint8_t 	tempVar3;  //temp vars that i have to keep around because data corruption

		int16_t 	sequenceJitter[9];
		uint16_t  beatCount;

		uint32_t 	beatLength;
//		uint32_t 	stepLength;
		uint32_t 	tempoX100;
		elapsedMicros sequenceTimer; // timer for sequence interval to sequence interval
		elapsedMicros pulseTimer;
		StepDatum stepData[MAX_STEPS_PER_SEQUENCE];

		OutputController* outputControl;

	private:

};


#endif

/*

The Sequence is the brain of the AFX-01

All musical logic is set here.

Each sequence at its base is just a set of numbers, stored in a few arrays.
The index of the array equates to the step number of the note value.

At every interval of the midi engine,
each sequencer object should be polled
to identify what, if any, notes should
be played, and should have already played.

Each sequence can be played differently.

HOW THIS SHOULD BE RUN:

noteToPlay(time) should be called very frequently
	it should check to see what note should have played between lastRun and now

There is basic linear sequencing. This has a few attributes:
	Step Length:			1/32 notes - 1 whole note
	Number of steps:	1 - 128
	Retrigger step: 	0  - Last

Each step has various attributes:
	Step Type:
		Sequence Trigger Type:
			Sequence number: 	Int
			Repeat:						boolean
		Note Trigger Type:

	Pitch 1: note value
	Pitch 2: note value
	Pitch 3: note value
	Velocity: 0 - 127
	¬µ timing:	-1000ms - 1000ms micro timing adjustments
	Note Length: 1 - max steps in sequence:

There are some modifications that are distructive changes to the sequence itself:

All generators share thes attributes:
		High Note:						highest note for generation
		Low Note:							highest note for generation
		Affects Notes:				boolean
		Affects Velocity: 		boolean
		Affects Note Length:	boolean

	Random Generator: 			Sets the sequence notes based on a random generator

	Perlin Noise Generator: Sets a sequence based on pre-rendered perlin noise
		Input Value:					Input source number for perlin noise
		Octave:								The octave of the perlin noise

	Waveform Generator:			Generate midi notes based on a graphical waveform
		Wave Form:						Sin, Square, Saw, Reverse Saw, Triangle, S+H
		Period:								0-2œÄ
		Amplitude:						0-64
		Quantization:					Scale

There are some post processing functions that can modify the sequence at runtime:

	Randomization:
		% chance to randomize: 0-100%
		Max High Note: 0 - 127
		Min Low Note: 0 - 127

	Reverse Order:
		Play notes in reverse order

	Random Order
		Play notes in random Order
		Note Length Preservation: boolean
			- preserve the original length of each note, just randomize note pitch

	Inverse:
		Play the note on the opposite side of middle C

	Quantization:
		Scale: 	The A B C D E F G scales, sharps and flats, all modes + some

*/



		/*

		other notes:

		Dynamic Arrays:
		http://www.fredosaurus.com/notes-cpp/newdelete/50dynamalloc.html


*/
