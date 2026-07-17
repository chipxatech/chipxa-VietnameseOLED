/**
 * @file FontEngine.h
 * @brief PROGMEM font data processor and metric calculator.
 *
 * Queries glyph entries from Flash memory, resolving character widths, advance steps, and layout positions.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#ifndef VN_FONT_ENGINE_H
#define VN_FONT_ENGINE_H

#include "VNConfig.h"
#include "VNTypes.h"

/**
 * @class FontEngine
 * @brief Retrieves glyph information from a bitmap font.
 *
 * Usage:
 * @code
 * FontEngine engine;
 * engine.setFont(&Font_8x16_VN);
 *
 * VNGlyphInfo info;
 * if (engine.getGlyph(0x1EC7, info)) {  // 'ệ'
 *     // info.bitmap points to the glyph's bitmap data
 *     // info.width, info.height, etc. contain metrics
 * }
 * @endcode
 */
class FontEngine {
public:
    /** @brief Construct a FontEngine with no font set. */
    FontEngine();

    /**
     * @brief Set the active font.
     * @param font Pointer to a VNFont descriptor (data must be in PROGMEM)
     */
    void setFont(const VNFont* font);

    /**
     * @brief Get the currently active font.
     * @return Pointer to the current font, or nullptr if none set
     */
    const VNFont* getFont() const;

    /**
     * @brief Retrieve glyph information for a Unicode codepoint.
     *
     * Looks up the codepoint in the font's Unicode mapping table,
     * then reads the glyph metadata from PROGMEM.
     *
     * @param codepoint Unicode codepoint to look up
     * @param[out] info Glyph information structure to populate
     * @return true if the glyph was found, false otherwise
     */
    bool getGlyph(uint32_t codepoint, VNGlyphInfo& info) const;

    /**
     * @brief Get the advance width of a character.
     *
     * @param codepoint Unicode codepoint
     * @return Advance width in pixels, or the font's default width if not found
     */
    uint8_t getCharWidth(uint32_t codepoint) const;

    /**
     * @brief Calculate the pixel width of a UTF-8 string.
     *
     * Sums the xAdvance values for all characters in the string.
     *
     * @param utf8Text Null-terminated UTF-8 string
     * @return Total width in pixels
     */
    uint16_t getTextWidth(const char* utf8Text) const;

    /**
     * @brief Get the line height of the current font.
     * @return Line height in pixels, or 0 if no font is set
     */
    uint8_t getLineHeight() const;

    /**
     * @brief Get the baseline offset of the current font.
     * @return Baseline offset from top of line in pixels
     */
    uint8_t getBaseline() const;

    /**
     * @brief Get the default character width of the current font.
     * @return Default width in pixels
     */
    uint8_t getDefaultWidth() const;

    /**
     * @brief Check if a font is currently set.
     * @return true if a font is active
     */
    bool hasFont() const;

private:
    const VNFont* font_;  /**< Pointer to the active font descriptor */

    /**
     * @brief Read a VNGlyphEntry from PROGMEM.
     * @param index Glyph index in the font's glyph table
     * @param[out] entry Glyph entry to populate
     */
    void readGlyphEntry(uint16_t index, VNGlyphEntry& entry) const;
};

#endif /* VN_FONT_ENGINE_H */
