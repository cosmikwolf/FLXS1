#include <Arduino.h>
#include <SPI.h>
#include "OLED_SSD1351/OLED_SSD1351.h"
#include "Sequencer.h"
#include "global.h"

#ifndef _display_h_
#define _display_h_

/* in addition to:
  BLACK  BLUE  RED ORANGE  GREEN CYAN MAGENTA YELLOW WHITE
  more colors available at: http://www.rinkydinkelectronics.com/calc_rgb565.php
  */
#define NAVY            0x000F      /*   0,   0, 128 */
#define DARKGREEN       0x03E0      /*   0, 128,   0 */
#define DARKCYAN        0x03EF      /*   0, 128, 128 */
#define MAROON          0x7800      /* 128,   0,   0 */
#define PURPLE          0x780F      /* 128,   0, 128 */
#define OLIVE           0x7BE0      /* 128, 128,   0 */
#define LIGHTGREY       0xC618      /* 192, 192, 192 */
#define DARKGREY        0x7BEF      /* 128, 128, 128 */
#define ORANGE          0xFD20      /* 255, 165,   0 */
#define GREENYELLOW     0xAFE5      /* 173, 255,  47 */
#define PINK            0xF81F

#define MAX_DISPLAY_ELEMENTS 17

#define LCD_DC        10//out (SSD1351/sharp refresh todo) OK
#define LCD_CS        6//out (SSD1351/sharp) OK
#define LCD_RST       9//out (SSD1351)/sharp disp todo) OK

class DisplayModule
{
  public:
    DisplayModule();

    int color = 0;
    uint32_t runcount;

    char *displayCache[MAX_DISPLAY_ELEMENTS];
    char *displayElement[MAX_DISPLAY_ELEMENTS];
    char *buf = new char[51]; // sprintf buffer

    uint8_t highlight;
    uint8_t previouslySelectedChannel;
    OLED_SSD1351 oled = OLED_SSD1351(LCD_CS, LCD_DC, LCD_RST);

    elapsedMicros displayTimer;
    void initialize(Sequencer (*sequenceArray)[4]);
    void displayLoop();

    void clearDisplay();
    void freeDisplayCache();

    void stepDisplay(char *buf);
    void patternSelectDisplay();
    void channelMenuDisplay(char *buf) ;

    void globalMenuDisplay();
    void gameOfLifeDisplay();
    void deleteMenuDisplay();
    void cleanupTextBuffers();
    void sequenceMenuDisplay();
    void instrumentSelectDisplay();
    void timingMenuDisplay();

    void renderOnce_StringBox(uint8_t index, uint8_t highlight, uint8_t previousHighlight, int16_t x, int16_t y, int16_t w, int16_t h, bool border, uint8_t textSize, uint16_t color, uint16_t bgColor) ;
private:

    Sequencer (*sequenceArray)[4];


    const char* midiNotes[128] = {
    "C -2","C#-2","D -2","D#-2","E -2","F -2","F#-2","G -2","G#-2","A -2","A#-2","B -2",
    "C -1","C#-1","D -1","D#-1","E -1","F -1","F#-1","G -1","G#-1","A -1","A#-1","B -1",
    "C  0","C# 0","D  0","D# 0","E  0","F  0","F# 0","G  0","G# 0","A  0","A# 0","B  0",
    "C  1","C# 1","D  1","D# 1","E  1","F  1","F# 1","G  1","G# 1","A  1","A# 1","B  1",
    "C  2","C# 2","D  2","D# 2","E  2","F  2","F# 2","G  2","G# 2","A  2","A# 2","B  2",
    "C  3","C# 3","D  3","D# 3","E  3","F  3","F# 3","G  3","G# 3","A  3","A# 3","B  3",
    "C  4","C# 4","D  4","D# 4","E  4","F  4","F# 4","G  4","G# 4","A  4","A# 4","B  4",
    "C  5","C# 5","D  5","D# 5","E  5","F  5","F# 5","G  5","G# 5","A  5","A# 5","B  5",
    "C  6","C# 6","D  6","D# 6","E  6","F  6","F# 6","G  6","G# 6","A  6","A# 6","B  6",
    "C  7","C# 7","D  7","D# 7","E  7","F  7","F# 7","G  7","G# 7","A  7","A# 7","B  7",
    "C  8","C# 8","D  8","D# 8","E  8","F  8","F# 8","G  8" };

    const char* instrumentNames[128] = { "Grand Piano  ", "Bright Piano ", "Electric Grnd", "HnkytonkPiano", "El. Piano 1", "El. Piano 2", "Harpsichord", "Clavi", "Celesta", "Glockenspiel", "Music Box", "Vibraphone", "Marimba", "Xylophone", "Tubular Bells", "Santur", "Drawbar Organ", "Percussive Organ", "Rock Organ", "Church Organ", "Reed Organ", "Accordion (French)", "Harmonica", "Tango Accordion", "Acoustic Guitar (nylon)", "Acoustic Guitar (steel)", "El. Guitar (jazz)", "El. Guitar (clean)", "El. Guitar (muted)", "Overdriven Guitar", "Distortion Guitar", "Guitar Harmonics", "Acoustic Bass", "Finger Bass", "Picked Bass", "Fretless Bass", "Slap Bass 1", "Slap Bass 2", "Synth Bass 1", "Synth Bass 2", "Violin", "Viola", "Cello", "Contrabass", "Tremolo Strings", "Pizzicato Strings", "Orchestral Harp", "Timpani", "String Ensemble 1", "String Ensemble 2", "Synth Strings 1", "Synth Strings 2", "Choir Aahs", "Voice Oohs", "Synth Voice", "Orchestra Hit", "Trumpet", "Trombone", "Tuba", "Muted Trumpet", "French Horn", "Brass Section", "Synth Brass 1", "Synth Brass 2", "Soprano Sax", "Alto Sax", "Tenor Sax", "Baritone Sax", "Oboe", "English Horn", "Bassoon", "Clarinet", "Piccolo", "Flute", "Recorder", "Pan Flute", "Blown Bottle", "Shakuhachi", "Whistle", "Ocarina", "Lead 1 (square)", "Lead 2 (sawtooth)", "Lead 3 (calliope)", "Lead 4 (chiff)", "Lead 5 (charang)", "Lead 6 (voice)", "Lead 7 (fifths)", "Lead 8 (bass+lead)", "Pad 1 (fantasia)", "Pad 2 (warm)", "Pad 3 (polysynth)", "Pad 4 (choir)", "Pad 5 (bowed)", "Pad 6 (metallic)", "Pad 7 (halo)", "Pad 8 (sweep)", "FX 1 (rain)", "FX 2 (soundtrack)", "FX 3 (crystal)", "FX4 (atmosphere)", "FX 5 (brightness)", "FX 6 (goblins)", "FX 7 (echoes)", "FX 8 (sci-fi)", "Sitar", "Banjo", "Shamisen", "Koto", "Kalimba", "Bagpipe", "Fiddle", "Shanai", "Tinkle Bell", "Agogo", "Steel Drums", "Woodblock", "Taiko Drum", "Melodic Tom", "Synth Drum", "Reverse Cymbal", "Guitar Fret Noise", "Breath Noise", "Seashore", "Bird Tweet", "Teleph. Ring", "Helicopter", "Applause", "Gunshot"};

};


#endif
