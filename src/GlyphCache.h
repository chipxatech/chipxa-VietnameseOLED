/**
 * @file GlyphCache.h
 * @brief Least-Recently-Used (LRU) glyph cache definition.
 *
 * Declares stack-allocated cache structures to store decoded glyph bitmaps in SRAM, minimizing flash lookups.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#ifndef VN_GLYPH_CACHE_H
#define VN_GLYPH_CACHE_H

#include "VNConfig.h"
#include "VNTypes.h"

/**
 * @class GlyphCache
 * @brief LRU cache for VNGlyphInfo, avoiding repeated PROGMEM reads.
 *
 * The cache has a fixed capacity (VN_GLYPH_CACHE_SIZE). When full, the
 * least-recently-used entry is evicted. All storage is stack-allocated.
 */
class GlyphCache {
public:
    /** @brief Construct an empty cache. */
    GlyphCache();

    /**
     * @brief Look up a codepoint in the cache.
     *
     * If found, the entry is promoted to most-recently-used.
     *
     * @param codepoint Unicode codepoint to search for
     * @param[out] info Glyph info to populate if found
     * @return true if found (cache hit), false otherwise (cache miss)
     */
    bool find(uint32_t codepoint, VNGlyphInfo& info);

    /**
     * @brief Insert a glyph info entry into the cache.
     *
     * If the cache is full, the least-recently-used entry is evicted.
     * If the codepoint already exists, it is updated and promoted.
     *
     * @param info Glyph info to cache (must have valid=true)
     */
    void insert(const VNGlyphInfo& info);

    /**
     * @brief Clear all cached entries.
     */
    void clear();

    /**
     * @brief Get the number of entries currently in the cache.
     * @return Current cache size
     */
    uint8_t getSize() const;

    /**
     * @brief Get the maximum cache capacity.
     * @return Cache capacity (VN_GLYPH_CACHE_SIZE)
     */
    uint8_t getCapacity() const;

    /**
     * @brief Get the cumulative number of cache hits since last clear.
     * @return Hit count
     */
    uint16_t getHitCount() const;

    /**
     * @brief Get the cumulative number of cache misses since last clear.
     * @return Miss count
     */
    uint16_t getMissCount() const;

    /**
     * @brief Reset hit/miss counters without clearing the cache.
     */
    void resetCounters();

private:
    /**
     * @brief Single cache entry with LRU ordering information.
     */
    struct CacheEntry {
        VNGlyphInfo info;     /**< Cached glyph info */
        uint16_t    lastUsed; /**< Timestamp for LRU ordering */
        bool        occupied; /**< True if this slot contains valid data */

        CacheEntry() : lastUsed(0), occupied(false) {}
    };

    CacheEntry entries_[VN_GLYPH_CACHE_SIZE]; /**< Fixed-size cache storage */
    uint8_t    size_;                          /**< Current number of occupied entries */
    uint16_t   accessCounter_;                 /**< Monotonic counter for LRU timestamps */
    uint16_t   hitCount_;                      /**< Cumulative cache hits */
    uint16_t   missCount_;                     /**< Cumulative cache misses */

    /**
     * @brief Find the index of the least-recently-used entry.
     * @return Index of the LRU entry
     */
    uint8_t findLRUIndex() const;
};

#endif /* VN_GLYPH_CACHE_H */
