# VietnameseOLED

[![Build and Test](https://github.com/contributor/VietnameseOLED/actions/workflows/ci.yml/badge.svg)](https://github.com/contributor/VietnameseOLED/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

**VietnameseOLED** is an advanced, production-ready, open-source C++17 library developed by **CHIPXA TECHNOLOGY CO., LTD. (CÔNG TY TNHH CÔNG NGHỆ CHIPXA)**. It is designed to provide an easy, precise, and highly efficient solution for rendering Vietnamese Unicode (UTF-8) diacritical text on monochrome and grayscale OLED displays. It is optimized for memory-constrained microcontrollers (AVR) up to high-performance systems (ESP32, ESP8266, STM32, CH32, RP2040, RP2350).

Currently, most popular rendering libraries on Arduino and embedded systems, such as Adafruit GFX, SSD1306, or SH1106, only support the standard ASCII character set. This makes displaying accented Vietnamese text highly challenging, requiring developers to construct custom fonts or perform manual character composition. VietnameseOLED was developed to resolve this limitation completely.

The library features **UTF-8 decoding**, mapping Unicode to bitmap fonts, and displaying the complete Vietnamese alphabet on various hardware platforms, including **Arduino, ESP8266, ESP32, STM32, CH32, Raspberry Pi Pico**, and other compatible microcontrollers. Additionally, the library is designed with an open architecture, allowing easy extension to support more displays and graphic drivers in the future.

---

## Project Goals

* **Full Vietnamese Unicode Support:** Support full Vietnamese Unicode (UTF-8) diacritical text rendering.
* **Simplified Integration API:** Provide a simple, intuitive, and easy-to-use API for embedded projects.
* **Broad Driver Compatibility:** Ensure compatibility with a wide range of OLED displays and popular graphics libraries.
* **Flexible Font Scale and Expansion:** Support multiple font sizes and custom font expansion capabilities.
* **Rapid Interface Development:** Reduce development time for Vietnamese user interfaces on embedded products.
* **Open Source Community Engagement:** Build a high-quality open-source library serving the Maker, IoT, STEM education, and commercial electronic device development communities.

---

## Target Audience

VietnameseOLED is suitable for:
* **Arduino & ESP Developers** looking for quick localization.
* **Embedded Software Engineers** who require highly optimized text drawing logic.
* **IoT Device Developers** creating smart hubs, weather stations, or industrial monitors.
* **Commercial Electronics Manufacturers** designing products for the Vietnamese market.
* **STEM & Robotics Centers** teaching students and makers how to display localized sensor data.
* **Students, Educators, and the Maker Community** building hobbyist projects.

---

## Future Roadmap

VietnameseOLED is not just a Vietnamese display library, but aims to become a standard font and text rendering platform for embedded systems. In the future, the project will expand to support more display drivers, additional font families, multiple Unicode languages, and professional font converter utilities to meet the demands of hardware developers worldwide.

---

## Key Features

- **Full Vietnamese Support:** Comprehensive encoding for all 134 uppercase/lowercase Vietnamese diacritical characters and standard ASCII.
- **Ultra-low Memory footprint:** Uses zero dynamic allocations (`new`/`malloc`), relying instead on static structures. All font glyph tables and mapping lists are stored directly in Flash memory via `PROGMEM` abstraction.
- **Hardware Agnostic (Adapter Pattern):** Decoupled display driver interface allowing easy integration of new screen types without modifying core library code.
- **DFA-based UTF-8 Decoder:** Robust, allocation-free, single-pass UTF-8 validation state machine.
- **LRU Glyph Cache:** Smart caching mechanism to minimize slow Flash memory read instructions during character layout.
- **Advanced Text Layout:** Includes a complete text rendering layout system supporting margins, padding, line spacing, horizontal alignment (left, center, right, justify), vertical alignment (top, middle, bottom), and word wrapping (none, character, word break).
- **Transforms:** Real-time scaling (1x to 4x), rotation (90° increments), and mirroring/flipping.
- **Built-in Fonts:** 2 complete hand-crafted Vietnamese bitmap fonts included:
  - `Font_8x16_VN` (medium size, crisp legibility)
  - `Font_16x16_VN` (large size)
- **Extensible:** Python-based CLI converter to generate custom fonts from standard TrueType (`.ttf`) or OpenType (`.otf`) vector files.

---

## Directory Structure

```
VietnameseOLED/
├── CMakeLists.txt              # Build configuration for unit testing & IDEs
├── Doxyfile                    # Doxygen configuration file
├── LICENSE                     # MIT License
├── library.properties          # Arduino Library specification
├── keywords.txt                # Syntax highlighting configurations
├── CHANGELOG.md                # Project version changelog
├── src/                        # Core library source code
│   ├── VietnameseOLED.h/.cpp   # Primary user-facing API
│   ├── VNConfig.h              # Compile-time macros and cache settings
│   ├── VNTypes.h               # Core data structures and enumerations
│   ├── UTF8Decoder.h/.cpp      # DFA-based UTF-8 state machine
│   ├── UnicodeMapper.h/.cpp    # Binary-search codepoint-to-glyph-index lookup
│   ├── FontEngine.h/.cpp       # Low-level PROGMEM glyph fetcher
│   ├── FontManager.h/.cpp      # Runtime font selector
│   ├── GlyphCache.h/.cpp       # LRU memory cache
│   ├── BitmapRenderer.h/.cpp   # Framebuffer pixel-drawing primitives
│   ├── GlyphRenderer.h/.cpp    # Drawing glyphs with scale, rotation, mirroring
│   ├── WordWrapper.h/.cpp      # Word-wrap boundary locator
│   ├── TextAlignment.h/.cpp    # Justify and centering calculators
│   ├── TextEngine.h/.cpp       # Layout & pipeline orchestrator
│   ├── drivers/                # Display adapters
│   │   ├── DisplayAdapter.h    # Abstract display driver base interface
│   │   ├── SSD1306Adapter.h    # SSD1306 I2C/SPI adapter (128x64 & 128x32)
│   │   ├── SH1106Adapter.h     # SH1106 I2C/SPI adapter with column offset
│   │   ├── SH1107Adapter.h     # SH1107 I2C/SPI adapter (128x128)
│   │   └── SSD1327Adapter.h    # SSD1327 I2C/SPI adapter (4-bit Grayscale)
│   └── fonts/                  # Hand-crafted built-in fonts
│       ├── Font_8x16_VN.h
│       └── Font_16x16_VN.h
├── examples/                   # 10 Platform-specific sketch examples
├── extras/                     # Python font converter tool
│   └── font_converter/
│       ├── font_converter.py
│       ├── requirements.txt
│       └── README.md
├── test/                       # C++ native build unit test suite
└── docs/                       # Technical documentation articles
```

---

## Supported Hardware

The library supports popular OLED display drivers out-of-the-box (using either I2C or SPI interface buses):

1. **SSD1306:** 128x64 and 128x32 configurations (monochrome).
2. **SH1106:** 1.3" 128x64 OLEDs with internal 132-column RAM mapping.
3. **SH1107:** 128x128 high-resolution OLEDs.
4. **SSD1327:** 128x128 4-bit grayscale displays (16 gray levels).

---

## Installation

### Via Arduino Library Manager
1. Open the Arduino IDE.
2. Go to **Sketch** -> **Include Library** -> **Manage Libraries...**
3. Search for `VietnameseOLED` and click **Install**.

### Manual Installation
1. Download the zip of this repository.
2. In Arduino IDE, go to **Sketch** -> **Include Library** -> **Add .ZIP Library...**
3. Select the downloaded ZIP file.

---

## Quick Start Example

Here is a simple example showing how to display Vietnamese text on an I2C SSD1306 display:

```cpp
#include <VietnameseOLED.h>
#include <drivers/SSD1306Adapter.h>
#include <fonts/Font_8x16_VN.h>

// Create a display adapter: SSD1306 128x64 using default I2C address 0x3C
SSD1306Adapter display(128, 64, 0x3C);

// Wrap display into VietnameseOLED interface
VietnameseOLED oled(display);

void setup() {
    // Initialize display and rendering subsystems
    oled.begin();
    
    // Clear screen buffer
    oled.clear();
    
    // Set active font
    oled.setFont(Font_8x16_VN);
    
    // Set color to white
    oled.setColor(VNColor::WHITE);
    
    // Render text with Vietnamese diacritical marks
    oled.drawUTF8(0, 0, "Xin chào Việt Nam!");
    oled.drawUTF8(0, 20, "Chào mừng bạn!");
    
    // Push buffer to the screen hardware
    oled.update();
}

void loop() {
    // idle
}
```

---

## Documentation Index

For in-depth explanations, configuration, and integration tutorials, consult the articles in the [docs/](file:///c:/Users/Admin/Desktop/VietnameseOLED/docs) directory:

- [API Reference](file:///c:/Users/Admin/Desktop/VietnameseOLED/docs/API_Reference.md): Detail specifications of all public C++ API classes and methods.
- [Architecture](file:///c:/Users/Admin/Desktop/VietnameseOLED/docs/Architecture.md): Architectural design explanation showing how subsystems coordinate.
- [Migration Guide](file:///c:/Users/Admin/Desktop/VietnameseOLED/docs/MigrationGuide.md): Upgrading from Adafruit GFX, U8g2, or older library designs.
- [Performance & Optimization](file:///c:/Users/Admin/Desktop/VietnameseOLED/docs/Performance.md): Details on cache hit rate, flash optimization, and hardware SPI usage.
- [Troubleshooting](file:///c:/Users/Admin/Desktop/VietnameseOLED/docs/Troubleshooting.md): Explanations and fixes for common I2C, SPI, and font rendering glitches.
- [FAQ](file:///c:/Users/Admin/Desktop/VietnameseOLED/docs/FAQ.md): Frequently asked questions on MCU support, grayscale settings, and memory usage.

---

## Author & Support

This library is developed and maintained by **CHIPXA TECHNOLOGY CO., LTD. (CÔNG TY TNHH CÔNG NGHỆ CHIPXA)**.

* **Headquarters:** Dong Lu, Hung Dao, Hanoi
* **Tax Code (MST):** 0111399950
* **Hotline:** +(84) 971 399 950 *(Zalo / WhatsApp / WeChat)*
* **Technical Support:** +(84) 928 000 084 *(Zalo / WhatsApp / WeChat)*
* **Email:** [info@chipxa.vn](mailto:info@chipxa.vn)
* **Website:** [chipxa.vn](https://chipxa.vn)

---

## License

This library is released under the **MIT License**. See [LICENSE](file:///c:/Users/Admin/Desktop/VietnameseOLED/LICENSE) for more details.

---

© CHIPXA TECHNOLOGY CO., LTD. All Rights Reserved.
