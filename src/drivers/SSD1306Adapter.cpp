/**
 * @file SSD1306Adapter.cpp
 * @brief SSD1306 controller communication adapter implementation.
 *
 * Handles connection protocol sequences and uploads framebuffer bytes using page addressing.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#include "SSD1306Adapter.h"

#ifndef VN_NATIVE_BUILD
    #include <Wire.h>
    #include <SPI.h>
#endif

/* ─────────────────────────────────────────────────────────────────────────────
 * SSD1306 Command Definitions
 * ────────────────────────────────────────────────────────────────────────── */

static constexpr uint8_t SSD1306_SETCONTRAST         = 0x81;
static constexpr uint8_t SSD1306_DISPLAYALLON_RESUME = 0xA4;
static constexpr uint8_t SSD1306_NORMALDISPLAY       = 0xA6;
static constexpr uint8_t SSD1306_DISPLAYOFF          = 0xAE;
static constexpr uint8_t SSD1306_DISPLAYON           = 0xAF;
static constexpr uint8_t SSD1306_SETDISPLAYOFFSET    = 0xD3;
static constexpr uint8_t SSD1306_SETCOMPINS          = 0xDA;
static constexpr uint8_t SSD1306_SETVCOMDETECT       = 0xDB;
static constexpr uint8_t SSD1306_SETDISPLAYCLOCKDIV  = 0xD5;
static constexpr uint8_t SSD1306_SETPRECHARGE        = 0xD9;
static constexpr uint8_t SSD1306_SETMULTIPLEX        = 0xA8;
static constexpr uint8_t SSD1306_SETLOWCOLUMN        = 0x00;
static constexpr uint8_t SSD1306_SETHIGHCOLUMN       = 0x10;
static constexpr uint8_t SSD1306_SETSTARTLINE        = 0x40;
static constexpr uint8_t SSD1306_MEMORYMODE          = 0x20;
static constexpr uint8_t SSD1306_COLUMNADDR          = 0x21;
static constexpr uint8_t SSD1306_PAGEADDR            = 0x22;
static constexpr uint8_t SSD1306_SEGREMAP            = 0xA0;
static constexpr uint8_t SSD1306_COMSCANDEC          = 0xC8;
static constexpr uint8_t SSD1306_CHARGEPUMP          = 0x8D;
static constexpr uint8_t SSD1306_DEACTIVATE_SCROLL   = 0x2E;

/** @brief I2C control byte for command data. */
static constexpr uint8_t I2C_COMMAND = 0x00;
/** @brief I2C control byte for display data. */
static constexpr uint8_t I2C_DATA    = 0x40;

/* ─────────────────────────────────────────────────────────────────────────────
 * Constructors
 * ────────────────────────────────────────────────────────────────────────── */

SSD1306Adapter::SSD1306Adapter(uint16_t width, uint16_t height, uint8_t address)
    : width_(width)
    , height_(height)
    , address_(address)
    , dcPin_(-1)
    , csPin_(-1)
    , rstPin_(-1)
    , useSPI_(false)
    , bufferSize_(static_cast<uint16_t>((width * height) / 8))
{
    if (bufferSize_ > MAX_BUFFER_SIZE) {
        bufferSize_ = MAX_BUFFER_SIZE;
    }
}

SSD1306Adapter::SSD1306Adapter(uint16_t width, uint16_t height,
                                 int8_t dcPin, int8_t csPin, int8_t rstPin)
    : width_(width)
    , height_(height)
    , address_(0)
    , dcPin_(dcPin)
    , csPin_(csPin)
    , rstPin_(rstPin)
    , useSPI_(true)
    , bufferSize_(static_cast<uint16_t>((width * height) / 8))
{
    if (bufferSize_ > MAX_BUFFER_SIZE) {
        bufferSize_ = MAX_BUFFER_SIZE;
    }
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Init
 * ────────────────────────────────────────────────────────────────────────── */

bool SSD1306Adapter::init() {
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

/* ─────────────────────────────────────────────────────────────────────────────
 * Send Framebuffer
 * ────────────────────────────────────────────────────────────────────────── */

void SSD1306Adapter::sendBuffer() {
#ifndef VN_NATIVE_BUILD
    /* Set column and page address range for full screen */
    sendCommand(SSD1306_COLUMNADDR);
    sendCommand(0);
    sendCommand(static_cast<uint8_t>(width_ - 1));
    sendCommand(SSD1306_PAGEADDR);
    sendCommand(0);
    sendCommand(static_cast<uint8_t>((height_ / 8) - 1));

    if (useSPI_) {
        if (csPin_ >= 0) digitalWrite(csPin_, LOW);
        if (dcPin_ >= 0) digitalWrite(dcPin_, HIGH); /* Data mode */
        for (uint16_t i = 0; i < bufferSize_; i++) {
            SPI.transfer(buffer_[i]);
        }
        if (csPin_ >= 0) digitalWrite(csPin_, HIGH);
    } else {
        /* I2C — send in 16-byte chunks (Wire buffer limit on some platforms) */
        for (uint16_t i = 0; i < bufferSize_; i += 16) {
            Wire.beginTransmission(address_);
            Wire.write(I2C_DATA);
            uint16_t chunkEnd = i + 16;
            if (chunkEnd > bufferSize_) chunkEnd = bufferSize_;
            for (uint16_t j = i; j < chunkEnd; j++) {
                Wire.write(buffer_[j]);
            }
            Wire.endTransmission();
        }
    }
#endif
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Clear Buffer
 * ────────────────────────────────────────────────────────────────────────── */

void SSD1306Adapter::clearBuffer() {
    for (uint16_t i = 0; i < bufferSize_; i++) {
        buffer_[i] = 0x00;
    }
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Pixel Operations (Page-Based Framebuffer)
 *
 * Buffer layout: Each byte represents 8 vertical pixels (one "page").
 * Bit 0 = top pixel, bit 7 = bottom pixel of the page.
 * Pages are arranged left-to-right, then top-to-bottom.
 *
 * Byte index = x + (y / 8) * width
 * Bit within byte = y % 8
 * ────────────────────────────────────────────────────────────────────────── */

void SSD1306Adapter::setPixel(int16_t x, int16_t y, uint8_t color) {
    if (x < 0 || x >= static_cast<int16_t>(width_) ||
        y < 0 || y >= static_cast<int16_t>(height_)) {
        return;
    }

    uint16_t byteIndex = static_cast<uint16_t>(x) +
                          (static_cast<uint16_t>(y) / 8) * width_;
    uint8_t  bitMask   = 1 << (static_cast<uint8_t>(y) & 0x07);

    switch (color) {
        case 0: /* BLACK — clear bit */
            buffer_[byteIndex] &= ~bitMask;
            break;
        case 1: /* WHITE — set bit */
            buffer_[byteIndex] |= bitMask;
            break;
        case 2: /* INVERSE — toggle bit */
            buffer_[byteIndex] ^= bitMask;
            break;
        default:
            break;
    }
}

uint8_t SSD1306Adapter::getPixel(int16_t x, int16_t y) const {
    if (x < 0 || x >= static_cast<int16_t>(width_) ||
        y < 0 || y >= static_cast<int16_t>(height_)) {
        return 0;
    }

    uint16_t byteIndex = static_cast<uint16_t>(x) +
                          (static_cast<uint16_t>(y) / 8) * width_;
    uint8_t  bitMask   = 1 << (static_cast<uint8_t>(y) & 0x07);

    return (buffer_[byteIndex] & bitMask) ? 1 : 0;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Display Properties
 * ────────────────────────────────────────────────────────────────────────── */

uint16_t SSD1306Adapter::getWidth() const { return width_; }
uint16_t SSD1306Adapter::getHeight() const { return height_; }
uint8_t* SSD1306Adapter::getBuffer() { return buffer_; }
const uint8_t* SSD1306Adapter::getBuffer() const { return buffer_; }
uint16_t SSD1306Adapter::getBufferSize() const { return bufferSize_; }

/* ─────────────────────────────────────────────────────────────────────────────
 * Display Control
 * ────────────────────────────────────────────────────────────────────────── */

void SSD1306Adapter::setContrast(uint8_t value) {
    sendCommand(SSD1306_SETCONTRAST);
    sendCommand(value);
}

void SSD1306Adapter::displayOn() {
    sendCommand(SSD1306_DISPLAYON);
}

void SSD1306Adapter::displayOff() {
    sendCommand(SSD1306_DISPLAYOFF);
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Internal — Command Communication
 * ────────────────────────────────────────────────────────────────────────── */

void SSD1306Adapter::sendCommand(uint8_t cmd) {
#ifndef VN_NATIVE_BUILD
    if (useSPI_) {
        if (csPin_ >= 0) digitalWrite(csPin_, LOW);
        if (dcPin_ >= 0) digitalWrite(dcPin_, LOW); /* Command mode */
        SPI.transfer(cmd);
        if (csPin_ >= 0) digitalWrite(csPin_, HIGH);
    } else {
        Wire.beginTransmission(address_);
        Wire.write(I2C_COMMAND);
        Wire.write(cmd);
        Wire.endTransmission();
    }
#else
    (void)cmd;
#endif
}

void SSD1306Adapter::sendCommandList(const uint8_t* cmds, uint8_t count) {
    for (uint8_t i = 0; i < count; i++) {
        sendCommand(cmds[i]);
    }
}

void SSD1306Adapter::hardwareReset() {
#ifndef VN_NATIVE_BUILD
    if (rstPin_ >= 0) {
        pinMode(rstPin_, OUTPUT);
        digitalWrite(rstPin_, HIGH);
        delay(1);
        digitalWrite(rstPin_, LOW);
        delay(10);
        digitalWrite(rstPin_, HIGH);
        delay(10);
    }
#endif
}

void SSD1306Adapter::sendInitSequence() {
    sendCommand(SSD1306_DISPLAYOFF);
    sendCommand(SSD1306_SETDISPLAYCLOCKDIV);
    sendCommand(0x80);
    sendCommand(SSD1306_SETMULTIPLEX);
    sendCommand(static_cast<uint8_t>(height_ - 1));
    sendCommand(SSD1306_SETDISPLAYOFFSET);
    sendCommand(0x00);
    sendCommand(SSD1306_SETSTARTLINE | 0x00);
    sendCommand(SSD1306_CHARGEPUMP);
    sendCommand(0x14); /* Internal VCC */
    sendCommand(SSD1306_MEMORYMODE);
    sendCommand(0x00); /* Horizontal addressing mode */
    sendCommand(SSD1306_SEGREMAP | 0x01);
    sendCommand(SSD1306_COMSCANDEC);
    sendCommand(SSD1306_SETCOMPINS);
    sendCommand((height_ == 32) ? 0x02 : 0x12);
    sendCommand(SSD1306_SETCONTRAST);
    sendCommand(0xCF);
    sendCommand(SSD1306_SETPRECHARGE);
    sendCommand(0xF1);
    sendCommand(SSD1306_SETVCOMDETECT);
    sendCommand(0x40);
    sendCommand(SSD1306_DISPLAYALLON_RESUME);
    sendCommand(SSD1306_NORMALDISPLAY);
    sendCommand(SSD1306_DEACTIVATE_SCROLL);
    sendCommand(SSD1306_DISPLAYON);
}
