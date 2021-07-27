// #include <Arduino.h>
#include "globalScaleBinaries.h"
#include "globalStateDefinitions.h"

#ifndef _global_h_
#define _global_h_

/*#ifdef min(a,b)
#undef min(a,b)
#endif

#define min(a,b) \
  ({ __typeof__ (a) _a = (a); \
      __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b; })

#ifdef max(a,b)
  #undef max(a,b)
#endif

#define max(a,b) \
  ({ __typeof__ (a) _a = (a); \
      __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b; })
*/
#define SEQUENCECOUNT 4

//#define kSerialSpeed 31250
#define kSerialSpeed 115200

#define kMasterClockInterval 200
#define kLedClockInterval 50000
#define kSequenceTimerInterval 600
#define kMidiClockInterval 1000
#define kCacheClockInterval 10000
#define kMosiPin 11
#define kSpiClockPin 13

#define DEBUG 

#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.println(x)
#else
#define DEBUG_PRINT(x)
#endif

#define NOTE_LENGTH_BUFFER 5000 // number of microseconds to end each gate early
#define FRAMES_PER_BEAT 16777216
//#define FRAMES_PER_BEAT  4194304

#define DISPLAY_INTERVAL 40000
#define INPUT_INTERVAL 10000
#define LED_INTERVAL 50000

#define PLAY_FORWARD 0
#define PLAY_REVERSE 1
#define PLAY_PENDULUM 2
#define PLAY_RANDOM 3

#define PIMILLION 3141593

#define MAX_STATE_COUNT 10
#define PITCH_GATE_MENU_ORDER 0

#define MODAL_SELECT_CH1 0
#define MODAL_SELECT_CH2 MODAL_SELECT_CH1 + 1
#define MODAL_SELECT_CH3 MODAL_SELECT_CH2 + 1
#define MODAL_SELECT_CH4 MODAL_SELECT_CH3 + 1
#define MODAL_MUTE_GATE MODAL_SELECT_CH4 + 1
#define MODAL_MUTE_CVA MODAL_MUTE_GATE + 1
#define MODAL_MUTE_CVB MODAL_MUTE_CVA + 1
#define MODAL_MUTE_CH4 MODAL_MUTE_CVB + 1
#define MODAL_UNMUTE_GATE MODAL_MUTE_CH4 + 1
#define MODAL_UNMUTE_CVA MODAL_UNMUTE_GATE + 1
#define MODAL_UNMUTE_CVB MODAL_UNMUTE_CVA + 1
#define MODAL_UNMUTE_CH4 MODAL_UNMUTE_CVB + 1
#define MODAL_SAVE MODAL_UNMUTE_CH4 + 1
#define MODAL_DIDDNTSAVE MODAL_SAVE + 1
#define MODAL_COPY_PAGE MODAL_DIDDNTSAVE + 1
#define MODAL_PASTE_PAGE MODAL_COPY_PAGE + 1
#define MODAL_CLEAR_PAGE MODAL_PASTE_PAGE + 1
#define MODAL_COPY_STEP MODAL_CLEAR_PAGE + 1
#define MODAL_COPY_PATTERN MODAL_COPY_STEP + 1
#define MODAL_PASTE_STEP MODAL_COPY_PATTERN + 1
#define MODAL_PASTE_PATTERN_CONFIRMED MODAL_PASTE_STEP + 1
#define MODAL_PASTE_PATTERN_ARMED MODAL_PASTE_PATTERN_CONFIRMED + 1
#define MODAL_CLEAR_PATTERN_CONFIRMED MODAL_PASTE_PATTERN_ARMED + 1
#define MODAL_CLEAR_PATTERN_ARMED MODAL_CLEAR_PATTERN_CONFIRMED + 1
#define MODAL_CLEAR_STEP MODAL_CLEAR_PATTERN_ARMED + 1
#define MODAL_COPY_CHANNEL MODAL_CLEAR_STEP + 1
#define MODAL_PASTE_CHANNEL MODAL_COPY_CHANNEL + 1
#define MODAL_CLEAR_CHANNEL MODAL_PASTE_CHANNEL + 1
#define MODAL_ERASEARMED MODAL_CLEAR_CHANNEL + 1
#define MODAL_ERASED MODAL_ERASEARMED + 1
#define MODAL_RANDOM_PITCH_GATE MODAL_ERASED + 1
#define MODAL_RANDOM_PITCH_GATE_WARNING MODAL_RANDOM_PITCH_GATE + 1
#define MODAL_SHORTCUT_REVERSE MODAL_RANDOM_PITCH_GATE_WARNING + 1
#define MODAL_SHORTCUT_RESET MODAL_SHORTCUT_REVERSE + 1
#define MODAL_TOBEIMPLEMENTED MODAL_SHORTCUT_RESET + 1
#define MODAL_EXPORTING MODAL_TOBEIMPLEMENTED + 1
#define MODAL_EXPORTCOMPLETE MODAL_EXPORTING + 1
#define MODAL_PREPARING_SYSEX MODAL_EXPORTCOMPLETE + 1
#define MODAL_IMPORTCOMPLETE MODAL_PREPARING_SYSEX + 1

#define PIN_EXT_TX 31
#define PIN_EXT_AD_1 27
#define PIN_EXT_AD_2 30
#define PIN_EXT_AD_3 28
#define PIN_EXT_AD_4 29
#define PIN_EXT_AD_5 15
#define PIN_EXT_AD_6 32
#define PIN_EXT_RX 26
#define CLOCK_PIN 23

#define SD_CS_PIN 15
#define LED_PIN 5
#define MIDPLANE_MAX7301_CS_PIN 0
#define BACKPLANE_MAX7301_CS_PIN 33
#define AD5676_CSPIN 2

// MIDI Messages:
#define MIDI_CLOCK 248
#define MIDI_START 250
#define MIDI_CONTINE 251
#define MIDI_STOP 252

#define CHROMATIC 0
#define IONIAN 1
#define DORIAN 2
#define PHRYGIAN 3
#define LYDIAN 4
#define MIXOLYDIAN 5
#define AEOLIAN 6
#define LOCRIAN 7
#define BLUESMAJOR 8
#define BLUESMINOR 9
#define PENT_MAJOR 10
#define PENT_MINOR 11
#define FOLK 12
#define JAPANESE 13
#define GAMELAN 14
#define GYPSY 15
#define ARABIAN 16
#define FLAMENCO 17
#define WHOLETONE 18

#define SEMITONE 0
#define PYTHAGOREAN 1
#define COLUNDI 2
#define TET_17 3

//these 12 bit INTs define chromatic scales
//the bit position indicates scale degree

#define POSITIVE 1
#define NEGATIVE 0

//INPUT MAPPINGS
#define SW_00 0
#define SW_01 1
#define SW_02 2
#define SW_03 3
#define SW_04 4
#define SW_05 5
#define SW_06 6
#define SW_07 7
#define SW_08 8
#define SW_09 9
#define SW_10 10
#define SW_11 11
#define SW_12 12
#define SW_13 13
#define SW_14 14
#define SW_15 15
#define SW_PLAY 17
#define SW_REC 18
#define SW_STOP 16
#define SW_CH0 19
#define SW_CH1 20
#define SW_CH2 21
#define SW_CH3 22
#define SW_PGDN 23
#define SW_PGUP 24
#define SW_PATTERN 25
#define SW_MENU 26
#define SW_SHIFT 27
#define SW_ENCODER_BACKPLANE 22

#define ENCODER1LEFTPIN 16
//#define ENCODER1LEFTPIN 32
#define ENCODER1RIGHTPIN 25
#define MAX7301PIN 5

//STEPDATUM CONSTANTS
#define AWAITING_TRIGGER 0                    // 0 indicates not playing, not queued
#define CURRENTLY_PLAYING 1                   // 1 indicates the note is currently playing
#define CURRENTLY_QUEUED 2                    // 2 indicates the note is currently queued.
#define CURRENTLY_PLAYING_AND_QUEUED 3        // 3 indicates that the note is currently playing and currently queued
#define NOTE_HAS_BEEN_PLAYED_THIS_ITERATION 4 // 4 indicates that the note has been played this iteration
#define CURRENTLY_ARPEGGIATING 5              // 4 indicates that the note has been played this iteration
#define BETWEEN_APEGGIATIONS 6

#define CURRENT_ARP_OFFSET 63 // difference between notestatus value and the current Arpeggiation note

#define MAX_STEPS_PER_SEQUENCE 64

#define GATETYPE_REST 0
#define GATETYPE_STEP 1
#define GATETYPE_TIE 2
#define GATETYPE_1HIT 3
#define GATETYPE_HOLD 4
#define GATETYPE_HALF 5
#define GATETYPE_RAND33 6
#define GATETYPE_RAND50 7
#define GATETYPE_RAND66 8

#define ARPTYPE_OFF 0
#define ARPTYPE_UP 1
#define ARPTYPE_DN 2
#define ARPTYPE_UPDN1 3
#define ARPTYPE_UPDN2 4
#define ARPTYPE_RNDM 5

extern uint8_t stepMode;
extern uint8_t notePage;
//extern uint8_t clockMode;
// FlashMemory
extern uint8_t currentPattern;
extern uint8_t chordArray[27][3];
extern uint16_t colundiArrayX100[56];
extern uint8_t stepModeOrder[1][MAX_STATE_COUNT];
extern boolean extClock;
extern boolean debugBoolean;
// MasterClock
// extern unsigned long beatLength;

//save file
// extern uint8_t saveDestination[4];

//midi
extern elapsedMillis modalTimer;
extern boolean firstRun;
extern boolean eraseAllFlag;
extern boolean midiTestActive;

extern uint8_t dacMap[8];
extern uint8_t selectedText;
extern uint8_t prevSelectedText;

// multi globalObj->selectedChannel

//void//Serial.println(String text);

uint32_t positive_modulo(int i, int n);
int modulo_minimum(int i, int n, int minimum);
int min_max(int value, int min, int max);
int min_max_cycle(int value, int min, int max);
int min_max_wrap(int value, int first, int count, int max);
int min_max_skip(int original, int change, int min, int max, int skip);

int intFloatHundreds(int value);
int intFloatTensOnes(int value);

uint8_t getNote(uint8_t index);

#endif
