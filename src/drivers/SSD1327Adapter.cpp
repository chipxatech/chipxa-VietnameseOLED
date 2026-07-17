/**
 * @file SSD1327Adapter.cpp
 * @brief SSD1327 grayscale display controller communication implementation.
 *
 * Manages pixel conversion to 4-bit nibbles and coordinates full buffer transmissions.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#include "SSD1327Adapter.h"

#ifndef VN_NATIVE_BUILD
    #include <Wire.h>
    #include <SPI.h>
#endif

static constexpr uint8_t SSD1327_DISPLAYOFF     = 0xAE;
static constexpr uint8_t SSD1327_DISPLAYON      = 0xAF;
static constexpr uint8_t SSD1327_SETCONTRAST    = 0x81;
static constexpr uint8_t SSD1327_NORMALDISPLAY  = 0xA4;
static constexpr uint8_t SSD1327_SETCOLADDR     = 0x15;
static constexpr uint8_t SSD1327_SETROWADDR     = 0x75;
static constexpr uint8_t SSD1327_SETREMAP       = 0xA0;
static constexpr uint8_t SSD1327_SETSTARTLINE   = 0xA1;
static constexpr uint8_t SSD1327_SETOFFSET      = 0xA2;
static constexpr uint8_t SSD1327_SETMUX         = 0xA8;
static constexpr uint8_t SSD1327_FUNCSEL        = 0xAB;
static constexpr uint8_t SSD1327_SETPHASELENGTH = 0xB1;
static constexpr uint8_t SSD1327_SETCLKDIV      = 0xB3;
static constexpr uint8_t SSD1327_SETVCOM        = 0xBE;
static constexpr uint8_t SSD1327_SETPRECHARGE   = 0xBC;
static constexpr uint8_t SSD1327_SETSECPRECHARGE = 0xB6;
static constexpr uint8_t SSD1327_SETGRAY        = 0xB8;
static constexpr uint8_t SSD1327_USEBUILTIN_LUT = 0xB9;

/** @brief Grayscale intensity for WHITE (maximum). */
static constexpr uint8_t GRAY_WHITE = 0x0F;
/** @brief Grayscale intensity for BLACK (off). */
static constexpr uint8_t GRAY_BLACK = 0x00;

static constexpr uint8_t I2C_COMMAND = 0x00;
static constexpr uint8_t I2C_DATA    = 0x40;

SSD1327Adapter::SSD1327Adapter(uint16_t width, uint16_t height, uint8_t address)
    : width_(width), height_(height), address_(address)
    , dcPin_(-1), csPin_(-1), rstPin_(-1), useSPI_(false)
    , bufferSize_(static_cast<uint16_t>((width * height) / 2))
{
    if (bufferSize_ > MAX_BUFFER_SIZE) bufferSize_ = MAX_BUFFER_SIZE;
}

SSD1327Adapter::SSD1327Adapter(uint16_t width, uint16_t height,
                                 int8_t dcPin, int8_t csPin, int8_t rstPin)
    : width_(width), height_(height), address_(0)
    , dcPin_(dcPin), csPin_(csPin), rstPin_(rstPin), useSPI_(true)
    , bufferSize_(static_cast<uint16_t>((width * height) / 2))
{
    if (bufferSize_ > MAX_BUFFER_SIZE) bufferSize_ = MAX_BUFFER_SIZE;
}

bool SSD1327Adapter::init() {
    clearBuffer();
#ifndef VN_NATIVE_BUILD
    if (useSPI_) {
        SPI.begin();
        if (dcPin_ >= 0) pinMode(dcPin_, OUTPUT);
        if (csPin_ >= 0) { pinMode(csPin_, OUTPUT); digitalWrite(csPin_, HIGH); }
    } else {
        Wire.begin();
        Wire.setClock(VN_DEFAULT_I2C_CLOCK);
    }
    hardwareReset();
#endif
    sendInitSequence();
    return true;
}

void SSD1327Adapter::sendBuffer() {
#ifndef VN_NATIVE_BUILD
    /* Set column and row address for full screen */
    sendCommand(SSD1327_SETCOLADDR);
    sendCommand(0x00);
    sendCommand(static_cast<uint8_t>((width_ / 2) - 1));
    sendCommand(SSD1327_SETROWADDR);
    sendCommand(0x00);
    sendCommand(static_cast<uint8_t>(height_ - 1));

    if (useSPI_) {
        if (csPin_ >= 0) digitalWrite(csPin_, LOW);
        if (dcPin_ >= 0) digitalWrite(dcPin_, HIGH);
        for (uint16_t i = 0; i < bufferSize_; i++) {
            SPI.transfer(buffer_[i]);
        }
        if (csPin_ >= 0) digitalWrite(csPin_, HIGH);
    } else {
        for (uint16_t i = 0; i < bufferSize_; i += 16) {
            Wire.beginTransmission(address_);
            Wire.write(I2C_DATA);
            uint16_t end = i + 16;
            if (end > bufferSize_) end = bufferSize_;
            for (uint16_t j = i; j < end; j++) {
                Wire.write(buffer_[j]);
            }
            Wire.endTransmission();
        }
    }
#endif
}

void SSD1327Adapter::clearBuffer() {
    for (uint16_t i = 0; i < bufferSize_; i++) buffer_[i] = 0x00;
}

/**
 * Buffer layout (4-bit grayscale, 2 pixels per byte):
 * Byte index = (y * width + x) / 2
 * Even x: high nibble, Odd x: low nibble
 */
void SSD1327Adapter::setPixel(int16_t x, int16_t y, uint8_t color) {
    if (x < 0 || x >= (int16_t)width_ || y < 0 || y >= (int16_t)height_) return;

    uint16_t idx = (static_cast<uint16_t>(y) * width_ + static_cast<uint16_t>(x)) / 2;
    bool isHighNibble = (x & 1) == 0;

    uint8_t gray;
    switch (color) {
        case 0: gray = GRAY_BLACK; break;
        case 1: gray = GRAY_WHITE; break;
        case 2: /* INVERSE */
            if (isHighNibble) {
                gray = 0x0F - ((buffer_[idx] >> 4) & 0x0F);
            } else {
                gray = 0x0F - (buffer_[idx] & 0x0F);
            }
            break;
        default: return;
    }

    if (isHighNibble) {
        buffer_[idx] = (buffer_[idx] & 0x0F) | (gray << 4);
    } else {
        buffer_[idx] = (buffer_[idx] & 0xF0) | (gray & 0x0F);
    }
}

uint8_t SSD1327Adapter::getPixel(int16_t x, int16_t y) const {
    if (x < 0 || x >= (int16_t)width_ || y < 0 || y >= (int16_t)height_) return 0;

    uint16_t idx = (static_cast<uint16_t>(y) * width_ + static_cast<uint16_t>(x)) / 2;
    uint8_t nibble;
    if ((x & 1) == 0) {
        nibble = (buffer_[idx] >> 4) & 0x0F;
    } else {
        nibble = buffer_[idx] & 0x0F;
    }
    return (nibble > 0) ? 1 : 0;
}

uint16_t SSD1327Adapter::getWidth() const { return width_; }
uint16_t SSD1327Adapter::getHeight() const { return height_; }
uint8_t* SSD1327Adapter::getBuffer() { return buffer_; }
const uint8_t* SSD1327Adapter::getBuffer() const { return buffer_; }
uint16_t SSD1327Adapter::getBufferSize() const { return bufferSize_; }

void SSD1327Adapter::setContrast(uint8_t value) {
    sendCommand(SSD1327_SETCONTRAST); sendCommand(value);
}
void SSD1327Adapter::displayOn()  { sendCommand(SSD1327_DISPLAYON); }
void SSD1327Adapter::displayOff() { sendCommand(SSD1327_DISPLAYOFF); }

void SSD1327Adapter::sendCommand(uint8_t cmd) {
#ifndef VN_NATIVE_BUILD
    if (useSPI_) {
        if (csPin_ >= 0) digitalWrite(csPin_, LOW);
        if (dcPin_ >= 0) digitalWrite(dcPin_, LOW);
        SPI.transfer(cmd);
        if (csPin_ >= 0) digitalWrite(csPin_, HIGH);
    } else {
        Wire.beginTransmission(address_);
        Wire.write(I2C_COMMAND); Wire.write(cmd);
        Wire.endTransmission();
    }
#else
    (void)cmd;
#endif
}

void SSD1327Adapter::sendCommandArg(uint8_t cmd, uint8_t arg) {
    sendCommand(cmd);
    sendCommand(arg);
}

void SSD1327Adapter::hardwareReset() {
#ifndef VN_NATIVE_BUILD
    if (rstPin_ >= 0) {
        pinMode(rstPin_, OUTPUT);
        digitalWrite(rstPin_, HIGH); delay(1);
        digitalWrite(rstPin_, LOW);  delay(10);
        digitalWrite(rstPin_, HIGH); delay(10);
    }
#endif
}

void SSD1327Adapter::sendInitSequence() {
    sendCommand(SSD1327_DISPLAYOFF);
    sendCommandArg(SSD1327_SETCLKDIV, 0x01);
    sendCommandArg(SSD1327_SETMUX, static_cast<uint8_t>(height_ - 1));
    sendCommandArg(SSD1327_SETOFFSET, 0x00);
    sendCommandArg(SSD1327_SETSTARTLINE, 0x00);
    sendCommandArg(SSD1327_SETREMAP, 0x51); /* Column remap + COM remap */
    sendCommandArg(SSD1327_FUNCSEL, 0x01);  /* Internal regulator */
    sendCommandArg(SSD1327_SETCONTRAST, 0x7F);
    sendCommandArg(SSD1327_SETPHASELENGTH, 0x11);
    sendCommand(SSD1327_USEBUILTIN_LUT);
    sendCommandArg(SSD1327_SETPRECHARGE, 0x08);
    sendCommandArg(SSD1327_SETVCOM, 0x07);
    sendCommandArg(SSD1327_SETSECPRECHARGE, 0x01);
    sendCommand(SSD1327_NORMALDISPLAY);
    sendCommand(SSD1327_DISPLAYON);
}
