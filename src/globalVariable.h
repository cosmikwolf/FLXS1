#include <Arduino.h>
#include "global.h"

#ifndef _globalVariable_h_
#define _globalVariable_h_

#define INTERNAL_CLOCK 0
#define EXTERNAL_MIDI_CLOCK 1
#define EXTERNAL_CLOCK_GATE_0 2
#define EXTERNAL_CLOCK_GATE_1 3
#define EXTERNAL_CLOCK_GATE_2 4
#define EXTERNAL_CLOCK_GATE_3 5
#define EXTERNAL_CLOCK_BIDIRECTIONAL_INPUT 6
#define GATE_CLOCK_TIMEOUT 5000000


class GlobalVariable {
  public:
    bool       multiSelection[64];
    bool       multiSelectSwitch;
    uint8_t    dataInputStyle;
    uint8_t    pageButtonStyle;
    uint8_t    outputNegOffset[4];
    bool       multi_pitch_switch;
    bool       multi_arpInterval_switch;
    bool       multi_gateType_switch;
    bool       multi_gateLength_switch;
    bool       multi_arpType_switch;
    bool       multi_arpOctave_switch;
    bool       multi_arpSpdNum_switch;
    bool       multi_arpSpdDen_switch;
    bool       multi_glide_switch;
    bool       multi_beatDiv_switch;
    bool       multi_velocity_switch;
    bool       multi_velocityType_switch;
    bool       multi_cv2speed_switch;
    uint8_t    multi_pitch;
    uint8_t    multi_arpInterval;
    uint8_t    multi_gateType;
    uint8_t    multi_gateLength;
    uint8_t    multi_arpType;
    uint8_t    multi_arpOctave;
    uint8_t    multi_arpSpdNum;
    uint8_t    multi_arpSpdDen;
    uint8_t    multi_glide;
    int8_t     multi_beatDiv;
    uint8_t    multi_velocity;
    uint8_t    multi_velocityType;
    uint8_t    multi_cv2speed;

    uint8_t   clockMode;
    int8_t  cvInputMapped[14];
    int16_t cvInputMapped1024[5];

    uint16_t  calibrationBuffer;
    uint16_t  dacCalibrationNeg[8];
    uint16_t  dacCalibrationPos[8];
    uint16_t  adcCalibrationPos[4];
    uint16_t  adcCalibrationNeg[4];
    uint16_t  adcCalibrationOffset[4];
    uint16_t cvInputRaw[12];
    uint8_t	gateInputRose[9];
    uint8_t	gateInputRaw[9];

    uint32_t tapTempoClockValues[4];
    uint8_t tapTempoCount;
    uint32_t tapTempoMasterClkCounter;

    uint8_t gateTestArray[4];
    uint8_t activeGate;
    uint8_t rheoTestLevel;
    uint8_t stepCopyIndex;
    uint8_t chCopyIndex;

    bool muteChannelSelect[4];
    bool parameterSelect;

    bool midiSetClockOut;
    bool gateTestComplete;
    elapsedMillis testTimer;

    bool extClock();
    int16_t quantizeSemitonePitch(int16_t note, uint8_t quantizeKey, uint16_t quantizeMode, bool direction);

    void initialize();
    void initGlobals();
};

#endif
