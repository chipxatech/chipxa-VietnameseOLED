/**
 * @file test_font_engine.cpp
 * @brief Unit tests for FontEngine class.
 *
 * @author VietnameseOLED Contributors
 * @version 1.0.0
 * @copyright MIT License
 */

#include "FontEngine.h"
#include "fonts/Font_8x16_VN.h"
#include "test_helpers.h"

void test_font_engine() {
    FontEngine engine;
    
    // Check initial state
    TEST_ASSERT_FALSE(engine.hasFont());
    TEST_ASSERT(engine.getFont() == nullptr);
    TEST_ASSERT_EQ(engine.getLineHeight(), 0);

    // Set font to Font_8x16_VN
    engine.setFont(&Font_8x16_VN);
    TEST_ASSERT(engine.hasFont());
    TEST_ASSERT(engine.getFont() == &Font_8x16_VN);
    TEST_ASSERT_EQ(engine.getLineHeight(), 16);
    TEST_ASSERT_EQ(engine.getBaseline(), 14);
    TEST_ASSERT_EQ(engine.getDefaultWidth(), 8);

    // Test getGlyph for ASCII 'A'
    {
        VNGlyphInfo info;
        bool found = engine.getGlyph(0x41, info);
        TEST_ASSERT(found);
        TEST_ASSERT(info.valid);
        TEST_ASSERT_EQ(info.codepoint, 0x41u);
        TEST_ASSERT_EQ(info.width, 8);
        TEST_ASSERT_EQ(info.height, 16);
        TEST_ASSERT_EQ(info.xAdvance, 9); // 8 + 1 pixel spacing
        TEST_ASSERT(info.bitmap != nullptr);
    }

    // Test getGlyph for Vietnamese 'ệ'
    {
        VNGlyphInfo info;
        bool found = engine.getGlyph(0x1EC6, info); // 'Ệ' (capital) or 'ệ' (0x1EC7)
        TEST_ASSERT(found);
        TEST_ASSERT(info.valid);
        TEST_ASSERT_EQ(info.width, 8);
        TEST_ASSERT_EQ(info.height, 16);
    }

    // Test getCharWidth
    TEST_ASSERT_EQ(engine.getCharWidth(0x41), 9);    // 'A'
    TEST_ASSERT_EQ(engine.getCharWidth(0x1EC7), 9);  // 'ệ'
    TEST_ASSERT_EQ(engine.getCharWidth(0x1F600), 8); // Non-existent uses default width (8)

    // Test getTextWidth with ASCII and UTF-8 mixed
    // "Xin chào" has 8 codepoints. 8 * 9 = 72.
    TEST_ASSERT_EQ(engine.getTextWidth("Xin chào"), 72);

    // "Đ" (1 char, 9px) + "ẹ" (1 char, 9px) + "p" (1 char, 9px) = 27.
    TEST_ASSERT_EQ(engine.getTextWidth("Đẹp"), 27);
}
