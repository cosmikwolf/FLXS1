#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 5

#define NUM_LEDS 23

#define BRIGHTNESS 255

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRBW + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code
  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  // Some example procedures showing how to display to the pixels:
  colorWipe(strip.Color(255, 0, 0), 0); // Red
delay(3000);
  colorWipe(strip.Color(0, 255, 0), 0); // Green
delay(3000);
  colorWipe(strip.Color(0, 0, 255), 0); // Blue
delay(3000);
  colorWipe(strip.Color(0, 0, 0, 255), 0); // White
  delay(3000);

for(int i=0; i<10; i++){
    colorWipe(strip.Color(255, 0, 0), 0); // Red
  colorWipe(strip.Color(0, 255, 0), 0); // Green
  colorWipe(strip.Color(0, 0, 255), 0); // Blue
  colorWipe(strip.Color(0, 0, 0, 255), 0); // White  
}

}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

