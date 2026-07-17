/**
 * @file test_helpers.h
 * @brief Helper assertion macros for unit tests.
 *
 * @author VietnameseOLED Contributors
 * @version 1.0.0
 * @copyright MIT License
 */

#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include <iostream>
#include <sstream>
#include <stdexcept>

#define TEST_ASSERT(cond) \
    do { \
        if (!(cond)) { \
            std::stringstream ss; \
            ss << "Assertion failed: " << #cond << " at " << __FILE__ << ":" << __LINE__; \
            throw std::runtime_error(ss.str()); \
        } \
    } while (0)

#define TEST_ASSERT_FALSE(cond) \
    do { \
        if (cond) { \
            std::stringstream ss; \
            ss << "Assertion failed (expected false, got true): " << #cond << " at " << __FILE__ << ":" << __LINE__; \
            throw std::runtime_error(ss.str()); \
        } \
    } while (0)

#define TEST_ASSERT_EQ(val1, val2) \
    do { \
        if ((val1) != (val2)) { \
            std::stringstream ss; \
            ss << "Assertion failed: " << #val1 << " == " << #val2 \
               << " (actual: " << (val1) << " vs " << (val2) << ") at " << __FILE__ << ":" << __LINE__; \
            throw std::runtime_error(ss.str()); \
        } \
    } while (0)

#define TEST_ASSERT_NE(val1, val2) \
    do { \
        if ((val1) == (val2)) { \
            std::stringstream ss; \
            ss << "Assertion failed: " << #val1 << " != " << #val2 \
               << " at " << __FILE__ << ":" << __LINE__; \
            throw std::runtime_error(ss.str()); \
        } \
    } while (0)

#endif /* TEST_HELPERS_H */
