# Performance & Optimization Guide

This document details memory usage, caching strategies, and speed optimizations for the **VietnameseOLED** library.

---

## 1. Memory Analysis

### Flash Footprint (Fonts)
Fonts are the largest consumer of Flash memory in the library. Built-in fonts only contain ASCII + Vietnamese glyphs (229 total), making them much smaller than full Unicode fonts:

| Font Size | Bytes per Glyph | Total Font Data Size |
|---|---|---|
| `Font_5x8_VN` | 8 bytes | ~1.8 KB |
| `Font_8x16_VN` | 16 bytes | ~3.6 KB |
| `Font_16x16_VN` | 32 bytes | ~7.3 KB |

### SRAM Footprint (Framebuffer)
All displays allocate a static framebuffer in SRAM. Choosing the correct display size is crucial for memory-constrained MCUs like the ATmega328P:

| Resolution | Color Format | Framebuffer Size | SRAM Impact (Uno) |
|---|---|---|---|
| 128x32 | 1-bit Mono | 512 bytes | 25% of total SRAM |
| 128x64 | 1-bit Mono | 1024 bytes | 50% of total SRAM |
| 128x128 | 1-bit Mono | 2048 bytes | 100% of total SRAM (Not suitable) |
| 128x128 | 4-bit Grayscale | 8192 bytes | Not compilation compatible (Requires ESP/ARM) |

---

## 2. Caching Strategy and Performance

Reading from Flash memory (`PROGMEM` on AVR and flash-bound SPI storage on ESP8266) is significantly slower than reading from SRAM.

### Cache Hit Rates
The `GlyphCache` implements a Least-Recently-Used (LRU) algorithm. The cache stores the metrics and bitmap pointers of recently accessed characters:

```
Cache Size: 16 (Default for ESP/ARM)
Typical Vietnamese text: "Xin chào quý khách hàng đã đến với cửa hàng"
Average Cache Hit Rate: 85% to 92%
```

With a high cache hit rate, the library avoids the $O(\log n)$ binary search on the `unicodeMap` and direct Flash register reads, increasing update speed by up to **2.5x** compared to cacheless layouts.

### Optimizing Cache Size
Modify the cache size inside `src/VNConfig.h` based on your hardware:
- For **AVR (Arduino Uno)**: Keep it at 4 to conserve SRAM.
- For **ESP32 / STM32**: Increase to 16 or 24 for maximum rendering speed when drawing long paragraphs.

---

## 3. Rendering Speed Optimizations

### Use Hardware SPI
If your display supports SPI, always prefer it over I2C.
- **I2C Speed:** Typically limited to 400kHz. Refreshing a 128x64 buffer (1024 bytes + commands) takes around **25ms**.
- **SPI Speed:** Typically runs at 8MHz or higher. Refreshing the same buffer takes less than **1.5ms**.

### Avoid Redundant Clears
Instead of calling `oled.clear()` and completely re-drawing every element on the screen, overwrite only the dirty text areas.
- **Example:** If you are updating a counter, draw a black solid rectangle over the old value (`fillRect()`) and draw the new number over it in transparent mode, or render the text in `VNRenderMode::OVERWRITE` mode. This avoids blinking and saves CPU cycles.

### Pre-calculate Text Sizes
Do not measure text widths repeatedly inside your main `loop()`.
- **Bad:** `oled.drawUTF8(64 - oled.getTextWidth(str)/2, 10, str);` (Calculates size on every frame).
- **Good:** Calculate the width once in `setup()` or only when the string content changes, and store it in a variable.
