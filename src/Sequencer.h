#include <Arduino.h>
#include "OutputController.h"
#include "StepDatum.h"

#include "global.h"
#include "globalVariable.h"

#ifndef _Sequencer_h_
#define _Sequencer_h_

#define DEFAULT_PITCH0_VAL 24
#define DEFAULT_GATETYPE 0
#define DEFAULT_VELTYPE 0
#define DEFAULT_VELOCITY 67
#define DEFAULT_CV2SPEED 64
#define DEFAULT_CV2OFFSET 0

class OutputController;

class Sequencer
{
public:
	//Sequencer();

	//	void 		initialize(uint8_t ch, uint8_t stepCount, uint8_t beatCount, uint32_t globalObj->tempoX100);
	void initialize(uint8_t ch, uint8_t stepCount, uint8_t beatCount, OutputController *outputControl, GlobalVariable *globalObj);
	void runSequence();
	// Sequencing Modes
	void sequenceModeStandardStep();
	// Note Trigger Utilities
	//void  	clearNoteData(NoteDatum *noteData);
	void noteTrigger(uint8_t stepNum, bool gateTrig, uint8_t arpTypeTrig, uint8_t arpOctaveTrig);
	uint16_t get_play_pitch(uint8_t stepNum, uint16_t *pitchArray,  uint8_t arpTypeTrig, uint8_t arpOctaveTrig);
	void noteShutOff(uint8_t stepNum, bool gateOff);

	void stoppedTrig(uint8_t stepNum, bool onOff, bool gate);

	uint32_t framesPerSequence();
	uint32_t getCurrentFrame();
	void calculateActiveStep();

	void clockStart();
	void clockReset(bool activeStepReset);
	void updateClockDivision(int8_t clockDiv);
	void ppqPulse(uint8_t pulsesPerBeat);
	bool toggleMute(uint8_t index); //returns true if target was muted

	void skipStep(uint8_t count);
	void jumpToStep(uint8_t stepNum);

	void masterClockPulse();

	bool isFrameSwinging(uint32_t frame);

	int getActivePage();

	int positive_modulo(int i, int n);
	void initNewSequence(uint8_t pattern, uint8_t ch);
	void initializeStep(uint8_t stepNum);
	void setStepPitch(uint8_t step, uint16_t pitch, uint8_t index);
	void setGateLength(uint8_t step, uint8_t length);
	void setGateType(uint8_t step, uint8_t gate);
	void setStepVelocity(uint8_t step, uint8_t velocity);
	void setStepGlide(uint8_t step, uint8_t glideTime);
	void setPlayRange(uint8_t first, uint8_t last);
	void setBeatCount(uint16_t beatCountNew);

	void setStepCount(uint8_t newStepCount);

	void gateInputTrigger(uint8_t inputNum);

	void randomize(uint8_t parameter);

	uint32_t framesPerBeat(int tempoX100);

	uint8_t clockDivisionNum()
	{
		if (clockDivision > 0)
		{
			return 1;
		}
		else
		{
			return abs(clockDivision) + 2;
		}
	}

	uint8_t clockDivisionDen()
	{
		if (clockDivision < 1)
		{
			return 1;
		}
		else
		{
			return clockDivision;
		}
	}

	bool hasData();
	uint8_t quantizePitch(uint8_t note, uint8_t key, uint8_t scale, bool direction);
	uint16_t getStepPitch(uint8_t step, uint8_t index);
	uint16_t getArpCount(uint8_t stepNum);
	uint32_t getArpStartFrame(uint8_t stepNum, uint16_t arpNum);
	uint32_t getArpLastFrame(uint8_t stepNum, uint16_t arpNum);
	uint8_t getArpSpeedNumerator(uint8_t stepNum);
	uint8_t getArpSpeedDenominator(uint8_t stepNum);

	uint32_t getStepLength();
	//uint32_t  ppqSequenceTime();

	StepDatum stepData[MAX_STEPS_PER_SEQUENCE];
	bool suspendNotesUntilReset;
	bool swingSwitch;
	bool swinging;
	bool receivedFirstExtClock;
	bool pendulumSwitch;
	uint8_t previousActiveStepSeqMode; // debug variable
	uint8_t swingCount;
	uint8_t activeStep;
	uint8_t firstStep; //first step to be played
	uint8_t stepCount;
	uint8_t stepsPlayed;

	uint8_t swingX100;
	uint8_t playMode; //forward, backward, pendulum
	uint8_t skipStepCount;
	uint8_t randomLow;
	uint8_t randomHigh;

	uint8_t quantizeKey;
	uint32_t quantizeMode;
	uint32_t quantizeMode_64bit;
	// unsigned long long int testtest; //quantizeMode data placeholder
	uint8_t quantizeModeIndex;
	uint8_t quantizeScale;
	bool quantize_is_edo_scale;
	uint8_t pattern;
	uint8_t channel;
	uint8_t beatCount;
	int8_t gpio_reset;
	int8_t gpio_skipstep;
	int8_t gpio_gatemute;
	int8_t gpio_randompitch;
	uint8_t cv_arptypemod;
	uint8_t cv_arpspdmod;
	uint8_t cv_arpoctmod;
	uint8_t cv_arpintmod;
	uint8_t cv_pitchmod;
	uint8_t cv_gatemod;
	uint8_t cv_glidemod;
	int8_t transpose;
	bool muteGate;
	bool muteCV1;
	bool muteCV2;
	bool fill;
	bool skipNextNoteTrigger;
	bool channelPlaying;
	bool tieFlag;
	bool fillActive;
	uint8_t fillSpeed;

	uint8_t arpTypeModulated[MAX_STEPS_PER_SEQUENCE];
	uint8_t arpOctaveModulated[MAX_STEPS_PER_SEQUENCE];
	int8_t arpSpeedModulation[MAX_STEPS_PER_SEQUENCE];

	int8_t clockDivision;
	uint32_t currentFrame;
	uint16_t pulsesRemaining;

private:
	boolean monophonic;
	boolean beatPulseResyncFlag;
	boolean firstPulse; // this signal is sent when midi clock starts.

	uint8_t lastActiveStep;
	uint8_t lastStepIndex;
	uint8_t pulsesPerBeat;
	uint16_t ppqPulseIndex;

	uint32_t lastStepFrame;
	uint32_t framesPerPulse;
	uint32_t avgClocksPerPulse;
	uint32_t lastPulseClockCount;
	uint32_t lastMasterClockCycleCount;

	OutputController *outputControl;
	GlobalVariable *globalObj;
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
