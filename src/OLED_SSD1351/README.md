# OLED_SSD1351
A fast library for OLED drived by SSD1351<br>
Early alpha version, perfectly working with Teensy 3.1 but NOT with avr's or other arms (wait beta).<br>
SSD1351 works in SPI mode 3!!!! This mean it will be not so nice if other devices are connected in the same SPI bus, I'm investigating some simple workaround so stay tuned.<br>

Using Paul Stoffregen's Font Library<br>
Copy https://github.com/sumotoy/commonFonts into your library folder (NOT inside this library!).<br>
Check extFont example.

Current Alpha Version Limitations:<br>
- setRotation works but not checked so errors can exists.
- code it's not fully optimized
- ONLY Teensy 3.x support for alpha until beta stage.
