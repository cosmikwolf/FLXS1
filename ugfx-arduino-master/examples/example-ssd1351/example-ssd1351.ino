/* example ssd1351 project for ugfx-arduino
1) make sure you have copied the libraries from the repo:
- libraries/ugfx-arduino-gdisp-ssd1351
- libraries/example-ssd1351-ugfx-config
to arduino's libraries folder

2) connect your SSD1351 display to your arduino board
- SCK, MOSI are the standard SPI pins
- RESET, CS and DC as per libraries/example-ssd1351-ugfx-config/ssd1351_pins.cpp
  or adapt ssd1351_pins.cpp to suit your needs
*/

#include <example-ssd1351-ugfx-config.h>
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

