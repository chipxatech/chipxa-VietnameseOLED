/**
 * @file GlyphCache.cpp
 * @brief Least-Recently-Used (LRU) glyph cache implementation.
 *
 * Maintains access lists to update hot-path glyphs and evict cold data on capacity limits.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#include "GlyphCache.h"

/* ─────────────────────────────────────────────────────────────────────────────
 * Constructor
 * ────────────────────────────────────────────────────────────────────────── */

GlyphCache::GlyphCache()
    : size_(0)
    , accessCounter_(0)
    , hitCount_(0)
    , missCount_(0)
{
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Find — Cache Lookup
 * ────────────────────────────────────────────────────────────────────────── */

bool GlyphCache::find(uint32_t codepoint, VNGlyphInfo& info) {
    for (uint8_t i = 0; i < VN_GLYPH_CACHE_SIZE; i++) {
        if (entries_[i].occupied && entries_[i].info.codepoint == codepoint) {
            /* Cache hit — promote to MRU */
            entries_[i].lastUsed = ++accessCounter_;
            info = entries_[i].info;
            hitCount_++;
            return true;
        }
    }

    /* Cache miss */
    missCount_++;
    return false;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Insert — Add or Update Entry
 * ────────────────────────────────────────────────────────────────────────── */

void GlyphCache::insert(const VNGlyphInfo& info) {
    if (!info.valid) {
        return;
    }

    /* Check if this codepoint already exists — update if so */
    for (uint8_t i = 0; i < VN_GLYPH_CACHE_SIZE; i++) {
        if (entries_[i].occupied && entries_[i].info.codepoint == info.codepoint) {
            entries_[i].info     = info;
            entries_[i].lastUsed = ++accessCounter_;
            return;
        }
    }

    /* Find a free slot or evict LRU */
    uint8_t targetSlot;

    if (size_ < VN_GLYPH_CACHE_SIZE) {
        /* Find first empty slot */
        targetSlot = 0;
        for (uint8_t i = 0; i < VN_GLYPH_CACHE_SIZE; i++) {
            if (!entries_[i].occupied) {
                targetSlot = i;
                break;
            }
        }
        size_++;
    }
    else {
        /* Cache is full — evict LRU */
        targetSlot = findLRUIndex();
    }

    entries_[targetSlot].info     = info;
    entries_[targetSlot].lastUsed = ++accessCounter_;
    entries_[targetSlot].occupied = true;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Clear
 * ────────────────────────────────────────────────────────────────────────── */

void GlyphCache::clear() {
    for (uint8_t i = 0; i < VN_GLYPH_CACHE_SIZE; i++) {
        entries_[i].occupied = false;
        entries_[i].lastUsed = 0;
    }
    size_          = 0;
    accessCounter_ = 0;
    hitCount_      = 0;
    missCount_     = 0;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Queries
 * ────────────────────────────────────────────────────────────────────────── */

uint8_t GlyphCache::getSize() const {
    return size_;
}

uint8_t GlyphCache::getCapacity() const {
    return VN_GLYPH_CACHE_SIZE;
}

uint16_t GlyphCache::getHitCount() const {
    return hitCount_;
}

uint16_t GlyphCache::getMissCount() const {
    return missCount_;
}

void GlyphCache::resetCounters() {
    hitCount_  = 0;
    missCount_ = 0;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Internal — Find Least Recently Used
 * ────────────────────────────────────────────────────────────────────────── */

uint8_t GlyphCache::findLRUIndex() const {
    uint8_t  lruIndex = 0;
    uint16_t minTime  = entries_[0].lastUsed;

    for (uint8_t i = 1; i < VN_GLYPH_CACHE_SIZE; i++) {
        if (entries_[i].occupied && entries_[i].lastUsed < minTime) {
            minTime  = entries_[i].lastUsed;
            lruIndex = i;
        }
    }

    return lruIndex;
}
