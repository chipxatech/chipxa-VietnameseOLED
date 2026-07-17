/**
 * @file WordWrap_Demo.ino
 * @brief Demonstrates word wrap and text alignment with Vietnamese text.
 */

#include <VietnameseOLED.h>
#include <drivers/SSD1306Adapter.h>
#include <fonts/Font_8x16_VN.h>

SSD1306Adapter display(128, 64, 0x3C);
VietnameseOLED oled(display);

static uint8_t alignIndex = 0;
static uint32_t lastSwitch = 0;

void setup() {
    oled.begin();
    showAlignment(0);
}

void loop() {
    uint32_t now = millis();
    if (now - lastSwitch >= 3000) {
        lastSwitch = now;
        alignIndex = (alignIndex + 1) % 3;
        showAlignment(alignIndex);
    }
}

void showAlignment(uint8_t idx) {
    oled.clear();
    oled.setFont(Font_8x16_VN);
    oled.setColor(VNColor::WHITE);
    oled.setTextWrap(VNTextWrap::WORD);

    /* Draw border to show the text area */
    oled.drawRect(0, 0, 128, 64, VNColor::WHITE);

    const char* label;
    switch (idx) {
        case 0:
            oled.setAlign(VNAlign::LEFT);
            label = "Căn trái";
            break;
        case 1:
            oled.setAlign(VNAlign::CENTER);
            label = "Căn giữa";
            break;
        case 2:
            oled.setAlign(VNAlign::RIGHT);
            label = "Căn phải";
            break;
        default:
            label = "";
            break;
    }

    /* Title */
    oled.drawUTF8(4, 4, label);
    oled.drawLine(4, 22, 123, 22, VNColor::WHITE);

    /* Word-wrapped Vietnamese text */
    oled.setCursor(4, 26);
    oled.printVN("Tự động xuống dòng ");
    oled.printVN("chữ tiếng Việt");

    oled.update();
}
