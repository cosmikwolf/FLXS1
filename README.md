Notes!


Timer:

Needs to run at 300 BPM

@300 BPM: each beat takes up 200ms

during this time, display needs to update roughly 4-6 times

notes need to be triggered based upon minimum beat division timing... meaning at 300 bpm, and 16 steps per beat... 

200ms / 16 notes/beat = clock need to be triggered every 12.5 ms minimum.

this means everything needs to happen within this grid. 

currently, clock takes about 2ms per trigger.



List of loops from most frequent to least:

clock,
LED loop,
button loop,
display loop.
save loop.





OVERCLOCK Notes:


168		Neopixels don't work at this speed
		SSD1351 doesn't work at this speed
144		SSD1351 - SLOWWWWWWWWWWWW @ SPI_CLOCK_DIV2 
144		SSD1351 - 515k pixels/sec @ SPI_CLOCK_DIV2 
							 SPI_CLOCK_DIV4

120		SSD1351 - 562k pixels/sec @ SPI_CLOCK_DIV2
96 		SSD1351 - 449k pixels/sec @ SPI_CLOCK_DIV2