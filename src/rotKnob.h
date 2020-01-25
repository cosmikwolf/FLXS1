
/* https://forum.pjrc.com/threads/44592-Encoders-(once-more)?highlight=encoder
 *  Compact and quick encoder class for these horribly bouncing
 * cheap rotary knobs with 4 transitions per click.
 * Default range is from -128 to +127 starting at 0.
 *
 * Might be initialized with begin(startVal); to start at
 * a predefined number.
 * Might be initialized with begin(startVal, lowBound, hiBound);
 * to reduce the range, see examples below.
 * Works with all TEENSY 3.x processors.
 *
 * Free to use for everybody - by (ThF) 2017 */
#include <Arduino.h>

template <uint8_t pinA, uint8_t pinB>
class rotKnob
{
public:
	void begin(int8_t startVal, int8_t lowBound, int8_t hiBound)
	{
		d._sValue = startVal;
		d._lBound = lowBound;
		d._hBound = hiBound;
		d._aConf = digital_pin_to_info_PGM[(pinA)].config;
		d._bConf = digital_pin_to_info_PGM[(pinB)].config;
		pinMode(pinA, INPUT_PULLUP);
		pinMode(pinB, INPUT_PULLUP);
		delay(1);
		d._aVal = digitalReadFast(pinA);
		d._bVal = digitalReadFast(pinB);
		attachInterrupt(pinA, intPinA, CHANGE);
		attachInterrupt(pinB, intPinB, CHANGE);
	}
	void begin(int8_t startVal)
	{
		begin(startVal, -128, 127);
	}
	void begin()
	{
		begin(0, -128, 127);
	}
	void end()
	{
		detachInterrupt(pinA);
		detachInterrupt(pinB);
	}
	int8_t read()
	{
		d._avail = false;
		return d._sValue;
	}
	bool available()
	{
		return d._avail;
	}

private:
	struct objData
	{
		volatile uint32_t *_aConf, *_bConf;
		volatile int8_t _lBound, _hBound, _sValue;
		volatile bool _aVal, _bVal, _avail;
	};
	static objData d;
	static void intPinA()
	{
		*d._aConf &= ~0x000F0000; // disable pin A interrupts
								  // decoding logic
		if (!d._aVal)
		{
			if ((d._sValue < d._hBound) && !d._bVal)
			{
				d._sValue++;
				d._avail = true;
			}
			if ((d._sValue > d._lBound) && d._bVal)
			{
				d._sValue--;
				d._avail = true;
			}
		}
		d._bVal = digitalReadFast(pinB); // read pinB which is stable after pinA transition
		*d._bConf |= 0x000B0000;		 // (re-) enable pinB interrupts
	}
	static void intPinB()
	{
		*d._bConf &= ~0x000F0000;		 // disable pinB interrupts
		d._aVal = digitalReadFast(pinA); // read pinA which is stable after pinB transition
		*d._aConf |= 0x000B0000;		 // (re-) enable pinA interrupts
	}
};
template <uint8_t pinA, uint8_t pinB>
typename rotKnob<pinA, pinB>::objData rotKnob<pinA, pinB>::d;
