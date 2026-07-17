/**
 * @file FontManager.cpp
 * @brief Registry implementation for multiple active fonts.
 *
 * Stores pointers to active VNFont structures and manages active selection updates.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#include "FontManager.h"

/* ─────────────────────────────────────────────────────────────────────────────
 * Constructor
 * ────────────────────────────────────────────────────────────────────────── */

FontManager::FontManager()
    : currentId_(0xFF)
    , fontCount_(0)
{
    for (uint8_t i = 0; i < VN_MAX_FONTS; i++) {
        fonts_[i] = nullptr;
    }
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Registration
 * ────────────────────────────────────────────────────────────────────────── */

bool FontManager::registerFont(uint8_t id, const VNFont* font) {
    if (id >= VN_MAX_FONTS || font == nullptr) {
        return false;
    }

    /* Track whether this is a new registration or an overwrite */
    if (fonts_[id] == nullptr) {
        fontCount_++;
    }

    fonts_[id] = font;

    /* If no font is currently selected, auto-select this one */
    if (currentId_ == 0xFF) {
        currentId_ = id;
    }

    return true;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Selection
 * ────────────────────────────────────────────────────────────────────────── */

bool FontManager::setCurrentFont(uint8_t id) {
    if (id >= VN_MAX_FONTS || fonts_[id] == nullptr) {
        return false;
    }
    currentId_ = id;
    return true;
}

const VNFont* FontManager::getCurrentFont() const {
    if (currentId_ >= VN_MAX_FONTS) {
        return nullptr;
    }
    return fonts_[currentId_];
}

const VNFont* FontManager::getFont(uint8_t id) const {
    if (id >= VN_MAX_FONTS) {
        return nullptr;
    }
    return fonts_[id];
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Queries
 * ────────────────────────────────────────────────────────────────────────── */

uint8_t FontManager::getFontCount() const {
    return fontCount_;
}

uint8_t FontManager::getCurrentFontId() const {
    return currentId_;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Clear
 * ────────────────────────────────────────────────────────────────────────── */

void FontManager::clear() {
    for (uint8_t i = 0; i < VN_MAX_FONTS; i++) {
        fonts_[i] = nullptr;
    }
    currentId_ = 0xFF;
    fontCount_ = 0;
}
