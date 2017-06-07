#include <Arduino.h>

#ifndef _global_h_
#define _global_h_

#define SEQUENCECOUNT         4

#define kSerialSpeed 115200
#define kMasterClockInterval 150
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


#define DISPLAY_INTERVAL 29000
#define INPUT_INTERVAL 20000
#define LED_INTERVAL 100000

#define PLAY_FORWARD          0
#define PLAY_REVERSE          1
#define PLAY_PENDULUM         2
#define PLAY_RANDOM           3

#define PIMILLION             3141593
//STATE DEFINITIONS:
#define STEP_DISPLAY		     	 7
#define TIMING_MENU 	      	 66
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
#define	CHANNEL_ENVELOPE_MODE	 99
#define	CHANNEL_STEP_MODE		   241
#define	CHANNEL_INPUT_MODE		 243
#define	DELETE_MENU			    	 242
#define	DEBUG_SCREEN		       127
#define	INPUT_DEBUG_MENU		     125

// MENUS
#define	PITCH_GATE_MENU   1
#define	ARPEGGIO_MENU     PITCH_GATE_MENU + 1
#define	SEQUENCE_MENU     ARPEGGIO_MENU + 1
#define	SCALE_MENU        SEQUENCE_MENU + 1
#define	INPUT_MENU        SCALE_MENU + 1
#define	TUNER_MENU        INPUT_MENU + 1
#define	VELOCITY_MENU     TUNER_MENU + 1
#define	CALIBRATION_MENU  VELOCITY_MENU + 1
#define TEMPO_MENU        CALIBRATION_MENU + 1
#define PATTERN_SELECT    TEMPO_MENU + 1
#define GLOBAL_MENU       PATTERN_SELECT + 1
#define MOD_MENU_1        GLOBAL_MENU + 1
#define MOD_MENU_2        MOD_MENU_1 + 1
#define MENU_MODAL        MOD_MENU_2 +1
#define NOTE_DISPLAY      MENU_MODAL +1
#define SAVE_MENU         NOTE_DISPLAY +1

//PITCH PAGE STEPMODES
#define STATE_PITCH0       1
#define STATE_GATELENGTH   STATE_PITCH0 +1
#define STATE_GATETYPE     STATE_GATELENGTH +1
#define STATE_GLIDE        STATE_GATETYPE +1

//ARPEGGIO MENU
#define STATE_ARPTYPE      STATE_GLIDE + 1
#define STATE_ARPSPEEDNUM  STATE_ARPTYPE + 1
#define STATE_ARPSPEEDDEN  STATE_ARPSPEEDNUM + 1
#define STATE_ARPOCTAVE    STATE_ARPSPEEDDEN + 1
#define STATE_CHORD        STATE_ARPOCTAVE + 1

//VELOCITY PAGE STEP MODES
#define STATE_VELOCITY      STATE_CHORD +1
#define STATE_VELOCITYTYPE  STATE_VELOCITY +1
#define STATE_LFOSPEED      STATE_VELOCITYTYPE +1

//SEQUENCE MENU
#define STATE_NOTEDISPLAY       STATE_LFOSPEED +1
#define STATE_FIRSTSTEP       STATE_NOTEDISPLAY +1
#define STATE_STEPCOUNT       STATE_FIRSTSTEP +1
#define STATE_BEATCOUNT       STATE_STEPCOUNT +1
#define STATE_SKIPSTEPCOUNT   STATE_BEATCOUNT + 1
#define STATE_YAXISINPUT       STATE_SKIPSTEPCOUNT +1
#define STATE_SCALE            STATE_YAXISINPUT+1
#define STATE_QUANTIZEKEY      STATE_SCALE+1
#define STATE_quantizeMode    STATE_QUANTIZEKEY+1
// MOD MENU 1
#define STATE_GATEMOD          STATE_quantizeMode +1
#define STATE_GATEINVERT       STATE_GATEMOD +1
#define STATE_RANDOMPITCH      STATE_GATEINVERT +1
#define STATE_PITCHMOD         STATE_RANDOMPITCH +1
#define STATE_GLIDEMOD         STATE_PITCHMOD +1
// MOD MENU 2
#define STATE_ARPTYPEMOD       STATE_GLIDEMOD +1
#define STATE_ARPSPDMOD        STATE_ARPTYPEMOD +1
#define STATE_ARPOCTMOD        STATE_ARPSPDMOD +1
#define STATE_ARPINTMOD        STATE_ARPOCTMOD +1
//TEMPO MENU
#define STATE_TEMPO            STATE_ARPINTMOD +1
#define STATE_EXTCLOCK         STATE_TEMPO +1
#define STATE_RESETINPUT       STATE_EXTCLOCK +1

#define STATE_PATTERNSELECT     STATE_RESETINPUT  +1
#define STATE_CALIBRATION       STATE_PATTERNSELECT +1
#define STATE_INPUTDEBUG        STATE_CALIBRATION +1
#define STATE_GLOBAL            STATE_INPUTDEBUG +1
#define STATE_PITCH1            STATE_GLOBAL +1
#define STATE_PITCH2            STATE_PITCH1 +1
#define STATE_PITCH3            STATE_PITCH2 +1
#define STATE_TUNER             STATE_PITCH3 +1

//SAVE MENU
#define STATE_SAVE               STATE_TUNER +1

// Calibration Menu
#define STATE_CALIB_INPUT0_OFFSET      STATE_SAVE  + 1
#define STATE_CALIB_INPUT0_LOW      STATE_CALIB_INPUT0_OFFSET  + 1
#define STATE_CALIB_INPUT0_HIGH     STATE_CALIB_INPUT0_LOW  + 1
#define STATE_CALIB_INPUT1_OFFSET    STATE_CALIB_INPUT0_HIGH  + 1
#define STATE_CALIB_INPUT1_LOW      STATE_CALIB_INPUT1_OFFSET  + 1
#define STATE_CALIB_INPUT1_HIGH     STATE_CALIB_INPUT1_LOW  + 1
#define STATE_CALIB_INPUT2_OFFSET    STATE_CALIB_INPUT1_HIGH  + 1
#define STATE_CALIB_INPUT2_LOW      STATE_CALIB_INPUT2_OFFSET  + 1
#define STATE_CALIB_INPUT2_HIGH     STATE_CALIB_INPUT2_LOW  + 1
#define STATE_CALIB_INPUT3_OFFSET    STATE_CALIB_INPUT2_HIGH  + 1
#define STATE_CALIB_INPUT3_LOW      STATE_CALIB_INPUT3_OFFSET  + 1
#define STATE_CALIB_INPUT3_HIGH     STATE_CALIB_INPUT3_LOW  + 1
#define STATE_CALIB_OUTPUT0_LOW      STATE_CALIB_INPUT3_HIGH  + 1
#define STATE_CALIB_OUTPUT0_HIGH     STATE_CALIB_OUTPUT0_LOW  + 1
#define STATE_CALIB_OUTPUT0_TEST      STATE_CALIB_OUTPUT0_HIGH  + 1
#define STATE_CALIB_OUTPUT1_LOW      STATE_CALIB_OUTPUT0_TEST  + 1
#define STATE_CALIB_OUTPUT1_HIGH     STATE_CALIB_OUTPUT1_LOW  + 1
#define STATE_CALIB_OUTPUT1_TEST      STATE_CALIB_OUTPUT1_HIGH  + 1
#define STATE_CALIB_OUTPUT2_LOW      STATE_CALIB_OUTPUT1_TEST  + 1
#define STATE_CALIB_OUTPUT2_HIGH     STATE_CALIB_OUTPUT2_LOW  + 1
#define STATE_CALIB_OUTPUT2_TEST      STATE_CALIB_OUTPUT2_HIGH  + 1
#define STATE_CALIB_OUTPUT3_LOW      STATE_CALIB_OUTPUT2_TEST  + 1
#define STATE_CALIB_OUTPUT3_HIGH     STATE_CALIB_OUTPUT3_LOW  + 1
#define STATE_CALIB_OUTPUT3_TEST      STATE_CALIB_OUTPUT3_HIGH  + 1
#define STATE_CALIB_OUTPUT4_LOW      STATE_CALIB_OUTPUT3_TEST  + 1
#define STATE_CALIB_OUTPUT4_HIGH     STATE_CALIB_OUTPUT4_LOW  + 1
#define STATE_CALIB_OUTPUT4_TEST      STATE_CALIB_OUTPUT4_HIGH  + 1
#define STATE_CALIB_OUTPUT5_LOW      STATE_CALIB_OUTPUT4_TEST  + 1
#define STATE_CALIB_OUTPUT5_HIGH     STATE_CALIB_OUTPUT5_LOW  + 1
#define STATE_CALIB_OUTPUT5_TEST      STATE_CALIB_OUTPUT5_HIGH  + 1
#define STATE_CALIB_OUTPUT6_LOW      STATE_CALIB_OUTPUT5_TEST  + 1
#define STATE_CALIB_OUTPUT6_HIGH     STATE_CALIB_OUTPUT6_LOW  + 1
#define STATE_CALIB_OUTPUT6_TEST      STATE_CALIB_OUTPUT6_HIGH  + 1
#define STATE_CALIB_OUTPUT7_LOW      STATE_CALIB_OUTPUT6_TEST  + 1
#define STATE_CALIB_OUTPUT7_HIGH     STATE_CALIB_OUTPUT7_LOW  + 1
#define STATE_CALIB_OUTPUT7_TEST      STATE_CALIB_OUTPUT7_HIGH  + 1
#define STATE_CALIBRATION_SAVE_MODAL  STATE_CALIB_OUTPUT7_TEST  + 1

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

#define INTERNAL_CLOCK 0
#define EXTERNAL_MIDI_CLOCK 1
#define EXTERNAL_CLOCK_GATE_0 2
#define EXTERNAL_CLOCK_GATE_1 3
#define EXTERNAL_CLOCK_GATE_2 4
#define EXTERNAL_CLOCK_GATE_3 5
#define EXTERNAL_CLOCK_BIDIRECTIONAL_INPUT 6
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

extern float   probability;
extern float   frequency;

extern uint8_t selectedChannel;
extern uint8_t selectedStep;
extern uint8_t patternChannelSelector;
extern uint8_t prevPtrnChannelSelector;
extern uint8_t selectedCalibrationChannel;
extern boolean extClock;
extern uint8_t clockMode;
extern boolean debugBoolean;
// FlashMemory
extern uint8_t currentPattern;
extern uint8_t queuePattern;
extern boolean playing;
extern uint8_t chordArray[27][3];
extern uint16_t colundiArrayX100[56];
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

extern uint32_t lfoClockCounter;

//save file
extern uint8_t saveDestination[4];

//midi
extern elapsedMicros testTimer;
extern boolean firstRun;
extern boolean eraseAllFlag;

extern elapsedMillis modalTimer;

extern uint16_t calibrationBuffer;
extern uint16_t  dacCalibrationNeg[8];
extern uint16_t  dacCalibrationPos[8];
extern uint16_t  adcCalibrationPos[4];
extern uint16_t  adcCalibrationNeg[4];
extern uint16_t  adcCalibrationOffset[4];
extern uint16_t cvInputRaw[4];
extern int8_t cvInputMapped[5];
extern int16_t cvInputMapped1024[4];
extern uint8_t	gateInputRose[5];
extern uint8_t	gateInputRaw[5];

extern uint8_t dacMap[8];

extern uint8_t selectedText;
extern uint8_t prevSelectedText;

//void//Serial.println(String text);
int positive_modulo(int i, int n);
int modulo_minimum(int i, int n, int minimum);
int min_max(int value, int min, int max);
int min_max_cycle(int value, int min, int max);

int intFloatHundreds(int value);
int intFloatTensOnes(int value);

uint8_t getNote(uint8_t index);
uint32_t FreeRam2();


extern elapsedMicros debugTimer2;

#endif
