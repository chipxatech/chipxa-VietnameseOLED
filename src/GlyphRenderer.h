/**
 * @file GlyphRenderer.h
 * @brief Glyph transform and pixel assembly subsystem.
 *
 * Defines coordinates rendering modes, scaling settings, mirror flips, and rotation configurations.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#ifndef VN_GLYPH_RENDERER_H
#define VN_GLYPH_RENDERER_H

#include "VNConfig.h"
#include "VNTypes.h"
#include "BitmapRenderer.h"

/* Forward declaration */
class DisplayAdapter;

/**
 * @class GlyphRenderer
 * @brief Renders glyph bitmaps with optional transformations.
 *
 * Supports integer scaling (1x–4x), 90° rotation increments,
 * horizontal/vertical mirroring, and three render modes.
 */
class GlyphRenderer {
public:
    /** @brief Construct a GlyphRenderer with default settings. */
    GlyphRenderer();

    /**
     * @brief Set the display adapter.
     * @param adapter Pointer to the display adapter
     */
    void setTarget(DisplayAdapter* adapter);

    /**
     * @brief Set the bitmap renderer for pixel operations.
     * @param renderer Pointer to BitmapRenderer
     */
    void setBitmapRenderer(BitmapRenderer* renderer);

    /**
     * @brief Set the rendering mode.
     * @param mode Render mode (TRANSPARENT, OVERWRITE, INVERT)
     */
    void setRenderMode(VNRenderMode mode);

    /**
     * @brief Get the current rendering mode.
     * @return Current VNRenderMode
     */
    VNRenderMode getRenderMode() const;

    /**
     * @brief Set integer scale factors.
     * @param scaleX Horizontal scale (1–4)
     * @param scaleY Vertical scale (1–4)
     */
    void setScale(uint8_t scaleX, uint8_t scaleY);

    /**
     * @brief Set rotation angle (0, 90, 180, 270 degrees).
     * @param degrees Rotation in degrees (rounded to nearest 90)
     */
    void setRotation(uint16_t degrees);

    /**
     * @brief Set mirror/flip flags.
     * @param horizontal Mirror horizontally
     * @param vertical   Mirror vertically
     */
    void setMirror(bool horizontal, bool vertical);

    /**
     * @brief Render a glyph at the given position.
     *
     * Applies current scale, rotation, mirror, and render mode settings.
     *
     * @param x     X position (left edge before transforms)
     * @param y     Y position (top edge before transforms)
     * @param glyph Glyph information (must be valid)
     * @param color Drawing color
     */
    void renderGlyph(int16_t x, int16_t y, const VNGlyphInfo& glyph, VNColor color);

    /**
     * @brief Reset all transform settings to defaults.
     *
     * Scale=1x1, rotation=0, mirror=none, mode=TRANSPARENT.
     */
    void resetTransforms();

private:
    BitmapRenderer* bitmapRenderer_;  /**< Pixel-level renderer */
    DisplayAdapter* adapter_;         /**< Target display */
    VNRenderMode    renderMode_;      /**< Current render mode */
    uint8_t         scaleX_;          /**< Horizontal scale factor (1–4) */
    uint8_t         scaleY_;          /**< Vertical scale factor (1–4) */
    uint16_t        rotation_;        /**< Rotation in degrees (0/90/180/270) */
    bool            mirrorH_;         /**< Horizontal mirror flag */
    bool            mirrorV_;         /**< Vertical mirror flag */

    /**
     * @brief Render without scaling (fast path).
     */
    void renderGlyphDirect(int16_t x, int16_t y, const VNGlyphInfo& glyph, VNColor color);

    /**
     * @brief Render with scaling and/or transforms.
     */
    void renderGlyphTransformed(int16_t x, int16_t y, const VNGlyphInfo& glyph, VNColor color);

    /**
     * @brief Read a pixel from a glyph bitmap.
     * @param glyph Glyph info
     * @param col   Column (0-based)
     * @param row   Row (0-based)
     * @return true if the pixel is set
     */
    bool readGlyphPixel(const VNGlyphInfo& glyph, uint16_t col, uint16_t row) const;

    /** @brief Clamp scale to valid range. */
    static uint8_t clampScale(uint8_t s);

    /** @brief Normalize rotation to 0/90/180/270. */
    static uint16_t normalizeRotation(uint16_t degrees);
};

#endif /* VN_GLYPH_RENDERER_H */
