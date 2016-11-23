#include <Audio.h>
#include <SPI.h>
#include <Wire.h>

#define kSerialSpeed 115200

AudioInputAnalog              adc(A6);
AudioAnalyzeNoteFrequency     notefreq;

AudioConnection               patchCord1(adc, 0, notefreq, 0);

void setup() {
  AudioMemory(30);
  notefreq.begin(.7);
  Serial.begin(kSerialSpeed);
}

void loop() {

  if (notefreq.available()) {
    float note = notefreq.read();
    float prob = notefreq.probability();
    Serial.printf("Note: %3.2f | Probability: %.2f\n", note, prob);
  }

}
