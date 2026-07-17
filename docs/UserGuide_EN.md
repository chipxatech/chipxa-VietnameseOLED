# DETAILED USER GUIDE - VIETNAMESEOLED

Welcome to **VietnameseOLED**! This is a step-by-step user manual designed specifically for absolute beginners. Just follow these simple instructions, and you'll be displaying accented Vietnamese text on your OLED screen in no time.

---

## 1. Overview
The **VietnameseOLED** library enables standard UTF-8 Vietnamese diacritical text rendering directly on popular OLED displays (such as SSD1306, SH1106, SH1107, SSD1327) using common microcontrollers:
* Arduino UNO, Nano, Mega.
* ESP8266, ESP32.
* STM32, CH32.
* Raspberry Pi Pico (RP2040/RP2350).

---

## 2. Hardware Wiring Guide
Most common OLED screens communicate using the **I2C** protocol (which requires only 4 wires: VCC, GND, SDA, SCL). Connect the pins to your microcontroller according to the table below:

| Microcontroller | OLED VCC Pin | OLED GND Pin | OLED SDA Pin | OLED SCL Pin |
| :--- | :--- | :--- | :--- | :--- |
| **Arduino UNO / Nano** | 5V or 3.3V | GND | **A4** | **A5** |
| **Arduino Mega 2560** | 5V or 3.3V | GND | **Pin 20** | **Pin 21** |
| **ESP8266 (NodeMCU)** | 3V3 | G (GND) | **D2** (GPIO4) | **D1** (GPIO5) |
| **ESP32 (WROOM)** | 3V3 | GND | **GPIO21** | **GPIO22** |
| **STM32 Blue Pill** | 3.3 | GND | **PB7** | **PB6** |
| **Raspberry Pi Pico** | 3V3 | GND | **GP4** | **GP5** |

> [!NOTE]
> If you are using an **SPI** screen, check the specific screen adapter driver code for custom pinout arrangements. However, I2C is highly recommended for beginners because it requires the fewest wires.

---

## 3. How to Install the Library in Arduino IDE

### Step 1: Download & Install
You can install the library in two ways:
* **Method 1 (Recommended):** Open Arduino IDE -> Select the **Sketch** menu -> **Include Library** -> **Manage Libraries...** Search for `VietnameseOLED` and click **Install**.
* **Method 2:** Download the `.zip` archive of this repository. In Arduino IDE, select **Sketch** -> **Include Library** -> **Add .ZIP Library...** -> Choose the downloaded ZIP file.

### Step 2: Ensure UTF-8 File Encoding in Arduino IDE
To write Vietnamese text directly in your code, your IDE must save files using UTF-8 encoding.
* **Arduino IDE 2.x and newer:** Automatically supports UTF-8. You can write Vietnamese characters out-of-the-box.
* **Legacy Arduino IDE 1.8.x:** Go to **File** -> **Preferences** and ensure that "Save files using UTF-8 encoding" is checked.

---

## 4. Quick Start Example

Copy the code below, paste it into your Arduino IDE, and upload it to your board (this example is pre-configured for a 128x64 I2C SSD1306 OLED display):

```cpp
#include <VietnameseOLED.h>
#include <drivers/SSD1306Adapter.h> // Driver for SSD1306 display
#include <fonts/Font_8x16_VN.h>       // Medium legible Vietnamese font

// Initialize a 128x64 display adapter at default I2C address 0x3C
SSD1306Adapter display(128, 64, 0x3C);

// Wrap the adapter in the VietnameseOLED interface
VietnameseOLED oled(display);

void setup() {
    // 1. Initialize screen hardware and buffers
    oled.begin();
    
    // 2. Clear display buffer
    oled.clear();
    
    // 3. Set display font
    oled.setFont(Font_8x16_VN);
    
    // 4. Set draw color to white
    oled.setColor(VNColor::WHITE);
    
    // 5. Draw text at coordinates X=0, Y=0 (top-left corner)
    oled.drawUTF8(0, 0, "Xin chào Việt Nam!");
    
    // 6. Draw another line at coordinates X=0, Y=20 (20 pixels below the first line)
    oled.drawUTF8(0, 20, "Học lập trình nhúng");
    
    // 7. Push buffer to physical display (MANDATORY)
    oled.update();
}

void loop() {
    // Idle loop
}
```

---

## 5. Explaining Core Functions & Features

### 5.1. Initialization & Screen Buffer Updates
* `oled.begin()`: Initializes the I2C/SPI bus and sets up hardware configuration registers.
* `oled.clear()`: Resets the entire display framebuffer.
* `oled.update()`: Pushes the drawn graphics from microcontroller SRAM buffer onto the physical OLED panel. Skipping this step will result in a blank screen.

### 5.2. Built-in Fonts
The library includes 2 premium Vietnamese bitmap fonts:
1. `Font_8x16_VN`: Medium font (each char is 8px wide, 16px high) - Default choice for most interfaces.
2. `Font_16x16_VN`: Large font - Excellent for titles, temperatures, or humidity readouts.

To change the font:
```cpp
#include <fonts/Font_16x16_VN.h> // Import font header
// ...
oled.setFont(Font_16x16_VN); // Set active font
```

### 5.3. Word Wrapping
When printing long strings that exceed the screen width, you can enable auto-wrapping:
```cpp
oled.setTextWrap(VNTextWrap::WORD); // Wrap at word boundaries (whitespace)
oled.setCursor(0, 10);              // Position draw cursor
oled.printVN("This is a very long string that will wrap automatically on the screen.");
```

### 5.4. Text Alignment
You can align text left, center, right, or justified:
* `oled.setAlign(VNAlign::CENTER)`: Center alignment.
* `oled.setAlign(VNAlign::RIGHT)`: Right alignment.
* `oled.setAlign(VNAlign::LEFT)`: Left alignment.
* `oled.setAlign(VNAlign::JUSTIFY)`: Justified alignment.

For example, centering text on a 128-pixel wide screen:
```cpp
oled.setAlign(VNAlign::CENTER);
oled.drawUTF8(64, 20, "CENTER"); // X=64 acts as the anchor point
```

### 5.5. Scaling, Rotating, and Mirroring
* **Scale Font:**
  ```cpp
  oled.setScale(2); // Double font height/width (Supports 1x to 4x integer scaling)
  ```
* **Rotate Screen:**
  ```cpp
  oled.setRotation(90); // Rotate canvas 90° clockwise (Supports 0°, 90°, 180°, 270°)
  ```
* **Mirror Flip:**
  ```cpp
  oled.setMirror(true, false); // Flip horizontally (mirror rendering)
  ```

---

## 6. Troubleshooting Common Issues

### Issue 1: Screen remains completely black
* **Check Wiring:** Double-check SDA and SCL connections. Make sure they are plugged into the correct hardware I2C pins of your microcontroller.
* **Wrong I2C Address:** While most screens use `0x3C`, some modules are hardwired to `0x3D`. Try modifying the initialization code:
  `SSD1306Adapter display(128, 64, 0x3D);`
* **Missing Update Call:** Make sure `oled.update();` is called at the end of your drawing routines.

### Issue 2: Text shows up as question marks or garbled characters
* **File Encoding Issue:** Your code file is not saved in **UTF-8** format. Create a brand-new sketch in Arduino IDE, type your accented Vietnamese characters, and upload again.
* **Font Not Set:** Ensure `oled.setFont(Font_8x16_VN);` is run before any print statements.

Have fun building your embedded projects!
