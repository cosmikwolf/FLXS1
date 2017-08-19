#include <Arduino.h>
#include "globalScaleBinaries.h"
#include "globalStateDefinitions.h"

#ifndef _global_h_
#define _global_h_

#ifdef min
#undef min
#endif

#define min(X, Y)  ((X) < (Y) ? (X) : (Y))

#ifdef max
  #undef max
#endif
#define max(X, Y)  ((X) > (Y) ? (X) : (Y))

#define SEQUENCECOUNT         4

#define kSerialSpeed 115200
#define kMasterClockInterval 200 
#define kLedClockInterval 30000
#define kSequenceTimerInterval 600
#define kPeripheralLoopTimer 15000
#define kMidiClockInterval  600
#define kCacheClockInterval 10000
#define kMosiPin 11
#define kSpiClockPin 13

#define DEBUG

#ifdef DEBUG
 #define DEBUG_PRINT(x)  Serial.println(x)
#else
 #define DEBUG_PRINT(x)
#endif


#define NOTE_LENGTH_BUFFER 5000  // number of microseconds to end each gate early
//#define FRAMES_PER_BEAT  16777216
#define FRAMES_PER_BEAT  4194304

#define DISPLAY_INTERVAL 30000
#define INPUT_INTERVAL 10000
#define LED_INTERVAL 50000

#define PLAY_FORWARD          0
#define PLAY_REVERSE          1
#define PLAY_PENDULUM         2
#define PLAY_RANDOM           3

#define PIMILLION             3141593

#define MAX_STATE_COUNT 10
#define PITCH_GATE_MENU_ORDER  0

#define MODAL_SELECT_CH1       0
#define MODAL_SELECT_CH2       1
#define MODAL_SELECT_CH3       2
#define MODAL_SELECT_CH4       3

#define MODAL_MUTE_CH1         4
#define MODAL_MUTE_CH2         5
#define MODAL_MUTE_CH3         6
#define MODAL_MUTE_CH4         7

#define MODAL_UNMUTE_CH1       8
#define MODAL_UNMUTE_CH2       9
#define MODAL_UNMUTE_CH3       10
#define MODAL_UNMUTE_CH4       11
#define MODAL_SAVE             12
#define MODAL_ERASEARMED       13
#define MODAL_ERASED           14

#define PIN_EXT_TX				    	 31
#define PIN_EXT_AD_1   27
#define PIN_EXT_AD_2   30
#define PIN_EXT_AD_3   28
#define PIN_EXT_AD_4   29
#define PIN_EXT_AD_5   15
#define PIN_EXT_AD_6   32
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

#define CHROMATIC         0
#define IONIAN           1
#define DORIAN           2
#define PHRYGIAN         3
#define LYDIAN           4
#define MIXOLYDIAN       5
#define AEOLIAN          6
#define LOCRIAN          7
#define BLUESMAJOR       8
#define BLUESMINOR       9
#define PENT_MAJOR      10
#define PENT_MINOR      11
#define FOLK            12
#define JAPANESE        13
#define GAMELAN         14
#define GYPSY           15
#define ARABIAN         16
#define FLAMENCO        17
#define WHOLETONE       18


#define SEMITONE        0
#define PYTHAGOREAN     1
#define COLUNDI         2


//these 12 bit INTs define chromatic scales
//the bit position indicates scale degree



#define POSITIVE 1
#define NEGATIVE 0


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
#define SW_PLAY  17
#define SW_REC   18
#define SW_STOP  16
#define SW_CH0    19
#define SW_CH1    20
#define SW_CH2    21
#define SW_CH3    22
#define SW_PGDN  23
#define SW_PGUP  24
#define SW_PATTERN  25
#define SW_MENU   26
#define SW_SHIFT   27
#define SW_ENCODER_BACKPLANE    22

#define ENCODER1LEFTPIN 16
//#define ENCODER1LEFTPIN 32
#define ENCODER1RIGHTPIN 25
#define MAX7301PIN  5

//STEPDATUM CONSTANTS
#define AWAITING_TRIGGER 								0   // 0 indicates not playing, not queued
#define CURRENTLY_PLAYING 									1		// 1 indicates the note is currently playing
#define CURRENTLY_QUEUED 										2   // 2 indicates the note is currently queued.
#define CURRENTLY_PLAYING_AND_QUEUED				3   // 3 indicates that the note is currently playing and currently queued
#define NOTE_HAS_BEEN_PLAYED_THIS_ITERATION	4   // 4 indicates that the note has been played this iteration
#define CURRENTLY_ARPEGGIATING							5   // 4 indicates that the note has been played this iteration
#define BETWEEN_APEGGIATIONS								6
#define CURRENT_ARP_OFFSET									63	// difference between notestatus value and the current Arpeggiation note

#define MAX_STEPS_PER_SEQUENCE							64

#define GATETYPE_REST				0
#define GATETYPE_STEP				1
#define GATETYPE_TIE        2
#define GATETYPE_1HIT       3

#define ARPTYPE_OFF   0
#define ARPTYPE_UP		1
#define ARPTYPE_DN		2
#define ARPTYPE_UPDN1 3
#define ARPTYPE_UPDN2 4
#define ARPTYPE_RNDM	5

extern uint32_t tempoX100;
extern uint8_t stepMode;
extern uint8_t notePage;
extern uint8_t currentMenu;       // Display module + LED module
extern uint8_t previousState;      // Display module + LED moduleextern


extern uint8_t selectedChannel;
extern uint8_t selectedStep;
extern uint8_t patternChannelSelector;
extern uint8_t prevPtrnChannelSelector;
//extern uint8_t clockMode;
// FlashMemory
extern uint8_t currentPattern;
extern uint8_t queuePattern;
extern uint8_t chordArray[27][3];
extern uint16_t colundiArrayX100[56];
extern uint8_t stepModeOrder[1][MAX_STATE_COUNT];
extern boolean extClock;
extern boolean debugBoolean;
extern boolean playing;

// MasterClock
extern unsigned long beatLength;
extern uint8_t masterPulseCount;
extern boolean wasPlaying;

extern uint32_t lfoClockCounter;

//save file
extern uint8_t saveDestination[4];

//midi
extern elapsedMillis modalTimer;
extern boolean firstRun;
extern boolean eraseAllFlag;


extern uint8_t dacMap[8];
extern uint8_t selectedText;
extern uint8_t prevSelectedText;

// multi selectedChannel


//void//Serial.println(String text);

int positive_modulo(int i, int n);
int modulo_minimum(int i, int n, int minimum);
int min_max(int value, int min, int max);
int min_max_cycle(int value, int min, int max);
int min_max_skip(int original, int change, int min, int max, int skip);

int intFloatHundreds(int value);
int intFloatTensOnes(int value);

uint8_t getNote(uint8_t index);



#endif
