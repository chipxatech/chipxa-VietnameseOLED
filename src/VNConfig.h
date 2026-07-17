/**
 * @file VNConfig.h
 * @brief Compile-time global parameters and hardware optimization configuration.
 *
 * Configures static memory buffers, cache thresholds, logging switches, and provides hardware board detection macros.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#ifndef VN_CONFIG_H
#define VN_CONFIG_H

/* ─────────────────────────────────────────────────────────────────────────────
 * Platform Detection & PROGMEM Abstraction
 * ────────────────────────────────────────────────────────────────────────── */

#ifdef VN_NATIVE_BUILD
    /* Native build for desktop unit testing — no Arduino dependencies */
    #include <cstdint>
    #include <cstring>
    #include <cstdio>
    #include <cstdarg>

    #define VN_PROGMEM
    #define VN_READ_BYTE(addr)  (*(const uint8_t*)(addr))
    #define VN_READ_WORD(addr)  (*(const uint16_t*)(addr))
    #define VN_READ_DWORD(addr) (*(const uint32_t*)(addr))

    /* Stub types for native build */
    #ifndef HIGH
        #define HIGH 1
    #endif
    #ifndef LOW
        #define LOW 0
    #endif

#else
    /* Arduino / embedded build */
    #include <Arduino.h>

    #if defined(__AVR__)
        /* AVR: Arduino UNO, Nano, Mega — Harvard architecture, must use pgm_read */
        #include <avr/pgmspace.h>
        #define VN_PROGMEM         PROGMEM
        #define VN_READ_BYTE(addr) pgm_read_byte(addr)
        #define VN_READ_WORD(addr) pgm_read_word(addr)
        #define VN_READ_DWORD(addr) pgm_read_dword(addr)

    #elif defined(ESP8266)
        /* ESP8266: Flash-mapped, PROGMEM supported via pgmspace */
        #include <pgmspace.h>
        #define VN_PROGMEM         PROGMEM
        #define VN_READ_BYTE(addr) pgm_read_byte(addr)
        #define VN_READ_WORD(addr) pgm_read_word(addr)
        #define VN_READ_DWORD(addr) pgm_read_dword(addr)

    #elif defined(ESP32)
        /* ESP32: Unified memory map, PROGMEM is a no-op */
        #include <pgmspace.h>
        #define VN_PROGMEM         PROGMEM
        #define VN_READ_BYTE(addr) pgm_read_byte(addr)
        #define VN_READ_WORD(addr) pgm_read_word(addr)
        #define VN_READ_DWORD(addr) pgm_read_dword(addr)

    #else
        /* STM32, CH32, RP2040, RP2350, and other ARM/RISC-V MCUs */
        /* Unified memory model — direct access to flash */
        #ifndef PROGMEM
            #define PROGMEM
        #endif
        #define VN_PROGMEM         PROGMEM
        #define VN_READ_BYTE(addr) (*(const uint8_t*)(addr))
        #define VN_READ_WORD(addr) (*(const uint16_t*)(addr))
        #define VN_READ_DWORD(addr) (*(const uint32_t*)(addr))
    #endif
#endif

/* ─────────────────────────────────────────────────────────────────────────────
 * Glyph Cache Configuration
 * ────────────────────────────────────────────────────────────────────────── */

/** @brief Maximum number of glyphs stored in the LRU cache. */
#ifndef VN_GLYPH_CACHE_SIZE
    #if defined(__AVR__)
        #define VN_GLYPH_CACHE_SIZE 4   /**< Conservative for 2KB SRAM */
    #else
        #define VN_GLYPH_CACHE_SIZE 16  /**< Generous for ESP/ARM */
    #endif
#endif

/* ─────────────────────────────────────────────────────────────────────────────
 * Font Manager Configuration
 * ────────────────────────────────────────────────────────────────────────── */

/** @brief Maximum number of fonts that can be registered simultaneously. */
#ifndef VN_MAX_FONTS
    #define VN_MAX_FONTS 4
#endif

/* ─────────────────────────────────────────────────────────────────────────────
 * Layout Engine Configuration
 * ────────────────────────────────────────────────────────────────────────── */

/** @brief Maximum number of lines the word wrapper can produce. */
#ifndef VN_MAX_WRAP_LINES
    #if defined(__AVR__)
        #define VN_MAX_WRAP_LINES 8    /**< Conservative for AVR SRAM */
    #else
        #define VN_MAX_WRAP_LINES 32   /**< Generous for ESP/ARM */
    #endif
#endif

/** @brief Maximum UTF-8 string length (bytes) for printf formatting. */
#ifndef VN_PRINTF_BUFFER_SIZE
    #if defined(__AVR__)
        #define VN_PRINTF_BUFFER_SIZE 64
    #else
        #define VN_PRINTF_BUFFER_SIZE 256
    #endif
#endif

/* ─────────────────────────────────────────────────────────────────────────────
 * Display Configuration
 * ────────────────────────────────────────────────────────────────────────── */

/** @brief Default I2C address for OLED displays. */
#ifndef VN_DEFAULT_I2C_ADDRESS
    #define VN_DEFAULT_I2C_ADDRESS 0x3C
#endif

/** @brief Default I2C clock speed in Hz. */
#ifndef VN_DEFAULT_I2C_CLOCK
    #define VN_DEFAULT_I2C_CLOCK 400000UL
#endif

/** @brief Maximum display width supported. */
#define VN_MAX_DISPLAY_WIDTH  128

/** @brief Maximum display height supported. */
#define VN_MAX_DISPLAY_HEIGHT 128

/* ─────────────────────────────────────────────────────────────────────────────
 * Unicode Configuration
 * ────────────────────────────────────────────────────────────────────────── */

/** @brief Unicode replacement character for invalid sequences. */
#define VN_UNICODE_REPLACEMENT_CHAR 0xFFFD

/** @brief Maximum Unicode codepoint supported (BMP). */
#define VN_UNICODE_MAX_BMP 0xFFFF

/* ─────────────────────────────────────────────────────────────────────────────
 * Debug Configuration
 * ────────────────────────────────────────────────────────────────────────── */

/** @brief Enable debug serial output (disabled by default). */
#ifndef VN_DEBUG
    #define VN_DEBUG 0
#endif

#if VN_DEBUG && !defined(VN_NATIVE_BUILD)
    #define VN_LOG(msg)       Serial.println(F(msg))
    #define VN_LOGF(fmt, ...) Serial.printf(fmt, ##__VA_ARGS__)
#else
    #define VN_LOG(msg)       ((void)0)
    #define VN_LOGF(fmt, ...) ((void)0)
#endif

#endif /* VN_CONFIG_H */
