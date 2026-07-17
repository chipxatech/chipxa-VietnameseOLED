/**
 * @file TextEngine.cpp
 * @brief Execution driver for the print pipeline.
 *
 * Orchestrates text parsing, wrapper calculations, cache checks, rendering transforms, and canvas writes.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#include "TextEngine.h"
#include "UTF8Decoder.h"
#include "drivers/DisplayAdapter.h"

/* ─────────────────────────────────────────────────────────────────────────────
 * Constructor
 * ────────────────────────────────────────────────────────────────────────── */

TextEngine::TextEngine()
    : adapter_(nullptr)
    , fontEngine_(nullptr)
    , layoutEngine_(nullptr)
    , glyphRenderer_(nullptr)
    , bitmapRenderer_(nullptr)
    , glyphCache_(nullptr)
    , cursorX_(0)
    , cursorY_(0)
{
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Component Setup
 * ────────────────────────────────────────────────────────────────────────── */

void TextEngine::setDisplayAdapter(DisplayAdapter* adapter) { adapter_ = adapter; }
void TextEngine::setFontEngine(FontEngine* fontEngine) { fontEngine_ = fontEngine; }
void TextEngine::setLayoutEngine(LayoutEngine* layout) { layoutEngine_ = layout; }
void TextEngine::setGlyphRenderer(GlyphRenderer* renderer) { glyphRenderer_ = renderer; }
void TextEngine::setBitmapRenderer(BitmapRenderer* bitmapRenderer) { bitmapRenderer_ = bitmapRenderer; }
void TextEngine::setGlyphCache(GlyphCache* cache) { glyphCache_ = cache; }

/* ─────────────────────────────────────────────────────────────────────────────
 * Draw Text at Absolute Position
 * ────────────────────────────────────────────────────────────────────────── */

void TextEngine::drawText(int16_t x, int16_t y, const char* text, VNColor color) {
    if (text == nullptr || fontEngine_ == nullptr || glyphRenderer_ == nullptr) {
        return;
    }

    UTF8Decoder decoder;
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(text);
    int16_t cx = x;

    while (*ptr != 0) {
        UTF8Decoder::Status status = decoder.decode(*ptr);
        ptr++;

        if (status == UTF8Decoder::Status::ACCEPT) {
            uint32_t cp = decoder.getCodepoint();

            /* Handle newline */
            if (cp == '\n') {
                cx = x;
                y += fontEngine_->getLineHeight();
                if (layoutEngine_ != nullptr) {
                    y += layoutEngine_->getLineSpacing();
                }
                continue;
            }

            /* Get glyph (cached or direct) */
            VNGlyphInfo info;
            if (getCachedGlyph(cp, info)) {
                glyphRenderer_->renderGlyph(cx, y, info, color);
                cx += info.xAdvance;
            } else {
                /* Glyph not found — advance by default width */
                cx += fontEngine_->getDefaultWidth();
            }
        }
    }
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Draw Text in Rectangle (with Layout)
 * ────────────────────────────────────────────────────────────────────────── */

void TextEngine::drawTextInRect(const VNRect& rect, const char* text, VNColor color) {
    if (text == nullptr || fontEngine_ == nullptr ||
        glyphRenderer_ == nullptr || layoutEngine_ == nullptr) {
        return;
    }

    /* Save and set viewport */
    VNRect prevViewport = layoutEngine_->getViewport();
    layoutEngine_->setViewport(rect);

    /* Layout text into lines */
    WordWrapper::LineInfo lines[VN_MAX_WRAP_LINES];
    uint8_t lineCount = layoutEngine_->layoutText(text, *fontEngine_,
                                                    lines, VN_MAX_WRAP_LINES);

    if (lineCount == 0) {
        layoutEngine_->setViewport(prevViewport);
        return;
    }

    /* Calculate total text height for vertical alignment */
    uint8_t lineHeight = fontEngine_->getLineHeight();
    uint8_t lineSpacing = layoutEngine_->getLineSpacing();
    uint16_t totalHeight = static_cast<uint16_t>(lineCount) * lineHeight;
    if (lineCount > 1) {
        totalHeight += static_cast<uint16_t>(lineCount - 1) * lineSpacing;
    }

    /* Get content area */
    VNPoint origin = layoutEngine_->getContentOrigin();
    uint16_t contentWidth  = layoutEngine_->getContentWidth();
    uint16_t contentHeight = layoutEngine_->getContentHeight();

    /* Vertical alignment offset */
    int16_t yOffset = TextAlignment::calculateY(totalHeight, contentHeight,
                                                  layoutEngine_->getVAlign());

    /* Draw each line */
    int16_t lineY = origin.y + yOffset;
    VNAlign hAlign = layoutEngine_->getHAlign();

    for (uint8_t i = 0; i < lineCount; i++) {
        /* Horizontal alignment offset */
        int16_t xOffset = TextAlignment::calculateX(lines[i].pixelWidth,
                                                      contentWidth, hAlign);
        int16_t lineX = origin.x + xOffset;

        /* Draw this line */
        drawLineFromSource(lineX, lineY, text + lines[i].startIndex,
                           lines[i].byteLength, color);

        lineY += lineHeight + lineSpacing;
    }

    /* Restore viewport */
    layoutEngine_->setViewport(prevViewport);
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Measure Text
 * ────────────────────────────────────────────────────────────────────────── */

VNRect TextEngine::measureText(const char* text) const {
    if (text == nullptr || fontEngine_ == nullptr) {
        return VNRect(0, 0, 0, 0);
    }

    if (layoutEngine_ != nullptr) {
        return layoutEngine_->calculateTextBounds(text, *fontEngine_);
    }

    /* No layout engine — simple measurement */
    uint16_t w = fontEngine_->getTextWidth(text);
    uint8_t h = fontEngine_->getLineHeight();
    return VNRect(0, 0, w, h);
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Cursor Management
 * ────────────────────────────────────────────────────────────────────────── */

void TextEngine::setCursor(int16_t x, int16_t y) {
    cursorX_ = x;
    cursorY_ = y;
}

int16_t TextEngine::getCursorX() const { return cursorX_; }
int16_t TextEngine::getCursorY() const { return cursorY_; }

void TextEngine::print(const char* text, VNColor color) {
    drawText(cursorX_, cursorY_, text, color);
    /* Advance cursor */
    if (fontEngine_ != nullptr) {
        cursorX_ += static_cast<int16_t>(fontEngine_->getTextWidth(text));
    }
}

void TextEngine::println(const char* text, VNColor color) {
    drawText(cursorX_, cursorY_, text, color);
    /* Move cursor to next line */
    cursorX_ = 0;
    if (fontEngine_ != nullptr) {
        cursorY_ += fontEngine_->getLineHeight();
        if (layoutEngine_ != nullptr) {
            cursorY_ += layoutEngine_->getLineSpacing();
        }
    }
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Internal — Cached Glyph Lookup
 * ────────────────────────────────────────────────────────────────────────── */

bool TextEngine::getCachedGlyph(uint32_t codepoint, VNGlyphInfo& info) {
    /* Try cache first */
    if (glyphCache_ != nullptr && glyphCache_->find(codepoint, info)) {
        return true;
    }

    /* Cache miss — look up from font */
    if (fontEngine_->getGlyph(codepoint, info)) {
        /* Store in cache for next time */
        if (glyphCache_ != nullptr) {
            glyphCache_->insert(info);
        }
        return true;
    }

    return false;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Internal — Draw Line from Source String
 * ────────────────────────────────────────────────────────────────────────── */

void TextEngine::drawLineFromSource(int16_t x, int16_t y, const char* text,
                                      uint16_t byteLen, VNColor color)
{
    if (text == nullptr || fontEngine_ == nullptr || glyphRenderer_ == nullptr) {
        return;
    }

    UTF8Decoder decoder;
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(text);
    int16_t cx = x;

    for (uint16_t i = 0; i < byteLen; i++) {
        UTF8Decoder::Status status = decoder.decode(ptr[i]);

        if (status == UTF8Decoder::Status::ACCEPT) {
            uint32_t cp = decoder.getCodepoint();

            VNGlyphInfo info;
            if (getCachedGlyph(cp, info)) {
                glyphRenderer_->renderGlyph(cx, y, info, color);
                cx += info.xAdvance;
            } else {
                cx += fontEngine_->getDefaultWidth();
            }
        }
    }
}
