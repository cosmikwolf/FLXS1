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
		void    noteTrigger(uint8_t stepNum, bool gateTrig);
		void    noteShutOff(uint8_t stepNum, bool gateOff);

		uint32_t 		calculateStepTimers();
		uint32_t    framesPerSequence();
		uint32_t   	getCurrentFrame();

		void 		clockStart(elapsedMicros startTime);
		void 		clockReset(bool activeStepReset);
		void 		ppqPulse(uint8_t pulsesPerBeat);
		void    masterClockPulse();

		void    getActiveStep(uint32_t frame);

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


		void 		gateInputTrigger(uint8_t inputNum);


		uint8_t   quantizePitch(uint8_t note, uint8_t key, uint8_t scale, bool direction);
		uint8_t  	getStepPitch(uint8_t step, uint8_t index);
		uint8_t   getArpCount(uint8_t stepNum);
		uint32_t  getStepLength();
		//uint32_t  ppqSequenceTime();

		StepDatum stepData[MAX_STEPS_PER_SEQUENCE];
		uint8_t	 	activeStep;
		uint8_t 	syncStep;
		uint8_t 	stepCount;
		uint8_t 	stepsPlayed;
		uint8_t		quantizeKey;
		uint8_t		quantizeScale;
		uint8_t 	pattern;
		uint8_t		channel;
		uint8_t   beatCount;
		uint8_t	  gpio_reset;
		uint8_t		gpio_yaxis;
		uint8_t		gpio_xaxis;
		uint8_t 	clockDivision;

	private:

		boolean  	monophonic;
		boolean  	beatPulseResyncFlag;
		boolean	 	firstPulse;		// this signal is sent when midi clock starts.


		uint8_t	 	lastActiveStep;
		uint8_t		ppqPulseIndex;
		uint8_t 	pulsesPerBeat;

		uint8_t 	lowEndStep;
		uint8_t 	stepsSinceResync;  //temp vars that i have to keep around because data corruption
		uint8_t	resyncSinceReset;

		uint32_t	avgPulseLength;
		uint32_t	framesPerPulse;
		uint8_t	  zeroSequenceCount;

		uint32_t	clockSinceLastPulse;

		uint32_t 	avgClocksPerPulse;
		uint32_t 	tempoX100;

		elapsedMicros pulseTimer;
		OutputController* outputControl;
};


#endif

/*

The Sequence is the brain of the FLXS1

All musical logic is set here.

At every interval of the clock source,
each sequencer object should be polled
to identify what, if any, notes should
be played over the next clock interval,
at the current tempo, and which should
be turned off.

An internal clock is kept, at 1024 frames per step.
at 64 steps, thats 65536 frames per max length sequence.
Whenever the internal clock is compared to the system clock,
it is compared to a timer that was set to 0 at the last time
the clock pulse was received plus the average .





future ideas:

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
