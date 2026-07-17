/**
 * @file test_main.cpp
 * @brief Main entry point for the VietnameseOLED unit test suite.
 *
 * Runs all unit test cases for individual library modules.
 *
 * @author VietnameseOLED Contributors
 * @version 1.0.0
 * @copyright MIT License
 */

#include <iostream>

/* Forward declarations of test suites */
void test_utf8_decoder();
void test_unicode_mapper();
void test_font_engine();
void test_glyph_renderer();
void test_layout_engine();
void test_word_wrapper();
void test_alignment();
void test_glyph_cache();

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "Running VietnameseOLED Unit Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;

    try {
        std::cout << "[RUN] test_utf8_decoder..." << std::endl;
        test_utf8_decoder();
        std::cout << "[OK]  test_utf8_decoder passed" << std::endl;

        std::cout << "[RUN] test_unicode_mapper..." << std::endl;
        test_unicode_mapper();
        std::cout << "[OK]  test_unicode_mapper passed" << std::endl;

        std::cout << "[RUN] test_font_engine..." << std::endl;
        test_font_engine();
        std::cout << "[OK]  test_font_engine passed" << std::endl;

        std::cout << "[RUN] test_glyph_renderer..." << std::endl;
        test_glyph_renderer();
        std::cout << "[OK]  test_glyph_renderer passed" << std::endl;

        std::cout << "[RUN] test_layout_engine..." << std::endl;
        test_layout_engine();
        std::cout << "[OK]  test_layout_engine passed" << std::endl;

        std::cout << "[RUN] test_word_wrapper..." << std::endl;
        test_word_wrapper();
        std::cout << "[OK]  test_word_wrapper passed" << std::endl;

        std::cout << "[RUN] test_alignment..." << std::endl;
        test_alignment();
        std::cout << "[OK]  test_alignment passed" << std::endl;

        std::cout << "[RUN] test_glyph_cache..." << std::endl;
        test_glyph_cache();
        std::cout << "[OK]  test_glyph_cache passed" << std::endl;

        std::cout << "========================================" << std::endl;
        std::cout << "ALL TESTS PASSED SUCCESSFULLY!" << std::endl;
        std::cout << "========================================" << std::endl;
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "[FAIL] Exception occurred during testing: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "[FAIL] Unknown error occurred during testing." << std::endl;
        return 1;
    }
}
