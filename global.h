#include <Arduino.h>
#include "Adafruit_MCP23017.h"
#include "Zetaohm_AD5676.h"
#include "Zetaohm_SAM2695.h"

#ifndef _global_h_
#define _global_h_

//STATE DEFINITIONS:
#define STEP_DISPLAY		 	  0
#define	SEQUENCE_MENU		 	  1
#define TIMING_MENU 	  	 66
#define INSTRUMENT_MENU 	 68
#define GENERATIVE_MENU 	 71
#define SEQUENCE_TRAN   	 67
#define SEQUENCE_NAME   	 65
#define SEQUENCE_QUAN   	 69
#define SEQUENCE_EUCL   	 70
#define SEQUENCE_ORDE   	 72
#define SEQUENCE_RAND   	 73
#define SEQUENCE_POSI   	 74
#define SEQUENCE_GLID   	 75
#define SEQUENCE_MIDI   	 76
#define SEQUENCE_CV     	 77
#define SEQUENCE_GATE   	 78
#define GLOBAL_MIDI     	 91
#define GLOBAL_SAVE     	 92
#define GLOBAL_LOAD     	 93
#define GLOBAL_FILE     	 94
#define TEMPO_MENU      	 95
#define PATTERN_SELECT  	 96
#define	CHANNEL_MENU		 97
#define	DELETE_MENU				 98
#define	DEBUG_SCREEN			127
#define DEBUG_PIN					 15
#define SD_CS_PIN					  2
#define OLED_DC						 10
#define OLED_CS						  6
#define OLED_RST					  9

extern Zetaohm_SAM2695 sam2695;
extern Zetaohm_AD5676 ad5676;
extern Adafruit_MCP23017 mcp;

extern uint32_t tempoX100;
extern uint8_t stepMode;
extern uint8_t notePage;
extern uint8_t currentState;       // Display module + LED module
extern uint8_t previousState;      // Display module + LED moduleextern

extern uint8_t selectedChannel;
extern uint8_t selectedStep;

extern boolean extClock;
extern boolean debugBoolean;
// fileops
extern uint8_t sequenceCount;
extern uint8_t currentPattern;
extern uint8_t queuePattern;
extern boolean playing;

// masterClock
extern IntervalTimer masterClockTimer;
extern uint32_t masterClockInterval;
extern uint8_t masterPulseCount;
extern boolean wasPlaying;
extern boolean tempoBlip;
extern unsigned long beatLength;
extern unsigned long lastTimer;
extern elapsedMicros masterTempoTimer;
extern elapsedMicros internalClockTimer;
extern elapsedMicros inputTimer;
extern elapsedMicros startTime;

//midi
extern elapsedMicros blipTimer;
extern elapsedMicros testTimer;
extern elapsedMicros pulseTimer;
extern boolean firstRun;
extern unsigned long lastBeatLength;

void debug(const char* text);
int positive_modulo(int i, int n);
uint8_t getNote(uint8_t index);
uint32_t FreeRam2();

#endif
