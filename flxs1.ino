#include <SPI.h>
#include <SD.h>
#include <MIDI.h>

#include <ssd1351-ugfx-config.h>

#include <Adafruit_NeoPixel.h>

#include "Zetaohm_SAM2695.h"
#include "Zetaohm_AD5676.h"

#include "Sequencer.h"
#include "NoteDatum.h"

//STATE DEFINITIONS:

#define STEP_MODE				0
#define	SEQUENCE_SELECT	1
#define SEQUENCE_NAME   65
#define SEQUENCE_SPED   66
#define SEQUENCE_TRAN   67
#define SEQUENCE_INST   68
#define SEQUENCE_QUAN   69
#define SEQUENCE_EUCL   70
#define SEQUENCE_GENE   71 
#define SEQUENCE_ORDE   72
#define SEQUENCE_RAND   73 
#define SEQUENCE_POSI   74
#define SEQUENCE_GLID   75 
#define SEQUENCE_MIDI   76
#define SEQUENCE_CV     77
#define SEQUENCE_GATE   78
#define GLOBAL_MIDI     91
#define GLOBAL_SAVE     92
#define GLOBAL_LOAD     93
#define GLOBAL_FILE     94
#define TEMPO_SET       95
#define PATTERN_SELECT  96
#define SEQUENCE_SELECT 97

int tempVar = 0;


uint8_t masterPulseCount =24;
uint8_t sequenceCount = 4;
uint8_t currentPattern = 0;
uint8_t queuePattern = 0;
uint8_t selectedSequence = 0;
uint8_t numSteps = 16;

uint8_t	currentState = 0;

uint8_t stepMode; 
uint8_t settingMode;
uint8_t selectedStep = 0;
uint8_t menuSelection = 127;

double bpm;

float tempo = 120.0;
float avgBpm;

boolean playing = false;
boolean wasPlaying = false;
boolean tempoBlip = false;
boolean firstRun = false;
boolean  extClock = false;
boolean need2save = false;

unsigned long beatLength = 60000000/tempo;
unsigned long lastBeatLength;
unsigned long lastMicros;

unsigned long avgDelta;
unsigned long lastPulseLength;
unsigned long avgPulseLength;
unsigned long avgPulseJitter;
unsigned long pulseLength;
unsigned long avgInterval;
unsigned long lastAvgInterval;
unsigned long intervalJitter;
unsigned long avgIntervalJitter;
unsigned long lastTimer;
unsigned long timerAvg;

uint32_t masterClockInterval = 2100;

elapsedMicros masterTempoTimer;
elapsedMicros internalClockTimer;
elapsedMicros blipTimer;
elapsedMicros testTimer;
elapsedMicros pulseTimer;
elapsedMicros printTimer;
elapsedMicros startTime;
elapsedMicros saveTimer;
elapsedMicros pixelTimer;
elapsedMicros displayTimer;

IntervalTimer masterClock;
Sequencer sequence[4];
NoteDatum noteData[4];
Zetaohm_SAM2695 sam2695;
File saveData;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(20, 0, NEO_RGB + NEO_KHZ800);


//end afx-01a

coord_t height, width;
font_t font;

void setup() {
	Serial.begin(115200);

	Serial.println("Initializing SPI");
	SPI.begin();
	SPI.setMOSI(11);
	SPI.setSCK(13);

	Serial.println("Initializing Neopixels");
	pixels.begin();
	pixels.setBrightness(25);

	Serial.println("Initializing Display");
	displayStartup();

	Serial.println("Initializing Button Array");
	buttonSetup();

	Serial.println("Initializing Sequence Objects");
	sequence[0].initialize(0, 16, 4, tempo);
	sequence[1].initialize(1, 16, 4, tempo);
	sequence[2].initialize(2, 16, 4, tempo);
	sequence[3].initialize(3, 16, 4, tempo);

	Serial.println("Beginning Master Clock");
	masterClock.begin(masterClockFunc,masterClockInterval);
}

void loop() {
//	nonBlockingRainbow(10);

//	gdispDrawStringBox(0, 0, width, 50, "ZETAOHM", font, White, justifyCenter);
//	gfxSleepMilliseconds(500);
//	gdispClear(Black);
//
//	gdispDrawStringBox(0, height/2, width, 30, "FLXS1 SEQUENCER", font, White, justifyCenter);
//	gfxSleepMilliseconds(500);
//	gdispClear(Black);
	displayLoop();
	ledLoop();
buttonLoop();
}

inline int positive_modulo(int i, int n) {
	return (i % n + n) % n;
}
