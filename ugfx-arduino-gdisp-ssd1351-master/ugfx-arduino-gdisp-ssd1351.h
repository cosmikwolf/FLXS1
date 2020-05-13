#ifndef UGFX_ARDUINO_SSD1351_H
#define UGFX_ARDUINO_SSD1351_H

#include <stdint.h>

typedef struct {
  const uint8_t reset;
  const uint8_t cs;
  const uint8_t dc;
} ssd1351_pins_t;

extern const ssd1351_pins_t ssd1351_pins;

#endif // UGFX_ARDUINO_SSD1351_H
