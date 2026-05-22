#include <unity.h>
#include "utils.h"

// Reference CRC-8 (polynomial 0x07, init 0x00, xorOut 0x00, refIn=false, refOut=false)
static uint8_t crc8_ref(const uint8_t* data, size_t len) {
    const uint8_t poly = 0x07;
    uint8_t crc = 0x00;
    for (size_t i = 0; i < len; ++i) {
        crc ^= data[i];
        for (int b = 0; b < 8; ++b) {
            if (crc & 0x80) {
                crc = (uint8_t)((crc << 1) ^ poly);
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

void test_crc8_matches_reference_for_123456789() {
    const char* s = "123456789"; // well-known check, expected 0xF4
    const uint8_t* bytes = (const uint8_t*)s;
    uint8_t got = calculateCRC8((uint8_t*)bytes, 9);
    uint8_t ref = crc8_ref(bytes, 9);
    TEST_ASSERT_EQUAL_HEX8(ref, got);
}

void test_crc8_zero_length_is_zero() {
    uint8_t got = calculateCRC8(nullptr, 0);
    uint8_t ref = crc8_ref(nullptr, 0);
    TEST_ASSERT_EQUAL_HEX8(ref, got);
}

void test_crc8_all_zeros_buffer() {
    uint8_t buf[16] = {0};
    uint8_t got = calculateCRC8(buf, sizeof(buf));
    uint8_t ref = crc8_ref(buf, sizeof(buf));
    TEST_ASSERT_EQUAL_HEX8(ref, got);
}

void test_crc8_varied_pattern() {
    uint8_t buf[] = {0x00, 0xFF, 0x55, 0xAA, 0x10, 0x20, 0x30, 0x40};
    uint8_t got = calculateCRC8(buf, sizeof(buf));
    uint8_t ref = crc8_ref(buf, sizeof(buf));
    TEST_ASSERT_EQUAL_HEX8(ref, got);
}

void test_hexCharToInt_valid_digits() {
    TEST_ASSERT_EQUAL_INT(0, hexCharToInt('0'));
    TEST_ASSERT_EQUAL_INT(9, hexCharToInt('9'));
}

void test_hexCharToInt_valid_upper_hex() {
    TEST_ASSERT_EQUAL_INT(10, hexCharToInt('A'));
    TEST_ASSERT_EQUAL_INT(15, hexCharToInt('F'));
}

void test_hexCharToInt_valid_lower_hex() {
    TEST_ASSERT_EQUAL_INT(10, hexCharToInt('a'));
    TEST_ASSERT_EQUAL_INT(15, hexCharToInt('f'));
}

void test_hexCharToInt_invalid_chars() {
    TEST_ASSERT_EQUAL_INT(-1, hexCharToInt('G'));
    TEST_ASSERT_EQUAL_INT(-1, hexCharToInt('g'));
    TEST_ASSERT_EQUAL_INT(-1, hexCharToInt(':'));
    TEST_ASSERT_EQUAL_INT(-1, hexCharToInt('/'));
}

void test_crc8_0_to_255_pattern() {
    uint8_t buf[256];
    for (size_t i = 0; i < sizeof(buf); ++i) buf[i] = (uint8_t)i;
    uint8_t got = calculateCRC8(buf, sizeof(buf));
    uint8_t ref = crc8_ref(buf, sizeof(buf));
    TEST_ASSERT_EQUAL_HEX8(ref, got);
}

void test_crc8_all_ff_buffer() {
    uint8_t buf[32];
    for (size_t i = 0; i < sizeof(buf); ++i) buf[i] = 0xFF;
    uint8_t got = calculateCRC8(buf, sizeof(buf));
    uint8_t ref = crc8_ref(buf, sizeof(buf));
    TEST_ASSERT_EQUAL_HEX8(ref, got);
}

void test_crc8_repeatability_consistency() {
    uint8_t buf[] = {0x01, 0x02, 0x03, 0x04, 0xFF, 0x00, 0x7E, 0x81};
    uint8_t got1 = calculateCRC8(buf, sizeof(buf));
    uint8_t got2 = calculateCRC8(buf, sizeof(buf));
    uint8_t ref = crc8_ref(buf, sizeof(buf));
    TEST_ASSERT_EQUAL_HEX8(ref, got1);
    TEST_ASSERT_EQUAL_HEX8(ref, got2);
}

void test_crc8_alternating_aa55() {
    const size_t N = 64;
    uint8_t buf[N];
    for (size_t i = 0; i < N; ++i) buf[i] = (i % 2 == 0) ? 0xAA : 0x55;
    uint8_t got = calculateCRC8(buf, N);
    uint8_t ref = crc8_ref(buf, N);
    TEST_ASSERT_EQUAL_HEX8(ref, got);
}

void test_hexCharToInt_additional_invalid_cases() {
    TEST_ASSERT_EQUAL_INT(-1, hexCharToInt('x'));
    TEST_ASSERT_EQUAL_INT(-1, hexCharToInt('Z'));
    TEST_ASSERT_EQUAL_INT(-1, hexCharToInt('z'));
    TEST_ASSERT_EQUAL_INT(-1, hexCharToInt(' '));
    TEST_ASSERT_EQUAL_INT(-1, hexCharToInt('@'));
    TEST_ASSERT_EQUAL_INT(-1, hexCharToInt('`'));
}

extern "C" void setUp(void) {}
extern "C" void tearDown(void) {}

#ifdef ARDUINO
#include <Arduino.h>
void setup() {
    delay(2000); // allow board to startup in case of HW run
    UNITY_BEGIN();
    RUN_TEST(test_crc8_matches_reference_for_123456789);
    RUN_TEST(test_crc8_zero_length_is_zero);
    RUN_TEST(test_crc8_all_zeros_buffer);
    RUN_TEST(test_crc8_varied_pattern);
    RUN_TEST(test_hexCharToInt_valid_digits);
    RUN_TEST(test_hexCharToInt_valid_upper_hex);
    RUN_TEST(test_hexCharToInt_valid_lower_hex);
    RUN_TEST(test_hexCharToInt_invalid_chars);
    RUN_TEST(test_crc8_0_to_255_pattern);
    RUN_TEST(test_crc8_all_ff_buffer);
    RUN_TEST(test_crc8_repeatability_consistency);
    RUN_TEST(test_crc8_alternating_aa55);
    RUN_TEST(test_hexCharToInt_additional_invalid_cases);
    UNITY_END();
}
void loop() {}
#else
int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_crc8_matches_reference_for_123456789);
    RUN_TEST(test_crc8_zero_length_is_zero);
    RUN_TEST(test_crc8_all_zeros_buffer);
    RUN_TEST(test_crc8_varied_pattern);
    RUN_TEST(test_hexCharToInt_valid_digits);
    RUN_TEST(test_hexCharToInt_valid_upper_hex);
    RUN_TEST(test_hexCharToInt_valid_lower_hex);
    RUN_TEST(test_hexCharToInt_invalid_chars);
    RUN_TEST(test_crc8_0_to_255_pattern);
    RUN_TEST(test_crc8_all_ff_buffer);
    RUN_TEST(test_crc8_repeatability_consistency);
    RUN_TEST(test_crc8_alternating_aa55);
    RUN_TEST(test_hexCharToInt_additional_invalid_cases);
    return UNITY_END();
}
#endif
