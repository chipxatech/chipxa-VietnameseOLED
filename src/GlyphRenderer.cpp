/**
 * @file GlyphRenderer.cpp
 * @brief Glyph transformer and blender implementation.
 *
 * Applies scale, mirror flips, and 90-degree rotation transforms during pixel-by-pixel glyph drawing.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#include "GlyphRenderer.h"
#include "drivers/DisplayAdapter.h"

/* ─────────────────────────────────────────────────────────────────────────────
 * Constants
 * ────────────────────────────────────────────────────────────────────────── */

static constexpr uint8_t MIN_SCALE = 1;
static constexpr uint8_t MAX_SCALE = 4;

/* ─────────────────────────────────────────────────────────────────────────────
 * Constructor
 * ────────────────────────────────────────────────────────────────────────── */

GlyphRenderer::GlyphRenderer()
    : bitmapRenderer_(nullptr)
    , adapter_(nullptr)
    , renderMode_(VNRenderMode::TRANSPARENT)
    , scaleX_(1)
    , scaleY_(1)
    , rotation_(0)
    , mirrorH_(false)
    , mirrorV_(false)
{
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Setup
 * ────────────────────────────────────────────────────────────────────────── */

void GlyphRenderer::setTarget(DisplayAdapter* adapter) {
    adapter_ = adapter;
}

void GlyphRenderer::setBitmapRenderer(BitmapRenderer* renderer) {
    bitmapRenderer_ = renderer;
}

void GlyphRenderer::setRenderMode(VNRenderMode mode) {
    renderMode_ = mode;
}

VNRenderMode GlyphRenderer::getRenderMode() const {
    return renderMode_;
}

void GlyphRenderer::setScale(uint8_t scaleX, uint8_t scaleY) {
    scaleX_ = clampScale(scaleX);
    scaleY_ = clampScale(scaleY);
}

void GlyphRenderer::setRotation(uint16_t degrees) {
    rotation_ = normalizeRotation(degrees);
}

void GlyphRenderer::setMirror(bool horizontal, bool vertical) {
    mirrorH_ = horizontal;
    mirrorV_ = vertical;
}

void GlyphRenderer::resetTransforms() {
    renderMode_ = VNRenderMode::TRANSPARENT;
    scaleX_     = 1;
    scaleY_     = 1;
    rotation_   = 0;
    mirrorH_    = false;
    mirrorV_    = false;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Render Glyph — Dispatch
 * ────────────────────────────────────────────────────────────────────────── */

void GlyphRenderer::renderGlyph(int16_t x, int16_t y, const VNGlyphInfo& glyph,
                                  VNColor color)
{
    if (!glyph.valid || glyph.bitmap == nullptr || bitmapRenderer_ == nullptr) {
        return;
    }

    /* Fast path: no transforms needed */
    if (scaleX_ == 1 && scaleY_ == 1 && rotation_ == 0 &&
        !mirrorH_ && !mirrorV_) {
        renderGlyphDirect(x, y, glyph, color);
    } else {
        renderGlyphTransformed(x, y, glyph, color);
    }
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Direct Render (No Transforms) — Fast Path
 * ────────────────────────────────────────────────────────────────────────── */

void GlyphRenderer::renderGlyphDirect(int16_t x, int16_t y,
                                        const VNGlyphInfo& glyph,
                                        VNColor color)
{
    /* Apply glyph offsets */
    int16_t drawX = x + glyph.xOffset;
    int16_t drawY = y + glyph.yOffset;

    /* Delegate to bitmap renderer which handles clipping and PROGMEM reads */
    bitmapRenderer_->drawBitmap(drawX, drawY, glyph.bitmap,
                                glyph.width, glyph.height,
                                color, renderMode_);
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Transformed Render — Pixel-by-Pixel with Scale/Rotate/Mirror
 * ────────────────────────────────────────────────────────────────────────── */

void GlyphRenderer::renderGlyphTransformed(int16_t x, int16_t y,
                                             const VNGlyphInfo& glyph,
                                             VNColor color)
{
    if (adapter_ == nullptr) return;

    const int16_t drawX = x + glyph.xOffset;
    const int16_t drawY = y + glyph.yOffset;
    const uint16_t srcW = glyph.width;
    const uint16_t srcH = glyph.height;

    /* Iterate over every pixel in the source glyph */
    for (uint16_t srcRow = 0; srcRow < srcH; srcRow++) {
        for (uint16_t srcCol = 0; srcCol < srcW; srcCol++) {
            /* Apply mirror to source coordinates */
            uint16_t readCol = mirrorH_ ? (srcW - 1 - srcCol) : srcCol;
            uint16_t readRow = mirrorV_ ? (srcH - 1 - srcRow) : srcRow;

            bool pixelSet = readGlyphPixel(glyph, readCol, readRow);

            /* Skip background pixels in transparent mode */
            if (!pixelSet && renderMode_ == VNRenderMode::TRANSPARENT) {
                continue;
            }

            /* Apply rotation to get destination offset */
            int16_t destOffX, destOffY;
            switch (rotation_) {
                case 90:
                    destOffX = static_cast<int16_t>(srcH - 1 - srcRow);
                    destOffY = static_cast<int16_t>(srcCol);
                    break;
                case 180:
                    destOffX = static_cast<int16_t>(srcW - 1 - srcCol);
                    destOffY = static_cast<int16_t>(srcH - 1 - srcRow);
                    break;
                case 270:
                    destOffX = static_cast<int16_t>(srcRow);
                    destOffY = static_cast<int16_t>(srcW - 1 - srcCol);
                    break;
                default: /* 0 degrees */
                    destOffX = static_cast<int16_t>(srcCol);
                    destOffY = static_cast<int16_t>(srcRow);
                    break;
            }

            /* Apply scaling — fill a scaleX × scaleY block for each pixel */
            for (uint8_t sy = 0; sy < scaleY_; sy++) {
                for (uint8_t sx = 0; sx < scaleX_; sx++) {
                    int16_t px = drawX + destOffX * scaleX_ + sx;
                    int16_t py = drawY + destOffY * scaleY_ + sy;

                    if (pixelSet) {
                        VNColor drawColor = (renderMode_ == VNRenderMode::INVERT)
                                          ? VNColor::INVERSE
                                          : color;
                        bitmapRenderer_->drawPixel(px, py, drawColor);
                    }
                    else if (renderMode_ == VNRenderMode::OVERWRITE) {
                        VNColor bgColor = (color == VNColor::WHITE)
                                        ? VNColor::BLACK : VNColor::WHITE;
                        bitmapRenderer_->drawPixel(px, py, bgColor);
                    }
                }
            }
        }
    }
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Read Pixel from Glyph Bitmap (PROGMEM-safe)
 * ────────────────────────────────────────────────────────────────────────── */

bool GlyphRenderer::readGlyphPixel(const VNGlyphInfo& glyph,
                                     uint16_t col, uint16_t row) const
{
    if (col >= glyph.width || row >= glyph.height) {
        return false;
    }

    uint16_t bytesPerRow = (glyph.width + 7) / 8;
    uint16_t byteIndex   = row * bytesPerRow + (col >> 3);
    uint8_t  bitIndex    = 7 - (col & 0x07);
    uint8_t  bitmapByte  = VN_READ_BYTE(&glyph.bitmap[byteIndex]);

    return (bitmapByte >> bitIndex) & 0x01;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Utility
 * ────────────────────────────────────────────────────────────────────────── */

uint8_t GlyphRenderer::clampScale(uint8_t s) {
    if (s < MIN_SCALE) return MIN_SCALE;
    if (s > MAX_SCALE) return MAX_SCALE;
    return s;
}

uint16_t GlyphRenderer::normalizeRotation(uint16_t degrees) {
    uint16_t normalized = degrees % 360;
    /* Snap to nearest 90° increment */
    if (normalized < 45)  return 0;
    if (normalized < 135) return 90;
    if (normalized < 225) return 180;
    if (normalized < 315) return 270;
    return 0;
}
