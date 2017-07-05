#include <Arduino.h>

#ifndef _globalVariable_h_
#define _globalVariable_h_

class GlobalVariable {
  public:
    bool       multiSelection[64];
    bool       multiSelectSwitch;

    bool       multi_pitch_switch;
    bool       multi_chord_switch;
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
    bool       multi_lfoSpeed_switch;
    uint8_t    multi_pitch;
    uint8_t    multi_chord;
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
    uint8_t    multi_lfoSpeed;

    void initialize(){
      this->multi_pitch_switch = 0;
      this->multi_chord_switch = 0;
      this->multi_gateType_switch = 0;
      this->multi_gateLength_switch = 0;
      this->multi_arpType_switch = 0;
      this->multi_arpOctave_switch = 0;
      this->multi_arpSpdNum_switch = 0;
      this->multi_arpSpdDen_switch = 0;
      this->multi_glide_switch = 0;
      this->multi_beatDiv_switch = 0;
      this->multi_velocity_switch = 0;
      this->multi_velocityType_switch = 0;
      this->multi_lfoSpeed_switch = 0;
      this->multi_pitch = 0;
      this->multi_chord = 0;
      this->multi_gateType = 0;
      this->multi_gateLength = 0;
      this->multi_arpType = 0;
      this->multi_arpOctave = 0;
      this->multi_arpSpdNum = 0;
      this->multi_arpSpdDen = 0;
      this->multi_glide = 0;
      this->multi_beatDiv = 0;
      this->multi_velocity = 0;
      this->multi_velocityType = 0;
      this->multi_lfoSpeed = 0;
       for(int i = 0; i < 64; i++){
           this->multiSelection[i]=false;
       }

    }
};

#endif
