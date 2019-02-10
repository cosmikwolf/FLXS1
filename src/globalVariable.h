#include <Arduino.h>
#include "global.h"
#include <ADC.h>

#ifndef _globalVariable_h_
#define _globalVariable_h_

#define INTERNAL_CLOCK 0
#define EXTERNAL_MIDI_35_CLOCK 1
#define EXTERNAL_MIDI_USB_CLOCK 2
#define EXTERNAL_CLOCK_GATE_0 3
#define EXTERNAL_CLOCK_GATE_1 4
#define EXTERNAL_CLOCK_GATE_2 5
#define EXTERNAL_CLOCK_GATE_3 6
#define EXTERNAL_CLOCK_BIDIRECTIONAL_INPUT 7

#define PATTERN_CHANGE_IMMEDIATE 0
#define PATTERN_CHANGE_CHANNEL_0 1
#define PATTERN_CHANGE_CHANNEL_1 2
#define PATTERN_CHANGE_CHANNEL_2 3
#define PATTERN_CHANGE_CHANNEL_3 4

#define GATE_CLOCK_TIMEOUT 5000000

#define CHAIN_COUNT_MAX 16
#define SONG_COUNT_MAX 1
#define CLOCK_PORT_OUTPUT 1
#define CLOCK_PORT_INPUT  0

class GlobalVariable {
  public:
    bool      multiSelection[64];
    bool      multiSelectSwitch;
    uint8_t   dataInputStyle;
    uint8_t   pageButtonStyle;
    uint8_t   outputNegOffset[4];
    bool      multi_pitch_switch;
    bool      multi_arpInterval_switch;
    bool      multi_gateType_switch;
    bool      multi_gateLength_switch;
    bool      multi_arpType_switch;
    bool      multi_arpOctave_switch;
    bool      multi_arpSpdNum_switch;
    bool      multi_arpSpdDen_switch;
    bool      multi_glide_switch;
    bool      multi_beatDiv_switch;
    bool      multi_velocity_switch;
    bool      multi_velocityType_switch;
    bool      multi_cv2speed_switch;
    bool      multi_cv2offset_switch;
    uint8_t   multi_pitch;
    uint8_t   multi_arpInterval;
    uint8_t   multi_gateType;
    uint8_t   multi_gateLength;
    uint8_t   multi_arpType;
    uint8_t   multi_arpOctave;
    uint8_t   multi_arpSpdNum;
    uint8_t   multi_arpSpdDen;
    uint8_t   multi_glide;
    int8_t    multi_beatDiv;
    uint8_t   multi_velocity;
    uint8_t   multi_velocityType;
    uint8_t   multi_cv2speed;
    int8_t    multi_cv2offset;
    uint8_t   currentMenu;
    uint8_t  selectedChannel;

    uint8_t   clockMode;
    uint32_t tempoX100;

    int8_t    cvInputMapped[14];
    int16_t   cvInputMapped1024[5];

    uint16_t  calibrationBuffer;
    uint16_t  dacCalibrationNeg[8];
    uint16_t  dacCalibrationPos[8];
    uint16_t  adcCalibrationPos[4];
    uint16_t  adcCalibrationNeg[4];
    uint16_t  adcCalibrationOffset[4];
    uint16_t  cvInputRaw[12];
    uint8_t	  gateInputRose[9];
    uint8_t	  gateInputRaw[9];

    uint32_t  tapTempoClockValues[4];
    uint8_t   tapTempoCount;
    uint32_t  tapTempoMasterClkCounter;

    uint8_t   gateTestArray[4];
    uint8_t   activeGate;
    uint8_t   rheoTestLevel;
    uint8_t   stepCopyIndex;
    uint8_t   copiedChannel;
    uint8_t   copiedPage;
    uint8_t   chCopyIndex;
    
    uint8_t   randomizeParamSelect;
    uint8_t   randomizeLow;
    uint8_t   randomizeSpan;
    uint8_t   randomize_cv2_sync;
    uint8_t   randomize_cv2_type;
    uint8_t   randomize_cv2_speedmin;
    uint8_t   randomize_cv2_speedmax;
    uint8_t   randomize_cv2_speedsync;
    bool      randomize_cv2_type_include_skip;
    int8_t   randomize_cv2_amplitude_min;
    int8_t   randomize_cv2_amplitude_max;
    int8_t   randomize_cv2_offset_min;
    int8_t   randomize_cv2_offset_max;
    uint8_t   importExportDisplaySwitch;
    uint8_t   patternChannelSelector;
    uint8_t   prevPtrnChannelSelector;
    uint8_t   queuePattern;
    uint8_t   patternChangeTrigger;
    uint8_t   selectedStep;
    uint8_t   midiChannel[4];
    bool channelResetSwich[4];
    bool savedSequences[4][16]; //[channel][sequence]
    bool muteChannelSelect[4];
    bool parameterSelect;
    bool playing;
    bool wasPlaying;
    bool chainModeActive;
    bool chainModeCountSwitch;

    bool waitingToResetAfterPatternLoad;
    bool patternLoadOperationInProgress;
    uint8_t songIndex;
    int16_t chainModeMasterPulseToGo;
    uint8_t chainModeIndex;
    uint8_t chainModeMasterChannel[16];
    uint8_t chainModeCount[16];
    uint8_t fastChainPatternSelect[16];
    uint8_t fastChainModePatternCount;
    uint8_t fastChainModeCurrentIndex;
    bool settingFastChain;
    bool    chainChannelSelect[4][16]; //channel, chain index
    bool    chainChannelMute[4][16];
    uint8_t chainPatternSelect[16];
    int8_t  chainPatternRepeatCount[16];
    uint8_t chainSelectedPattern;
    uint8_t previousChainSelectedPattern;
    bool midiSetClockOut;
    bool gateTestComplete;
    
    uint8_t pattern_page;

    elapsedMillis testTimer;
    elapsedMillis screenSaverTimeout;
    int generateRandomNumber(int minVal, int maxVal);
    int generateRandomNumberIncludeZero(int minVal, int maxVal);
    
    bool extClock();
    bool clockPortDirection();
    void setClockPortDirection(bool direction);

    int16_t quantizeSemitonePitch(int16_t note, uint8_t quantizeKey, uint16_t quantizeMode, bool direction);
    void initialize(ADC *adc);
    void initGlobals();
    void initSongData();
    uint8_t convertBoolToByte(bool ch1, bool ch2, bool ch3, bool ch4);


  private:
    ADC *adc;
};

#endif
