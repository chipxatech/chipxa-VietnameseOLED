/**
 * @file test_utf8_decoder.cpp
 * @brief Unit tests for UTF8Decoder class.
 *
 * @author VietnameseOLED Contributors
 * @version 1.0.0
 * @copyright MIT License
 */

#include "UTF8Decoder.h"
#include "test_helpers.h"

void test_utf8_decoder() {
    // Test Case 1: ASCII character 'A' (0x41)
    {
        UTF8Decoder decoder;
        UTF8Decoder::Status status = decoder.decode(0x41);
        TEST_ASSERT_EQ(static_cast<int>(status), static_cast<int>(UTF8Decoder::Status::ACCEPT));
        TEST_ASSERT_EQ(decoder.getCodepoint(), 0x41u);
    }

    // Test Case 2: Vietnamese character 'đ' (U+0111, UTF-8: 0xC4 0x91)
    {
        UTF8Decoder decoder;
        UTF8Decoder::Status status1 = decoder.decode(0xC4);
        TEST_ASSERT(status1 != UTF8Decoder::Status::ACCEPT);
        TEST_ASSERT(status1 != UTF8Decoder::Status::REJECT);

        UTF8Decoder::Status status2 = decoder.decode(0x91);
        TEST_ASSERT_EQ(static_cast<int>(status2), static_cast<int>(UTF8Decoder::Status::ACCEPT));
        TEST_ASSERT_EQ(decoder.getCodepoint(), 0x0111u);
    }

    // Test Case 3: Vietnamese character 'ệ' (U+1EC7, UTF-8: 0xE1 0xBB 0x87)
    {
        UTF8Decoder decoder;
        UTF8Decoder::Status status1 = decoder.decode(0xE1);
        TEST_ASSERT(status1 != UTF8Decoder::Status::ACCEPT);
        TEST_ASSERT(status1 != UTF8Decoder::Status::REJECT);

        UTF8Decoder::Status status2 = decoder.decode(0xBB);
        TEST_ASSERT(status2 != UTF8Decoder::Status::ACCEPT);
        TEST_ASSERT(status2 != UTF8Decoder::Status::REJECT);

        UTF8Decoder::Status status3 = decoder.decode(0x87);
        TEST_ASSERT_EQ(static_cast<int>(status3), static_cast<int>(UTF8Decoder::Status::ACCEPT));
        TEST_ASSERT_EQ(decoder.getCodepoint(), 0x1EC7u);
    }

    // Test Case 4: Invalid UTF-8 (overlong sequence / unexpected continuation)
    {
        UTF8Decoder decoder;
        // Continuation byte 0x80 out of nowhere should reject immediately
        UTF8Decoder::Status status = decoder.decode(0x80);
        TEST_ASSERT_EQ(static_cast<int>(status), static_cast<int>(UTF8Decoder::Status::REJECT));
    }
}
