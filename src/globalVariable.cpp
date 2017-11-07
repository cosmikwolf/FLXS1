#include <Arduino.h>
#include "globalVariable.h"

void GlobalVariable::initialize(){
    this->clockMode = INTERNAL_CLOCK;
  //    this->clockMode = EXTERNAL_MIDI_CLOCK;

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
      for(int i = 0; i < 4; i++){
        this->gateTestArray[i]=255;
      }
      activeGate = 0;
      rheoTestLevel = 0;
      gateTestComplete = false;

      this->parameterSelect = false;
      this->midiSetClockOut = false;
      this->calibrationBuffer = 0;

       for(int i=0; i<8; i++){ this->dacCalibrationNeg[i]= 0; };
       for(int i=0; i<8; i++){ this->dacCalibrationPos[i]= 65535; };
       for(int i=0; i<4; i++){ this->adcCalibrationPos[i]= 0; };
       for(int i=0; i<4; i++){ this->adcCalibrationNeg[i]= 65535; };
       for(int i=0; i<4; i++){ this->adcCalibrationOffset[i]= 32767; };

    }

uint8_t GlobalVariable::quantizeSemitonePitch(uint8_t note, uint8_t quantizeMode, uint8_t quantizeKey, bool direction){

	uint8_t count = 0;
	uint16_t scaleExpanded;

	switch(quantizeMode){
    case SEMITONE:    scaleExpanded = SEMITONE_BIN;    break;
    case IONIAN:      scaleExpanded = IONIAN_BIN;      break;
    case DORIAN:      scaleExpanded = DORIAN_BIN;      break;
    case PHRYGIAN:    scaleExpanded = PHRYGIAN_BIN;    break;
    case LYDIAN:      scaleExpanded = LYDIAN_BIN;      break;
    case MIXOLYDIAN:  scaleExpanded = MIXOLYDIAN_BIN;  break;
    case AEOLIAN:     scaleExpanded = AEOLIAN_BIN;     break;
    case LOCRIAN:     scaleExpanded = LOCRIAN_BIN;     break;
    case BLUESMAJOR:  scaleExpanded = BLUESMAJOR_BIN;  break;
    case BLUESMINOR:  scaleExpanded = BLUESMINOR_BIN;  break;
    case PENT_MAJOR:  scaleExpanded = PENT_MAJOR_BIN;  break;
    case PENT_MINOR:  scaleExpanded = PENT_MINOR_BIN;  break;
    case FOLK:        scaleExpanded = FOLK_BIN;        break;
    case JAPANESE:    scaleExpanded = JAPANESE_BIN;    break;
    case GAMELAN:     scaleExpanded = GAMELAN_BIN;     break;
    case GYPSY:       scaleExpanded = GYPSY_BIN;       break;
    case ARABIAN:     scaleExpanded = ARABIAN_BIN;     break;
    case FLAMENCO:    scaleExpanded = FLAMENCO_BIN;    break;
    case WHOLETONE:   scaleExpanded = WHOLETONE_BIN;   break;
		default:          scaleExpanded = SEMITONE_BIN;    break;
	}


	//Serial.println("Original Scale:\t" + String(scaleExpanded, BIN) );
	for (int i=0; i< quantizeKey; i++){
		//bitwise rotation - 11 bits rotate to the right. Do it once for each scale degree
		scaleExpanded = (scaleExpanded >> 1) | ((0b01 & scaleExpanded) << 11);
	}
	//Serial.println("Shifted to " + String(quantizeKey) + "\t" + String(scaleExpanded, BIN) );

	while ( (0b100000000000 >> (note % 12) ) & ~scaleExpanded ) {
		if (direction){
			note += 1;
		} else {
			note -= 1;
		}
		count += 1;
		if (count > 12) {
			break; // emergency break if while loop goes OOC
		}
	}
	return note;
}
