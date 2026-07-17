/**
 * @file FontEngine.cpp
 * @brief Doxygen-compatible font processor implementation.
 *
 * Interprets structural metadata tables and computes overall string widths before canvas drawing.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#include "FontEngine.h"
#include "UnicodeMapper.h"
#include "UTF8Decoder.h"

/* ─────────────────────────────────────────────────────────────────────────────
 * Constructor
 * ────────────────────────────────────────────────────────────────────────── */

FontEngine::FontEngine()
    : font_(nullptr)
{
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Font Selection
 * ────────────────────────────────────────────────────────────────────────── */

void FontEngine::setFont(const VNFont* font) {
    font_ = font;
}

const VNFont* FontEngine::getFont() const {
    return font_;
}

bool FontEngine::hasFont() const {
    return font_ != nullptr;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Glyph Retrieval
 * ────────────────────────────────────────────────────────────────────────── */

bool FontEngine::getGlyph(uint32_t codepoint, VNGlyphInfo& info) const {
    info.valid = false;

    if (font_ == nullptr) {
        return false;
    }

    /* Look up the glyph index via the Unicode mapper */
    uint16_t glyphIndex = UnicodeMapper::findGlyphIndex(font_, codepoint);
    if (glyphIndex == VN_GLYPH_NOT_FOUND || glyphIndex >= font_->glyphCount) {
        return false;
    }

    /* Read glyph metadata from PROGMEM */
    VNGlyphEntry entry;
    readGlyphEntry(glyphIndex, entry);

    /* Populate the runtime glyph info */
    info.codepoint = codepoint;
    info.width     = entry.width;
    info.height    = entry.height;
    info.xOffset   = entry.xOffset;
    info.yOffset   = entry.yOffset;
    info.xAdvance  = entry.xAdvance;
    info.valid     = true;

    /* Calculate the bitmap pointer: base + offset */
    info.bitmap = font_->bitmapData + entry.bitmapOffset;

    return true;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Character Width
 * ────────────────────────────────────────────────────────────────────────── */

uint8_t FontEngine::getCharWidth(uint32_t codepoint) const {
    if (font_ == nullptr) {
        return 0;
    }

    VNGlyphInfo info;
    if (getGlyph(codepoint, info)) {
        return info.xAdvance;
    }

    /* Character not found — return default width */
    return font_->defaultWidth;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Text Width Measurement
 * ────────────────────────────────────────────────────────────────────────── */

uint16_t FontEngine::getTextWidth(const char* utf8Text) const {
    if (font_ == nullptr || utf8Text == nullptr) {
        return 0;
    }

    uint16_t totalWidth = 0;
    UTF8Decoder decoder;
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(utf8Text);

    while (*ptr != 0) {
        UTF8Decoder::Status status = decoder.decode(*ptr);
        ptr++;

        if (status == UTF8Decoder::Status::ACCEPT) {
            totalWidth += getCharWidth(decoder.getCodepoint());
        }
        else if (status == UTF8Decoder::Status::REJECT) {
            /* Invalid character — skip, add default width */
            totalWidth += font_->defaultWidth;
        }
    }

    return totalWidth;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Font Metrics
 * ────────────────────────────────────────────────────────────────────────── */

uint8_t FontEngine::getLineHeight() const {
    return (font_ != nullptr) ? font_->lineHeight : 0;
}

uint8_t FontEngine::getBaseline() const {
    return (font_ != nullptr) ? font_->baseline : 0;
}

uint8_t FontEngine::getDefaultWidth() const {
    return (font_ != nullptr) ? font_->defaultWidth : 0;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Internal — Read Glyph Entry from PROGMEM
 * ────────────────────────────────────────────────────────────────────────── */

void FontEngine::readGlyphEntry(uint16_t index, VNGlyphEntry& entry) const {
    const uint8_t* addr = reinterpret_cast<const uint8_t*>(&font_->glyphTable[index]);

    /* VNGlyphEntry layout (8 bytes):
     *   uint16_t bitmapOffset  [offset 0]
     *   uint8_t  width         [offset 2]
     *   uint8_t  height        [offset 3]
     *   int8_t   xOffset       [offset 4]
     *   int8_t   yOffset       [offset 5]
     *   uint8_t  xAdvance      [offset 6]
     */
    entry.bitmapOffset = VN_READ_WORD(addr + 0);
    entry.width        = VN_READ_BYTE(addr + 2);
    entry.height       = VN_READ_BYTE(addr + 3);
    entry.xOffset      = static_cast<int8_t>(VN_READ_BYTE(addr + 4));
    entry.yOffset      = static_cast<int8_t>(VN_READ_BYTE(addr + 5));
    entry.xAdvance     = VN_READ_BYTE(addr + 6);
}
