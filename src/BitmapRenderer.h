/**
 * @file BitmapRenderer.h
 * @brief Low-level geometry and pixel drawing engine definition.
 *
 * Provides basic functions to draw points, draw lines, fill rects, and write bitmap grids to display buffers.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#ifndef VN_BITMAP_RENDERER_H
#define VN_BITMAP_RENDERER_H

#include "VNConfig.h"
#include "VNTypes.h"

/* Forward declaration */
class DisplayAdapter;

/**
 * @class BitmapRenderer
 * @brief Draws pixels, rectangles, and bitmaps onto a DisplayAdapter framebuffer.
 */
class BitmapRenderer {
public:
    /** @brief Construct a BitmapRenderer with no target. */
    BitmapRenderer();

    /**
     * @brief Set the display adapter to render into.
     * @param adapter Pointer to the display adapter
     */
    void setTarget(DisplayAdapter* adapter);

    /**
     * @brief Draw a single pixel.
     * @param x     X coordinate
     * @param y     Y coordinate
     * @param color Pixel color
     */
    void drawPixel(int16_t x, int16_t y, VNColor color);

    /**
     * @brief Draw a 1-bit bitmap from PROGMEM.
     *
     * Bitmap format: row-major, MSB-first. Each row is padded to byte boundary.
     *
     * @param x      Top-left X coordinate
     * @param y      Top-left Y coordinate
     * @param bitmap Pointer to bitmap data (may be in PROGMEM)
     * @param w      Bitmap width in pixels
     * @param h      Bitmap height in pixels
     * @param color  Foreground color for set bits
     * @param mode   Rendering mode (transparent, overwrite, invert)
     */
    void drawBitmap(int16_t x, int16_t y, const uint8_t* bitmap,
                    uint16_t w, uint16_t h, VNColor color, VNRenderMode mode);

    /**
     * @brief Fill a rectangle with a solid color.
     * @param x     Top-left X coordinate
     * @param y     Top-left Y coordinate
     * @param w     Width in pixels
     * @param h     Height in pixels
     * @param color Fill color
     */
    void fillRect(int16_t x, int16_t y, uint16_t w, uint16_t h, VNColor color);

    /**
     * @brief Draw a horizontal line.
     * @param x     Start X coordinate
     * @param y     Y coordinate
     * @param w     Length in pixels
     * @param color Line color
     */
    void drawHLine(int16_t x, int16_t y, uint16_t w, VNColor color);

    /**
     * @brief Draw a vertical line.
     * @param x     X coordinate
     * @param y     Start Y coordinate
     * @param h     Length in pixels
     * @param color Line color
     */
    void drawVLine(int16_t x, int16_t y, uint16_t h, VNColor color);

    /**
     * @brief Draw a line between two points using Bresenham's algorithm.
     * @param x0    Start X
     * @param y0    Start Y
     * @param x1    End X
     * @param y1    End Y
     * @param color Line color
     */
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, VNColor color);

    /**
     * @brief Draw an unfilled rectangle outline.
     * @param x     Top-left X
     * @param y     Top-left Y
     * @param w     Width
     * @param h     Height
     * @param color Outline color
     */
    void drawRect(int16_t x, int16_t y, uint16_t w, uint16_t h, VNColor color);

private:
    DisplayAdapter* adapter_;  /**< Target display adapter */

    /**
     * @brief Check if coordinates are within display bounds.
     * @param x X coordinate
     * @param y Y coordinate
     * @return true if within bounds
     */
    bool inBounds(int16_t x, int16_t y) const;
};

#endif /* VN_BITMAP_RENDERER_H */
