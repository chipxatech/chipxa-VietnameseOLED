/**
 * @file ArduinoMega_SSD1306_Full.ino
 * @brief Full feature demo on Arduino Mega + SSD1306 128×64.
 *
 * Arduino Mega has 8KB SRAM — enough for 128×64 buffer + larger fonts.
 *
 * Wiring:
 *   SDA -> Pin 20
 *   SCL -> Pin 21
 *   VCC -> 5V
 *   GND -> GND
 */

#include <VietnameseOLED.h>
#include <drivers/SSD1306Adapter.h>
#include <fonts/Font_8x16_VN.h>

SSD1306Adapter display(128, 64, 0x3C);
VietnameseOLED oled(display);

static uint8_t currentPage = 0;
static const uint8_t TOTAL_PAGES = 3;
static uint32_t lastPageChange = 0;
static const uint32_t PAGE_INTERVAL_MS = 3000;

void setup() {
    Serial.begin(9600);
    oled.begin();
    showPage(0);
}

void loop() {
    uint32_t now = millis();
    if (now - lastPageChange >= PAGE_INTERVAL_MS) {
        lastPageChange = now;
        currentPage = (currentPage + 1) % TOTAL_PAGES;
        showPage(currentPage);
    }
}

void showPage(uint8_t page) {
    oled.clear();
    oled.setFont(Font_8x16_VN);
    oled.setColor(VNColor::WHITE);

    switch (page) {
        case 0:
            /* Vietnamese alphabet demo */
            oled.drawUTF8(0, 0, "Bảng chữ cái");
            oled.drawLine(0, 17, 127, 17, VNColor::WHITE);
            oled.drawUTF8(0, 20, "a ă â b c d đ e ê");
            oled.drawUTF8(0, 36, "g h i k l m n o ô");
            oled.drawUTF8(0, 52, "ơ p q r s t u ư x y");
            break;

        case 1:
            /* Diacritics demo */
            oled.drawUTF8(0, 0, "Dấu tiếng Việt");
            oled.drawLine(0, 17, 127, 17, VNColor::WHITE);
            oled.drawUTF8(0, 20, "Sắc/Huyền: á à");
            oled.drawUTF8(0, 36, "Hỏi/Ngã: ả ã");
            oled.drawUTF8(0, 52, "Nặng: ạ");
            break;

        case 2:
            /* Info page */
            oled.drawUTF8(0, 0, "Thông tin");
            oled.drawLine(0, 17, 127, 17, VNColor::WHITE);
            oled.drawUTF8(0, 20, "Mega 2560 - SSD1306");
            oled.drawUTF8(0, 36, "VietnameseOLED v1.0");
            oled.drawUTF8(0, 52, "MIT License");
            break;
    }

    oled.update();
}
