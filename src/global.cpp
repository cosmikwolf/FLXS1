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

boolean extClock = false;
boolean debugBoolean = 0;

uint8_t sequenceCount = 4;
uint8_t currentPattern = 0;
uint8_t queuePattern = 0;
boolean playing = false;

// MasterClock
boolean wasPlaying = false;
boolean tempoBlip = false;
uint8_t masterPulseCount = 24;
unsigned long beatLength = 60000000/(tempoX100/100);
unsigned long lastTimer;
elapsedMicros masterTempoTimer;
elapsedMicros internalClockTimer;
elapsedMicros startTime;

//midi
elapsedMicros blipTimer;
elapsedMicros testTimer;
elapsedMicros pulseTimer;
boolean firstRun = false;
unsigned long lastBeatLength;


void debug(const char* text){
	if (debugBoolean == 1){
		Serial.println(text);
	}

}

int positive_modulo(int i, int n) {
	return (i % n + n) % n;
}

uint8_t getNote(uint8_t index){
	return index + notePage * 16;
}

uint32_t FreeRam2(){ // for Teensy 3.0
	uint32_t stackTop;
	uint32_t heapTop;

    // current position of the stack.
	stackTop = (uint32_t) &stackTop;

    // current position of heap.
	void* hTop = malloc(1);
	heapTop = (uint32_t) hTop;
	free(hTop);

    // The difference is the free, available ram.
	return stackTop - heapTop;
}
