#include <Arduino.h>
#include "global.h"

//Zetaohm_SAM2695 sam2695;

uint32_t tempoX100 = 12000;
uint8_t stepMode = 0;
uint8_t notePage;
uint8_t	currentState = STEP_DISPLAY;       // Display module + LED module
uint8_t previousState = 0;      // Display module + LED moduleextern

uint8_t selectedChannel = 0;
uint8_t selectedStep;
uint8_t patternChannelSelector = 0b1111;
float   probability;
float   frequency;

boolean extClock = false;
boolean debugBoolean = false;
uint8_t currentPattern = 0;
uint8_t queuePattern = 0;
boolean playing = false;

// MasterClock
boolean wasPlaying = false;
uint8_t masterPulseCount = 24;
unsigned long beatLength = 60000000/(tempoX100/100);
elapsedMicros masterTempoTimer;
elapsedMicros internalClockTimer;
elapsedMicros startTime;
uint16_t calibrationBuffer = 0;

//midi
elapsedMicros blipTimer;
elapsedMicros testTimer;

uint8_t chordArray[27][3] = {
{255,255,255},
{4,7,255},
{3,7,255},
{4,7,10},
{3,7,10},
{4,7,11},
{3,7,11},
{4,7,9},
{3,7,9},
{4,8,255},
{4,6,255},
{5,7,255},
{5,7,10},
{4,7,14},
{4,8,10},
{3,8,10},
{4,8,11},
{4,6,10},
{3,6,10},
{4,6,11},
{2,7,255},
{2,7,10},
{3,6,9},
{3,6,255},
{3,6,10},
{7,255,255},
{4,10,15}
};

void debug(const char* text){
	if (debugBoolean == 1){
		Serial.println(text);
	}
}

int positive_modulo(int i, int n) {
	return (i % n + n) % n;
}

int modulo_minimum(int i, int n, int minimum){
	int mod = (i % n + n) % n;
	if (mod < minimum){
		return minimum;
	} else {
		return mod;
	}
}

int min_max(int value, int min, int max){
	if (value < min){
		return min;
	} else if( value > max){
		return max;
	} else {
		return value;
	}
}

uint8_t getNote(uint8_t index){
	return index + notePage * 16;
}


uint32_t FreeRam2(){ // for Teensy 3.0
//	uint32_t stackTop;
//	uint32_t heapTop;

//    // current position of the stack.
//	stackTop = (uint32_t) &stackTop;

//    // current position of heap.
//	void* hTop = malloc(1);
//	heapTop = (uint32_t) hTop;
//	free(hTop);

//    // The difference is the free, available ram.
//	return stackTop - heapTop;
}
