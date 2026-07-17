# VietnameseOLED API Reference

This document provides a detailed description of the classes, enumerations, structures, and functions in the **VietnameseOLED** library.

---

## 1. Primary API Class: `VietnameseOLED`

The primary class that developers interact with. It coordinates all rendering, font mapping, caching, and layout operations.

### Constructor
```cpp
explicit VietnameseOLED(DisplayAdapter& display);
```
- **Description:** Creates an instance associated with a specific display hardware driver adapter.
- **Parameters:** `display` - Concrete subclass of `DisplayAdapter` (e.g. `SSD1306Adapter`).

### Lifecycle Methods
- `bool begin()`: Initializes hardware interface communication (I2C/SPI) and internally binds subsystems. Returns `true` on success.
- `void clear()`: Clears the display memory buffer and resets the print cursor position to `(0, 0)`.
- `void update()`: Transmits the compiled in-memory framebuffer to the physical display screen.

### Text Rendering
- `void drawUTF8(int16_t x, int16_t y, const char* text)`: Draws a null-terminated UTF-8 string at absolute pixel coordinates.
- `void printVN(const char* text)`: Prints text starting from the current cursor position, automatically advancing the cursor horizontally.
- `void printlnVN(const char* text)`: Prints text at the current cursor position and moves the cursor to the beginning of the next line.
- `void printfVN(const char* fmt, ...)`: Printf-style formatted printing support with safety bounds checks.

### Drawing Configurations
- `void setFont(const VNFont& font)`: Sets the current active font.
- `void setColor(VNColor color)`: Sets the primary color (`VNColor::WHITE`, `VNColor::BLACK`, `VNColor::INVERSE`).
- `void setAlign(VNAlign align)`: Sets horizontal alignment (`LEFT`, `CENTER`, `RIGHT`, `JUSTIFY`).
- `void setVAlign(VNVAlign align)`: Sets vertical alignment (`TOP`, `MIDDLE`, `BOTTOM`).
- `void setTextWrap(VNTextWrap wrap)`: Sets wrapping mode (`NONE`, `WORD`, `CHARACTER`).
- `void setLineSpacing(uint8_t pixels)`: Sets extra pixel separation spacing between text lines.
- `void setParagraphSpacing(uint8_t pixels)`: Sets extra paragraph spacing.
- `void setRenderMode(VNRenderMode mode)`: Sets glyph rendering mode (`TRANSPARENT`, `OVERWRITE`, `INVERT`).
- `void setScale(uint8_t scale)`: Sets integer scaling factor (1x to 4x).
- `void setRotation(uint16_t degrees)`: Sets display rotation (0, 90, 180, 270 degrees).
- `void setMirror(bool horizontal, bool vertical)`: Flips drawings vertically or horizontally.

### Cursor Controls
- `void setCursor(int16_t x, int16_t y)`: Positions the cursor coordinates.
- `int16_t getCursorX() const`: Returns the current horizontal cursor coordinate.
- `int16_t getCursorY() const`: Returns the current vertical cursor coordinate.

### Size Measuring
- `uint16_t getTextWidth(const char* text) const`: Measures the pixel width of a string in the current font.
- `uint8_t getTextHeight() const`: Returns the line height of the current font.
- `VNRect getTextBounds(const char* text) const`: Calculates the layout bounding rectangle for a string.

### Geometric Primitive Drawing
- `void drawPixel(int16_t x, int16_t y, VNColor color)`: Sets a single pixel.
- `void drawBitmap(int16_t x, int16_t y, const uint8_t* bitmap, uint16_t w, uint16_t h)`: Draws a custom 1-bit bitmap.
- `void fillRect(int16_t x, int16_t y, uint16_t w, uint16_t h, VNColor color)`: Draws a filled rectangle.
- `void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, VNColor color)`: Draws a line using Bresenham's algorithm.
- `void drawRect(int16_t x, int16_t y, uint16_t w, uint16_t h, VNColor color)`: Draws a rectangle outline.

### Screen Controls
- `void setContrast(uint8_t value)`: Adjusts contrast (0 to 255).
- `void displayOn()`: Wakes the display from sleep mode.
- `void displayOff()`: Puts the display into sleep mode.

---

## 2. Subsystem Interface: `DisplayAdapter`

Abstract base class defining the hardware interface interface. Subclasses implement the physical I2C or SPI communication protocol.

```cpp
class DisplayAdapter {
public:
    virtual ~DisplayAdapter() = default;
    virtual bool init() = 0;
    virtual void sendBuffer() = 0;
    virtual void clearBuffer() = 0;
    virtual void setPixel(int16_t x, int16_t y, uint8_t color) = 0;
    virtual uint8_t getPixel(int16_t x, int16_t y) const = 0;
    virtual uint16_t getWidth() const = 0;
    virtual uint16_t getHeight() const = 0;
    virtual uint8_t* getBuffer() = 0;
    virtual const uint8_t* getBuffer() const = 0;
    virtual uint16_t getBufferSize() const = 0;
    virtual void setContrast(uint8_t value) = 0;
    virtual void displayOn() = 0;
    virtual void displayOff() = 0;
};
```

### Concrete Adapters Included
- **`SSD1306Adapter`**: For SSD1306-driven OLEDs.
  - `SSD1306Adapter(uint16_t width, uint16_t height, uint8_t address = 0x3C)` (I2C)
  - `SSD1306Adapter(uint16_t width, uint16_t height, int8_t dcPin, int8_t csPin, int8_t rstPin = -1)` (SPI)
- **`SH1106Adapter`**: For SH1106 1.3" OLEDs. Implements page-by-page rendering with a 2-column RAM address offset.
- **`SH1107Adapter`**: For SH1107 128x128 displays.
- **`SSD1327Adapter`**: For SSD1327 128x128 grayscale displays (allocates a 4-bit per pixel framebuffer).

---

## 3. Enumerations

### `VNColor`
Determines the color state of pixels:
- `VNColor::BLACK` (0): Pixel turned off.
- `VNColor::WHITE` (1): Pixel turned on (maximum intensity on SSD1327).
- `VNColor::INVERSE` (2): Toggle pixel state via XOR operation.

### `VNAlign`
Horizontal text layout alignment modes:
- `VNAlign::LEFT` (0): Left-align text.
- `VNAlign::CENTER` (1): Center text.
- `VNAlign::RIGHT` (2): Right-align text.
- `VNAlign::JUSTIFY` (3): Distribute extra space between words.

### `VNVAlign`
Vertical text layout alignment modes:
- `VNVAlign::TOP` (0): Align to container top.
- `VNVAlign::MIDDLE` (1): Center text vertically.
- `VNVAlign::BOTTOM` (2): Align to container bottom.

### `VNTextWrap`
Line wrapping behaviors:
- `VNTextWrap::NONE` (0): No line wrapping.
- `VNTextWrap::WORD` (1): Wrap on space and hyphen characters.
- `VNTextWrap::CHARACTER` (2): Wrap exactly on character bounds.

### `VNRenderMode`
Glyph blending modes:
- `VNRenderMode::TRANSPARENT` (0): Background pixels of the glyph remain unchanged.
- `VNRenderMode::OVERWRITE` (1): Background pixels are set to the inverse of the text color.
- `VNRenderMode::INVERT` (2): Characters are drawn using XOR.
