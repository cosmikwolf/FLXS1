/* project template for ugfx-arduino

How to use this template:
1) save this file as a new sketch named <SKETCH> (this is the placeholder used in the following instructions).

2) create a sketch-specific configuration library called "<SKETCH>-ugfx-config".
   You can use the library template to get started:
     - copy the whole project-ugfx-config/ folder to your arduino libraries folder
     - rename it to <SKETCH>-ugfx-config/
     - rename the main library header from project-ugfx-config.h to <SKETCH>-ugfx-config.h
   This config library is needed to make arduino add an include path to your compiler's command line.

3) adjust the sketch-specific configuration library:
     - add driver include directives in <SKETCH>-ugfx-config.h
     - [optional] adjust gfxconf.h if necessary.
       gfxconf.example.h lists available ugfx options
     - [optional] add custom fonts to userfonts.h

Have fun!
*/

#include <project-ugfx-config.h> // adjust to <SKETCH>-ugfx-config.h
#include <SPI.h>

void setup() {
  coord_t    width, height;
  coord_t   i, j;

  SPI.begin();

  // Initialize and clear the display
  gfxInit();

  // Get the screen size
  width = gdispGetWidth();
  height = gdispGetHeight();

  // Code Here
  gdispDrawBox(10, 10, width / 2, height / 2, Yellow);
  gdispFillArea(width / 2, height / 2, width / 2 - 10, height / 2 - 10, Blue);
  gdispDrawLine(5, 30, width - 50, height - 40, Red);

  for (i = 5, j = 0; i < width && j < height; i += 7, j += i / 20)
    gdispDrawPixel(i, j, White);

  SPI.end();
}

void loop() {
}

