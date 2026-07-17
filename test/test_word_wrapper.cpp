/**
 * @file test_word_wrapper.cpp
 * @brief Unit tests for WordWrapper class.
 *
 * @author VietnameseOLED Contributors
 * @version 1.0.0
 * @copyright MIT License
 */

#include "WordWrapper.h"
#include "FontEngine.h"
#include "fonts/Font_8x16_VN.h"
#include "test_helpers.h"

void test_word_wrapper() {
    FontEngine font;
    font.setFont(&Font_8x16_VN); // char width is 8, advance is 9

    WordWrapper wrapper;
    WordWrapper::LineInfo lines[8];

    // Test Case 1: Wrap NONE
    {
        const char* text = "hello\nworld";
        uint8_t count = wrapper.wrap(text, 100, font, VNTextWrap::NONE, lines, 8);
        TEST_ASSERT_EQ(count, 1);
        TEST_ASSERT_EQ(lines[0].startIndex, 0);
        TEST_ASSERT_EQ(lines[0].byteLength, 5); // "hello" (stops at '\n')
        TEST_ASSERT_EQ(lines[0].pixelWidth, 45); // 5 * 9 = 45
        TEST_ASSERT_EQ(lines[0].charCount, 5);
    }

    // Test Case 2: Wrap CHARACTER
    {
        // 8x16 font: 9px advance width.
        // If maxWidth is 30, we can fit at most 3 characters (3 * 9 = 27px).
        // Text: "abcdef" -> should split into "abc" and "def".
        const char* text = "abcdef";
        uint8_t count = wrapper.wrap(text, 30, font, VNTextWrap::CHARACTER, lines, 8);
        TEST_ASSERT_EQ(count, 2);
        
        // Line 1: "abc"
        TEST_ASSERT_EQ(lines[0].startIndex, 0);
        TEST_ASSERT_EQ(lines[0].byteLength, 3);
        TEST_ASSERT_EQ(lines[0].pixelWidth, 27);
        TEST_ASSERT_EQ(lines[0].charCount, 3);

        // Line 2: "def"
        TEST_ASSERT_EQ(lines[1].startIndex, 3);
        TEST_ASSERT_EQ(lines[1].byteLength, 3);
        TEST_ASSERT_EQ(lines[1].pixelWidth, 27);
        TEST_ASSERT_EQ(lines[1].charCount, 3);
    }

    // Test Case 3: Wrap WORD
    {
        // Text: "hello world" (5 chars + space + 5 chars = 11 chars).
        // Max width: 60 (fits 6 chars = 54px).
        // Should wrap at the space to:
        // Line 1: "hello "
        // Line 2: "world"
        const char* text = "hello world";
        uint8_t count = wrapper.wrap(text, 60, font, VNTextWrap::WORD, lines, 8);
        TEST_ASSERT_EQ(count, 2);

        // Line 1: "hello " (includes trailing space)
        TEST_ASSERT_EQ(lines[0].startIndex, 0);
        TEST_ASSERT_EQ(lines[0].byteLength, 6);
        TEST_ASSERT_EQ(lines[0].charCount, 6);

        // Line 2: "world"
        TEST_ASSERT(lines[1].startIndex >= 5);
        TEST_ASSERT_EQ(lines[1].byteLength, 5);
        TEST_ASSERT_EQ(lines[1].charCount, 5);
    }
}
