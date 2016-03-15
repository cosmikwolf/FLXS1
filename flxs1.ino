#include <SPI.h>
#include <SD.h>
#include <MIDI.h>
#include <ssd1351-ugfx-config.h>
#include <Adafruit_NeoPixel.h>
#include <i2c_t3.h>
#include <Zetaohm_MAX7301.h>
#include <Encoder.h>
#include "Adafruit_MCP23017.h"
#include "Zetaohm_SAM2695.h"
#include "Zetaohm_AD5676.h"
#include "Sequencer.h"
#include "NoteDatum.h"
#include <Audio.h>
#include <analyze_notefreq.h>

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

float midiFreq[128] = { 8.17, 8.66, 9.17, 9.72, 10.30, 10.91, 11.56, 12.24, 12.97, 13.75, 14.56, 15.43, 16.35, 17.32, 18.35,
19.44, 20.60, 21.82, 23.12, 24.49, 25.95, 27.50, 29.13, 30.86, 32.70, 34.64, 36.70, 38.89, 41.20, 43.65, 46.24, 48.99, 51.91,
55.00, 58.27, 61.73, 65.40, 69.29, 73.41, 77.78, 82.40, 87.30, 92.49, 97.99, 103.82, 110.00, 116.54, 123.47, 130.81, 138.59,
146.83, 155.56, 164.81, 174.61, 184.99, 195.99, 207.65, 220.00, 233.08, 246.94, 261.62, 277.18, 293.66, 311.12, 329.62, 349.22,
369.99, 391.99, 415.30, 440.00, 466.16, 493.88, 523.25, 554.36, 587.32, 622.25, 659.25, 698.45, 739.98, 783.99, 830.60, 880.00,
932.32, 987.76, 1046.50, 1108.73, 1174.65, 1244.50, 1318.51, 1396.91, 1479.97, 1567.98, 1661.21, 1760.00, 1864.65, 1975.53,
2093.00, 2217.46, 2349.31, 2489.01, 2637.02, 2793.82, 2959.95, 3135.96, 3322.43, 3520.00, 3729.31, 3951.06, 4186.00, 4434.92,
4698.*63, 4978.03, 5274.04, 5587.65, 5919.91, 6271.92, 6644.87, 7040.00, 7458.62, 7902.13, 8372.01, 8869.84, 9397.27, 9956.06,
10548.08, 11175.30, 11839.82, 12543.85 };

const char* midiNotes[] = {
"C -2","C#-2","D -2","D#-2","E -2","F -2","F#-2","G -2","G#-2","A -2","A#-2","B -2",
"C -1","C#-1","D -1","D#-1","E -1","F -1","F#-1","G -1","G#-1","A -1","A#-1","B -1",
"C  0","C# 0","D  0","D# 0","E  0","F  0","F# 0","G  0","G# 0","A  0","A# 0","B  0",
"C  1","C# 1","D  1","D# 1","E  1","F  1","F# 1","G  1","G# 1","A  1","A# 1","B  1",
"C  2","C# 2","D  2","D# 2","E  2","F  2","F# 2","G  2","G# 2","A  2","A# 2","B  2",
"C  3","C# 3","D  3","D# 3","E  3","F  3","F# 3","G  3","G# 3","A  3","A# 3","B  3",
"C  4","C# 4","D  4","D# 4","E  4","F  4","F# 4","G  4","G# 4","A  4","A# 4","B  4",
"C  5","C# 5","D  5","D# 5","E  5","F  5","F# 5","G  5","G# 5","A  5","A# 5","B  5",
"C  6","C# 6","D  6","D# 6","E  6","F  6","F# 6","G  6","G# 6","A  6","A# 6","B  6",
"C  7","C# 7","D  7","D# 7","E  7","F  7","F# 7","G  7","G# 7","A  7","A# 7","B  7",
"C  8","C# 8","D  8","D# 8","E  8","F  8","F# 8","G  8" }; 

const char* instrumentNames[] = { "Grand Piano  ", "Bright Piano ", "Electric Grnd", "HnkytonkPiano", "El. Piano 1", "El. Piano 2", "Harpsichord", "Clavi", "Celesta", "Glockenspiel", "Music Box", "Vibraphone", "Marimba", "Xylophone", "Tubular Bells", "Santur", "Drawbar Organ", "Percussive Organ", "Rock Organ", "Church Organ", "Reed Organ", "Accordion (French)", "Harmonica", "Tango Accordion", "Acoustic Guitar (nylon)", "Acoustic Guitar (steel)", "El. Guitar (jazz)", "El. Guitar (clean)", "El. Guitar (muted)", "Overdriven Guitar", "Distortion Guitar", "Guitar Harmonics", "Acoustic Bass", "Finger Bass", "Picked Bass", "Fretless Bass", "Slap Bass 1", "Slap Bass 2", "Synth Bass 1", "Synth Bass 2", "Violin", "Viola", "Cello", "Contrabass", "Tremolo Strings", "Pizzicato Strings", "Orchestral Harp", "Timpani", "String Ensemble 1", "String Ensemble 2", "Synth Strings 1", "Synth Strings 2", "Choir Aahs", "Voice Oohs", "Synth Voice", "Orchestra Hit", "Trumpet", "Trombone", "Tuba", "Muted Trumpet", "French Horn", "Brass Section", "Synth Brass 1", "Synth Brass 2", "Soprano Sax", "Alto Sax", "Tenor Sax", "Baritone Sax", "Oboe", "English Horn", "Bassoon", "Clarinet", "Piccolo", "Flute", "Recorder", "Pan Flute", "Blown Bottle", "Shakuhachi", "Whistle", "Ocarina", "Lead 1 (square)", "Lead 2 (sawtooth)", "Lead 3 (calliope)", "Lead 4 (chiff)", "Lead 5 (charang)", "Lead 6 (voice)", "Lead 7 (fifths)", "Lead 8 (bass+lead)", "Pad 1 (fantasia)", "Pad 2 (warm)", "Pad 3 (polysynth)", "Pad 4 (choir)", "Pad 5 (bowed)", "Pad 6 (metallic)", "Pad 7 (halo)", "Pad 8 (sweep)", "FX 1 (rain)", "FX 2 (soundtrack)", "FX 3 (crystal)", "FX4 (atmosphere)", "FX 5 (brightness)", "FX 6 (goblins)", "FX 7 (echoes)", "FX 8 (sci-fi)", "Sitar", "Banjo", "Shamisen", "Koto", "Kalimba", "Bagpipe", "Fiddle", "Shanai", "Tinkle Bell", "Agogo", "Steel Drums", "Woodblock", "Taiko Drum", "Melodic Tom", "Synth Drum", "Reverse Cymbal", "Guitar Fret Noise", "Breath Noise", "Seashore", "Bird Tweet", "Teleph. Ring", "Helicopter", "Applause", "Gunshot"};

uint8_t masterPulseCount =24;
uint8_t sequenceCount = 4;
uint8_t currentPattern = 0;
uint8_t queuePattern = 0;
uint8_t selectedChannel = 0;
uint8_t numSteps = 16;
uint8_t	currentState = 0;
uint8_t previousState = 0;
uint8_t stepMode = 0; 
uint8_t settingMode = 0;
uint8_t selectedStep = 0;
uint8_t notePage;
uint32_t tempoX100 = 12000;

boolean playing = false;
boolean wasPlaying = false;
boolean tempoBlip = false;
boolean firstRun = false;
boolean extClock = false;
boolean dispSwitch = false;;
boolean debugBoolean = 0;

unsigned long beatLength = 60000000/(tempoX100/100);
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
elapsedMicros inputTimer;
elapsedMicros displayDebugTimer;
elapsedMicros inputDebugTimer;

IntervalTimer masterClock;
uint32_t masterClockInterval = 500;

Sequencer sequence[4];
NoteDatum noteData[4];
File saveData;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(20, 0, NEO_GRB + NEO_KHZ800);
Zetaohm_SAM2695 sam2695;
Zetaohm_AD5676 ad5676;
Adafruit_MCP23017 mcp;

//end afx-01a
AudioInputAnalog			adc1(A9);           //xy=579.2000274658203,275.2000274658203
AudioAnalyzeNoteFrequency	notefreq;      //xy=716.2000274658203,275.2000274658203
AudioConnection				patchCord1(adc1, notefreq);

coord_t height, width;
font_t fontTny;
font_t fontSm;
font_t fontMd;
font_t fontLg;

uint16_t voltManual = 0;

void setup() {
	Serial.begin(115200);
	Serial.println("Initializing SPI");
	AudioMemory(25);

	SPI.begin();
	SPI.setMOSI(11);
	SPI.setSCK(13);

	Serial.println("Freeram: " + String(FreeRam()));
	delay(500);
	Serial.println("Initializing Sequence Objects");
	sequence[0].initialize(0, 16, 4, (tempoX100/100));
	sequence[1].initialize(1, 16, 4, (tempoX100/100));
	sequence[2].initialize(2, 16, 4, (tempoX100/100));
	sequence[3].initialize(3, 16, 4, (tempoX100/100));

	Serial.println("Initializing SAM2695");
	sam2695.begin();
	sam2695.programChange(0, 0, 38);       // give our two channels different voices
	sam2695.programChange(0, 1, 30);
	sam2695.programChange(0, 2, 128);       // give our two channels different voices
	sam2695.programChange(0, 3, 29);


	Serial.println("Initializing Neopixels");
	ledSetup();

	Serial.println("Initializing Display");
	displayStartup();

	Serial.println("Initializing Button Array");
	buttonSetup();

	Serial.println("Initializing MIDI");
	MIDI.begin(MIDI_CHANNEL_OMNI);
	//midiSetup();

	Serial.println("Initializing DAC");

	ad5676.begin(3);
	ad5676.softwareReset();
	delay(1);
	ad5676.internalReferenceEnable(true);
	ad5676.internalReferenceEnable(true);
	Serial.println("Setting up debug pin");
	pinMode(DEBUG_PIN, OUTPUT);
	pinMode(4, OUTPUT);

	Serial.println("initializing gate outputs");
	mcp.begin(1);      // use default address 0
	mcp.pinMode(9, INPUT);
	mcp.pinMode(4, OUTPUT);
	mcp.pinMode(5, OUTPUT);
	mcp.pinMode(6, OUTPUT);
	mcp.pinMode(7, OUTPUT);
//
	Serial.println("Initializing Flash Memory");
	initializeFlashMemory();

	Serial.println("Beginning Master Clock");
	masterClock.begin(masterClockFunc,masterClockInterval);
	SPI.usingInterrupt(masterClock);
	//notefreq.begin(.1);

	//currentState = DEBUG_SCREEN;

}

void loop() {
	ledLoop();
  	buttonLoop();

	if( displayTimer > 10000){
		if (!dispSwitch){
			displayLoop();
	    displayTimer = 0;
		}
	}

	for (int i=0; i<8; i++){
//	    ad5676.setVoltage(i,  voltManual );
	  //  ad5676.setVoltage(i,  positive_modulo(10*millis(), 65535) );
	}	
	//debugScreenInputHandler();
//	if (notefreq.available()) {
//	   float note = notefreq.read();
//	   float prob = notefreq.probability();
//	   Serial.printf("Note: %3.2f | Probability: %.2f\n", note, prob);
//	   Serial.println(String(millis() )+ " -- " + String(AudioMemoryUsage()));
//	}  
}
// 
// 
// 
// 
// 
// 

//	digitalWriteFast(DEBUG_PIN, HIGH);
//
//	noInterrupts();
//	displayDebugTimer = 0;
//	displayLoop();
//	Serial.println("display loop timer: " + String(displayDebugTimer));
//	ledLoop();
//	inputDebugTimer = 0;
//	buttonLoop();
//	Serial.println("input loop timer: " + String(inputDebugTimer));
//	interrupts();
//    digitalWriteFast(DEBUG_PIN, LOW);

//	mcp.digitalWrite(0, HIGH);
//    ad5676.setVoltage(0, 0);
//    delay(10);
//	mcp.digitalWrite(0, LOW);
//    ad5676.setVoltage(0, 65535);
//    delay(10);
//	Serial.println("Testing " + String(millis()));
/*Serial.println("Testing 1 - 0");
displayDebugTimer = 0;
ad5676.setVoltage(0,  0 );
int temp1 = displayDebugTimer;
Serial.println("debug Timer: " + String(temp1) );
delay(1);
Serial.println("Testing 2 - 20000");
displayDebugTimer = 0;

ad5676.setVoltage(0,  20000 );
 temp1 = displayDebugTimer;

Serial.println("debug Timer: " + String(temp1) );


Serial.println("Testing 3 - 4k");
	displayDebugTimer = 0;

ad5676.setVoltage(0,  40000 );
 temp1 = displayDebugTimer;

Serial.println("debug Timer: " + String(temp1) );


Serial.println("Testing 4 - 6k");
	displayDebugTimer = 0;

ad5676.setVoltage(0,  60000 );
 temp1 = displayDebugTimer;
Serial.println("debug Timer: " + String(temp1) );

*/
void debug(const char* text){
	if (debugBoolean == 1){
		Serial.println(text);
	}

}


inline int positive_modulo(int i, int n) {
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


