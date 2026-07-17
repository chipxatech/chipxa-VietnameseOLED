# Migration Guide

This guide assists developers in migrating existing codebases from popular libraries (like **Adafruit GFX** or **U8g2**) to **VietnameseOLED**.

---

## 1. Comparing with Adafruit GFX

Adafruit GFX does not natively support multi-byte UTF-8 decoding. Displaying Vietnamese accents on Adafruit GFX typically requires either custom extended ASCII fonts (which limits you to 256 characters) or manual character composition code.

### API Comparison

| Feature | Adafruit SSD1306 / GFX | VietnameseOLED |
|---|---|---|
| Include | `#include <Adafruit_GFX.h>` | `#include <VietnameseOLED.h>` |
| Driver Include | `#include <Adafruit_SSD1306.h>` | `#include <drivers/SSD1306Adapter.h>` |
| Instance | `Adafruit_SSD1306 display(128, 64, &Wire);` | `SSD1306Adapter adapter(128, 64);`<br>`VietnameseOLED oled(adapter);` |
| Initialization | `display.begin(SSD1306_SWITCHCAPVCC, 0x3C);` | `oled.begin();` |
| Clear Buffer | `display.clearDisplay();` | `oled.clear();` |
| Push to Screen | `display.display();` | `oled.update();` |
| Set Font | `display.setFont(&FreeSans9pt7b);` | `oled.setFont(Font_8x16_VN);` |
| Set Text Color | `display.setTextColor(SSD1306_WHITE);` | `oled.setColor(VNColor::WHITE);` |
| Draw UTF-8 Text | (Not directly supported/corrupts accents) | `oled.drawUTF8(x, y, "Tiếng Việt");` |
| Print at Cursor | `display.setCursor(x, y);`<br>`display.print("Text");` | `oled.setCursor(x, y);`<br>`oled.printVN("Tiếng Việt");` |

### Code Migration Example

#### Before (Adafruit GFX):
```cpp
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 64, &Wire);

void setup() {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    // Standard print corrupts Vietnamese UTF-8 strings
    display.print("Xin chao Viet Nam!"); 
    display.display();
}
```

#### After (VietnameseOLED):
```cpp
#include <VietnameseOLED.h>
#include <drivers/SSD1306Adapter.h>
#include <fonts/Font_8x16_VN.h>

SSD1306Adapter display(128, 64, 0x3C);
VietnameseOLED oled(display);

void setup() {
    oled.begin();
    oled.clear();
    oled.setFont(Font_8x16_VN);
    oled.setColor(VNColor::WHITE);
    
    // Natively draws full Vietnamese UTF-8 text crisp and clear
    oled.drawUTF8(0, 0, "Xin chào Việt Nam!");
    oled.update();
}
```

---

## 2. Comparing with U8g2

U8g2 supports UTF-8 and is highly functional. However, it is a very large library, and compiling it with full Vietnamese fonts can quickly exhaust the Flash memory on smaller MCUs like the ATmega328P (Arduino UNO).

### Key Architectural Differences

1. **Memory consumption:** U8g2 includes broad character lists. **VietnameseOLED** built-in fonts are strictly pruned to cover *only* ASCII and the 134 Vietnamese diacritics (229 glyphs total), keeping the flash size of a complete 8x16 font under 4KB.
2. **Page Loop vs Framebuffer:** U8g2 often forces developers to draw inside a page rendering loop (`firstPage`/`nextPage`) to save SRAM. **VietnameseOLED** uses a unified in-memory framebuffer, which makes primitive operations and updates much simpler to write.

### API Mapping

| U8g2 Method | VietnameseOLED Equivalent |
|---|---|
| `u8g2.setFont(u8g2_font_unifont_t_vietnamese1)` | `oled.setFont(Font_8x16_VN)` |
| `u8g2.drawUTF8(x, y, str)` | `oled.drawUTF8(x, y, str)` |
| `u8g2.sendBuffer()` | `oled.update()` |
| `u8g2.clearBuffer()` | `oled.clear()` |
| `u8g2.setCursor(x, y)` | `oled.setCursor(x, y)` |
| `u8g2.print(str)` | `oled.printVN(str)` |
