#ifndef NoteDatum_h
#define NoteDatum_h

	typedef struct {
	  boolean   noteOn;             // is a note going to play?
	  boolean   noteOff;            // does a note need to be shut off?
	  uint8_t   noteOnArray[16];    // contains all notes that need to be played
	  uint8_t   noteVelArray[16];   // contains CC info for notes to be played
	  uint8_t   noteOffArray[16];   // contains all notes that need to be stopped
	  uint8_t   channel;            // midi channel to send the message
	  uint8_t   noteOnStep;         // step number that originated the noteOn message.
	  uint8_t   noteOffStep;        // step number that originated the noteOff message.
		unsigned long	triggerTime;
		unsigned long	offset;
		unsigned long	sequenceTime;

	} NoteDatum ;

#endif
