/**
 * @file TextAlignment.cpp
 * @brief Horizontal, vertical, and justify calculations implementation.
 *
 * Calculates distribution indices and spacing gaps to format justified alignments.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#include "TextAlignment.h"

/* ─────────────────────────────────────────────────────────────────────────────
 * Horizontal Alignment
 * ────────────────────────────────────────────────────────────────────────── */

int16_t TextAlignment::calculateX(uint16_t lineWidth, uint16_t containerWidth,
                                    VNAlign align)
{
    if (lineWidth >= containerWidth) {
        return 0; /* Text is wider than container — start at left edge */
    }

    switch (align) {
        case VNAlign::LEFT:
        case VNAlign::JUSTIFY:
            /* Left and Justify both start at X=0; justify adds inter-word spacing */
            return 0;

        case VNAlign::CENTER:
            return static_cast<int16_t>((containerWidth - lineWidth) / 2);

        case VNAlign::RIGHT:
            return static_cast<int16_t>(containerWidth - lineWidth);
    }

    return 0;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Vertical Alignment
 * ────────────────────────────────────────────────────────────────────────── */

int16_t TextAlignment::calculateY(uint16_t textHeight, uint16_t containerHeight,
                                    VNVAlign vAlign)
{
    if (textHeight >= containerHeight) {
        return 0; /* Text is taller than container — start at top */
    }

    switch (vAlign) {
        case VNVAlign::TOP:
            return 0;

        case VNVAlign::MIDDLE:
            return static_cast<int16_t>((containerHeight - textHeight) / 2);

        case VNVAlign::BOTTOM:
            return static_cast<int16_t>(containerHeight - textHeight);
    }

    return 0;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Justify Spacing
 * ────────────────────────────────────────────────────────────────────────── */

void TextAlignment::calculateJustify(uint16_t lineWidth, uint16_t containerWidth,
                                       uint16_t wordCount,
                                       uint16_t& extraSpace, uint16_t& remainder)
{
    extraSpace = 0;
    remainder  = 0;

    /* Need at least 2 words and line must be narrower than container */
    if (wordCount < 2 || lineWidth >= containerWidth) {
        return;
    }

    uint16_t totalGap = containerWidth - lineWidth;
    uint16_t gapCount = wordCount - 1;

    extraSpace = totalGap / gapCount;
    remainder  = totalGap % gapCount;
}
