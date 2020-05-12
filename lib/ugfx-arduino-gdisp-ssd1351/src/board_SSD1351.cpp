//#include <Arduino.h>
#include <SPI.h>
//#include <Wire.h>

#include "board_SSD1351.h"

// GPIO Pins
#include "ugfx-arduino-gdisp-ssd1351.h"
//#include "ugfx-arduino-ssd1351-pins.h"

#define GPIO_RESET ssd1351_pins.reset
#define GPIO_CS ssd1351_pins.cs
#define GPIO_DC ssd1351_pins.dc

#define SPILOWSPEED 1000000
#define SPIHIGHSPEED 12000000
#define SPIBITORDER MSBFIRST
#define SPIMODE SPI_MODE0

static SPISettings spiSettings(SPILOWSPEED, SPIBITORDER, SPIMODE);

static inline void cmdmode()
{
  digitalWrite(GPIO_DC, 0);
}

static inline void datamode()
{
  digitalWrite(GPIO_DC, 1);
}

void SSD1351_init_board(void) {
  Serial.println("init board");
  pinMode(GPIO_RESET, OUTPUT);
  pinMode(GPIO_RESET, 1);
  pinMode(GPIO_CS, OUTPUT);
  digitalWrite(GPIO_CS, 1);
  pinMode(GPIO_DC, OUTPUT);
  datamode();
}

void SSD1351_post_init_board(void) {
  // speed up SPI (12 MHz)
  spiSettings = SPISettings(SPIHIGHSPEED, SPIBITORDER, SPIMODE);
}

void SSD1351_setpin_reset(int state) {
  if(state)
    digitalWrite(GPIO_RESET, 0);
  else
    digitalWrite(GPIO_RESET, 1);
}

void SSD1351_aquirebus(void) {
  SPI.beginTransaction(spiSettings);
  digitalWrite(GPIO_CS, 0);
}

void SSD1351_releasebus(void) {
  digitalWrite(GPIO_CS, 1);
  SPI.endTransaction();
}

void SSD1351_write_cmd(uint8_t cmd) {
  cmdmode();
  SPI.transfer(cmd);
  datamode();
}

void SSD1351_write_data(uint8_t data) {
  SPI.transfer(data);
}
