/**
 * @file FontShowcase.ino
 * @brief Demonstrates the built-in Vietnamese fonts.
 *
 * Shows the same text rendered in Font_8x16_VN and Font_16x16_VN on separate screens.
 */

#include <VietnameseOLED.h>
#include <drivers/SSD1306Adapter.h>
#include <fonts/Font_8x16_VN.h>
#include <fonts/Font_16x16_VN.h>

SSD1306Adapter display(128, 64, 0x3C);
VietnameseOLED oled(display);

static uint8_t fontIndex = 0;
static uint32_t lastSwitch = 0;
static const uint32_t SWITCH_INTERVAL_MS = 3000;

void setup() {
    oled.begin();
    showFont(0);
}

void loop() {
    uint32_t now = millis();
    if (now - lastSwitch >= SWITCH_INTERVAL_MS) {
        lastSwitch = now;
        fontIndex = (fontIndex + 1) % 2;
        showFont(fontIndex);
    }
}

void showFont(uint8_t idx) {
    oled.clear();

    switch (idx) {
        case 0:
            oled.setFont(Font_8x16_VN);
            oled.drawUTF8(0, 0, "Font 8x16");
            oled.drawUTF8(0, 16, "Xin chào!");
            oled.drawUTF8(0, 32, "Việt Nam");
            oled.drawUTF8(0, 48, "ĂÂĐÊÔƠƯăâ");
            break;

        case 1:
            oled.setFont(Font_16x16_VN);
            oled.drawUTF8(0, 0, "Font 16");
            oled.drawUTF8(0, 16, "Chào!");
            oled.drawUTF8(0, 32, "Việt");
            oled.drawUTF8(0, 48, "Nam");
            break;
    }

    oled.update();
}
