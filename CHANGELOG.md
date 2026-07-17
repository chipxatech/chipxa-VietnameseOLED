# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2024-01-01

### Added
- Full Vietnamese UTF-8 support with 134 diacritical characters (uppercase and lowercase)
- Custom UTF-8 decoder with DFA-based state machine
- Unicode mapper with binary search on sorted PROGMEM tables
- Font engine with PROGMEM-based bitmap fonts
- Three built-in Vietnamese bitmap fonts: 5×8, 8×16, 16×16
- Font manager supporting up to 4 registered fonts
- LRU glyph cache for performance optimization
- Glyph renderer with transparent, overwrite, and invert modes
- Integer scaling (1x–4x), rotation (0°/90°/180°/270°), and mirroring
- Bitmap renderer for low-level pixel and rectangle operations
- Layout engine with viewport, padding, margin, and line/paragraph spacing
- Word wrapper with word-break and character-break modes
- Text alignment: left, center, right, justify (horizontal) and top, middle, bottom (vertical)
- Text engine orchestrating the full decode → layout → render pipeline
- Display adapter interface (abstract base class) for extensibility
- SSD1306 adapter (I2C and SPI, 128×64 and 128×32)
- SH1106 adapter (I2C and SPI, 128×64 with column offset)
- SH1107 adapter (I2C and SPI, 128×128)
- SSD1327 adapter (I2C and SPI, 128×128, 4-bit grayscale)
- User-facing API: `drawUTF8()`, `printVN()`, `printlnVN()`, `printfVN()`
- Python font converter tool (TTF/OTF/BDF/PCF → C header)
- 10 example sketches for ESP8266, ESP32, Arduino UNO/Mega, STM32, CH32
- Unit tests for all core modules
- Full Doxygen documentation
- GitHub Actions CI for multi-board compilation
- Support for Arduino UNO/Nano/Mega, ESP8266, ESP32, STM32, CH32, RP2040, RP2350
