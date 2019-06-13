#include "Arduino.h"
#include "Sequencer.h"

void Sequencer::initialize(uint8_t ch, uint8_t stepCount, uint8_t beatCount, OutputController *outputControl, GlobalVariable *globalObj)
{
	// initialization routine that runs during setup
	Serial.println("Initializing Sequencer Object");
	this->channel = ch;
	this->pattern = 0;
	this->stepCount = stepCount;
	this->beatCount = beatCount;
	this->clockDivision = 4;
	this->swingX100 = 50;
	//this->beatLength = 60000000/(globalObj->tempoX100/100);
	this->monophonic = true;
	this->outputControl = outputControl;
	this->globalObj = globalObj;
};

void Sequencer::initNewSequence(uint8_t pattern, uint8_t ch)
{
	// Serial.println("*&*&*&*& Initializing pattern: " + String(pattern) + " channel: " + String(ch));

	this->stepCount = 16;
	this->firstStep = 0;
	this->beatCount = 4;
	this->swingX100 = 50;
	this->quantizeKey = 0;
	this->quantizeMode = 0b1111111111111111;
	this->quantizeScale = 12;
	this->quantize_is_edo_scale = true;
	this->pattern = pattern;
	this->channel = ch;
	this->clockDivision = 4;
	this->gpio_reset = 0;
	this->gpio_skipstep = 0;
	this->skipStepCount = 4;
	this->cv_arptypemod = 0;
	this->gpio_gatemute = 0;
	this->gpio_randompitch = 0;
	this->randomHigh = 6;
	this->randomLow = 6;
	this->cv_arpspdmod = 0;
	this->cv_arpoctmod = 0;
	this->cv_arpintmod = 0;
	this->cv_pitchmod = 0;
	this->cv_gatemod = 0;
	this->cv_glidemod = 0;
	this->muteGate = 0;
	this->muteCV1 = 0;
	this->muteCV2 = 0;
	this->fill = 0;
	this->skipNextNoteTrigger = 0;
	this->tieFlag = 0;
	this->transpose = 0;
	for (int stepNum = 0; stepNum < MAX_STEPS_PER_SEQUENCE; stepNum++)
	{
		this->initializeStep(stepNum);
	}

	outputControl->clearVelocityOutput(ch);
};

void Sequencer::initializeStep(uint8_t stepNum)
{
	if (stepNum > MAX_STEPS_PER_SEQUENCE)
	{
		return;
	}
	this->stepData[stepNum].pitch[0] = DEFAULT_PITCH0_VAL;
	for (int i = 1; i < 4; i++)
	{
		this->stepData[stepNum].pitch[i] = 0;
	}
	this->stepData[stepNum].chord = 0;
	this->stepData[stepNum].gateType = DEFAULT_GATETYPE;
	this->stepData[stepNum].gateLength = 3;
	this->stepData[stepNum].arpType = 0;
	this->stepData[stepNum].arpOctave = 1;
	this->stepData[stepNum].arpSpdNum = 1;
	this->stepData[stepNum].arpSpdDen = 2;
	this->stepData[stepNum].glide = 0;
	this->stepData[stepNum].beatDiv = 4;
	this->stepData[stepNum].velocity = DEFAULT_VELOCITY;
	this->stepData[stepNum].velocityType = DEFAULT_VELTYPE;
	this->stepData[stepNum].cv2speed = DEFAULT_CV2SPEED;
	this->stepData[stepNum].cv2offset = DEFAULT_CV2OFFSET;
	this->stepData[stepNum].noteStatus = AWAITING_TRIGGER;
	this->stepData[stepNum].arpStatus = 0;
	this->stepData[stepNum].notePlaying = 0;
	this->stepData[stepNum].framesRemaining = 0;
}

bool Sequencer::hasData()
{
	for (int stepNum = 0; stepNum < MAX_STEPS_PER_SEQUENCE; stepNum++)
	{
		if (this->stepData[stepNum].pitch[0] != DEFAULT_PITCH0_VAL)
		{
			return true;
		};
		if (this->stepData[stepNum].gateType != DEFAULT_GATETYPE)
		{
			return true;
		};
		if (this->stepData[stepNum].velocityType != DEFAULT_VELTYPE)
		{
			return true;
		};
		if (this->stepData[stepNum].velocity != DEFAULT_VELOCITY)
		{
			return true;
		};
		if (this->stepData[stepNum].cv2speed != DEFAULT_CV2SPEED)
		{
			return true;
		};
		if (this->stepData[stepNum].cv2offset != DEFAULT_CV2OFFSET)
		{
			return true;
		};
	}
	return false;
}

bool Sequencer::toggleMute(uint8_t index)
{
	switch (index)
	{
	case 0:
		this->muteGate = !muteGate;

		if (muteGate)
		{
			for (int stepNum = 0; stepNum < (firstStep + stepCount); stepNum++)
			{
				if (stepNum == activeStep)
				{
					break;
				}
				stepData[stepNum].noteStatus = AWAITING_TRIGGER;
				stepData[stepNum].arpStatus = 0;
			}
			outputControl->allNotesOff(channel);
			//outputControl->clearVelocityOutput(channel);
		}

		return muteGate;
		break;
	case 1:
		this->muteCV1 = !muteCV1;
		return muteCV1;
		break;
	case 2:
		this->muteCV2 = !muteCV2;
		outputControl->clearVelocityOutput(channel);
		return muteCV2;
		break;
	}
};

uint16_t Sequencer::getStepPitch(uint8_t step, uint8_t index)
{ //read step pitch, which is stored as dac_code, and present it as an EDO pitch based on current scale quantization
	uint16_t test = globalObj->quantize_edo_dac_code_to_scale_degree(stepData[step].pitch[index], this->quantizeScale);
	// Serial.printf("%d    %d\n", test, stepData[step].pitch[index]);
	return test;
};

void Sequencer::setStepPitch(uint8_t step, uint16_t pitch, uint8_t index)
{ //read in scale degree and save step data as dac_code, so it can be remapped to any other EDO
	stepData[step].pitch[index] = globalObj->quantize_edo_scale_degree_to_dac_code(pitch, quantizeScale);
	Serial.println("step: " + String(step) + " pitch: " + String(pitch) + " index: " + String(index) + " set pitch: " + String(stepData[step].pitch[index]));
};

void Sequencer::setGateLength(uint8_t step, uint8_t length)
{
	stepData[step].gateLength = length;
};

void Sequencer::setBeatCount(uint16_t beatCountNew)
{
	beatCount = beatCountNew;
};

void Sequencer::setGateType(uint8_t step, uint8_t gate)
{
	stepData[step].gateType = gate;
}

void Sequencer::setStepVelocity(uint8_t step, uint8_t velocity)
{
	stepData[step].velocity = velocity;
};

void Sequencer::setStepGlide(uint8_t step, uint8_t glideTime)
{
	stepData[step].glide = glideTime;
}

uint8_t Sequencer::quantizePitch(uint8_t note, uint8_t key, uint8_t scale, bool direction)
{

	uint8_t count = 0;
	uint16_t scaleExpanded;

	switch (scale)
	{
		// case 1: scaleExpanded = CHROMATIC         ; break;
		// case 2: scaleExpanded = MAJOR             ; break;
		// case 3: scaleExpanded = MINOR             ; break;
		// case 4: scaleExpanded = MAJORMINOR        ; break;
		// case 5: scaleExpanded = PENTATONIC_MAJOR  ; break;
		// case 6: scaleExpanded = PENTATONIC_MINOR  ; break;
		// case 7: scaleExpanded = PENTATONIC_BLUES  ; break;
		// case 8: scaleExpanded = IONIAN            ; break;
		// case 9: scaleExpanded = AEOLIAN           ; break;
		// case 10: scaleExpanded = DORIAN           ; break;
		// case 11: scaleExpanded = MIXOLYDIAN       ; break;
		// case 12: scaleExpanded = PHRYGIAN         ; break;
		// case 13: scaleExpanded = LYDIAN           ; break;
		// case 14: scaleExpanded = LOCRIAN          ; break;
		// default: scaleExpanded = CHROMATIC				; break;
	}

	//Serial.println("Original Scale:\t" + String(scaleExpanded, BIN) );
	for (int i = 0; i < key; i++)
	{
		//bitwise rotation - 11 bits rotate to the right. Do it once for each scale degree
		scaleExpanded = (scaleExpanded >> 1) | ((0b01 & scaleExpanded) << 11);
	}
	//Serial.println("Shifted to " + String(quantizeKey) + "\t" + String(scaleExpanded, BIN) );

	while ((0b100000000000 >> (note % 12)) & ~scaleExpanded)
	{
		if (direction)
		{
			note += 1;
		}
		else
		{
			note -= 1;
		}
		count += 1;
		if (count > 12)
		{
			break; // emergency break if while loop goes OOC
		}
	}
	return note;
}

int Sequencer::positive_modulo(int i, int n)
{
	return (i % n + n) % n;
}

void Sequencer::gateInputTrigger(uint8_t inputNum)
{
	if (inputNum == channel + 5)
	{
		return; //don't let a channel reset or y axis itself.
	}
	if (gpio_reset == inputNum)
	{
		this->clockReset(true);
	}

	if (gpio_skipstep == inputNum)
	{
		if (globalObj->playing)
		{
			this->skipStep(skipStepCount);
		}
	}
};

void Sequencer::randomize(uint8_t parameter)
{
	uint8_t highval;

	switch (parameter)
	{
	case RANDOMIZE_PARAM_PITCHGATE:
		highval = min_max(globalObj->randomizeLow + 12 * globalObj->randomizeSpan, 0, 127);

		for (int stepNum = 0; stepNum < MAX_STEPS_PER_SEQUENCE; stepNum++)
		{
			this->stepData[stepNum].pitch[0] = globalObj->generateRandomNumber(globalObj->randomizeLow, highval);
			this->stepData[stepNum].gateType = globalObj->generateRandomNumber(0, 2);
			this->stepData[stepNum].gateLength = globalObj->generateRandomNumber(1, 10);
			;
			//Serial.print(String(stepData[stepNum].pitch[0]) + " ");
			//if (stepNum % 16 == 0) 			Serial.println(" ");
		}
		//Serial.println(" ");
		break;
	case RANDOMIZE_PARAM_PITCH:
		highval = min_max(globalObj->randomizeLow + 12 * globalObj->randomizeSpan, 0, 127);
		for (int stepNum = 0; stepNum < MAX_STEPS_PER_SEQUENCE; stepNum++)
		{
			this->stepData[stepNum].pitch[0] = globalObj->generateRandomNumber(globalObj->randomizeLow, highval);
		}
		break;
	case RANDOMIZE_PARAM_GATE:
		for (int stepNum = 0; stepNum < MAX_STEPS_PER_SEQUENCE; stepNum++)
		{
			this->stepData[stepNum].gateType = globalObj->generateRandomNumber(0, 2);
			this->stepData[stepNum].gateLength = globalObj->generateRandomNumber(1, 10);
			;
		}
		break;
	case RANDOMIZE_PARAM_CV2_TYPE:
		for (int stepNum = 0; stepNum < MAX_STEPS_PER_SEQUENCE; stepNum++)
		{
			if (globalObj->randomize_cv2_type == 0)
			{
				if (globalObj->randomize_cv2_type_include_skip)
				{
					this->stepData[stepNum].velocityType = globalObj->generateRandomNumberIncludeZero(4, 7);
				}
				else
				{
					this->stepData[stepNum].velocityType = globalObj->generateRandomNumber(4, 7);
				}
			}
			else
			{
				if (globalObj->randomize_cv2_type_include_skip)
				{
					this->stepData[stepNum].velocityType = globalObj->generateRandomNumberIncludeZero(8, 14);
				}
				else
				{
					this->stepData[stepNum].velocityType = globalObj->generateRandomNumber(8, 14);
				}
			}
		}
		// CV2 type values are based on the velTypeArray listed below
		//  velTypeArray[] = { "skip","trigger","quantized", "voltage","Env Decay","Env Attack","Env AR","Env ASR","LFO Sine","LFO Tri","LFO Square", "LFO RndSq", "LFO SawUp","LFO SawDn","LFO S+H" };
		break;
	case RANDOMIZE_PARAM_CV2_SPEED:
		for (int stepNum = 0; stepNum < MAX_STEPS_PER_SEQUENCE; stepNum++)
		{
			uint8_t randomNum = globalObj->generateRandomNumber(globalObj->randomize_cv2_speedmin, globalObj->randomize_cv2_speedmax);
			if (globalObj->randomize_cv2_speedsync)
			{
				this->stepData[stepNum].cv2speed = randomNum - (randomNum % (64 / globalObj->randomize_cv2_speedsync));
			}
			else
			{
				this->stepData[stepNum].cv2speed = randomNum;
			}
		}
		break;

	case RANDOMIZE_PARAM_CV2_AMPLITUDE:
		for (int stepNum = 0; stepNum < MAX_STEPS_PER_SEQUENCE; stepNum++)
		{
			this->stepData[stepNum].velocity = globalObj->generateRandomNumber(globalObj->randomize_cv2_amplitude_min, globalObj->randomize_cv2_amplitude_max);
		}
		break;

	case RANDOMIZE_PARAM_CV2_OFFSET:
		for (int stepNum = 0; stepNum < MAX_STEPS_PER_SEQUENCE; stepNum++)
		{
			this->stepData[stepNum].cv2offset = globalObj->generateRandomNumber(globalObj->randomize_cv2_offset_min, globalObj->randomize_cv2_offset_max);
		}
		break;
	}
}

void Sequencer::skipStep(uint8_t count)
{
	//Serial.println("skipStep: " + String(count) + "\tppqPulseIndex: "+ String(ppqPulseIndex) + "\tpulsesPerBeat: " + String(pulsesPerBeat) + "\t" );
	if (count == 0)
	{
		count = random(1, 16);
	}
	// uint16_t oldPpqPulseIndex = ppqPulseIndex;
	// ppqPulseIndex = (ppqPulseIndex + count*pulsesPerBeat/clockDivision) % (pulsesPerBeat*stepCount/clockDivision );
	// for(int stepNum = 0; stepNum < stepCount; stepNum++){
	//   stepData[stepNum].framesRemaining += oldPpqPulseIndex-ppqPulseIndex;
	// }
	switch (playMode)
	{
	case PLAY_REVERSE:
		activeStep -= count;
		break;
	case PLAY_PENDULUM:
		if (pendulumSwitch)
		{
			activeStep += count;
		}
		else
		{
			activeStep -= count;
		}
		break;
	default:
		activeStep += count;
		break;
	}

	int32_t framesToSubtract = getStepLength() * count;

	activeStep = min_max_wrap(activeStep, firstStep, stepCount, MAX_STEPS_PER_SEQUENCE);
	for (int stepNum = 0; stepNum < MAX_STEPS_PER_SEQUENCE; stepNum++)
	{

		if (framesToSubtract >= stepData[stepNum].framesRemaining)
		{
			stepData[stepNum].framesRemaining = 0;
		}
		else
		{
			stepData[stepNum].framesRemaining -= framesToSubtract;
		}
	}
}

void Sequencer::jumpToStep(uint8_t stepNum)
{
	//  Serial.println("skipStep: " + String(count) + "\tppqPulseIndex: "+ String(ppqPulseIndex) + "\tpulsesPerBeat: " + String(pulsesPerBeat) + "\t" );

	uint16_t oldPpqPulseIndex = ppqPulseIndex;
	ppqPulseIndex = clockDivisionNum() * stepNum * pulsesPerBeat / clockDivisionDen();
	for (int stepNum = 0; stepNum < stepCount; stepNum++)
	{
		stepData[stepNum].framesRemaining += ppqPulseIndex;
	}
}

void Sequencer::stoppedTrig(uint8_t stepNum, bool onOff, bool gate)
{

	if (onOff)
	{
		//  Serial.println("Stopped trig note on");
		// if (quantizeMode > 0){
		// 	stepData[stepNum].notePlaying = quantizePitch(stepData[stepNum].pitch[0], quantizeKey, quantizeMode, 1);
		// } else {

		stepData[stepNum].notePlaying = globalObj->quantize_edo_dac_code_to_scale_degree(stepData[stepNum].pitch[0], quantizeScale);
		stepData[stepNum].notePlaying = globalObj->quantize_edo_scale_degree_to_key(stepData[stepNum].notePlaying, this->quantizeKey, this->quantizeMode, this->quantizeScale, 0);

		//	outputControl->noteOff(channel, stepData[stepNum].notePlaying, false );
		stepData[stepNum].notePlaying += this->transpose;

		outputControl->noteOn(channel, stepNum, stepData[stepNum].notePlaying, stepData[stepNum].velocity, stepData[stepNum].velocityType, stepData[stepNum].cv2speed, stepData[stepNum].cv2offset, stepData[stepNum].glide, gate, 0, quantizeScale, quantizeMode, quantizeKey, muteCV1, stepData[stepNum].stepStartFrame, true);

		stepData[stepNum].noteStatus == CURRENTLY_PLAYING;
	}
	else
	{
		//Serial.println("Stopped trig note off");
		outputControl->noteOff(channel, stepData[stepNum].notePlaying, true);
		//stepData[stepNum].noteStatus = AWAITING_TRIGGER;
		//stepData[stepNum].arpStatus = 0;
	}
};

void Sequencer::noteTrigger(uint8_t stepNum, bool gateTrig, uint8_t arpTypeTrig, uint8_t arpOctaveTrig)
{
	uint16_t pitchArray[22];
	pitchArray[0] = globalObj->quantize_edo_dac_code_to_scale_degree(stepData[stepNum].pitch[0], quantizeScale);

	// Serial.printf("-%d-\n", pitchArray[0]);
	pitchArray[1] = globalObj->quantize_edo_dac_code_to_scale_degree(stepData[stepNum].pitch[0] + stepData[stepNum].pitch[1], quantizeScale);
	pitchArray[2] = globalObj->quantize_edo_dac_code_to_scale_degree(stepData[stepNum].pitch[0] + stepData[stepNum].pitch[2], quantizeScale);
	pitchArray[3] = globalObj->quantize_edo_dac_code_to_scale_degree(stepData[stepNum].pitch[0] + stepData[stepNum].pitch[3], quantizeScale);

	if (skipNextNoteTrigger)
	{ // so that when first step is changed, it doesn't constantly retrigger
		skipNextNoteTrigger = false;
		return;
	}

	if (globalObj->extClock() && !receivedFirstExtClock)
	{
		return;
	}

	//figure out how many steps are nil (255)
	uint8_t arpSteps = 4;
	for (int i = 1; i < 4; i++)
	{
		if (stepData[stepNum].pitch[i] == 255)
		{
			arpSteps = arpSteps - 1;
		}
	}

	//fill up the rest of the array with octave up pitches
	for (int i = arpSteps; i < 5 * arpSteps; i++)
	{
		pitchArray[i] = pitchArray[i - arpSteps] + 12;
	}

	if (arpOctaveTrig > 0)
	{
		arpSteps = arpOctaveTrig * arpSteps;
	}

	uint16_t index;

	uint16_t playPitch; //pitch that will be triggered in this loop

	switch (arpTypeTrig)
	{
	case ARPTYPE_UP:
		index = stepData[stepNum].arpStatus % arpSteps;
		playPitch = pitchArray[index];
		break;

	case ARPTYPE_DN:
		index = arpSteps - stepData[stepNum].arpStatus % arpSteps;
		playPitch = pitchArray[index];
		break;

	case ARPTYPE_UPDN1:
		// repeating top and bottom note
		if (stepData[stepNum].arpStatus / arpSteps % 2)
		{
			index = stepData[stepNum].arpStatus % arpSteps;
		}
		else
		{
			index = arpSteps - stepData[stepNum].arpStatus % arpSteps;
		}

		playPitch = pitchArray[index];
		break;

	case ARPTYPE_UPDN2: // no repeat of top and bottom note
		//arpsteps:           4
		//index:              0 1 2 3 2 1 0 1 2 3
		//arpStatus:          0 1 2 3 4 5 6 7 8 9
		//arpStatus/arpSteps: 0 0 0 0 1 1 1 1 2 2
		//arpstatus%arpSteps: 0 1 2 3 0 1 2 3 0 1
		//                    0 0 0 0 1 1 1
		//										0 0 0 1 1 1 0 0 0 1 1 1
		//up:                 0 1 2 3 0 1 2 3 0 1 2 3
		//down:      			  3 2 1 0 3 2 1 0 3 2

		if (stepData[stepNum].arpStatus / (arpSteps - 1) % 2)
		{
			index = stepData[stepNum].arpStatus % arpSteps;
		}
		else
		{
			index = arpSteps - stepData[stepNum].arpStatus % (arpSteps - 1);
		}
		playPitch = pitchArray[index];
		break;

	case ARPTYPE_RNDM: // no repeat of top and bottom note
		index = random(0, arpSteps);
		playPitch = pitchArray[index];
		break;

	default:
		playPitch = pitchArray[0];
		break;
	}

	//if (quantizeKey == 1){
	//	stepData[stepNum].notePlaying = quantizePitch(playPitch, aminor, 1);
	//if (quantizeMode > 0){
	//		stepData[stepNum].notePlaying = quantizePitch(playPitch, quantizeKey, quantizeMode, 1);
	//	} else {
	stepData[stepNum].notePlaying = playPitch;
	//	}
	//Serial.println("noteOn"); delay(10);
	// Serial.printf("-%d-\
	n", stepData[stepNum].notePlaying);

	//BEGIN INPUT MAPPING SECTION
	//	if (gateInputRaw[gpio_randompitch]){
	if (outputControl->gpioCheck(gpio_randompitch))
	{
		// Serial.println("Randomizng Pitch step: " + String(stepNum) + "\tgateInputRaw 0: " + String(globalObj->gateInputRaw[0] ) + "\tgpipmaping: " + String(gpio_randompitch));
		stepData[stepNum].notePlaying = constrain(random(stepData[stepNum].notePlaying - randomLow, stepData[stepNum].notePlaying + randomHigh), 0, 127);
	}

	stepData[stepNum].notePlaying += outputControl->cvInputCheck(cv_pitchmod);
	stepData[stepNum].notePlaying += this->transpose;
	uint8_t glideVal = min_max(stepData[stepNum].glide + outputControl->cvInputCheck(cv_glidemod), 0, 255);

	//if(gateInputRaw[gpio_gatemute]){
	if (outputControl->gpioCheck(gpio_gatemute))
	{
		gateTrig = false;
	}
	if (stepData[stepNum].arpStatus == 0)
	{
		stepData[stepNum].framesRemaining = 0;
		stepData[stepNum].stepStartFrame = currentFrame;
	}
	if (stepData[stepNum].arpType == ARPTYPE_OFF)
	{
		// THIS INPUT MAPPING STILL NEEDS WORK.
		// CUTS NOTES OFF WHEN GATE IS TOO LONG.
		// NEED TO ADD WATCHDOG TO TURN NOTES OFF BEFORE A NEW ONE IS TRIGGERED
		stepData[stepNum].framesRemaining += (2 * stepData[stepNum].gateLength + 2 + outputControl->cvInputCheck(cv_gatemod)) * framesPerBeat(globalObj->tempoX100) * clockDivisionNum() / (8 * clockDivisionDen());

		if (swingX100 != 50)
		{
			if (!swinging)
			{
				stepData[stepNum].framesRemaining *= 200 - 2 * swingX100;
				stepData[stepNum].framesRemaining /= 100;
			}
			else
			{
				stepData[stepNum].framesRemaining *= 2 * swingX100;
				stepData[stepNum].framesRemaining /= 100;
			}
		}

		stepData[stepNum].arpLastFrame = stepData[stepNum].framesRemaining / 5;

		if (stepData[stepNum].arpLastFrame < getStepLength() / 64)
		{
			stepData[stepNum].arpLastFrame = getStepLength() / 64;
		}
		//  Serial.println(
		//  "stepNum: " + String(stepNum) +
		//    "\tgateLength: " + String(stepData[stepNum].gateLength) +
		//     "\tminmax: " + String(min_max(stepData[stepNum].framesRemaining, 1, 64 )) +
		//  //"\tcurrentFrame: "  + String(currentFrame) +
		//    "\tswinging: " + String(swingCount % 2) +
		//  "\tFramesRem: " + String(stepData[stepNum].framesRemaining) +
		//  "\tgetStepLength: " + String(getStepLength()) +
		// "\tarptype: "  + String(stepData[stepNum].arpType)
		//    );
	}
	else
	{

		stepData[stepNum].framesRemaining += framesPerBeat(globalObj->tempoX100) * clockDivisionNum();
		stepData[stepNum].framesRemaining *= getArpSpeedNumerator(stepNum);
		stepData[stepNum].framesRemaining /= getArpSpeedDenominator(stepNum);
		stepData[stepNum].framesRemaining /= clockDivisionDen();

		if (swingX100 != 50)
		{
			//if ((stepNum + swingSwitch + (stepData[stepNum].arpStatus * getArpSpeedNumerator(stepNum)) / getArpSpeedDenominator(stepNum)) % 2){
			if (!swinging)
			{
				stepData[stepNum].framesRemaining *= 200 - 2 * swingX100;
				stepData[stepNum].framesRemaining /= 100;
			}
			else
			{
				stepData[stepNum].framesRemaining *= 2 * swingX100;
				stepData[stepNum].framesRemaining /= 100;
			}
		}

		stepData[stepNum].arpLastFrame = stepData[stepNum].framesRemaining / 4;
		if (stepData[stepNum].arpLastFrame < getStepLength() / 16)
		{
			stepData[stepNum].arpLastFrame = getStepLength() / 16;
		}
	}

	/*
	Serial.println(
	"millis: " + String(millis()) +
	"\tstepNum: " + String(stepNum) +
	"\tchannel: " + String(channel) +
	"\tpattern: " + String(pattern) +
  //"\tgateLength: " + String(stepData[stepNum].gateLength) +
  //"\tminmax: " + String(min_max(stepData[stepNum].framesRemaining, 1, 64 )) +
  //"\tcurrentFrame: "  + String(currentFrame) +
  //"\tswinging: " + String(swingCount % 2) +
  //"\tFramesRem: " + String(stepData[stepNum].framesRemaining) +
  //"\tgetStepLength: " + String(getStepLength()) +
 	"\tarptype: "  + String(stepData[stepNum].arpType)
);
*/

	//DEBUG_PRINT("clockDivNum:" + String(clockDivisionNum()) + "clockDivDen:" + String(clockDivisionDen()) );

	//END INPUT MAPPING SECTION

	if (!globalObj->waitingToResetAfterPatternLoad)
	{
		// Serial.printf("Triggering note on: %04d\t%04d\n", stepData[stepNum].notePlaying, stepData[stepNum].pitch[0]);
		outputControl->noteOn(channel, stepNum, stepData[stepNum].notePlaying, stepData[stepNum].velocity, stepData[stepNum].velocityType, stepData[stepNum].cv2speed, stepData[stepNum].cv2offset, glideVal, gateTrig, tieFlag, quantizeScale, quantizeMode, quantizeKey, muteCV1, stepData[stepNum].stepStartFrame, stepData[stepNum].arpStatus);
		tieFlag = (stepData[stepNum].gateType == GATETYPE_TIE && gateTrig == true);

		stepData[stepNum].arpStatus++;
	}
	else
	{
		// Serial.println("Prevented note from playing ch:" + String(channel) + "\tstep: " + String(stepNum) + "\t"+ String(globalObj->chainModeMasterPulseToGo));
	}

	//ensuring that gate is turned off before next step:
	// uint8_t nextStep = stepCount;
	// for(int nxtStp = stepNum + 1; nxtStp < stepCount; nxtStp++){
	// 	if (stepData[nxtStp].gateType != GATETYPE_REST){
	// 		nextStep = nxtStp;
	// 		break;
	// 	}
	// }
}
uint32_t Sequencer::getArpLastFrame(uint8_t stepNum, uint16_t arpNum)
{
}

uint32_t Sequencer::getArpStartFrame(uint8_t stepNum, uint16_t arpNum)
{
	uint32_t stepFrames = stepData[stepNum].stepStartFrame;
	uint16_t arpFullSteps = arpNum * getArpSpeedNumerator(stepNum) / getArpSpeedDenominator(stepNum);
	uint16_t arpRemainder = arpNum - arpFullSteps * getArpSpeedDenominator(stepNum) / getArpSpeedNumerator(stepNum);

	if (swingX100 == 50)
	{
		stepFrames += arpNum * getStepLength() * getArpSpeedNumerator(stepNum) / getArpSpeedDenominator(stepNum); //unscaled arp frames
	}
	else if ((stepNum + swingSwitch + (arpNum * getArpSpeedNumerator(stepNum) / getArpSpeedDenominator(stepNum)) % 2))
	{
		stepFrames += arpFullSteps * getStepLength();																									//full steps within the arpeggiation
		stepFrames += (getStepLength() * 2 * swingX100) / 100 - getStepLength();																		// swing step offset
		stepFrames += (arpRemainder * getStepLength() * getArpSpeedNumerator(stepNum) / getArpSpeedDenominator(stepNum)) * (200 - 2 * swingX100) / 100; //scaled arp frames
																																						//Serial.println("+ swing Arp " + String(arpNum) + "\t" + String(stepFrames) + "\tarpFullSteps: " + String(arpFullSteps) + "\tarpRemainder: " + String(arpRemainder) + "\tinitialStepFrames: " + String( (stepNum-firstStep)*getStepLength()) + "\tswingOffset: " + String( (getStepLength() * 2 * swingX100)/ 100 - getStepLength()) + "\tstepLength: " +  String(getStepLength()));
	}
	else
	{
		stepFrames += arpFullSteps * getStepLength();																							  //full steps within the arpeggiation
		stepFrames += (arpRemainder * getStepLength() * getArpSpeedNumerator(stepNum) / getArpSpeedDenominator(stepNum)) * (2 * swingX100) / 100; //scaled arp frames
																																				  //Serial.println("- swing Arp " + String(arpNum) + "\t" + String(stepFrames) + "\tarpFullSteps: " + String(arpFullSteps) + "\tarpRemainder: " + String(arpRemainder) + "\tinitialStepFrames: " + String( (stepNum-firstStep)*getStepLength()) + "\tswingOffset: " + String((getStepLength() * (100 - 2 * swingX100) )/ 100));
	};
	return stepFrames;
};

uint8_t Sequencer::getArpSpeedNumerator(uint8_t stepNum)
{
	// if (arpSpeedModulation[stepNum] < 0){
	// 	return min_max(stepData[stepNum].arpSpdNum - abs(arpSpeedModulation[stepNum]), 1, 64) ;
	// } else {
	return stepData[stepNum].arpSpdNum;
	//}
};
uint8_t Sequencer::getArpSpeedDenominator(uint8_t stepNum)
{
	if (arpSpeedModulation[stepNum] > 0)
	{
		return stepData[stepNum].arpSpdDen * arpSpeedModulation[stepNum];
	}
	else
	{
		return stepData[stepNum].arpSpdDen;
	}
};

uint16_t Sequencer::getArpCount(uint8_t stepNum)
{
	uint16_t arpCount;

	if (arpTypeModulated[stepNum] == ARPTYPE_OFF)
	{
		arpCount = 0;
	}
	else
	{
		arpCount = (stepData[stepNum].gateLength * getArpSpeedDenominator(stepNum) / getArpSpeedNumerator(stepNum)) / 4;
	}

	return arpCount;
}

void Sequencer::noteShutOff(uint8_t stepNum, bool gateOff)
{
	//shut off any other notes that might still be playing.
	//Serial.println("shutting step " + String(stepNum) + " off");
	if (stepData[stepNum].noteStatus == CURRENTLY_PLAYING)
	{
		outputControl->noteOff(channel, stepData[stepNum].notePlaying, gateOff);
	}
}

void Sequencer::setPlayRange(uint8_t first, uint8_t last)
{
	stepCount = abs(last - first) + 1;
	if (first < last)
	{
		firstStep = first + notePage * 16;
		playMode = PLAY_FORWARD;
	}
	else
	{
		firstStep = last + notePage * 16;
		playMode = PLAY_REVERSE;
	}
	skipNextNoteTrigger = true;
};
