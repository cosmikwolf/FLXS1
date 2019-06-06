// #include <Arduino.h>
// #include <unity.h>

// using namespace fakeit;
// #include <Audio.h>
// #include <Wire.h>
// #include <SPI.h>
// #include <malloc.h>
// #include <ADC.h>
// #include "TimeController.h"

// #include "OutputController.h"
// #include "Sequencer.h"
// #include "midiModule.h"
// #include "global.h"
// #include "DisplayModule.h"
#include "globalVariable.h"
#include <unity.h>

// uint16_t GlobalVariable::quantize_edo_pitch(uint16_t note, uint16_t quantizeKey, uint32_t quantize_mode, uint8_t scale_divisions, bool direction);

// GlobalVariable globalObj;

void test_function_quantize_edo_pitch(void)
{
    TEST_ASSERT_EQUAL(32, 32);
    // globalObj.quantize_edo_pitch()
}

int main(int argc, char **argv)
{
    Serial.begin(115200);
    delay(100);
    Serial.println("_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_ TESTING _+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_");
    Serial.println("_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_ TESTING _+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_");
    Serial.println("_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_ TESTING _+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_");
    Serial.println("_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_ TESTING _+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_");
    Serial.println("_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_ TESTING _+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_");
    Serial.println("_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_ TESTING _+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_");
    UNITY_BEGIN();
    RUN_TEST(test_function_quantize_edo_pitch);
    UNITY_END();
    Serial.println("_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_ COMPLETE _+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_");
    Serial.println("_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_ COMPLETE _+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_");
    Serial.println("_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_ COMPLETE _+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_");
    Serial.println("_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_ COMPLETE _+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_");
    Serial.println("_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_ COMPLETE _+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_");
    Serial.println("_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_ COMPLETE _+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_");

    return 0;
}