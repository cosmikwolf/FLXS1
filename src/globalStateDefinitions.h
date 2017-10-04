
#ifndef _global_state_definitions_h_
#define _global_state_definitions_h_
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
#define	QUANTIZE_MENU        SEQUENCE_MENU + 1
#define	INPUT_MENU        QUANTIZE_MENU + 1
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
#define MENU_MULTISELECT  SAVE_MENU +1
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
#define STATE_VELOCITYTYPE      STATE_CHORD +1
#define STATE_VELOCITY      STATE_VELOCITYTYPE +1
#define STATE_LFOSPEED      STATE_VELOCITY +1

//SEQUENCE MENU
#define STATE_NOTEDISPLAY     STATE_LFOSPEED +1
#define STATE_FIRSTSTEP       STATE_NOTEDISPLAY +1
#define STATE_STEPCOUNT       STATE_FIRSTSTEP +1
#define STATE_BEATCOUNT       STATE_STEPCOUNT +1
#define STATE_SKIPSTEPCOUNT   STATE_BEATCOUNT + 1
#define STATE_SKIPSTEP        STATE_SKIPSTEPCOUNT +1
#define STATE_SWING           STATE_SKIPSTEP +1

//QUANTIZE MENU
#define STATE_QUANTIZESCALE    STATE_SWING+1
#define STATE_QUANTIZEKEY      STATE_QUANTIZESCALE+1
#define STATE_QUANTIZEMODE    STATE_QUANTIZEKEY+1
// MOD MENU 1
#define STATE_GATEMOD          STATE_QUANTIZEMODE +1
#define STATE_GATEMUTE       STATE_GATEMOD +1
#define STATE_RANDOMPITCH      STATE_GATEMUTE +1
#define STATE_RANDOMHIGH      STATE_RANDOMPITCH +1
#define STATE_RANDOMLOW      STATE_RANDOMHIGH +1

#define STATE_PITCHMOD         STATE_RANDOMLOW +1
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
#define STATE_CALIB_OUTPUT0_HIGH     STATE_CALIB_OUTPUT0_LOW + 1
#define STATE_CALIB_OUTPUT1_LOW      STATE_CALIB_OUTPUT0_HIGH + 1
#define STATE_CALIB_OUTPUT1_HIGH     STATE_CALIB_OUTPUT1_LOW + 1
#define STATE_CALIB_OUTPUT2_LOW      STATE_CALIB_OUTPUT1_HIGH + 1
#define STATE_CALIB_OUTPUT2_HIGH     STATE_CALIB_OUTPUT2_LOW + 1
#define STATE_CALIB_OUTPUT3_LOW      STATE_CALIB_OUTPUT2_HIGH + 1
#define STATE_CALIB_OUTPUT3_HIGH     STATE_CALIB_OUTPUT3_LOW + 1
#define STATE_CALIB_OUTPUT4_LOW      STATE_CALIB_OUTPUT3_HIGH + 1
#define STATE_CALIB_OUTPUT4_HIGH     STATE_CALIB_OUTPUT4_LOW + 1
#define STATE_CALIB_OUTPUT5_LOW      STATE_CALIB_OUTPUT4_HIGH + 1
#define STATE_CALIB_OUTPUT5_HIGH     STATE_CALIB_OUTPUT5_LOW + 1
#define STATE_CALIB_OUTPUT6_LOW      STATE_CALIB_OUTPUT5_HIGH + 1
#define STATE_CALIB_OUTPUT6_HIGH     STATE_CALIB_OUTPUT6_LOW + 1
#define STATE_CALIB_OUTPUT7_LOW      STATE_CALIB_OUTPUT6_HIGH + 1
#define STATE_CALIB_OUTPUT7_HIGH     STATE_CALIB_OUTPUT7_LOW + 1
#define STATE_TEST_MIDI              STATE_CALIB_OUTPUT7_HIGH + 1
#define STATE_TEST_GATES             STATE_TEST_MIDI + 1
#define STATE_TEST_RHEOSTAT          STATE_TEST_GATES + 1

#define STATE_CALIBRATION_SAVE_MODAL STATE_TEST_RHEOSTAT + 1

#define STATE_MULTISELECT             STATE_CALIBRATION_SAVE_MODAL + 1

//LFO TYPES

#define LFO_OFF         0
#define LFO_TRIGGER     1
#define LFO_VOLTAGE     2
#define LFO_ENV_DECAY   3
#define LFO_ENV_ATTACK  4
#define LFO_ENV_AR      5
#define LFO_ENV_ASR     6
#define LFO_SINE        7
#define LFO_TRIANGLE    8
#define LFO_SQUARE      9
#define LFO_RNDSQUARE   10
#define LFO_SAWUP       11
#define LFO_SAWDN       12
#define LFO_SAMPLEHOLD  13


#endif
