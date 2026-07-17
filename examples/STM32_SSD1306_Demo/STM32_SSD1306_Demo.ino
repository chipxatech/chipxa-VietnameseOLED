/**
 * @file STM32_SSD1306_Demo.ino
 * @brief Vietnamese text on STM32 Blue Pill + SSD1306.
 *
 * Wiring (STM32F103C8T6 Blue Pill):
 *   SDA -> PB7
 *   SCL -> PB6
 *   VCC -> 3.3V
 *   GND -> GND
 */

#include <VietnameseOLED.h>
#include <drivers/SSD1306Adapter.h>
#include <fonts/Font_8x16_VN.h>

SSD1306Adapter display(128, 64, 0x3C);
VietnameseOLED oled(display);

void setup() {
    Serial.begin(115200);

    oled.begin();
    oled.clear();

    oled.setFont(Font_8x16_VN);
    oled.setColor(VNColor::WHITE);

    oled.drawUTF8(0, 0, "STM32 Blue Pill");
    oled.drawUTF8(0, 16, "Xin chào!");
    oled.drawUTF8(0, 32, "Tiếng Việt");
    oled.drawUTF8(0, 48, "Hoàng Tiến Sơn");

    oled.update();
}

void loop() {
    delay(1000);
}
