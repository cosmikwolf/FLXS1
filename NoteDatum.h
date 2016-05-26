#ifndef NoteDatum_h
#define NoteDatum_h

typedef struct NoteDatum{
	  boolean   noteOn;             // is a note going to play?
	  boolean   noteOff;            // does a note need to be shut off?
	  uint8_t   noteOnArray[128];   // contains all notes that need to be played
	  uint8_t   noteVelArray[128];  // contains CC info for notes to be played
	  uint8_t   noteOffArray[128];  // contains all notes that need to be stopped
	  uint8_t   channel;            // sequence channel (0-3)
	  uint8_t   noteOnStep;         // step number that originated the noteOn message.
	  uint8_t   noteOffStep;        // step number that originated the noteOff message.
	  uint32_t	triggerTime;
	  uint32_t	offset;
	  uint32_t	sequenceTime;
	} NoteDatum ;

extern NoteDatum noteData[4];

#endif
