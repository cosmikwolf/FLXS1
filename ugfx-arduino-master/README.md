# ugfx-arduino
How to use ugfx in arduino. Libraries and example sketch

hackaday.io project: https://hackaday.io/project/8539-using-ugfx-in-arduino

It is assumed that you are familiar with uGFX or have some experience in the arduino environment, including writing and using custom libraries.

## Installation
Well, sort of:
- this library should be put into arduino's library folder. If you have downloaded it as a .zip file, rename the contained main folder from ugfx-arduino-master to ugfx-arduino.
- get uGFX from http://ugfx.org/ (Downloads section, I've tested the git repo state as of 2015-11-24. Version 2.4 is too old as it doesn't contain a fix required for arduino) and put it into arduino's library directory. You should end up with _arduino libraries directory_/ugfx/src and other files and folders. The main include file (gfx.h) should be at _arduino libraries directory_/ugfx/gfx.h .
- Copy the libraries from this repo's library directory to arduino's library directory where all other user libraries are.
- The example sketch goes into your sketchbook, or some other place. It doesn't really matter.

You should then be able to compile the example sketch. If you have an arduino-compatible board and an SSD1351 display, it should display some demo text and graphics.


## Repo Structure and Usage
The main folder is a library consisting of ugfx-arduino/ugfx-arduino.h and ugfx-arduino/ugfx-arduino.c

**examples/example/example-ssd1351** is an example sketch. Copy this to your sketchbook.

**libraries/example-ssd1351-ugfx-config** is an example config library for the sketch above. Copy this to your arduino libraries folder.

**libraries/ugfx-arduino-gdisp-ssd1351** is an example driver library included as an external repo. It's also availabe at https://github.com/crteensy/ugfx-arduino-gdisp-ssd1351. It has been tested with the included example sketch and config library on a Teensy 3.1 with adafruit's SSD1351 OLED display. You can use this as a template for your own drivers or come up with something smarter - I'm sure the structure can be improved.

**templates/project** is a template sketch very similar to the example sketch. Usage instructions are at the top of the .ino file.

**templates/project-ugfx-config** is a template ugfx config library.

You have to create a config library for each project you want to ugfx-arduino in. Sorry, I have not yet found a workaround for that!
