/**
 * @file ESP8266_SH1106_HelloVN.ino
 * @brief Vietnamese text on ESP8266 + SH1106 1.3" OLED.
 *
 * Wiring (ESP8266 NodeMCU):
 *   SDA -> D2 (GPIO4)
 *   SCL -> D1 (GPIO5)
 *   VCC -> 3.3V
 *   GND -> GND
 */

#include <VietnameseOLED.h>
#include <drivers/SH1106Adapter.h>
#include <fonts/Font_8x16_VN.h>

/* SH1106 1.3" OLED: 128×64, I2C */
SH1106Adapter display(128, 64, 0x3C);
VietnameseOLED oled(display);

void setup() {
    Serial.begin(115200);

    oled.begin();
    oled.clear();
    oled.setFont(Font_8x16_VN);
    oled.setColor(VNColor::WHITE);

    /* Vietnamese greetings */
    oled.drawUTF8(0, 0, "Xin chào!");
    oled.drawUTF8(0, 16, "Việt Nam");
    oled.drawUTF8(0, 32, "SH1106 OLED");
    oled.drawUTF8(0, 48, "Hoạt động tốt");

    oled.update();
}

void loop() {
    delay(1000);
}
