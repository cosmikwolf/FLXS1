#include <Arduino.h>
#include "global.h"


//Zetaohm_SAM2695 sam2695;

//uint32_t globalObj->tempoX100 = 99900;
uint8_t stepMode = 0;
uint8_t notePage;
uint8_t	currentMenu = STATE_PITCH0;       // Display module + LED module
uint8_t previousState = 0;      // Display module + LED moduleextern

uint8_t selectedChannel = 0;
float   probability;
float   frequency;

boolean eraseAllFlag = false;
boolean midiTestActive;

//uint8_t clockMode = INTERNAL_CLOCK;
//uint8_t clockMode = EXTERNAL_MIDI_CLOCK;
//uint8_t clockMode = EXTERNAL_CLOCK_GATE_0;

boolean debugBoolean = false;
uint8_t currentPattern = 0;
boolean playing = false;

// MasterClock
boolean wasPlaying = false;
uint8_t masterPulseCount = 24;
//unsigned long beatLength = 60000000/(globalObj->tempoX100/100);

uint32_t lfoClockCounter;
elapsedMillis modalTimer;
uint8_t dacMap[8] = {7,1,0,6,5,2,3,4};

//multi select variables;
//midi

uint8_t selectedText = 0;
uint8_t prevSelectedText = 255 ;

//save file
uint8_t saveDestination[4] = {0,0,0,0};

uint8_t chordArray[27][3] = {
{255,255,255},
{4,7,255},
{3,7,255},
{4,7,10},
{3,7,10},
{4,7,11},
{3,7,11},
{4,7,9},
{3,7,9},
{4,8,255},
{4,6,255},
{5,7,255},
{5,7,10},
{4,7,14},
{4,8,10},
{3,8,10},
{4,8,11},
{4,6,10},
{3,6,10},
{4,6,11},
{2,7,255},
{2,7,10},
{3,6,9},
{3,6,255},
{3,6,10},
{7,255,255},
{4,10,15}
};

uint16_t colundiArrayX100[56] = {  0, 161, 165, 212, 235, 247, 254, 254, 270, 277, 294, 319, 328, 335, 336, 377, 378, 399, 428, 436, 461, 476, 498, 520, 523, 524, 525, 526, 527, 527, 528, 535, 536, 537, 543, 561, 576, 587, 589, 599, 610, 630, 635, 658, 661, 678, 823, 847, 852, 858, 863, 868, 910, 913, 953, 1012 };

// void//Serial.println(String text){
// 	if (debugBoolean == 1){
// 		Serial.println(text);
// 	}
//}

uint32_t positive_modulo(int i, int n) {
	return (i % n + n) % n;
}

int modulo_minimum(int i, int n, int minimum){
	int mod = (i % n + n) % n;
	if (mod < minimum){
		return minimum;
	} else {
		return mod;
	}
}

int min_max(int value, int min, int max){
	if (value < min){
		return min;
	} else if( value > max){
		return max;
	} else {
		return value;
	}
}

int min_max_skip(int original, int change, int min, int max, int skip){
  int returnValue;
	if (original+change < min){
		returnValue = min;
	} else if( original+change > max){
		returnValue = max;
	} else {
		returnValue = original+change;
	}
  if (returnValue == skip){
    if (returnValue == max){
      return returnValue - 1;
    } else {
      if (change > 0){
        return returnValue +1;
      } else {
        return returnValue - 1;
      }
    }
  } else {
    return returnValue;
  }
}

// if first = 48 & step = 32, go from 48 to 16
//63 -> 63
//64 -> 0
//1 -> 1

int min_max_wrap(int value, int first, int count, int max){
	return (((value - (first)) % (count) ) + first ) % (max);
}

int min_max_cycle(int value, int min, int max){
	//cycle through back to
	if (value < min){
		return max;
	} else if( value > max){
		return min;
	} else {
		return value;
	}
}

uint8_t getNote(uint8_t index){
	return index + notePage * 16;
}

int intFloatHundreds(int value){
	return value/100;
};
int intFloatTensOnes(int value){
	return abs(value - (value/100*100));
};
