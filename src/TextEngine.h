/**
 * @file TextEngine.h
 * @brief Main print pipeline orchestrator.
 *
 * Wires together layout calculations, font loaders, caches, and display driver interfaces.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#ifndef VN_TEXT_ENGINE_H
#define VN_TEXT_ENGINE_H

#include "VNConfig.h"
#include "VNTypes.h"
#include "FontEngine.h"
#include "GlyphCache.h"
#include "GlyphRenderer.h"
#include "BitmapRenderer.h"
#include "LayoutEngine.h"
#include "TextAlignment.h"
#include "WordWrapper.h"

class DisplayAdapter;

/**
 * @class TextEngine
 * @brief Orchestrates the complete UTF-8 → pixel rendering pipeline.
 */
class TextEngine {
public:
    TextEngine();

    /** @name Component Setup */
    /** @{ */
    void setDisplayAdapter(DisplayAdapter* adapter);
    void setFontEngine(FontEngine* fontEngine);
    void setLayoutEngine(LayoutEngine* layout);
    void setGlyphRenderer(GlyphRenderer* renderer);
    void setBitmapRenderer(BitmapRenderer* bitmapRenderer);
    void setGlyphCache(GlyphCache* cache);
    /** @} */

    /**
     * @brief Draw UTF-8 text at an absolute position.
     *
     * Decodes the string, looks up glyphs, and renders them left-to-right
     * starting at (x, y). No wrapping or alignment is applied.
     *
     * @param x     X position
     * @param y     Y position
     * @param text  Null-terminated UTF-8 string
     * @param color Drawing color
     */
    void drawText(int16_t x, int16_t y, const char* text, VNColor color);

    /**
     * @brief Draw UTF-8 text within a rectangle, with wrapping and alignment.
     *
     * Uses the layout engine's current settings for wrap, alignment, and spacing.
     *
     * @param rect  Bounding rectangle
     * @param text  Null-terminated UTF-8 string
     * @param color Drawing color
     */
    void drawTextInRect(const VNRect& rect, const char* text, VNColor color);

    /**
     * @brief Measure the rendered size of a UTF-8 string.
     * @param text Null-terminated UTF-8 string
     * @return Bounding rectangle of the rendered text
     */
    VNRect measureText(const char* text) const;

    /** @name Cursor-Based Printing */
    /** @{ */
    void setCursor(int16_t x, int16_t y);
    int16_t getCursorX() const;
    int16_t getCursorY() const;

    /**
     * @brief Print text at the current cursor, advancing the cursor.
     */
    void print(const char* text, VNColor color);

    /**
     * @brief Print text at cursor and move cursor to next line.
     */
    void println(const char* text, VNColor color);
    /** @} */

private:
    DisplayAdapter*  adapter_;         /**< Display target */
    FontEngine*      fontEngine_;      /**< Font data source */
    LayoutEngine*    layoutEngine_;    /**< Layout manager */
    GlyphRenderer*   glyphRenderer_;   /**< Glyph drawer */
    BitmapRenderer*  bitmapRenderer_;  /**< Pixel-level drawer */
    GlyphCache*      glyphCache_;      /**< Glyph cache (optional) */
    int16_t          cursorX_;         /**< Current cursor X */
    int16_t          cursorY_;         /**< Current cursor Y */

    /**
     * @brief Get glyph info, using cache if available.
     * @param codepoint Unicode codepoint
     * @param[out] info Glyph info to populate
     * @return true if glyph was found
     */
    bool getCachedGlyph(uint32_t codepoint, VNGlyphInfo& info);

    /**
     * @brief Draw a single line of text (already decoded/wrapped).
     */
    void drawLineFromSource(int16_t x, int16_t y, const char* text,
                            uint16_t byteLen, VNColor color);
};

#endif /* VN_TEXT_ENGINE_H */
