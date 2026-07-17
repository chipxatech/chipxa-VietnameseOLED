cmake_minimum_required(VERSION 3.14)
project(VietnameseOLED
    VERSION 1.0.0
    DESCRIPTION "Vietnamese UTF-8 OLED display library"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Library source files
set(VOLED_SOURCES
    src/UTF8Decoder.cpp
    src/UnicodeMapper.cpp
    src/FontEngine.cpp
    src/FontManager.cpp
    src/GlyphCache.cpp
    src/GlyphRenderer.cpp
    src/BitmapRenderer.cpp
    src/LayoutEngine.cpp
    src/WordWrapper.cpp
    src/TextAlignment.cpp
    src/TextEngine.cpp
    src/VietnameseOLED.cpp
    src/drivers/SSD1306Adapter.cpp
    src/drivers/SH1106Adapter.cpp
    src/drivers/SH1107Adapter.cpp
    src/drivers/SSD1327Adapter.cpp
)

set(VOLED_HEADERS
    src/VietnameseOLED.h
    src/VNConfig.h
    src/VNTypes.h
    src/UTF8Decoder.h
    src/UnicodeMapper.h
    src/FontEngine.h
    src/FontManager.h
    src/GlyphCache.h
    src/GlyphRenderer.h
    src/BitmapRenderer.h
    src/LayoutEngine.h
    src/WordWrapper.h
    src/TextAlignment.h
    src/TextEngine.h
    src/drivers/DisplayAdapter.h
    src/drivers/SSD1306Adapter.h
    src/drivers/SH1106Adapter.h
    src/drivers/SH1107Adapter.h
    src/drivers/SSD1327Adapter.h
    src/fonts/Font_8x16_VN.h
    src/fonts/Font_16x16_VN.h
)

# Define the library target
add_library(VietnameseOLED STATIC ${VOLED_SOURCES})

target_include_directories(VietnameseOLED PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fonts
)

# For native builds (testing), define a flag to stub out Arduino dependencies
if(NOT DEFINED ARDUINO)
    target_compile_definitions(VietnameseOLED PUBLIC VN_NATIVE_BUILD)
endif()

# Testing support
option(VOLED_BUILD_TESTS "Build unit tests" OFF)

if(VOLED_BUILD_TESTS)
    enable_testing()

    set(TEST_SOURCES
        test/test_utf8_decoder.cpp
        test/test_unicode_mapper.cpp
        test/test_font_engine.cpp
        test/test_glyph_renderer.cpp
        test/test_layout_engine.cpp
        test/test_word_wrapper.cpp
        test/test_alignment.cpp
        test/test_glyph_cache.cpp
        test/test_main.cpp
    )

    add_executable(voled_tests ${TEST_SOURCES})
    target_link_libraries(voled_tests PRIVATE VietnameseOLED)
    target_compile_definitions(voled_tests PRIVATE VN_NATIVE_BUILD)

    add_test(NAME VietnameseOLED_Tests COMMAND voled_tests)
endif()
