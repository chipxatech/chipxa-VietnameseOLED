/**
 * @file test_glyph_cache.cpp
 * @brief Unit tests for GlyphCache class.
 *
 * @author VietnameseOLED Contributors
 * @version 1.0.0
 * @copyright MIT License
 */

#include "GlyphCache.h"
#include "test_helpers.h"

void test_glyph_cache() {
    GlyphCache cache;

    // Check initial state
    TEST_ASSERT_EQ(cache.getSize(), 0);
    TEST_ASSERT_EQ(cache.getHitCount(), 0);
    TEST_ASSERT_EQ(cache.getMissCount(), 0);

    // Fetch non-existent glyph info
    VNGlyphInfo info;
    bool found = cache.find(0x41, info);
    TEST_ASSERT_FALSE(found);
    TEST_ASSERT_EQ(cache.getMissCount(), 1);
    TEST_ASSERT_EQ(cache.getHitCount(), 0);

    // Insert glyph info for 'A' (0x41)
    VNGlyphInfo infoA;
    infoA.codepoint = 0x41;
    infoA.valid = true;
    cache.insert(infoA);
    TEST_ASSERT_EQ(cache.getSize(), 1);

    // Find 'A' now
    found = cache.find(0x41, info);
    TEST_ASSERT(found);
    TEST_ASSERT_EQ(info.codepoint, 0x41u);
    TEST_ASSERT_EQ(cache.getHitCount(), 1);

    // Test LRU Eviction behavior
    // Fill the cache up to capacity
    uint8_t capacity = cache.getCapacity();
    for (uint8_t i = 0; i < capacity; i++) {
        VNGlyphInfo tmp;
        tmp.codepoint = 100 + i;
        tmp.valid = true;
        cache.insert(tmp);
    }
    TEST_ASSERT_EQ(cache.getSize(), capacity);

    // Access the element at index 0 (codepoint 100) to update its LRU status
    bool found100 = cache.find(100, info);
    TEST_ASSERT(found100);

    // Insert another element to trigger eviction.
    // The LRU element should be codepoint 101 (since 100 was accessed recently).
    VNGlyphInfo overflow;
    overflow.codepoint = 999;
    overflow.valid = true;
    cache.insert(overflow);

    TEST_ASSERT_EQ(cache.getSize(), capacity);

    // 100 should still be in cache
    bool stillHas100 = cache.find(100, info);
    TEST_ASSERT(stillHas100);

    // 101 should have been evicted
    bool stillHas101 = cache.find(101, info);
    TEST_ASSERT_FALSE(stillHas101);

    // Clear cache
    cache.clear();
    TEST_ASSERT_EQ(cache.getSize(), 0);
}
