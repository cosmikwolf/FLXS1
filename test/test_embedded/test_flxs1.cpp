#include "Arduino.h"
#include <unity.h>
#include <globalVariable.h>
// #include <ADC.h>

GlobalVariable globalObj;
// ADC *adc = new ADC(); // adc object

// test values generated with this google sheet: https://docs.google.com/spreadsheets/d/1RMNEjRl1BBNpSV7_Q8sHFQ88lOWaSeANp2Z0QyocYu8/edit#gid=0

void test_function_convert_edo_scale_degree_to_dac_code(void)
{
    //uint16_t GlobalVariable::quantize_edo_pitch(uint16_t note, uint16_t quantizeKey, uint32_t quantize_mode, uint8_t scale_divisions, bool direction)
    TEST_ASSERT_EQUAL(12012, globalObj.convert_edo_scale_degree_to_dac_code(22, 12));
    TEST_ASSERT_EQUAL(18564, globalObj.convert_edo_scale_degree_to_dac_code(34, 12));
    TEST_ASSERT_EQUAL(30576, globalObj.convert_edo_scale_degree_to_dac_code(294, 63));
    TEST_ASSERT_EQUAL(47424, globalObj.convert_edo_scale_degree_to_dac_code(456, 63));
}

void test_quantize_edo_dac_code_to_scale_degree(void)
{
    TEST_ASSERT_EQUAL(29, globalObj.quantize_edo_dac_code_to_scale_degree(14742, 13));
    TEST_ASSERT_EQUAL(40, globalObj.quantize_edo_dac_code_to_scale_degree(17472, 15));
    TEST_ASSERT_EQUAL(294, globalObj.quantize_edo_dac_code_to_scale_degree(30576, 63));
}

void test_quantize_edo_scale_degree_to_key(void)
{

    uint64_t test_key = 0b01;
    uint8_t test_edo_divs = 7;
    uint16_t max_notes = 99;
    uint16_t scale_deg;
    for (int i = 0; i < max_notes; i++)
    {
        scale_deg = globalObj.quantize_edo_scale_degree_to_key(i, 1, test_key, test_edo_divs, 0);
        Serial.printf("%03d --> %03d --> %05d\n", i, scale_deg, globalObj.convert_edo_scale_degree_to_dac_code(scale_deg, test_edo_divs));
    }
    TEST_ASSERT_EQUAL(0, globalObj.quantize_edo_scale_degree_to_key(17, 1, test_key, 7, 0));
    for (int i = 0; i < test_edo_divs; i++)
    {
        Serial.printf("%d\n", globalObj.quantize_edo_scale_degree_to_key(i, 1, test_key, test_edo_divs, 0));
    }
}

// int main(int argc, char **argv)
void setup()
{

    delay(2000);
    while (!Serial)
    {
    }

    // globalObj.initialize(adc);

    UNITY_BEGIN();
    RUN_TEST(test_function_convert_edo_scale_degree_to_dac_code);
    RUN_TEST(test_quantize_edo_dac_code_to_scale_degree);
    RUN_TEST(test_quantize_edo_scale_degree_to_key);
    UNITY_END();
}

void loop()
{
}