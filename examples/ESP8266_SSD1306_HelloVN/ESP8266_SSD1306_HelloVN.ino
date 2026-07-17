/**
 * @file ESP8266_SSD1306_HelloVN.ino
 * @brief Basic Vietnamese text on ESP8266 + SSD1306 I2C OLED.
 *
 * Wiring (ESP8266 NodeMCU):
 *   SDA -> D2 (GPIO4)
 *   SCL -> D1 (GPIO5)
 *   VCC -> 3.3V
 *   GND -> GND
 */

#include "VietnameseOLED.h"
#include "drivers/SSD1306Adapter.h"
#include "fonts/Font_8x16_VN.h"

/* Create display adapter: 128×64, I2C address 0x3C */
SSD1306Adapter display(128, 64, 0x3C);

/* Create VietnameseOLED instance */
VietnameseOLED oled(display);

void setup() {
    Serial.begin(115200);
    Serial.println(F("VietnameseOLED - ESP8266 SSD1306 Demo"));

    /* Initialize display */
    oled.begin();
    oled.clear();

    /* Set 8×16 font */
    oled.setFont(Font_8x16_VN);
    oled.setColor(VNColor::WHITE);

    /* Draw Vietnamese text */
    oled.drawUTF8(0, 0, "Xin chào!");
    oled.drawUTF8(0, 16, "Việt Nam");
    oled.drawUTF8(0, 32, "Đẹp lắm");

    /* Draw footer in 8x16 font */
    oled.drawUTF8(0, 48, "Voled v1.0");

    /* Push to display */
    oled.update();
}

void loop() {
    /* Nothing to do in loop */
    delay(1000);
}
