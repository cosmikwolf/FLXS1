#include <OLED_SSD1351.h>
#include <SPI.h>


#define LCD_DC        10//out (SSD1351/sharp refresh todo) OK
#define LCD_CS        6//out (SSD1351/sharp) OK
#define LCD_RST       9//out (SSD1351)/sharp disp todo) OK

OLED_SSD1351 oled = OLED_SSD1351(LCD_CS, LCD_DC, LCD_RST);

float p = 3.1415926;

elapsedMillis timer;

void setup(void) {
  Serial.begin(38400);
  long unsigned debug_start = millis ();
  while (!Serial && ((millis () - debug_start) <= 5000)) ;
  Serial.println("start");
  oled.begin();

  //oled.setRotation(2);

  uint16_t time = millis();
  oled.fillRect(0, 0, oled.width(), oled.height(), BLACK);
  time = millis() - time;

  Serial.println("Width: " + String(oled.width()));
  Serial.println("Height: " + String(oled.height()));
  Serial.println(time, DEC);
  delay(500);

  fillpixelbypixel();
  delay(500);
  oled.clearScreen();

  lcdTestPattern();
  delay(500);

  oled.invert(true);
  delay(100);
  oled.invert(false);
  delay(100);

  oled.clearScreen();
  testdrawtext("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", WHITE);
  delay(500);

  // oled print function!
  oledPrintTest();
  delay(500);

  //a single pixel
  oled.drawPixel(oled.width() / 2, oled.height() / 2, GREEN);
  delay(500);

  // line draw test
  testlines(YELLOW);
  delay(500);

  // optimized lines
  testfastlines(RED, BLUE);
  delay(500);


  testdrawrects(GREEN);
  delay(1000);

  testfillrects(YELLOW, MAGENTA);
  delay(1000);

  oled.clearScreen();
  testfillcircles(10, BLUE);
  testdrawcircles(10, WHITE);
  delay(1000);

  testroundrects();
  delay(500);

  testtriangles();
  delay(500);

  Serial.println("done");
  delay(1000);
}

void loop() {

  testdrawtext("TESTING", WHITE);
  delay(300);
  timer = 0;

  for(int i=0; i<1000; i++){
    oled.fillScreen(BLACK);
    oled.fillScreen(RED);
    oled.fillScreen(GREEN);
    oled.fillScreen(BLUE);
  }
  oled.clearScreen();
  testdrawtext( String(timer).c_str(), WHITE);
  delay(10000);
}

void fillColor(uint16_t color) {
  for (int16_t x = 0; x < oled.width(); x++) {
    for (int16_t y = 0; y < oled.height(); y++) {
      oled.drawPixel(x, y, color);
    }
  }
}

void fillpixelbypixel() {
  for (int16_t x = 0; x < oled.width(); x++) {
    for (int16_t y = 0; y < oled.height(); y++) {
      uint16_t color = RED;
      oled.drawPixel(x, y, color);
      color += 100;
    }
  }
  delay(100);
}

void testlines(uint16_t color) {
  oled.clearScreen();
  for (uint16_t x = 0; x < oled.width() - 1; x += 6) {
    oled.drawLine(0, 0, x, oled.height() - 1, color);
  }
  for (uint16_t y = 0; y < oled.height() - 1; y += 6) {
    oled.drawLine(0, 0, oled.width() - 1, y, color);
  }

  oled.clearScreen();
  for (uint16_t x = 0; x < oled.width() - 1; x += 6) {
    oled.drawLine(oled.width() - 1, 0, x, oled.height() - 1, color);
  }
  for (uint16_t y = 0; y < oled.height() - 1; y += 6) {
    oled.drawLine(oled.width() - 1, 0, 0, y, color);
  }

  oled.clearScreen();
  for (uint16_t x = 0; x < oled.width() - 1; x += 6) {
    oled.drawLine(0, oled.height() - 1, x, 0, color);
  }
  for (uint16_t y = 0; y < oled.height() - 1; y += 6) {
    oled.drawLine(0, oled.height() - 1, oled.width() - 1, y, color);
  }

  oled.clearScreen();
  for (uint16_t x = 0; x < oled.width() - 1; x += 6) {
    oled.drawLine(oled.width() - 1, oled.height() - 1, x, 0, color);
  }
  for (uint16_t y = 0; y < oled.height() - 1; y += 6) {
    oled.drawLine(oled.width() - 1, oled.height() - 1, 0, y, color);
  }

}

void testdrawtext(const char *text, uint16_t color) {
  oled.setCursor(0, 0);
  oled.setTextColor(color);
  oled.print(text);
}

void testfastlines(uint16_t color1, uint16_t color2) {
  oled.clearScreen();
  for (uint16_t y = 0; y < oled.height() - 1; y += 5) {
    oled.drawFastHLine(0, y, oled.width() - 1, color1);
  }
  for (uint16_t x = 0; x < oled.width() - 1; x += 5) {
    oled.drawFastVLine(x, 0, oled.height() - 1, color2);
  }
}

void testdrawrects(uint16_t color) {
  oled.clearScreen();
  for (uint16_t x = 0; x < oled.height() - 1; x += 6) {
    oled.drawRect((oled.width() - 1) / 2 - x / 2, (oled.height() - 1) / 2 - x / 2 , x, x, color);
  }
}

void testfillrects(uint16_t color1, uint16_t color2) {
  oled.clearScreen();
  for (uint16_t x = oled.height() - 1; x > 6; x -= 6) {
    oled.fillRect((oled.width() - 1) / 2 - x / 2, (oled.height() - 1) / 2 - x / 2 , x, x, color1);
    oled.drawRect((oled.width() - 1) / 2 - x / 2, (oled.height() - 1) / 2 - x / 2 , x, x, color2);
  }
}

void testfillcircles(uint8_t radius, uint16_t color) {
  for (int16_t x = radius; x < oled.width() - 1; x += radius * 2) {
    for (int16_t y = radius; y < oled.height() - 1; y += radius * 2) {
      oled.fillCircle(x, y, radius, color);
    }
  }
}

void testdrawcircles(uint8_t radius, uint16_t color) {
  for (int16_t x = 0; x < oled.width() - 1 + radius; x += radius * 2) {
    for (int16_t y = 0; y < oled.height() - 1 + radius; y += radius * 2) {
      oled.drawCircle(x, y, radius, color);
    }
  }
}

void testtriangles() {
 oled.clearScreen();
  uint16_t color = 0xF800;
  int t;
  int16_t w = oled.width() / 2;
  int16_t x = oled.height();
  int16_t y = 0;
  int16_t z = oled.width();
  for (t = 0 ; t <= 15; t += 1) {
    oled.drawTriangle(w, y, y, x, z, x, color);
    x -= 4;
    y += 4;
    z -= 4;
    color += 100;
  }
}

void testroundrects() {
  oled.clearScreen();
  uint16_t color = 100;

  int16_t x = 0;
  int16_t y = 0;
  int16_t w = oled.width();
  int16_t h = oled.height();
  for (int16_t i = 0 ; i <= 24; i++) {
    oled.drawRoundRect(x, y, w, h, 5, color);
    x += 2;
    y += 3;
    w -= 4;
    h -= 6;
    color += 1100;
    Serial.println(i);
  }
}

void oledPrintTest() {
  oled.clearScreen();
  oled.setCursor(0, 5);
  oled.setTextColor(RED);
  oled.setTextSize(1);
  oled.println("Hello World!");
  oled.setTextColor(YELLOW);
  oled.setTextSize(2);
  oled.println("Hello World!");
  oled.setTextColor(BLUE);
  oled.setTextSize(3);
  oled.print(1234.567);
  delay(1500);
  oled.setCursor(0, 5);
  oled.clearScreen();
  oled.setTextColor(WHITE);
  oled.setTextSize(0);
  oled.println("Hello World!");
  oled.setTextSize(1);
  oled.setTextColor(GREEN);
  oled.print(p, 6);
  oled.println(" Want pi?");
  oled.println(" ");
  oled.print(8675309, HEX); // print 8,675,309 out in HEX!
  oled.println(" Print HEX!");
  oled.println(" ");
  oled.setTextColor(WHITE);
  oled.println("Sketch has been");
  oled.println("running for: ");
  oled.setTextColor(MAGENTA);
  oled.print(millis() / 1000);
  oled.setTextColor(WHITE);
  oled.print(" seconds.");
}

void mediabuttons() {
  // play
  oled.clearScreen();
  oled.fillRoundRect(25, 10, 78, 60, 8, WHITE);
  oled.fillTriangle(42, 20, 42, 60, 90, 40, RED);
  delay(500);
  // pause
  oled.fillRoundRect(25, 90, 78, 60, 8, WHITE);
  oled.fillRoundRect(39, 98, 20, 45, 5, GREEN);
  oled.fillRoundRect(69, 98, 20, 45, 5, GREEN);
  delay(500);
  // play color
  oled.fillTriangle(42, 20, 42, 60, 90, 40, BLUE);
  delay(50);
  // pause color
  oled.fillRoundRect(39, 98, 20, 45, 5, RED);
  oled.fillRoundRect(69, 98, 20, 45, 5, RED);
  // play color
  oled.fillTriangle(42, 20, 42, 60, 90, 40, GREEN);
}

/**************************************************************************/
/*!
    @brief  Renders a simple test pattern on the LCD
*/
/**************************************************************************/
void lcdTestPattern(void)
{
  int16_t i, j;
  oled.goTo(0, 0);

  for (i = 0; i < 128; i++) {
    for (j = 0; j < 128; j++) {
      if (i < 16) {
        oled.sendData(RED);
      } else if (i < 32) {
        oled.sendData(YELLOW);
      } else if (i < 48) {
        oled.sendData(GREEN);
      } else if (i < 64) {
        oled.sendData(CYAN);
      } else if (i < 80) {
        oled.sendData(BLUE);
      } else if (i < 96) {
        oled.sendData(MAGENTA);
      } else if (i < 112) {
        oled.sendData(BLACK);
      } else {
        oled.sendData(WHITE);
      }
    }
  }
}