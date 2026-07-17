/**
 * @file WordWrapper.h
 * @brief Word wrap layout boundaries calculator.
 *
 * Calculates line-wrap split indexes inside UTF-8 strings according to page constraints and word tokens.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#ifndef VN_WORD_WRAPPER_H
#define VN_WORD_WRAPPER_H

#include "VNConfig.h"
#include "VNTypes.h"

/* Forward declaration */
class FontEngine;

/**
 * @class WordWrapper
 * @brief Breaks UTF-8 text into lines that fit a given pixel width.
 */
class WordWrapper {
public:
    /**
     * @brief Descriptor for a single wrapped line.
     */
    struct LineInfo {
        uint16_t startIndex;   /**< Byte offset of line start in source string */
        uint16_t byteLength;   /**< Byte length of this line (excluding trailing space) */
        uint16_t pixelWidth;   /**< Rendered width of this line in pixels */
        uint16_t charCount;    /**< Number of Unicode characters in this line */
    };

    /** @brief Construct a WordWrapper. */
    WordWrapper();

    /**
     * @brief Wrap a UTF-8 string into lines.
     *
     * @param text      Null-terminated UTF-8 string
     * @param maxWidth  Maximum line width in pixels
     * @param font      Font engine for measuring character widths
     * @param wrapMode  Wrapping mode (NONE, WORD, CHARACTER)
     * @param[out] lines Array to receive line descriptors
     * @param maxLines  Maximum number of lines to produce
     * @return Number of lines produced
     */
    uint8_t wrap(const char* text, uint16_t maxWidth, const FontEngine& font,
                 VNTextWrap wrapMode, LineInfo* lines, uint8_t maxLines);

private:
    /**
     * @brief Handle NONE wrap mode (no wrapping, single line).
     */
    uint8_t wrapNone(const char* text, const FontEngine& font,
                     LineInfo* lines, uint8_t maxLines);

    /**
     * @brief Handle WORD wrap mode (break at word boundaries).
     */
    uint8_t wrapWord(const char* text, uint16_t maxWidth, const FontEngine& font,
                     LineInfo* lines, uint8_t maxLines);

    /**
     * @brief Handle CHARACTER wrap mode (break at any character).
     */
    uint8_t wrapCharacter(const char* text, uint16_t maxWidth, const FontEngine& font,
                          LineInfo* lines, uint8_t maxLines);
};

#endif /* VN_WORD_WRAPPER_H */
