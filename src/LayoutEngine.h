/**
 * @file LayoutEngine.h
 * @brief High-level page metrics and wrap parameters manager.
 *
 * Coordinates viewports, margins, padding, spacing, alignment, and wrapping properties.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#ifndef VN_LAYOUT_ENGINE_H
#define VN_LAYOUT_ENGINE_H

#include "VNConfig.h"
#include "VNTypes.h"
#include "WordWrapper.h"

class FontEngine;

/**
 * @class LayoutEngine
 * @brief Manages text layout parameters and coordinates word wrapping.
 */
class LayoutEngine {
public:
    LayoutEngine();

    /** @name Viewport */
    /** @{ */
    void setViewport(const VNRect& viewport);
    const VNRect& getViewport() const;
    /** @} */

    /** @name Insets */
    /** @{ */
    void setPadding(uint8_t top, uint8_t right, uint8_t bottom, uint8_t left);
    void setMargin(uint8_t top, uint8_t right, uint8_t bottom, uint8_t left);
    const VNEdgeInsets& getPadding() const;
    const VNEdgeInsets& getMargin() const;
    /** @} */

    /** @name Spacing */
    /** @{ */
    void setLineSpacing(uint8_t pixels);
    void setParagraphSpacing(uint8_t pixels);
    uint8_t getLineSpacing() const;
    uint8_t getParagraphSpacing() const;
    /** @} */

    /** @name Alignment */
    /** @{ */
    void setAlignment(VNAlign hAlign, VNVAlign vAlign);
    VNAlign getHAlign() const;
    VNVAlign getVAlign() const;
    /** @} */

    /** @name Text Wrap */
    /** @{ */
    void setTextWrap(VNTextWrap wrap);
    VNTextWrap getTextWrap() const;
    /** @} */

    /**
     * @brief Calculate the bounding rectangle of rendered text.
     * @param text UTF-8 text
     * @param font Font engine for measurements
     * @return Bounding rectangle
     */
    VNRect calculateTextBounds(const char* text, const FontEngine& font) const;

    /**
     * @brief Layout text into wrapped lines within the viewport.
     * @param text     UTF-8 text
     * @param font     Font engine for measurements
     * @param[out] lines Line info array
     * @param maxLines Maximum lines
     * @return Number of lines
     */
    uint8_t layoutText(const char* text, const FontEngine& font,
                       WordWrapper::LineInfo* lines, uint8_t maxLines) const;

    /**
     * @brief Get the available content width (viewport minus padding and margins).
     * @return Content area width in pixels
     */
    uint16_t getContentWidth() const;

    /**
     * @brief Get the available content height.
     * @return Content area height in pixels
     */
    uint16_t getContentHeight() const;

    /**
     * @brief Get the content area origin (viewport + margin + padding).
     * @return Top-left point of content area
     */
    VNPoint getContentOrigin() const;

private:
    VNRect       viewport_;         /**< Display viewport rectangle */
    VNEdgeInsets padding_;          /**< Inner padding */
    VNEdgeInsets margin_;           /**< Outer margin */
    uint8_t      lineSpacing_;     /**< Extra pixels between lines */
    uint8_t      paragraphSpacing_; /**< Extra pixels between paragraphs */
    VNAlign      hAlign_;           /**< Horizontal alignment */
    VNVAlign     vAlign_;           /**< Vertical alignment */
    VNTextWrap   textWrap_;         /**< Word wrap mode */
};

#endif /* VN_LAYOUT_ENGINE_H */
