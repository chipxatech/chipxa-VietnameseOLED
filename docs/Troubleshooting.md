# Troubleshooting Guide

This guide covers common issues, compilation errors, and hardware glitches encountered when using **VietnameseOLED**.

---

## 1. Accents or Characters Displayed as Garbled Boxes (U+FFFD)

### Symptom:
Vietnamese characters display as double-question-mark symbols or generic replacement boxes (`?` or ``).

### Cause:
1. **File Encoding:** The source code file (`.ino` or `.cpp`) is not saved with UTF-8 encoding. Many default editors (like Arduino IDE 1.x on Windows) sometimes save sketches using Windows-1258, Windows-1252, or local ANSI encodings.
2. **Missing Glyphs:** The codepoint is not in the active font's glyph map.

### Solution:
1. **Ensure UTF-8:** In your editor, check the status bar or file settings and save the file explicitly with **UTF-8 Encoding** (without BOM). In Arduino IDE 2.x, files are UTF-8 by default.
2. **Check Font Coverage:** Make sure the character is a standard Vietnamese codepoint. If you generated a custom font, verify that the font file itself supports Vietnamese diacritics.

---

## 2. Display remains completely Blank (No Pixels render)

### Symptom:
The code compiles and runs, but the screen stays blank.

### Causes & Solutions:
1. **Wiring Error:** Double-check your wire pins.
   - For **I2C**: Ensure SDA goes to the microcontroller's SDA pin, and SCL to the SCL pin.
   - For **SPI**: Ensure DC, CS, and Reset pins match the numbers passed to the `SSD1306Adapter` constructor.
2. **Wrong I2C Address:** Many clone displays use I2C address `0x3D` instead of the default `0x3C`.
   - **Fix:** Try changing the address in the constructor: `SSD1306Adapter display(128, 64, 0x3D);`.
3. **Pull-Up Resistors:** Some microcontrollers (like ESP32 or STM32) do not enable internal pull-up resistors on I2C pins.
   - **Fix:** Add external 4.7kΩ pull-up resistors to the SDA and SCL lines.
4. **Missing `oled.update()`:** All rendering functions modify the in-memory framebuffer. You *must* call `oled.update()` at the end of drawing to transmit the buffer to the screen.

---

## 3. Slow Update Rates (Low FPS)

### Symptom:
It takes a visible fraction of a second to render or refresh a screen page.

### Causes & Solutions:
1. **I2C Clock Speed is low:** The default I2C clock on Arduino AVR is standard mode (100kHz).
   - **Fix:** Ensure Fast Mode is active by modifying the configuration clock, or call `Wire.setClock(400000UL)` in `setup()`.
2. **Too many absolute string measurements:** Calling `getTextWidth()` or `getTextBounds()` multiple times per frame recalculates layout.
   - **Fix:** Measure static strings once in `setup()` and cache their width dimensions in local variables.
3. **Low Cache Size:** If the cache size is set to a low value (e.g. 2 or 4), characters are constantly evicted and read from Flash.
   - **Fix:** On ESP/ARM, increase cache size in `src/VNConfig.h` to 16 or 32: `#define VN_GLYPH_CACHE_SIZE 16`.

---

## 4. Compiler Errors (C++17 or std:: symbols)

### Symptom:
Compiler outputs syntax errors related to templates, standard library symbols, or compilation flags.

### Cause:
Older platforms (like old Arduino AVR core versions) compile code using the C++11 or C++14 standards.

### Solution:
1. Update your Board Manager cores in the Arduino IDE to the latest versions.
2. If compiling via command-line or platformio, add the compiler standard flag to your configuration:
   - PlatformIO: `build_unflags = -std=gnu++11` and `build_flags = -std=c++17`.
