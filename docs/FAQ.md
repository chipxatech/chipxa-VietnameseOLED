# Frequently Asked Questions (FAQ)

---

### Q1: Which microcontrollers can I compile this library for?
**A:** The library compiles on any standard C++17 microcontroller compiler. It has been tested and optimized for:
- **AVR**: Arduino Uno, Nano, Pro Micro, Mega.
- **ESP**: ESP8266, ESP32, ESP32-S2, ESP32-S3, ESP32-C3.
- **ARM**: STM32 (Blue Pill, Black Pill), Raspberry Pi Pico (RP2040, RP2350).
- **RISC-V**: CH32V003, CH32V307.

---

### Q2: How can I use custom TTF/OTF fonts in my project?
**A:** You can generate C++ font headers using the Python font converter tool located in the `extras/font_converter/` directory. For example:
```bash
python extras/font_converter/font_converter.py -i MyFont.ttf -s 12 -o src/fonts/MyFont_12.h
```
Then `#include "fonts/MyFont_12.h"` and call `oled.setFont(Font_MyFont_12_VN)`.

---

### Q3: Why does compiling for Arduino Uno fail or cause instabilities?
**A:** The Arduino Uno (ATmega328P) has only 2KB of SRAM. A full 128x64 framebuffer requires 1KB (50% of total SRAM), which leaves very little memory for stack, local variables, and other libraries.
- **Recommendation:** If using an Uno, use a 128x32 display (which requires only 512 bytes of framebuffer SRAM).
- **Optimization:** Keep the cache size low by defining `#define VN_GLYPH_CACHE_SIZE 4` in `VNConfig.h`.

---

### Q4: Does the library support full color (RGB) TFT displays?
**A:** Not directly out-of-the-box, as the core library is designed for monochrome and grayscale (4-bit) framebuffers. However, because it uses the **Adapter Pattern**, you can write a custom `DisplayAdapter` subclass that maps monochrome/grayscale pixels to RGB pixels on your TFT screen.

---

### Q5: Can I display emojis or special symbols?
**A:** Only if those symbols are compiled into the active font's mapping tables. By default, the generator includes printable ASCII and Vietnamese characters (229 glyphs). If you need emojis, you can run the `font_converter.py` tool and modify the source array to include the Unicode codepoints of your target emojis.

---

### Q6: How do I adjust the I2C speed?
**A:** By default, the display adapters configure the I2C bus to run at Fast Mode (400kHz). If you need to change this, modify `#define VN_DEFAULT_I2C_CLOCK` in `VNConfig.h` before compilation, or call `Wire.setClock(speed)` in your sketch after calling `oled.begin()`.
