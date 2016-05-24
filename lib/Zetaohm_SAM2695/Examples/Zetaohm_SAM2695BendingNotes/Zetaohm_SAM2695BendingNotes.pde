/* Zetaohm_SAM2695BendingNotes.pde
* Demonstrates note bending on the Modern Device Zetaohm_SAM2695 Shield
* ModernDevice.com
*/

#include "Zetaohm_SAM2695.h"

Zetaohm_SAM2695 synth;

int i = 512;                   // middle of pitch bend range

void setup()
{
  Serial1.begin(31250);

  synth.programChange(127, 0, 91);
  synth.pitchBendRange(0, 4);  //double the pitch bend sensitivity (4 semitones)

  synth.noteOn(0, 62, 127);
}

void loop()
{
  while (i < 1023)             // loop to upper bound of pitch bend range
  {
    synth.pitchBend(0, i);
    i+= 16;
    delay(10);
  }

  while (i > 0)                // loop to lower bound of pitch bend range
  {
    synth.pitchBend(0, i);
    i-= 16;
    delay(10);
  }
}
