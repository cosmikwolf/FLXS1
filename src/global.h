#include <Arduino.h>

#ifndef _global_h_
#define _global_h_

#define SEQUENCECOUNT         4

#define PIMILLION             3141593
//STATE DEFINITIONS:
#define STEP_DISPLAY		     	 7
#define	SEQUENCE_MENU		     	  2
#define TIMING_MENU 	      	 66
#define GENERATIVE_MENU     	 71
#define GLOBAL_MENU            68
#define SEQUENCE_TRAN       	 67
#define SEQUENCE_NAME       	 65
#define SEQUENCE_QUAN       	 69
#define SEQUENCE_EUCL       	 70
#define SEQUENCE_ORDE       	 72
#define SEQUENCE_RAND       	 73
#define SEQUENCE_POSI       	 74
#define SEQUENCE_GLID       	 75
#define SEQUENCE_MIDI       	 76
#define SEQUENCE_CV         	 77
#define SEQUENCE_GATE       	 78
#define GLOBAL_MIDI         	 91
#define GLOBAL_SAVE         	 92
#define GLOBAL_LOAD         	 93
#define GLOBAL_FILE         	 94
#define TEMPO_MENU          	 95
#define PATTERN_SELECT      	 96
#define	CHANNEL_PITCH_MODE     97
#define	CHANNEL_VELOCITY_MODE		   98
#define	CHANNEL_ENVELOPE_MODE	 99
#define	CHANNEL_STEP_MODE		   100
#define	CHANNEL_TUNER_MODE		 102
#define	DELETE_MENU			    	 101
#define	DEBUG_SCREEN		       127
#define	CALIBRATION_MENU		   126
#define	INPUT_DEBUG_MENU		     125


//PITCH PAGE STEPMODES
#define STEPMODE_PITCH0       0
#define STEPMODE_CHORD        1
#define STEPMODE_GATELENGTH   2
#define STEPMODE_GATETYPE     3
#define STEPMODE_GLIDE        4
#define STEPMODE_ARPTYPE      5
#define STEPMODE_ARPSPEEDNUM  6
#define STEPMODE_ARPSPEEDDEN  7
#define STEPMODE_ARPOCTAVE    8
#define STEPMODE_STEPCOUNT    9
#define STEPMODE_BEATCOUNT    10
#define STEPMODE_PITCH1       11
#define STEPMODE_PITCH2       12
#define STEPMODE_PITCH3       13

#define STEPMODE_GATE         9
//ARP PAGE STEP MODES
//#define STEPMODE_GATELENGTH   10
//#define STEPMODE_GATETYPE     11
//#define STEPMODE_ARPTYPE      12
//#define STEPMODE_ARPSPEED     13
//#define STEPMODE_ARPOCTAVE    14

//VELOCITY PAGE STEP MODES
#define STEPMODE_VELOCITY       0
#define STEPMODE_VELOCITYTYPE   1
#define STEPMODE_LFOSPEED       2

//STEP PAGE STEP MODES:
#define STEPMODE_QUANTIZEKEY     17
#define STEPMODE_QUANTIZESCALE   18

#define STEPMODE_TEMPO          6
#define STEPMODE_EXTCLOCK       7

// for stepmode order
#define MAX_STEPMODE_COUNT 10
#define CHANNEL_PITCH_MODE_ORDER  0
#define CHANNEL_VELOCITY_MODE_ORDER   1
#define CHANNEL_TUNER_MODE_ORDER  2


#define DEBUG_PIN				    	 31
#define SD_CS_PIN				    	  15
#define LED_PIN                 5
#define MIDPLANE_MAX7301_CS_PIN  0
#define BACKPLANE_MAX7301_CS_PIN 33
#define AD5676_CSPIN              2

// MIDI Messages:
#define MIDI_CLOCK   248
#define MIDI_START   250
#define MIDI_CONTINE   251
#define MIDI_STOP   252


//these 12 bit INTs define chromatic scales
//the bit position indicates scale degree

#define CHROMATIC         0b111111111111
#define MAJOR             0b101011010101
#define MINOR             0b101101010101
#define MAJORMINOR        0b101111010101
#define PENTATONIC_MAJOR  0b101001010100
#define PENTATONIC_MINOR  0b100101010010
#define PENTATONIC_BLUES  0b100101010010
#define IONIAN            0b101011010101
#define AEOLIAN           0b101101011010
#define DORIAN            0b101101010110
#define MIXOLYDIAN        0b101011010110
#define PHRYGIAN          0b110101011010
#define LYDIAN            0b101010110101
#define LOCRIAN           0b110101101010

#define POSITIVE 1
#define NEGATIVE 0

#define INTERNAL_CLOCK 0
#define EXTERNAL_MIDI_CLOCK 1
#define EXTERNAL_CLOCK_GATE_0 2
#define EXTERNAL_CLOCK_GATE_1 3
#define EXTERNAL_CLOCK_GATE_2 4
#define EXTERNAL_CLOCK_GATE_3 5
#define GATE_CLOCK_TIMEOUT 5000000

extern uint32_t tempoX100;
extern uint8_t stepMode;
extern uint8_t notePage;
extern uint8_t currentState;       // Display module + LED module
extern uint8_t previousState;      // Display module + LED moduleextern

extern float   probability;
extern float   frequency;

extern uint8_t selectedChannel;
extern uint8_t selectedStep;
extern uint8_t patternChannelSelector;

extern boolean extClock;
extern uint8_t clockMode;
extern boolean debugBoolean;
// FlashMemory
extern uint8_t currentPattern;
extern uint8_t queuePattern;
extern boolean playing;
extern uint8_t chordArray[27][3];

extern uint8_t stepModeOrder[1][MAX_STEPMODE_COUNT];

// MasterClock
extern uint8_t masterPulseCount;
extern boolean wasPlaying;
extern unsigned long beatLength;
extern unsigned long lastTimer;
extern elapsedMicros masterTempoTimer;
extern elapsedMicros internalClockTimer;
extern elapsedMicros startTime;
extern uint16_t masterDebugCounter;

//midi
extern elapsedMicros testTimer;
extern boolean firstRun;
extern boolean eraseAllFlag;

extern uint16_t calibrationBuffer;
extern uint16_t  dacCalibration[16];
extern uint16_t  cvInputRaw[4];
extern uint8_t			gateInputRaw[4];


void debug(const char* text);
int positive_modulo(int i, int n);
int modulo_minimum(int i, int n, int minimum);
int min_max(int value, int min, int max);
uint8_t getNote(uint8_t index);
uint32_t FreeRam2();



extern elapsedMicros debugTimer2;

#endif
