/**
 * @file ESP32_SSD1306_Demo.ino
 * @brief Multi-font demo with sensor display on ESP32 + SSD1306.
 *
 * Wiring (ESP32):
 *   SDA -> GPIO21
 *   SCL -> GPIO22
 *   VCC -> 3.3V
 *   GND -> GND
 */

#include <VietnameseOLED.h>
#include <drivers/SSD1306Adapter.h>
#include <fonts/Font_8x16_VN.h>
#include <fonts/Font_16x16_VN.h>

SSD1306Adapter display(128, 64, 0x3C);
VietnameseOLED oled(display);

static uint32_t lastUpdate = 0;
static const uint32_t UPDATE_INTERVAL_MS = 2000;
static int temperature = 25;
static int humidity = 65;

void setup() {
    Serial.begin(115200);
    Serial.println(F("VietnameseOLED - ESP32 Demo"));

    oled.begin();
    drawScreen();
}

void loop() {
    uint32_t now = millis();
    if (now - lastUpdate >= UPDATE_INTERVAL_MS) {
        lastUpdate = now;

        /* Simulate sensor readings */
        temperature = 20 + (int)(millis() / 3000) % 15;
        humidity = 50 + (int)(millis() / 5000) % 30;

        drawScreen();
    }
}

void drawScreen() {
    oled.clear();

    /* Title in large font */
    oled.setFont(Font_8x16_VN);
    oled.setColor(VNColor::WHITE);
    oled.drawUTF8(0, 0, "Trạm thời tiết");

    /* Sensor data in medium font */
    oled.setFont(Font_8x16_VN);

    /* Temperature */
    oled.setCursor(0, 16);
    oled.printfVN("Nhiệt độ: %d°C", temperature);

    /* Humidity */
    oled.setCursor(0, 32);
    oled.printfVN("Độ ẩm: %d%%", humidity);

    /* Status in 8x16 font */
    oled.drawUTF8(0, 48, "Trạng thái: OK");

    oled.update();
}
