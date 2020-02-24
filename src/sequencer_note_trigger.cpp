#include "Arduino.h"
#include "Sequencer.h"

uint16_t Sequencer::get_play_pitch(uint8_t stepNum, uint16_t *pitchArray,  uint8_t arpTypeTrig, uint8_t arpOctaveTrig){

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

    return playPitch;
}

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

	stepData[stepNum].notePlaying = this->get_play_pitch(stepNum, pitchArray,  arpTypeTrig, arpOctaveTrig);

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
		stepData[stepNum].framesRemaining += (3 * stepData[stepNum].gateLength + 2 + outputControl->cvInputCheck(cv_gatemod)) * framesPerBeat(globalObj->tempoX100) * clockDivisionNum() / (8 * clockDivisionDen());

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
			// if ((stepNum + swingSwitch + (stepData[stepNum].arpStatus * getArpSpeedNumerator(stepNum)) / getArpSpeedDenominator(stepNum)) % 2){
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

		stepData[stepNum].arpLastFrame = stepData[stepNum].framesRemaining / 64;
		if (stepData[stepNum].arpLastFrame < getStepLength() / 64)
		{
			stepData[stepNum].arpLastFrame = getStepLength() / 64;
		}

	}


	// Serial.println(
	// "millis: " + String(millis()) +
	// "\tstepNum: " + String(stepNum) +
	// "\tchannel: " + String(channel) +
	// "\tpattern: " + String(pattern) +
	// "\tgateLength: " + String(stepData[stepNum].gateLength) +
	// //"\tminmax: " + String(min_max(stepData[stepNum].framesRemaining, 1, 64 )) +
	// //"\tcurrentFrame: "  + String(currentFrame) +
	// //"\tswinging: " + String(swingCount % 2) +
	// //"\tFramesRem: " + String(stepData[stepNum].framesRemaining) +
	// "\tgetStepLength: " + String(getStepLength()) +
	// "\tarpStatus: "  + String(stepData[stepNum].arpStatus) +
	// "\tarptype: "  + String(stepData[stepNum].arpType)
	// );


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