#include <SPI.h>
#include <Zetaohm_MAX7301.h>   // import library

#define S00    0
#define S01    1
#define S02    2
#define S03    3
#define S04    4
#define S05    5
#define S06    6
#define S07    7
#define S08    8
#define S09    9
#define S10    10
#define S11    11
#define S12    12
#define S13    13
#define S14    14
#define S15    15
#define SPLAY  16
#define SPAUSE 17
#define SSTOP  18
#define SM0    19
#define SM1    20
#define SM2    21
#define SM3    22
#define SPGDN  23
#define SPGUP  24
#define SMENU  25
#define SALT   26

max7301 max7301(5);

void setup(){
  Serial.begin(115200);
  delay(1000);

  Serial.println("Beginning button test");
  delay(100);
  max7301.begin(0);
  delay(1);
  max7301.gpioPinMode(INPUT_PULLUP);

  max7301.init( 0,    14);
  max7301.init( 1,    7);
  max7301.init( 2,    5);
  max7301.init( 3,    26);
  max7301.init( 4,    15);
  max7301.init( 5,    11);
  max7301.init( 6,    9);
  max7301.init( 7,    1);
  max7301.init( 8,    16);
  max7301.init( 9,    12);
  max7301.init( 10,   6);
  max7301.init( 11,   27);
  max7301.init( 12,   17);
  max7301.init( 13,   13);
  max7301.init( 14,   10);
  max7301.init( 15,   0);
  max7301.init( 16,   8);
  max7301.init( 17,   4);
  max7301.init( 18,   2);
  max7301.init( 19,   21);
  max7301.init( 20,   20);
  max7301.init( 21,   19);
  max7301.init( 22,   18);
  max7301.init( 23,   3);
  max7301.init( 24,   25);
  max7301.init( 25,   24);
  max7301.init( 26,   23);

  Serial.println("Setup Complete, loop begins");
  delay(100);
}

void loop(){
  max7301.update();
  delay(10);
  for (int i= 0; i < 27; i++){
    if (max7301.fell(i) ){
      Serial.println("BUTTON " + String(i) + " FELL");
    }
    if (max7301.rose(i) ){
      Serial.println("BUTTON " + String(i) + " ROSE");
    }

  }
}
