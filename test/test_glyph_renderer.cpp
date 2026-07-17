#include "GlyphRenderer.h"
#include "BitmapRenderer.h"
#include "FontEngine.h"
#include "fonts/Font_8x16_VN.h"
#include "mock_display_adapter.h"
#include "test_helpers.h"

void test_glyph_renderer() {
    MockDisplayAdapter display(128, 64);
    BitmapRenderer bitMapRenderer;
    bitMapRenderer.setTarget(&display);

    GlyphRenderer glyphRenderer;
    glyphRenderer.setTarget(&display);
    glyphRenderer.setBitmapRenderer(&bitMapRenderer);

    FontEngine fontEngine;
    fontEngine.setFont(&Font_8x16_VN);

    // Get a glyph to test with: '!' (ASCII 0x21) which is a simple vertical line
    VNGlyphInfo info;
    bool found = fontEngine.getGlyph(0x21, info);
    TEST_ASSERT(found);

    // Test Case 1: Direct rendering (no transforms)
    {
        display.clearBuffer();
        glyphRenderer.resetTransforms();
        glyphRenderer.renderGlyph(10, 10, info, VNColor::WHITE);

        bool hasPixels = false;
        for (int y = 10; y < 26; y++) {
            for (int x = 10; x < 18; x++) {
                if (display.getPixel(x, y) == 1) {
                    hasPixels = true;
                }
            }
        }
        TEST_ASSERT(hasPixels);
    }

    // Test Case 2: Scaled rendering (2x)
    {
        display.clearBuffer();
        glyphRenderer.resetTransforms();
        glyphRenderer.setScale(2, 2);
        glyphRenderer.renderGlyph(10, 10, info, VNColor::WHITE);

        // Scaled height should double.
        int pixelCount = 0;
        for (int y = 10; y < 42; y++) {
            for (int x = 10; x < 26; x++) {
                if (display.getPixel(x, y) == 1) {
                    pixelCount++;
                }
            }
        }
        TEST_ASSERT(pixelCount > 0);
    }

    // Test Case 3: Overwrite mode
    {
        display.clearBuffer();
        // pre-fill area with white
        for (int y = 10; y < 26; y++) {
            for (int x = 10; x < 18; x++) {
                display.setPixel(x, y, 1);
            }
        }

        glyphRenderer.resetTransforms();
        glyphRenderer.setRenderMode(VNRenderMode::OVERWRITE);
        glyphRenderer.renderGlyph(10, 10, info, VNColor::WHITE); // uses WHITE, so background should be BLACK

        // Some pixels should now be black (0) because overwrite clears background
        bool hasBlack = false;
        for (int y = 10; y < 26; y++) {
            for (int x = 10; x < 18; x++) {
                if (display.getPixel(x, y) == 0) {
                    hasBlack = true;
                }
            }
        }
        TEST_ASSERT(hasBlack);
    }
}
