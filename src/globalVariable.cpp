#include <Arduino.h>
#include "globalVariable.h"

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
