/**
 * @file CH32_SSD1306_Demo.ino
 * @brief Vietnamese text on CH32V003/CH32V307 + SSD1306.
 *
 * Wiring (CH32V003):
 *   SDA -> PC1
 *   SCL -> PC2
 *   VCC -> 3.3V
 *   GND -> GND
 */

#include <VietnameseOLED.h>
#include <drivers/SSD1306Adapter.h>
#include <fonts/Font_8x16_VN.h>

/* CH32 has limited RAM — use 128×32 if needed */
SSD1306Adapter display(128, 64, 0x3C);
VietnameseOLED oled(display);

void setup() {
    oled.begin();
    oled.clear();

    oled.setFont(Font_8x16_VN);
    oled.setColor(VNColor::WHITE);

    oled.drawUTF8(0, 0, "CH32 RISC-V");
    oled.drawUTF8(0, 16, "Xin chào VN!");
    oled.drawUTF8(0, 32, "Nhiệt độ: 28°C");
    oled.drawUTF8(0, 48, "Độ ẩm: 70%");

    oled.update();
}

void loop() {
    delay(1000);
}
