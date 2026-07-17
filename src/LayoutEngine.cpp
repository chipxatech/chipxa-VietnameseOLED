/**
 * @file LayoutEngine.cpp
 * @brief Doxygen-compatible layout processor implementation.
 *
 * Pre-calculates text layout lines and shapes text blocks within viewport and margin boundaries.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#include "LayoutEngine.h"
#include "FontEngine.h"

/* ─────────────────────────────────────────────────────────────────────────────
 * Constructor
 * ────────────────────────────────────────────────────────────────────────── */

LayoutEngine::LayoutEngine()
    : viewport_(0, 0, VN_MAX_DISPLAY_WIDTH, VN_MAX_DISPLAY_HEIGHT)
    , lineSpacing_(0)
    , paragraphSpacing_(0)
    , hAlign_(VNAlign::LEFT)
    , vAlign_(VNVAlign::TOP)
    , textWrap_(VNTextWrap::WORD)
{
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Viewport
 * ────────────────────────────────────────────────────────────────────────── */

void LayoutEngine::setViewport(const VNRect& viewport) {
    viewport_ = viewport;
}

const VNRect& LayoutEngine::getViewport() const {
    return viewport_;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Insets
 * ────────────────────────────────────────────────────────────────────────── */

void LayoutEngine::setPadding(uint8_t top, uint8_t right,
                               uint8_t bottom, uint8_t left)
{
    padding_ = VNEdgeInsets(top, right, bottom, left);
}

void LayoutEngine::setMargin(uint8_t top, uint8_t right,
                              uint8_t bottom, uint8_t left)
{
    margin_ = VNEdgeInsets(top, right, bottom, left);
}

const VNEdgeInsets& LayoutEngine::getPadding() const { return padding_; }
const VNEdgeInsets& LayoutEngine::getMargin() const { return margin_; }

/* ─────────────────────────────────────────────────────────────────────────────
 * Spacing
 * ────────────────────────────────────────────────────────────────────────── */

void LayoutEngine::setLineSpacing(uint8_t pixels) { lineSpacing_ = pixels; }
void LayoutEngine::setParagraphSpacing(uint8_t pixels) { paragraphSpacing_ = pixels; }
uint8_t LayoutEngine::getLineSpacing() const { return lineSpacing_; }
uint8_t LayoutEngine::getParagraphSpacing() const { return paragraphSpacing_; }

/* ─────────────────────────────────────────────────────────────────────────────
 * Alignment
 * ────────────────────────────────────────────────────────────────────────── */

void LayoutEngine::setAlignment(VNAlign hAlign, VNVAlign vAlign) {
    hAlign_ = hAlign;
    vAlign_ = vAlign;
}

VNAlign  LayoutEngine::getHAlign() const { return hAlign_; }
VNVAlign LayoutEngine::getVAlign() const { return vAlign_; }

/* ─────────────────────────────────────────────────────────────────────────────
 * Text Wrap
 * ────────────────────────────────────────────────────────────────────────── */

void LayoutEngine::setTextWrap(VNTextWrap wrap) { textWrap_ = wrap; }
VNTextWrap LayoutEngine::getTextWrap() const { return textWrap_; }

/* ─────────────────────────────────────────────────────────────────────────────
 * Content Area Calculations
 * ────────────────────────────────────────────────────────────────────────── */

uint16_t LayoutEngine::getContentWidth() const {
    uint16_t insets = margin_.horizontal() + padding_.horizontal();
    if (insets >= viewport_.width) return 0;
    return viewport_.width - insets;
}

uint16_t LayoutEngine::getContentHeight() const {
    uint16_t insets = margin_.vertical() + padding_.vertical();
    if (insets >= viewport_.height) return 0;
    return viewport_.height - insets;
}

VNPoint LayoutEngine::getContentOrigin() const {
    return VNPoint(
        viewport_.x + static_cast<int16_t>(margin_.left + padding_.left),
        viewport_.y + static_cast<int16_t>(margin_.top + padding_.top)
    );
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Calculate Text Bounds
 * ────────────────────────────────────────────────────────────────────────── */

VNRect LayoutEngine::calculateTextBounds(const char* text,
                                           const FontEngine& font) const
{
    WordWrapper wrapper;
    WordWrapper::LineInfo lines[VN_MAX_WRAP_LINES];
    uint8_t lineCount = wrapper.wrap(text, getContentWidth(), font,
                                     textWrap_, lines, VN_MAX_WRAP_LINES);

    if (lineCount == 0) {
        return VNRect(0, 0, 0, 0);
    }

    /* Find maximum line width */
    uint16_t maxWidth = 0;
    for (uint8_t i = 0; i < lineCount; i++) {
        if (lines[i].pixelWidth > maxWidth) {
            maxWidth = lines[i].pixelWidth;
        }
    }

    /* Calculate total height */
    uint8_t lineHeight = font.getLineHeight();
    uint16_t totalHeight = static_cast<uint16_t>(lineCount) * lineHeight;
    if (lineCount > 1) {
        totalHeight += static_cast<uint16_t>(lineCount - 1) * lineSpacing_;
    }

    return VNRect(0, 0, maxWidth, totalHeight);
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Layout Text into Lines
 * ────────────────────────────────────────────────────────────────────────── */

uint8_t LayoutEngine::layoutText(const char* text, const FontEngine& font,
                                   WordWrapper::LineInfo* lines,
                                   uint8_t maxLines) const
{
    WordWrapper wrapper;
    return wrapper.wrap(text, getContentWidth(), font, textWrap_,
                        lines, maxLines);
}
