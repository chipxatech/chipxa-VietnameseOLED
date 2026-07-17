/**
 * @file BitmapRenderer.cpp
 * @brief Bresenham-based low-level graphics renderer implementation.
 *
 * Coordinates coordinate viewport boundaries, clips out-of-bounds drawing, and performs direct writes to framebuffers.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#include "BitmapRenderer.h"
#include "drivers/DisplayAdapter.h"

/* ─────────────────────────────────────────────────────────────────────────────
 * Constructor
 * ────────────────────────────────────────────────────────────────────────── */

BitmapRenderer::BitmapRenderer()
    : adapter_(nullptr)
{
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Target Setup
 * ────────────────────────────────────────────────────────────────────────── */

void BitmapRenderer::setTarget(DisplayAdapter* adapter) {
    adapter_ = adapter;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Bounds Check
 * ────────────────────────────────────────────────────────────────────────── */

bool BitmapRenderer::inBounds(int16_t x, int16_t y) const {
    if (adapter_ == nullptr) return false;
    return x >= 0 && x < static_cast<int16_t>(adapter_->getWidth()) &&
           y >= 0 && y < static_cast<int16_t>(adapter_->getHeight());
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Draw Pixel
 * ────────────────────────────────────────────────────────────────────────── */

void BitmapRenderer::drawPixel(int16_t x, int16_t y, VNColor color) {
    if (!inBounds(x, y)) return;
    adapter_->setPixel(x, y, static_cast<uint8_t>(color));
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Draw Bitmap (1-bit, row-major, MSB-first, from PROGMEM)
 * ────────────────────────────────────────────────────────────────────────── */

void BitmapRenderer::drawBitmap(int16_t x, int16_t y, const uint8_t* bitmap,
                                 uint16_t w, uint16_t h, VNColor color,
                                 VNRenderMode mode)
{
    if (adapter_ == nullptr || bitmap == nullptr) return;

    const int16_t dispW = static_cast<int16_t>(adapter_->getWidth());
    const int16_t dispH = static_cast<int16_t>(adapter_->getHeight());
    const uint16_t bytesPerRow = (w + 7) / 8;

    for (uint16_t row = 0; row < h; row++) {
        int16_t py = y + static_cast<int16_t>(row);
        if (py < 0) continue;
        if (py >= dispH) break;

        for (uint16_t col = 0; col < w; col++) {
            int16_t px = x + static_cast<int16_t>(col);
            if (px < 0) continue;
            if (px >= dispW) break;

            /* Read pixel bit from bitmap (row-major, MSB-first) */
            uint16_t byteIndex = row * bytesPerRow + (col >> 3);
            uint8_t bitIndex   = 7 - (col & 0x07);
            uint8_t bitmapByte = VN_READ_BYTE(&bitmap[byteIndex]);
            bool    pixelSet   = (bitmapByte >> bitIndex) & 0x01;

            switch (mode) {
                case VNRenderMode::TRANSPARENT:
                    /* Only draw foreground pixels */
                    if (pixelSet) {
                        adapter_->setPixel(px, py, static_cast<uint8_t>(color));
                    }
                    break;

                case VNRenderMode::OVERWRITE:
                    /* Draw foreground and background */
                    if (pixelSet) {
                        adapter_->setPixel(px, py, static_cast<uint8_t>(color));
                    } else {
                        /* Clear background pixel */
                        uint8_t bgColor = (color == VNColor::WHITE)
                                        ? static_cast<uint8_t>(VNColor::BLACK)
                                        : static_cast<uint8_t>(VNColor::WHITE);
                        adapter_->setPixel(px, py, bgColor);
                    }
                    break;

                case VNRenderMode::INVERT:
                    /* XOR mode — toggle pixel if set in bitmap */
                    if (pixelSet) {
                        adapter_->setPixel(px, py, static_cast<uint8_t>(VNColor::INVERSE));
                    }
                    break;
            }
        }
    }
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Fill Rectangle
 * ────────────────────────────────────────────────────────────────────────── */

void BitmapRenderer::fillRect(int16_t x, int16_t y, uint16_t w, uint16_t h,
                               VNColor color)
{
    if (adapter_ == nullptr) return;

    const int16_t dispW = static_cast<int16_t>(adapter_->getWidth());
    const int16_t dispH = static_cast<int16_t>(adapter_->getHeight());

    /* Clip to display bounds */
    int16_t x1 = x;
    int16_t y1 = y;
    int16_t x2 = x + static_cast<int16_t>(w);
    int16_t y2 = y + static_cast<int16_t>(h);

    if (x1 < 0) x1 = 0;
    if (y1 < 0) y1 = 0;
    if (x2 > dispW) x2 = dispW;
    if (y2 > dispH) y2 = dispH;

    if (x1 >= x2 || y1 >= y2) return;

    for (int16_t py = y1; py < y2; py++) {
        for (int16_t px = x1; px < x2; px++) {
            adapter_->setPixel(px, py, static_cast<uint8_t>(color));
        }
    }
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Draw Horizontal Line
 * ────────────────────────────────────────────────────────────────────────── */

void BitmapRenderer::drawHLine(int16_t x, int16_t y, uint16_t w, VNColor color) {
    if (adapter_ == nullptr) return;

    const int16_t dispW = static_cast<int16_t>(adapter_->getWidth());
    const int16_t dispH = static_cast<int16_t>(adapter_->getHeight());

    if (y < 0 || y >= dispH) return;

    int16_t x1 = x;
    int16_t x2 = x + static_cast<int16_t>(w);
    if (x1 < 0) x1 = 0;
    if (x2 > dispW) x2 = dispW;

    for (int16_t px = x1; px < x2; px++) {
        adapter_->setPixel(px, y, static_cast<uint8_t>(color));
    }
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Draw Vertical Line
 * ────────────────────────────────────────────────────────────────────────── */

void BitmapRenderer::drawVLine(int16_t x, int16_t y, uint16_t h, VNColor color) {
    if (adapter_ == nullptr) return;

    const int16_t dispW = static_cast<int16_t>(adapter_->getWidth());
    const int16_t dispH = static_cast<int16_t>(adapter_->getHeight());

    if (x < 0 || x >= dispW) return;

    int16_t y1 = y;
    int16_t y2 = y + static_cast<int16_t>(h);
    if (y1 < 0) y1 = 0;
    if (y2 > dispH) y2 = dispH;

    for (int16_t py = y1; py < y2; py++) {
        adapter_->setPixel(x, py, static_cast<uint8_t>(color));
    }
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Draw Line — Bresenham's Algorithm
 * ────────────────────────────────────────────────────────────────────────── */

void BitmapRenderer::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                               VNColor color)
{
    /* Special cases for horizontal and vertical lines */
    if (y0 == y1) {
        if (x0 > x1) { int16_t t = x0; x0 = x1; x1 = t; }
        drawHLine(x0, y0, static_cast<uint16_t>(x1 - x0 + 1), color);
        return;
    }
    if (x0 == x1) {
        if (y0 > y1) { int16_t t = y0; y0 = y1; y1 = t; }
        drawVLine(x0, y0, static_cast<uint16_t>(y1 - y0 + 1), color);
        return;
    }

    /* Bresenham's line algorithm */
    bool steep = false;
    int16_t dx = x1 - x0;
    int16_t dy = y1 - y0;
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;

    if (dy > dx) {
        steep = true;
        int16_t t;
        t = x0; x0 = y0; y0 = t;
        t = x1; x1 = y1; y1 = t;
    }

    if (x0 > x1) {
        int16_t t;
        t = x0; x0 = x1; x1 = t;
        t = y0; y0 = y1; y1 = t;
    }

    dx = x1 - x0;
    dy = y1 - y0;
    if (dy < 0) dy = -dy;
    int16_t err = dx / 2;
    int16_t ystep = (y0 < y1) ? 1 : -1;
    int16_t cy = y0;

    for (int16_t cx = x0; cx <= x1; cx++) {
        if (steep) {
            drawPixel(cy, cx, color);
        } else {
            drawPixel(cx, cy, color);
        }
        err -= dy;
        if (err < 0) {
            cy += ystep;
            err += dx;
        }
    }
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Draw Rectangle Outline
 * ────────────────────────────────────────────────────────────────────────── */

void BitmapRenderer::drawRect(int16_t x, int16_t y, uint16_t w, uint16_t h,
                               VNColor color)
{
    drawHLine(x, y, w, color);                                          /* Top */
    drawHLine(x, y + static_cast<int16_t>(h) - 1, w, color);          /* Bottom */
    drawVLine(x, y, h, color);                                          /* Left */
    drawVLine(x + static_cast<int16_t>(w) - 1, y, h, color);          /* Right */
}
