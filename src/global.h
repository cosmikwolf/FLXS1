#include <Arduino.h>

#ifndef _global_h_
#define _global_h_

#define SEQUENCECOUNT         4

#define kSerialSpeed 115200
#define kMasterClockInterval 300
#define kSequenceTimerInterval 500
#define kPeripheralLoopTimer 15000
#define kMidiClockInterval  600
#define kCacheClockInterval 10000
#define kMosiPin 11
#define kSpiClockPin 13

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


#define	CHANNEL_ENVELOPE_MODE	 99
#define	CHANNEL_STEP_MODE		   100
#define	CHANNEL_INPUT_MODE		 103
#define	DELETE_MENU			    	 101
#define	DEBUG_SCREEN		       127
#define	CALIBRATION_MENU		   126
#define	INPUT_DEBUG_MENU		     125

// MENUS
#define	PITCH_GATE_MENU        97
#define	ARPEGGIO_MENU          98
#define	SEQUENCE_MENU_1        99
#define	SEQUENCE_MENU_2        100
#define	TUNER_MENU	         	 101
#define	VELOCITY_MENU          102


//PITCH PAGE STEPMODES
#define STATE_PITCH0       0
#define STATE_GATELENGTH   1
#define STATE_GATETYPE     2
#define STATE_GLIDE        3
#define STATE_ARPTYPE      4
#define STATE_ARPSPEEDNUM  5
#define STATE_ARPSPEEDDEN  6
#define STATE_ARPOCTAVE    7
#define STATE_CHORD        8
#define STATE_STEPCOUNT    9
#define STATE_BEATCOUNT    10
#define STATE_PITCH1       11
#define STATE_PITCH2       12
#define STATE_PITCH3       13

#define STATE_TUNER         14
//VELOCITY PAGE STEP MODES
#define STATE_VELOCITY       15
#define STATE_VELOCITYTYPE   16
#define STATE_LFOSPEED       17

//STEP PAGE STEP MODES:
#define STATE_QUANTIZEKEY     18
#define STATE_QUANTIZESCALE   19

#define STATE_TEMPO          20
#define STATE_EXTCLOCK       21

#define STATE_PATTERNSELECT   22



#define MAX_STATE_COUNT 10
#define PITCH_GATE_MENU_ORDER  0

#define DEBUG_PIN				    	 31
#define PIN_EXT_AD_1   27
#define PIN_EXT_AD_2   30
#define PIN_EXT_RX     26
#define CLOCK_PIN				    	  23

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

//INPUT MAPPINGS
#define SW_00    0
#define SW_01    1
#define SW_02    2
#define SW_03    3
#define SW_04    4
#define SW_05    5
#define SW_06    6
#define SW_07    7
#define SW_08    8
#define SW_09    9
#define SW_10    10
#define SW_11    11
#define SW_12    12
#define SW_13    13
#define SW_14    14
#define SW_15    15
#define SW_PLAY  16
#define SW_REC   17
#define SW_STOP  18
#define SW_M0    19
#define SW_M1    20
#define SW_M2    21
#define SW_M3    22
#define SW_PGDN  23
#define SW_PGUP  24
#define SW_PATTERN  25
#define SW_MENU   26
#define SW_SHIFT   27
#define ENCODER1LEFTPIN 16
//#define ENCODER1LEFTPIN 32
#define ENCODER1RIGHTPIN 25
#define MAX7301PIN  5


extern uint32_t tempoX100;
extern uint8_t stepMode;
extern uint8_t notePage;
extern uint8_t currentMenu;       // Display module + LED module
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

extern uint8_t stepModeOrder[1][MAX_STATE_COUNT];

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
extern uint16_t dacCalibration[16];
extern uint16_t cvInputRaw[4];
extern uint8_t	gateInputRaw[4];


void debug(const char* text);
int positive_modulo(int i, int n);
int modulo_minimum(int i, int n, int minimum);
int min_max(int value, int min, int max);
uint8_t getNote(uint8_t index);
uint32_t FreeRam2();



extern elapsedMicros debugTimer2;

#endif
