/**
 * @file test_alignment.cpp
 * @brief Unit tests for TextAlignment class.
 *
 * @author VietnameseOLED Contributors
 * @version 1.0.0
 * @copyright MIT License
 */

#include "TextAlignment.h"
#include "test_helpers.h"

void test_alignment() {
    // Test Case 1: Horizontal Alignment calculateX
    {
        uint16_t lineW = 40;
        uint16_t containerW = 100;

        TEST_ASSERT_EQ(TextAlignment::calculateX(lineW, containerW, VNAlign::LEFT), 0);
        TEST_ASSERT_EQ(TextAlignment::calculateX(lineW, containerW, VNAlign::CENTER), 30);
        TEST_ASSERT_EQ(TextAlignment::calculateX(lineW, containerW, VNAlign::RIGHT), 60);
        TEST_ASSERT_EQ(TextAlignment::calculateX(lineW, containerW, VNAlign::JUSTIFY), 0);

        // Line wider than container should default to 0
        TEST_ASSERT_EQ(TextAlignment::calculateX(120, containerW, VNAlign::CENTER), 0);
    }

    // Test Case 2: Vertical Alignment calculateY
    {
        uint16_t textH = 20;
        uint16_t containerH = 60;

        TEST_ASSERT_EQ(TextAlignment::calculateY(textH, containerH, VNVAlign::TOP), 0);
        TEST_ASSERT_EQ(TextAlignment::calculateY(textH, containerH, VNVAlign::MIDDLE), 20);
        TEST_ASSERT_EQ(TextAlignment::calculateY(textH, containerH, VNVAlign::BOTTOM), 40);

        // Text taller than container should default to 0
        TEST_ASSERT_EQ(TextAlignment::calculateY(80, containerH, VNVAlign::MIDDLE), 0);
    }

    // Test Case 3: Justification calculateJustify
    {
        uint16_t extraSpace = 0;
        uint16_t remainder = 0;

        // Gap = 20, words = 3, gaps = 2 -> extraSpace = 10, remainder = 0
        TextAlignment::calculateJustify(80, 100, 3, extraSpace, remainder);
        TEST_ASSERT_EQ(extraSpace, 10);
        TEST_ASSERT_EQ(remainder, 0);

        // Gap = 20, words = 4, gaps = 3 -> extraSpace = 6, remainder = 2
        TextAlignment::calculateJustify(80, 100, 4, extraSpace, remainder);
        TEST_ASSERT_EQ(extraSpace, 6);
        TEST_ASSERT_EQ(remainder, 2);

        // Less than 2 words -> no justification
        TextAlignment::calculateJustify(80, 100, 1, extraSpace, remainder);
        TEST_ASSERT_EQ(extraSpace, 0);
        TEST_ASSERT_EQ(remainder, 0);
    }
}
