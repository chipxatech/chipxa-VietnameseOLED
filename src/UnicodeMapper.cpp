/**
 * @file UnicodeMapper.cpp
 * @brief Unicode binary search lookup map implementation.
 *
 * Performs O(log n) search queries on sorted PROGMEM tables to resolve glyph indexes in flash space.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#include "UnicodeMapper.h"

/* ─────────────────────────────────────────────────────────────────────────────
 * findGlyphIndex — Binary Search on PROGMEM Unicode Map
 * ────────────────────────────────────────────────────────────────────────── */

uint16_t UnicodeMapper::findGlyphIndex(const VNFont* font, uint32_t codepoint) {
    if (font == nullptr || font->unicodeMap == nullptr || font->unicodeMapSize == 0) {
        return VN_GLYPH_NOT_FOUND;
    }

    /* Codepoints beyond BMP are not supported by our 16-bit mapping table */
    if (codepoint > VN_UNICODE_MAX_BMP) {
        return VN_GLYPH_NOT_FOUND;
    }

    const uint16_t target = static_cast<uint16_t>(codepoint);
    const VNUnicodeEntry* map = font->unicodeMap;
    const uint16_t mapSize = font->unicodeMapSize;

    /* Binary search on sorted PROGMEM array */
    uint16_t low  = 0;
    uint16_t high = mapSize;

    while (low < high) {
        uint16_t mid = low + ((high - low) >> 1);

        /* Read the codepoint at mid from PROGMEM */
        const uint8_t* entryAddr = reinterpret_cast<const uint8_t*>(&map[mid]);
        uint16_t midCodepoint = VN_READ_WORD(entryAddr);

        if (midCodepoint == target) {
            /* Found — read the glyph index from the same entry */
            uint16_t glyphIdx = VN_READ_WORD(entryAddr + sizeof(uint16_t));
            return glyphIdx;
        }
        else if (midCodepoint < target) {
            low = mid + 1;
        }
        else {
            high = mid;
        }
    }

    return VN_GLYPH_NOT_FOUND;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * isVietnamese — Codepoint Classification
 * ────────────────────────────────────────────────────────────────────────── */

bool UnicodeMapper::isVietnamese(uint32_t codepoint) {
    /*
     * Vietnamese-specific characters span four Unicode blocks.
     * We check each block's relevant codepoints individually.
     */

    /* Latin-1 Supplement — Vietnamese subset (not all are Vietnamese) */
    switch (codepoint) {
        /* Uppercase accented vowels */
        case 0x00C0: /* À */ case 0x00C1: /* Á */ case 0x00C2: /* Â */
        case 0x00C3: /* Ã */ case 0x00C8: /* È */ case 0x00C9: /* É */
        case 0x00CA: /* Ê */ case 0x00CC: /* Ì */ case 0x00CD: /* Í */
        case 0x00D2: /* Ò */ case 0x00D3: /* Ó */ case 0x00D4: /* Ô */
        case 0x00D5: /* Õ */ case 0x00D9: /* Ù */ case 0x00DA: /* Ú */
        case 0x00DD: /* Ý */
        /* Lowercase accented vowels */
        case 0x00E0: /* à */ case 0x00E1: /* á */ case 0x00E2: /* â */
        case 0x00E3: /* ã */ case 0x00E8: /* è */ case 0x00E9: /* é */
        case 0x00EA: /* ê */ case 0x00EC: /* ì */ case 0x00ED: /* í */
        case 0x00F2: /* ò */ case 0x00F3: /* ó */ case 0x00F4: /* ô */
        case 0x00F5: /* õ */ case 0x00F9: /* ù */ case 0x00FA: /* ú */
        case 0x00FD: /* ý */
            return true;

        /* Latin Extended-A */
        case 0x0102: /* Ă */ case 0x0103: /* ă */
        case 0x0110: /* Đ */ case 0x0111: /* đ */
        case 0x0128: /* Ĩ */ case 0x0129: /* ĩ */
        case 0x0168: /* Ũ */ case 0x0169: /* ũ */
            return true;

        /* Latin Extended-B */
        case 0x01A0: /* Ơ */ case 0x01A1: /* ơ */
        case 0x01AF: /* Ư */ case 0x01B0: /* ư */
            return true;

        default:
            break;
    }

    /* Latin Extended Additional — contiguous Vietnamese block U+1EA0..U+1EF9 */
    if (codepoint >= LATIN_EXT_ADD_START && codepoint <= LATIN_EXT_ADD_END) {
        return true;
    }

    return false;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * isASCII — Printable ASCII Check
 * ────────────────────────────────────────────────────────────────────────── */

bool UnicodeMapper::isASCII(uint32_t codepoint) {
    return codepoint >= 0x0020 && codepoint <= 0x007E;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * isWhitespace
 * ────────────────────────────────────────────────────────────────────────── */

bool UnicodeMapper::isWhitespace(uint32_t codepoint) {
    switch (codepoint) {
        case 0x0020: /* Space */
        case 0x0009: /* Tab */
        case 0x000A: /* Line Feed */
        case 0x000D: /* Carriage Return */
        case 0x00A0: /* Non-breaking space */
            return true;
        default:
            return false;
    }
}

/* ─────────────────────────────────────────────────────────────────────────────
 * isBreakable — Word-break Opportunity
 * ────────────────────────────────────────────────────────────────────────── */

bool UnicodeMapper::isBreakable(uint32_t codepoint) {
    switch (codepoint) {
        case 0x0020: /* Space */
        case 0x0009: /* Tab */
        case 0x002D: /* Hyphen-minus */
        case 0x2010: /* Hyphen */
        case 0x2013: /* En dash */
        case 0x2014: /* Em dash */
            return true;
        default:
            return false;
    }
}
