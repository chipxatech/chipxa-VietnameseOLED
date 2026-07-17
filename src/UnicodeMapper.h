/**
 * @file UnicodeMapper.h
 * @brief Unicode codepoint-to-glyph index translation utility.
 *
 * Provides static check filters for Vietnamese diacritics and binary search interfaces on flash-bound maps.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#ifndef VN_UNICODE_MAPPER_H
#define VN_UNICODE_MAPPER_H

#include "VNConfig.h"
#include "VNTypes.h"

/**
 * @class UnicodeMapper
 * @brief Provides static methods to map Unicode codepoints to font glyph indices.
 *
 * All methods are static — this class has no state and is used as a namespace
 * for Unicode-related utilities.
 */
class UnicodeMapper {
public:
    /**
     * @brief Find the glyph index for a given Unicode codepoint in a font.
     *
     * Performs binary search on the font's sorted Unicode mapping table,
     * reading from PROGMEM on AVR platforms.
     *
     * @param font      Pointer to the font descriptor
     * @param codepoint Unicode codepoint to look up
     * @return Glyph index, or VN_GLYPH_NOT_FOUND (0xFFFF) if not in the font
     */
    static uint16_t findGlyphIndex(const VNFont* font, uint32_t codepoint);

    /**
     * @brief Check if a codepoint is a Vietnamese-specific character.
     *
     * Returns true for the 134 Vietnamese diacritical characters in:
     * - Latin-1 Supplement (accented vowels: À-ý)
     * - Latin Extended-A (Ă, ă, Đ, đ, Ĩ, ĩ, Ũ, ũ)
     * - Latin Extended-B (Ơ, ơ, Ư, ư)
     * - Latin Extended Additional (Ạ-ỹ, U+1EA0–U+1EF9)
     *
     * @param codepoint Unicode codepoint
     * @return true if the codepoint is a Vietnamese-specific character
     */
    static bool isVietnamese(uint32_t codepoint);

    /**
     * @brief Check if a codepoint is in the ASCII range.
     *
     * @param codepoint Unicode codepoint
     * @return true if codepoint is U+0020–U+007E (printable ASCII)
     */
    static bool isASCII(uint32_t codepoint);

    /**
     * @brief Check if a codepoint is a whitespace character.
     *
     * Recognizes space (U+0020), tab (U+0009), and other standard whitespace.
     *
     * @param codepoint Unicode codepoint
     * @return true if the codepoint is whitespace
     */
    static bool isWhitespace(uint32_t codepoint);

    /**
     * @brief Check if a codepoint is a word-break opportunity.
     *
     * Returns true for characters after which line-breaking is allowed
     * (spaces, hyphens, etc.).
     *
     * @param codepoint Unicode codepoint
     * @return true if breaking is allowed after this character
     */
    static bool isBreakable(uint32_t codepoint);

private:
    /* ── Vietnamese codepoint range boundaries ──────────────────────────── */

    /** @brief Start of Latin-1 Supplement Vietnamese uppercase. */
    static constexpr uint32_t LATIN1_UPPER_START = 0x00C0;
    /** @brief End of Latin-1 Supplement Vietnamese uppercase. */
    static constexpr uint32_t LATIN1_UPPER_END   = 0x00DD;
    /** @brief Start of Latin-1 Supplement Vietnamese lowercase. */
    static constexpr uint32_t LATIN1_LOWER_START = 0x00E0;
    /** @brief End of Latin-1 Supplement Vietnamese lowercase. */
    static constexpr uint32_t LATIN1_LOWER_END   = 0x00FD;

    /** @brief Start of Latin Extended-A Vietnamese. */
    static constexpr uint32_t LATIN_EXT_A_START  = 0x0102;
    /** @brief End of Latin Extended-A Vietnamese. */
    static constexpr uint32_t LATIN_EXT_A_END    = 0x0169;

    /** @brief Start of Latin Extended-B Vietnamese. */
    static constexpr uint32_t LATIN_EXT_B_START  = 0x01A0;
    /** @brief End of Latin Extended-B Vietnamese. */
    static constexpr uint32_t LATIN_EXT_B_END    = 0x01B0;

    /** @brief Start of Latin Extended Additional Vietnamese. */
    static constexpr uint32_t LATIN_EXT_ADD_START = 0x1EA0;
    /** @brief End of Latin Extended Additional Vietnamese. */
    static constexpr uint32_t LATIN_EXT_ADD_END   = 0x1EF9;
};

#endif /* VN_UNICODE_MAPPER_H */
