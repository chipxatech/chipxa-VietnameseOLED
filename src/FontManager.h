/**
 * @file FontManager.h
 * @brief Multi-font runtime selector registry.
 *
 * Manages up to 4 concurrent registered fonts, allowing real-time switching of styles, sizes, and baselines.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#ifndef VN_FONT_MANAGER_H
#define VN_FONT_MANAGER_H

#include "VNConfig.h"
#include "VNTypes.h"

/**
 * @class FontManager
 * @brief Registry for multiple bitmap fonts.
 *
 * Usage:
 * @code
 * FontManager mgr;
 * mgr.registerFont(0, &Font_8x16_VN);
 * mgr.registerFont(1, &Font_16x16_VN);
 * mgr.setCurrentFont(1);
 * const VNFont* f = mgr.getCurrentFont();
 * @endcode
 */
class FontManager {
public:
    /** @brief Construct a FontManager with no registered fonts. */
    FontManager();

    /**
     * @brief Register a font at the given slot.
     *
     * @param id   Font slot index (0 to VN_MAX_FONTS-1)
     * @param font Pointer to a VNFont descriptor
     * @return true if registration succeeded, false if id is out of range
     */
    bool registerFont(uint8_t id, const VNFont* font);

    /**
     * @brief Set the currently active font by slot index.
     *
     * @param id Font slot index
     * @return true if the slot contains a registered font
     */
    bool setCurrentFont(uint8_t id);

    /**
     * @brief Get the currently active font.
     * @return Pointer to the current font, or nullptr if none selected
     */
    const VNFont* getCurrentFont() const;

    /**
     * @brief Get a font by slot index.
     * @param id Font slot index
     * @return Pointer to the font, or nullptr if slot is empty/invalid
     */
    const VNFont* getFont(uint8_t id) const;

    /**
     * @brief Get the number of registered fonts.
     * @return Count of non-null font slots
     */
    uint8_t getFontCount() const;

    /**
     * @brief Get the current font slot index.
     * @return Current font id, or 0xFF if none selected
     */
    uint8_t getCurrentFontId() const;

    /**
     * @brief Unregister all fonts and reset state.
     */
    void clear();

private:
    const VNFont* fonts_[VN_MAX_FONTS]; /**< Font registry slots */
    uint8_t       currentId_;            /**< Active font slot index */
    uint8_t       fontCount_;            /**< Number of registered fonts */
};

#endif /* VN_FONT_MANAGER_H */
