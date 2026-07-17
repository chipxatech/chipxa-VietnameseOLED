/**
 * @file ArduinoUNO_SSD1306_Basic.ino
 * @brief Minimal RAM Vietnamese display on Arduino UNO + SSD1306 128×32.
 *
 * Arduino UNO has only 2KB SRAM, so we use 128×32 (512-byte buffer)
 * and the compact 5×8 font to stay within memory limits.
 *
 * Wiring:
 *   SDA -> A4
 *   SCL -> A5
 *   VCC -> 5V
 *   GND -> GND
 */

#include <VietnameseOLED.h>
#include <drivers/SSD1306Adapter.h>
#include <fonts/Font_8x16_VN.h>

/* 128×32 display uses only 512 bytes of buffer */
SSD1306Adapter display(128, 32, 0x3C);
VietnameseOLED oled(display);

void setup() {
    oled.begin();
    oled.clear();

    oled.setFont(Font_8x16_VN);
    oled.setColor(VNColor::WHITE);

    /* Vietnamese text on Arduino UNO */
    oled.drawUTF8(0, 0, "Việt Nam!");
    oled.drawUTF8(0, 16, "Arduino UNO");

    oled.update();
}

void loop() {
    delay(1000);
}
