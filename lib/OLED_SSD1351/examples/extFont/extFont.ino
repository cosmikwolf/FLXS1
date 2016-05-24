
#include <SPI.h>
#include <OLED_SSD1351.h>
#include "../commonFonts/fonts/font_Arial.c"

//Uncomment this block to use hardware SPI
#define OLED_DC     21
#define OLED_CS     10
#define OLED_RESET  6

OLED_SSD1351 oled(OLED_CS, OLED_DC, OLED_RESET);

void setup()   {
  Serial.begin(38400);
  long unsigned debug_start = millis ();
  while (!Serial && ((millis () - debug_start) <= 5000)) ;
  Serial.println("start");
  oled.begin();
  if (oled.getError() != 0) {
    Serial.println(oled.getError());
  }
  oled.setFont(Arial_12);
  oled.print("SUMOTOY");
}


void loop() {

}