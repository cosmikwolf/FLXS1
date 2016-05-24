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
 *  This is an implementation of the AllPass filter : AP^g_N
 * as defined in https://ccrma.stanford.edu/~jos/pasp/Freeverb.html
 */ 
 
#ifndef filter_ap_h_
#define filter_ap_h_

#include "AudioStream.h"

class AudioFilterAP : public AudioStream
{
public:
	AudioFilterAP(void) : AudioStream(1, inputQueueArray) {
		// by default, the filter will not pass anything
		for (int i=0; i<32; i++) definition[i] = 0;
		for (int i=0; i < 4; i++) inDelayline[i] = NULL;
		for (int i=0; i < 4; i++) outDelayline[i] = NULL;
		for (int i=0; i < 4; i++) delay_length[i] = 0;
		for (int i=0; i < 4; i++) circ_idx[i] = 0;
	}
	virtual void update(void);

	// Set the AP coefficients directly
	void setCoefficients(uint32_t stage, const int *coefficients);
	void setCoefficients(uint32_t stage, const double g, uint32_t *in_del_line, uint32_t *out_del_line, int del_length) {
		int coef[5];
		coef[0] = g * 1073741824.0;
		coef[1] = stage; 
		coef[2] = 1.0 * 1073741824.0;
		coef[3] = 1.0 * 1073741824.0;
		coef[4] = g * 1073741824.0;  
		__disable_irq();
		circ_idx[stage] = 0;
		delay_length[stage] = del_length;
		inDelayline[stage] = in_del_line;
		outDelayline[stage] = out_del_line;
		__enable_irq();
		setCoefficients(stage, coef);
	}

private:
	int32_t definition[32];  // up to 4 cascaded APs
	audio_block_t *inputQueueArray[1];
	
	uint32_t *inDelayline[4];
	uint32_t *outDelayline[4];
	int delay_length[4];
	short circ_idx[4];
};

#endif