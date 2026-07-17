/**
 * @file WordWrapper.cpp
 * @brief Vietnamese and Latin word wrapping layout locator.
 *
 * Tracks whitespace and hyphens to segment strings into wrapping lists without memory copying.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#include "WordWrapper.h"
#include "FontEngine.h"
#include "UTF8Decoder.h"
#include "UnicodeMapper.h"

/* ─────────────────────────────────────────────────────────────────────────────
 * Constructor
 * ────────────────────────────────────────────────────────────────────────── */

WordWrapper::WordWrapper() {
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Public Entry Point
 * ────────────────────────────────────────────────────────────────────────── */

uint8_t WordWrapper::wrap(const char* text, uint16_t maxWidth,
                           const FontEngine& font, VNTextWrap wrapMode,
                           LineInfo* lines, uint8_t maxLines)
{
    if (text == nullptr || lines == nullptr || maxLines == 0 || !font.hasFont()) {
        return 0;
    }

    switch (wrapMode) {
        case VNTextWrap::NONE:
            return wrapNone(text, font, lines, maxLines);
        case VNTextWrap::WORD:
            return wrapWord(text, maxWidth, font, lines, maxLines);
        case VNTextWrap::CHARACTER:
            return wrapCharacter(text, maxWidth, font, lines, maxLines);
    }

    return 0;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * No Wrap — Single Line
 * ────────────────────────────────────────────────────────────────────────── */

uint8_t WordWrapper::wrapNone(const char* text, const FontEngine& font,
                               LineInfo* lines, uint8_t maxLines)
{
    if (maxLines == 0) return 0;

    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(text);
    uint16_t width = 0;
    uint16_t chars = 0;
    uint16_t byteLen = 0;

    UTF8Decoder decoder;

    while (*ptr != 0) {
        UTF8Decoder::Status status = decoder.decode(*ptr);
        ptr++;
        byteLen++;

        if (status == UTF8Decoder::Status::ACCEPT) {
            uint32_t cp = decoder.getCodepoint();
            if (cp == '\n') {
                /* Stop at first newline in no-wrap mode */
                byteLen--; /* Don't include newline in byte length */
                break;
            }
            width += font.getCharWidth(cp);
            chars++;
        }
    }

    lines[0].startIndex = 0;
    lines[0].byteLength = byteLen;
    lines[0].pixelWidth = width;
    lines[0].charCount  = chars;

    return 1;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Word Wrap — Break at Space/Hyphen Boundaries
 * ────────────────────────────────────────────────────────────────────────── */

uint8_t WordWrapper::wrapWord(const char* text, uint16_t maxWidth,
                               const FontEngine& font,
                               LineInfo* lines, uint8_t maxLines)
{
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(text);
    uint8_t lineCount = 0;

    uint16_t lineStart  = 0;   /* Byte offset of current line start */
    uint16_t lineWidth  = 0;   /* Pixel width of current line */
    uint16_t lineChars  = 0;   /* Character count of current line */
    uint16_t lineBytes  = 0;   /* Byte length of current line */

    /* Track last breakable position */
    uint16_t lastBreakByte  = 0; /* Byte offset after last break opportunity */
    uint16_t lastBreakWidth = 0; /* Pixel width at last break opportunity */
    uint16_t lastBreakChars = 0; /* Char count at last break opportunity */
    bool     hasBreak       = false;

    uint16_t bytePos = 0;
    UTF8Decoder decoder;

    while (ptr[bytePos] != 0) {
        /* Decode one character */
        uint16_t charStartByte = bytePos;
        uint32_t codepoint = 0;
        bool decoded = false;

        while (ptr[bytePos] != 0) {
            UTF8Decoder::Status status = decoder.decode(ptr[bytePos]);
            bytePos++;

            if (status == UTF8Decoder::Status::ACCEPT) {
                codepoint = decoder.getCodepoint();
                decoded = true;
                break;
            }
            else if (status == UTF8Decoder::Status::REJECT) {
                codepoint = VN_UNICODE_REPLACEMENT_CHAR;
                decoded = true;
                break;
            }
        }

        if (!decoded) break;

        /* Handle explicit newline */
        if (codepoint == '\n') {
            if (lineCount < maxLines) {
                lines[lineCount].startIndex = lineStart;
                lines[lineCount].byteLength = lineBytes;
                lines[lineCount].pixelWidth = lineWidth;
                lines[lineCount].charCount  = lineChars;
                lineCount++;
            }
            /* Reset for next line */
            lineStart = bytePos;
            lineWidth = 0;
            lineChars = 0;
            lineBytes = 0;
            hasBreak  = false;
            continue;
        }

        uint8_t charWidth = font.getCharWidth(codepoint);

        /* Check if adding this character would exceed the line width */
        if (lineWidth + charWidth > maxWidth && lineChars > 0) {
            /* Need to break the line */
            if (hasBreak) {
                /* Break at last word boundary */
                if (lineCount < maxLines) {
                    lines[lineCount].startIndex = lineStart;
                    lines[lineCount].byteLength = static_cast<uint16_t>(lastBreakByte - lineStart);
                    lines[lineCount].pixelWidth = lastBreakWidth;
                    lines[lineCount].charCount  = lastBreakChars;
                    lineCount++;
                }
                /* Continue from after the break */
                lineStart = lastBreakByte;
                /* Recalculate remaining width */
                lineWidth = lineWidth - lastBreakWidth;
                lineChars = lineChars - lastBreakChars;
                lineBytes = bytePos - lastBreakByte;
            }
            else {
                /* No break opportunity — force break at current position */
                if (lineCount < maxLines) {
                    lines[lineCount].startIndex = lineStart;
                    lines[lineCount].byteLength = lineBytes;
                    lines[lineCount].pixelWidth = lineWidth;
                    lines[lineCount].charCount  = lineChars;
                    lineCount++;
                }
                lineStart = charStartByte;
                lineWidth = 0;
                lineChars = 0;
                lineBytes = bytePos - charStartByte;
            }
            hasBreak = false;

            if (lineCount >= maxLines) return lineCount;
        }

        /* Accumulate character */
        lineWidth += charWidth;
        lineChars++;
        lineBytes = bytePos - lineStart;

        /* Track break opportunities (after spaces and hyphens) */
        if (UnicodeMapper::isBreakable(codepoint)) {
            lastBreakByte  = bytePos;
            lastBreakWidth = lineWidth;
            lastBreakChars = lineChars;
            hasBreak = true;
        }
    }

    /* Emit final line if there's remaining text */
    if (lineChars > 0 && lineCount < maxLines) {
        lines[lineCount].startIndex = lineStart;
        lines[lineCount].byteLength = lineBytes;
        lines[lineCount].pixelWidth = lineWidth;
        lines[lineCount].charCount  = lineChars;
        lineCount++;
    }

    return lineCount;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Character Wrap — Break at Any Character
 * ────────────────────────────────────────────────────────────────────────── */

uint8_t WordWrapper::wrapCharacter(const char* text, uint16_t maxWidth,
                                    const FontEngine& font,
                                    LineInfo* lines, uint8_t maxLines)
{
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(text);
    uint8_t lineCount = 0;

    uint16_t lineStart = 0;
    uint16_t lineWidth = 0;
    uint16_t lineChars = 0;
    uint16_t lineBytes = 0;

    uint16_t bytePos = 0;
    UTF8Decoder decoder;

    while (ptr[bytePos] != 0) {
        uint16_t charStartByte = bytePos;
        uint32_t codepoint = 0;
        bool decoded = false;

        while (ptr[bytePos] != 0) {
            UTF8Decoder::Status status = decoder.decode(ptr[bytePos]);
            bytePos++;

            if (status == UTF8Decoder::Status::ACCEPT) {
                codepoint = decoder.getCodepoint();
                decoded = true;
                break;
            }
            else if (status == UTF8Decoder::Status::REJECT) {
                codepoint = VN_UNICODE_REPLACEMENT_CHAR;
                decoded = true;
                break;
            }
        }

        if (!decoded) break;

        /* Handle explicit newline */
        if (codepoint == '\n') {
            if (lineCount < maxLines) {
                lines[lineCount].startIndex = lineStart;
                lines[lineCount].byteLength = lineBytes;
                lines[lineCount].pixelWidth = lineWidth;
                lines[lineCount].charCount  = lineChars;
                lineCount++;
            }
            lineStart = bytePos;
            lineWidth = 0;
            lineChars = 0;
            lineBytes = 0;
            continue;
        }

        uint8_t charWidth = font.getCharWidth(codepoint);

        /* Break before this character if it would overflow */
        if (lineWidth + charWidth > maxWidth && lineChars > 0) {
            if (lineCount < maxLines) {
                lines[lineCount].startIndex = lineStart;
                lines[lineCount].byteLength = lineBytes;
                lines[lineCount].pixelWidth = lineWidth;
                lines[lineCount].charCount  = lineChars;
                lineCount++;
            }
            lineStart = charStartByte;
            lineWidth = 0;
            lineChars = 0;
            lineBytes = 0;

            if (lineCount >= maxLines) return lineCount;
        }

        lineWidth += charWidth;
        lineChars++;
        lineBytes = bytePos - lineStart;
    }

    /* Emit final line */
    if (lineChars > 0 && lineCount < maxLines) {
        lines[lineCount].startIndex = lineStart;
        lines[lineCount].byteLength = lineBytes;
        lines[lineCount].pixelWidth = lineWidth;
        lines[lineCount].charCount  = lineChars;
        lineCount++;
    }

    return lineCount;
}
