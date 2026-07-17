/**
 * @file test_unicode_mapper.cpp
 * @brief Unit tests for UnicodeMapper class.
 *
 * @author VietnameseOLED Contributors
 * @version 1.0.0
 * @copyright MIT License
 */

#include "UnicodeMapper.h"
#include "fonts/Font_8x16_VN.h"
#include "test_helpers.h"

void test_unicode_mapper() {
    // Test Case 1: isASCII
    TEST_ASSERT(UnicodeMapper::isASCII(0x20));
    TEST_ASSERT(UnicodeMapper::isASCII(0x41));
    TEST_ASSERT(UnicodeMapper::isASCII(0x7E));
    TEST_ASSERT_FALSE(UnicodeMapper::isASCII(0x1F));
    TEST_ASSERT_FALSE(UnicodeMapper::isASCII(0x7F));
    TEST_ASSERT_FALSE(UnicodeMapper::isASCII(0x0111));

    // Test Case 2: isVietnamese
    TEST_ASSERT(UnicodeMapper::isVietnamese(0x00C0));  // À
    TEST_ASSERT(UnicodeMapper::isVietnamese(0x0111));  // đ
    TEST_ASSERT(UnicodeMapper::isVietnamese(0x01B0));  // ư
    TEST_ASSERT(UnicodeMapper::isVietnamese(0x1EC7));  // ệ
    TEST_ASSERT_FALSE(UnicodeMapper::isVietnamese(0x0041)); // A (standard ASCII)
    TEST_ASSERT_FALSE(UnicodeMapper::isVietnamese(0x1000)); // Non-Vietnamese Unicode

    // Test Case 3: isWhitespace
    TEST_ASSERT(UnicodeMapper::isWhitespace(' '));
    TEST_ASSERT(UnicodeMapper::isWhitespace('\t'));
    TEST_ASSERT(UnicodeMapper::isWhitespace('\n'));
    TEST_ASSERT_FALSE(UnicodeMapper::isWhitespace('A'));

    // Test Case 4: isBreakable
    TEST_ASSERT(UnicodeMapper::isBreakable(' '));
    TEST_ASSERT(UnicodeMapper::isBreakable('-'));
    TEST_ASSERT_FALSE(UnicodeMapper::isBreakable('A'));

    // Test Case 5: findGlyphIndex using built-in Font_8x16_VN
    {
        const VNFont* font = &Font_8x16_VN;
        
        // Find 'A' (ASCII 0x41)
        uint16_t idxA = UnicodeMapper::findGlyphIndex(font, 0x41);
        TEST_ASSERT_NE(idxA, VN_GLYPH_NOT_FOUND);
        
        // Find 'ệ' (U+1EC7)
        uint16_t idxEe = UnicodeMapper::findGlyphIndex(font, 0x1EC7);
        TEST_ASSERT_NE(idxEe, VN_GLYPH_NOT_FOUND);

        // Find non-existent character (e.g. Emoji U+1F600)
        uint16_t idxEmoji = UnicodeMapper::findGlyphIndex(font, 0x1F600);
        TEST_ASSERT_EQ(idxEmoji, VN_GLYPH_NOT_FOUND);
    }
}
