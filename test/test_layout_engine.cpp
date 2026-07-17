/**
 * @file test_layout_engine.cpp
 * @brief Unit tests for LayoutEngine class.
 *
 * @author VietnameseOLED Contributors
 * @version 1.0.0
 * @copyright MIT License
 */

#include "LayoutEngine.h"
#include "FontEngine.h"
#include "fonts/Font_8x16_VN.h"
#include "test_helpers.h"

void test_layout_engine() {
    LayoutEngine layout;
    
    // Viewport defaults
    VNRect vp = layout.getViewport();
    TEST_ASSERT_EQ(vp.width, 128);
    TEST_ASSERT_EQ(vp.height, 128); // default max dimensions

    // Set viewport
    layout.setViewport(VNRect(10, 20, 100, 50));
    TEST_ASSERT_EQ(layout.getViewport().x, 10);
    TEST_ASSERT_EQ(layout.getViewport().y, 20);
    TEST_ASSERT_EQ(layout.getViewport().width, 100);
    TEST_ASSERT_EQ(layout.getViewport().height, 50);

    // Padding & Margins
    layout.setMargin(2, 4, 6, 8); // top, right, bottom, left
    layout.setPadding(1, 3, 5, 7);

    // Content width calculation: viewport width (100) - margin.horiz (4+8=12) - padding.horiz (3+7=10) = 78
    TEST_ASSERT_EQ(layout.getContentWidth(), 78);
    // Content height calculation: viewport height (50) - margin.vert (2+6=8) - padding.vert (1+5=6) = 36
    TEST_ASSERT_EQ(layout.getContentHeight(), 36);

    // Content origin calculation: viewport x (10) + margin.left (8) + padding.left (7) = 25
    // viewport y (20) + margin.top (2) + padding.top (1) = 23
    VNPoint origin = layout.getContentOrigin();
    TEST_ASSERT_EQ(origin.x, 25);
    TEST_ASSERT_EQ(origin.y, 23);

    // Text Wrap & Spacing
    layout.setTextWrap(VNTextWrap::CHARACTER);
    TEST_ASSERT(layout.getTextWrap() == VNTextWrap::CHARACTER);

    layout.setLineSpacing(2);
    TEST_ASSERT_EQ(layout.getLineSpacing(), 2);

    layout.setParagraphSpacing(4);
    TEST_ASSERT_EQ(layout.getParagraphSpacing(), 4);

    // Alignment
    layout.setAlignment(VNAlign::CENTER, VNVAlign::MIDDLE);
    TEST_ASSERT(layout.getHAlign() == VNAlign::CENTER);
    TEST_ASSERT(layout.getVAlign() == VNVAlign::MIDDLE);

    // Bounding Box Calculation
    FontEngine font;
    font.setFont(&Font_8x16_VN);
    layout.setViewport(VNRect(0, 0, 128, 64));
    layout.setMargin(0, 0, 0, 0);
    layout.setPadding(0, 0, 0, 0);
    layout.setTextWrap(VNTextWrap::NONE);

    // Single line text: "ABC" -> 3 * 9 = 27px width, 16px height
    VNRect bounds = layout.calculateTextBounds("ABC", font);
    TEST_ASSERT_EQ(bounds.width, 27);
    TEST_ASSERT_EQ(bounds.height, 16);
}
