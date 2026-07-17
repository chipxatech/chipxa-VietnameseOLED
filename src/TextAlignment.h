/**
 * @file TextAlignment.h
 * @brief Layout alignment offset coordinates calculator.
 *
 * Calculates offsets for centered, right-aligned, or justified text formatting.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#ifndef VN_TEXT_ALIGNMENT_H
#define VN_TEXT_ALIGNMENT_H

#include "VNConfig.h"
#include "VNTypes.h"

/**
 * @class TextAlignment
 * @brief Static utility class for text alignment calculations.
 */
class TextAlignment {
public:
    /**
     * @brief Calculate horizontal X offset for a line of text.
     *
     * @param lineWidth      Width of the text line in pixels
     * @param containerWidth Available container width in pixels
     * @param align          Horizontal alignment mode
     * @return X offset from the container's left edge
     */
    static int16_t calculateX(uint16_t lineWidth, uint16_t containerWidth, VNAlign align);

    /**
     * @brief Calculate vertical Y offset for a block of text.
     *
     * @param textHeight      Total text block height in pixels
     * @param containerHeight Available container height in pixels
     * @param vAlign          Vertical alignment mode
     * @return Y offset from the container's top edge
     */
    static int16_t calculateY(uint16_t textHeight, uint16_t containerHeight, VNVAlign vAlign);

    /**
     * @brief Calculate extra spacing between words for justify alignment.
     *
     * Distributes remaining space evenly between words on a line.
     *
     * @param lineWidth      Rendered width of all text on this line
     * @param containerWidth Target container width
     * @param wordCount      Number of words on this line
     * @param[out] extraSpace Extra pixels to add between each word
     * @param[out] remainder  Remaining pixels after even distribution
     */
    static void calculateJustify(uint16_t lineWidth, uint16_t containerWidth,
                                  uint16_t wordCount,
                                  uint16_t& extraSpace, uint16_t& remainder);

private:
    TextAlignment() = delete; /**< Static-only class, no instantiation */
};

#endif /* VN_TEXT_ALIGNMENT_H */
