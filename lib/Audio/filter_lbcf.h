/* Audio Library for Teensy 3.X
 * Copyright (c) 2014, Paul Stoffregen, paul@pjrc.com
 *
 * Development of this audio library was funded by PJRC.COM, LLC by sales of
 * Teensy and Audio Adaptor boards.  Please support PJRC's efforts to develop
 * open source software by purchasing Teensy or other PJRC products.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 *  This is an implementation of the Lowpass Feedback Comb Filter : LBCF^{f,d}_N
 * as defined in https://ccrma.stanford.edu/~jos/pasp/Lowpass_Feedback_Comb_Filter.html
 */ 
 
#ifndef filter_lbcf_h_
#define filter_lbcf_h_

#include "AudioStream.h"

class AudioFilterLBCF : public AudioStream
{
public:
	AudioFilterLBCF(void) : AudioStream(1, inputQueueArray) {
		// by default, the filter will not pass anything
		for (int i=0; i<32; i++) definition[i] = 0;
		for (int i=0; i < 4; i++) outDelayline[i] = NULL;
		for (int i=0; i < 4; i++) delay_length[i] = 0;
		for (int i=0; i < 4; i++) circ_idx[i] = 0;
	}
	virtual void update(void);

	// Set the LBCF coefficients directly
	void setCoefficients(uint32_t stage, const int *coefficients);
	void setCoefficients(uint32_t stage, const double *coefficients, uint32_t *del_line, int del_length) {
		int coef[5];
		coef[0] = 1.0 * 1073741824.0;
		coef[1] = coefficients[1] * 1073741824.0; // d
		coef[2] = stage;
		coef[3] = coefficients[1] * 1073741824.0;
		coef[4] = (coefficients[0] * (1 - coefficients[1])) * 1073741824.0;  // f*(1-d)
		__disable_irq();
		circ_idx[stage] = 0;
		delay_length[stage] = del_length;
		outDelayline[stage] = del_line;
		__enable_irq();
		setCoefficients(stage, coef);
	}

private:
	int32_t definition[32];  // up to 4 cascaded LBCFs
	audio_block_t *inputQueueArray[1];
	
	uint32_t *outDelayline[4];
	int delay_length[4];
	short circ_idx[4];
};

#endif