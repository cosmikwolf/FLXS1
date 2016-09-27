#include <Arduino.h>

#ifndef _global_h_
#define _global_h_

//STATE DEFINITIONS:
#define STEP_DISPLAY		     	 7
#define	SEQUENCE_MENU		     	  2
#define TIMING_MENU 	      	 66
#define INSTRUMENT_MENU     	 68
#define GENERATIVE_MENU     	 71
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
#define	CHANNEL_GATE_MODE		   98
#define	CHANNEL_ENVELOPE_MODE	 99
#define	CHANNEL_STEP_MODE		   100
#define	DELETE_MENU			    	 101
#define	DEBUG_SCREEN		       127
#define	CALIBRATION_MENU		   126

#define STEPMODE_PITCH0       0
#define STEPMODE_PITCH1       9
#define STEPMODE_PITCH2       10
#define STEPMODE_PITCH3       11
#define STEPMODE_VELOCITY     2
#define STEPMODE_GLIDE        12
#define STEPMODE_TEMPO        3
#define STEPMODE_STEPCOUNT    6
#define STEPMODE_BEATCOUNT    7

#define STEPMODE_GATELENGTH   1
#define STEPMODE_GATETYPE     8
#define STEPMODE_ARPTYPE      13
#define STEPMODE_ARPSPEED     14
#define STEPMODE_ARPOCTAVE     15


#define DEBUG_PIN				    	 127
#define SD_CS_PIN				    	  15
#define LED_PIN                 5
#define MIDPLANE_MAX7301_CS_PIN  0
#define BACKPLANE_MAX7301_CS_PIN 33
#define AD5676_CSPIN              2

// MIDI Messages:
# define MIDI_CLOCK   248
# define MIDI_START   250
# define MIDI_CONTINE   251
# define MIDI_STOP   252


extern uint32_t tempoX100;
extern uint8_t stepMode;
extern uint8_t notePage;
extern uint8_t currentState;       // Display module + LED module
extern uint8_t previousState;      // Display module + LED moduleextern

extern uint8_t selectedChannel;
extern uint8_t selectedStep;
extern uint8_t patternChannelSelector;

extern boolean extClock;
extern boolean debugBoolean;
// FlashMemory
extern uint8_t sequenceCount;
extern uint8_t currentPattern;
extern uint8_t queuePattern;
extern boolean playing;

// MasterClock
extern uint8_t masterPulseCount;
extern boolean wasPlaying;
extern unsigned long beatLength;
extern unsigned long lastTimer;
extern elapsedMicros masterTempoTimer;
extern elapsedMicros internalClockTimer;
extern elapsedMicros startTime;

//midi
extern elapsedMicros testTimer;
extern boolean firstRun;

extern uint16_t calibrationBuffer;

void debug(const char* text);
int positive_modulo(int i, int n);
uint8_t getNote(uint8_t index);
uint32_t FreeRam2();

#endif
