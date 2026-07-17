/**
 * @file VietnameseOLED.h
 * @brief Main user-facing API facade for the VietnameseOLED library.
 *
 * Orchestrates internal decoding, layout, caching, and rendering subsystems behind a simplified interface. Compatible with standard Arduino printing conventions.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#ifndef VIETNAMESE_OLED_H
#define VIETNAMESE_OLED_H

#include "VNConfig.h"
#include "VNTypes.h"
#include "UTF8Decoder.h"
#include "UnicodeMapper.h"
#include "FontEngine.h"
#include "FontManager.h"
#include "GlyphCache.h"
#include "GlyphRenderer.h"
#include "BitmapRenderer.h"
#include "LayoutEngine.h"
#include "TextEngine.h"
#include "drivers/DisplayAdapter.h"

/**
 * @class VietnameseOLED
 * @brief Top-level API class for Vietnamese OLED text rendering.
 *
 * Wraps all internal subsystems (font engine, layout engine, text engine,
 * glyph cache, renderers) behind a clean, Arduino-friendly API.
 */
class VietnameseOLED {
public:
    /**
     * @brief Construct a VietnameseOLED instance.
     * @param display Reference to a DisplayAdapter (e.g., SSD1306Adapter)
     */
    explicit VietnameseOLED(DisplayAdapter& display);

    /* ── Lifecycle ─────────────────────────────────────────────────────── */

    /**
     * @brief Initialize the display and internal subsystems.
     * @return true if initialization succeeded
     */
    bool begin();

    /**
     * @brief Clear the framebuffer.
     */
    void clear();

    /**
     * @brief Push the framebuffer to the physical display.
     */
    void update();

    /* ── Font Selection ────────────────────────────────────────────────── */

    /**
     * @brief Set the active font.
     * @param font Reference to a VNFont descriptor (e.g., Font_8x16_VN)
     */
    void setFont(const VNFont& font);

    /* ── Drawing Properties ────────────────────────────────────────────── */

    /** @brief Set the drawing color. */
    void setColor(VNColor color);

    /** @brief Set horizontal text alignment. */
    void setAlign(VNAlign align);

    /** @brief Set vertical text alignment. */
    void setVAlign(VNVAlign align);

    /** @brief Set text wrapping mode. */
    void setTextWrap(VNTextWrap wrap);

    /** @brief Set extra spacing between lines (pixels). */
    void setLineSpacing(uint8_t pixels);

    /** @brief Set extra spacing between paragraphs (pixels). */
    void setParagraphSpacing(uint8_t pixels);

    /** @brief Set rendering mode (transparent/overwrite/invert). */
    void setRenderMode(VNRenderMode mode);

    /** @brief Set integer scale factor (1–4). */
    void setScale(uint8_t scale);

    /** @brief Set rotation (0, 90, 180, 270 degrees). */
    void setRotation(uint16_t degrees);

    /** @brief Set mirror/flip. */
    void setMirror(bool horizontal, bool vertical);

    /* ── Cursor ────────────────────────────────────────────────────────── */

    /** @brief Set the text cursor position. */
    void setCursor(int16_t x, int16_t y);

    /** @brief Get current cursor X position. */
    int16_t getCursorX() const;

    /** @brief Get current cursor Y position. */
    int16_t getCursorY() const;

    /* ── Text Drawing ──────────────────────────────────────────────────── */

    /**
     * @brief Draw UTF-8 text at an absolute position.
     * @param x    X position
     * @param y    Y position
     * @param text Null-terminated UTF-8 string
     */
    void drawUTF8(int16_t x, int16_t y, const char* text);

    /**
     * @brief Print Vietnamese text at the current cursor position.
     *
     * The cursor advances horizontally after printing.
     *
     * @param text Null-terminated UTF-8 string
     */
    void printVN(const char* text);

    /**
     * @brief Print Vietnamese text and move cursor to the next line.
     * @param text Null-terminated UTF-8 string
     */
    void printlnVN(const char* text);

    /**
     * @brief Formatted print with Vietnamese support (like printf).
     * @param fmt  Format string (printf-style)
     * @param ...  Format arguments
     */
    void printfVN(const char* fmt, ...);

    /* ── Text Measurement ──────────────────────────────────────────────── */

    /** @brief Get the rendered pixel width of a UTF-8 string. */
    uint16_t getTextWidth(const char* text) const;

    /** @brief Get the line height of the current font. */
    uint8_t getTextHeight() const;

    /** @brief Get the bounding rectangle of rendered text. */
    VNRect getTextBounds(const char* text) const;

    /* ── Primitive Drawing ─────────────────────────────────────────────── */

    /** @brief Draw a single pixel. */
    void drawPixel(int16_t x, int16_t y, VNColor color);

    /** @brief Draw a 1-bit bitmap. */
    void drawBitmap(int16_t x, int16_t y, const uint8_t* bitmap,
                    uint16_t w, uint16_t h);

    /** @brief Fill a rectangle. */
    void fillRect(int16_t x, int16_t y, uint16_t w, uint16_t h, VNColor color);

    /** @brief Draw a line. */
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, VNColor color);

    /** @brief Draw a rectangle outline. */
    void drawRect(int16_t x, int16_t y, uint16_t w, uint16_t h, VNColor color);

    /* ── Display Control ───────────────────────────────────────────────── */

    /** @brief Set display contrast/brightness (0–255). */
    void setContrast(uint8_t value);

    /** @brief Turn the display on. */
    void displayOn();

    /** @brief Turn the display off. */
    void displayOff();

    /** @brief Get display width in pixels. */
    uint16_t getWidth() const;

    /** @brief Get display height in pixels. */
    uint16_t getHeight() const;

private:
    DisplayAdapter&  display_;         /**< Display hardware adapter */
    FontEngine       fontEngine_;      /**< Font data access */
    FontManager      fontManager_;     /**< Multi-font registry */
    GlyphCache       glyphCache_;      /**< LRU glyph cache */
    GlyphRenderer    glyphRenderer_;   /**< Glyph drawing with transforms */
    BitmapRenderer   bitmapRenderer_;  /**< Pixel-level operations */
    LayoutEngine     layoutEngine_;    /**< Text layout manager */
    TextEngine       textEngine_;      /**< Pipeline orchestrator */
    VNColor          color_;           /**< Current drawing color */
    char             printfBuf_[VN_PRINTF_BUFFER_SIZE]; /**< Buffer for printfVN */
};

#endif /* VIETNAMESE_OLED_H */
