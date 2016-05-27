/* Zetaohm_SAM2695PlayAllVoices
* Plays all voices with a random algorithmic tune
* on the Modern Device Zetaohm_SAM2695 Shield
* ModernDevice.com
*/

#include <Zetaohm_SAM2695.h>
#include <PgmChange.h>


    Zetaohm_SAM2695 synth = Zetaohm_SAM2695();

    int counter = 0;
    int channel = 0;
    int voice = 0;
    int j = 0;

    void setup() {
      Serial1.begin(31250);
      synth.programChange(0, 9, 40);
      synth.programChange(0, 1, 0);
    }

    void loop()
    {

      while(counter < 128)
      {
        if(voice >128)
        {
          voice = 0;
        }

        synth.programChange(channel, 0, voice);



        for(int i=0; i<10; i++)
        {

          int note = int(random(36,100));

          synth.noteOn(channel, note, 127);

          int coinflip = int(random(0,20));

          if(coinflip == 5)
          {
            while(j < 200)
            {
              synth.pitchBend(channel, j);
              j+= 16;
              delay(10);
            }
            j = 0;
          }

          delay(100);
          synth.noteOff(channel, note);
        }

        voice++;

      }
      channel++;
      counter++;
    }
