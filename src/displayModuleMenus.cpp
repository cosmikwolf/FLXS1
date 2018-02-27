#include <Arduino.h>
#include "DisplayModule.h"


void DisplayModule::shortcutRandomMenu(){
      displayElement[0] = strdup("random menu");
      renderStringBox(0,  DISPLAY_LABEL,  0,  0, 86, 16, false, STYLE1X, background, contrastColor ); //  digitalWriteFast(PIN_EXT_RX, LOW);
};
