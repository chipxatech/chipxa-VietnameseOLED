/**
 * @file SH1106Adapter.cpp
 * @brief SH1106 display controller adapter implementation.
 *
 * Implements page-by-page data transmission combined with column offset addressing.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#include "SH1106Adapter.h"

#ifndef VN_NATIVE_BUILD
    #include <Wire.h>
    #include <SPI.h>
#endif

/* SH1106 commands (compatible subset with SSD1306) */
static constexpr uint8_t SH1106_DISPLAYOFF          = 0xAE;
static constexpr uint8_t SH1106_DISPLAYON           = 0xAF;
static constexpr uint8_t SH1106_SETCONTRAST         = 0x81;
static constexpr uint8_t SH1106_NORMALDISPLAY       = 0xA6;
static constexpr uint8_t SH1106_DISPLAYALLON_RESUME = 0xA4;
static constexpr uint8_t SH1106_SETDISPLAYCLOCKDIV  = 0xD5;
static constexpr uint8_t SH1106_SETMULTIPLEX        = 0xA8;
static constexpr uint8_t SH1106_SETDISPLAYOFFSET    = 0xD3;
static constexpr uint8_t SH1106_SETSTARTLINE        = 0x40;
static constexpr uint8_t SH1106_SEGREMAP            = 0xA0;
static constexpr uint8_t SH1106_COMSCANDEC          = 0xC8;
static constexpr uint8_t SH1106_SETCOMPINS          = 0xDA;
static constexpr uint8_t SH1106_SETPRECHARGE        = 0xD9;
static constexpr uint8_t SH1106_SETVCOMDETECT       = 0xDB;
static constexpr uint8_t SH1106_SETPAGEADDR         = 0xB0;
static constexpr uint8_t SH1106_SETLOWCOLUMN        = 0x00;
static constexpr uint8_t SH1106_SETHIGHCOLUMN       = 0x10;

static constexpr uint8_t I2C_COMMAND = 0x00;
static constexpr uint8_t I2C_DATA    = 0x40;

/* ── Constructors ─────────────────────────────────────────────────────────── */

SH1106Adapter::SH1106Adapter(uint16_t width, uint16_t height, uint8_t address)
    : width_(width), height_(height), address_(address)
    , dcPin_(-1), csPin_(-1), rstPin_(-1), useSPI_(false)
    , bufferSize_(static_cast<uint16_t>((width * height) / 8))
{
    if (bufferSize_ > MAX_BUFFER_SIZE) bufferSize_ = MAX_BUFFER_SIZE;
}

SH1106Adapter::SH1106Adapter(uint16_t width, uint16_t height,
                               int8_t dcPin, int8_t csPin, int8_t rstPin)
    : width_(width), height_(height), address_(0)
    , dcPin_(dcPin), csPin_(csPin), rstPin_(rstPin), useSPI_(true)
    , bufferSize_(static_cast<uint16_t>((width * height) / 8))
{
    if (bufferSize_ > MAX_BUFFER_SIZE) bufferSize_ = MAX_BUFFER_SIZE;
}

/* ── Init ─────────────────────────────────────────────────────────────────── */

bool SH1106Adapter::init() {
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

/* ── Send Buffer (Page-by-Page) ───────────────────────────────────────────── */

void SH1106Adapter::sendBuffer() {
#ifndef VN_NATIVE_BUILD
    uint8_t pages = static_cast<uint8_t>(height_ / 8);

    for (uint8_t page = 0; page < pages; page++) {
        /* Set page address and column address with offset */
        sendCommand(SH1106_SETPAGEADDR | page);
        sendCommand(SH1106_SETLOWCOLUMN | (COLUMN_OFFSET & 0x0F));
        sendCommand(SH1106_SETHIGHCOLUMN | ((COLUMN_OFFSET >> 4) & 0x0F));

        uint16_t offset = static_cast<uint16_t>(page) * width_;

        if (useSPI_) {
            if (csPin_ >= 0) digitalWrite(csPin_, LOW);
            if (dcPin_ >= 0) digitalWrite(dcPin_, HIGH);
            for (uint16_t col = 0; col < width_; col++) {
                SPI.transfer(buffer_[offset + col]);
            }
            if (csPin_ >= 0) digitalWrite(csPin_, HIGH);
        } else {
            for (uint16_t col = 0; col < width_; col += 16) {
                Wire.beginTransmission(address_);
                Wire.write(I2C_DATA);
                uint16_t chunkEnd = col + 16;
                if (chunkEnd > width_) chunkEnd = width_;
                for (uint16_t c = col; c < chunkEnd; c++) {
                    Wire.write(buffer_[offset + c]);
                }
                Wire.endTransmission();
            }
        }
    }
#endif
}

/* ── Framebuffer Operations (identical to SSD1306) ────────────────────────── */

void SH1106Adapter::clearBuffer() {
    for (uint16_t i = 0; i < bufferSize_; i++) buffer_[i] = 0x00;
}

void SH1106Adapter::setPixel(int16_t x, int16_t y, uint8_t color) {
    if (x < 0 || x >= (int16_t)width_ || y < 0 || y >= (int16_t)height_) return;
    uint16_t idx = (uint16_t)x + ((uint16_t)y / 8) * width_;
    uint8_t mask = 1 << ((uint8_t)y & 0x07);
    switch (color) {
        case 0: buffer_[idx] &= ~mask; break;
        case 1: buffer_[idx] |= mask;  break;
        case 2: buffer_[idx] ^= mask;  break;
    }
}

uint8_t SH1106Adapter::getPixel(int16_t x, int16_t y) const {
    if (x < 0 || x >= (int16_t)width_ || y < 0 || y >= (int16_t)height_) return 0;
    uint16_t idx = (uint16_t)x + ((uint16_t)y / 8) * width_;
    return (buffer_[idx] & (1 << ((uint8_t)y & 0x07))) ? 1 : 0;
}

uint16_t SH1106Adapter::getWidth() const { return width_; }
uint16_t SH1106Adapter::getHeight() const { return height_; }
uint8_t* SH1106Adapter::getBuffer() { return buffer_; }
const uint8_t* SH1106Adapter::getBuffer() const { return buffer_; }
uint16_t SH1106Adapter::getBufferSize() const { return bufferSize_; }

void SH1106Adapter::setContrast(uint8_t value) {
    sendCommand(SH1106_SETCONTRAST);
    sendCommand(value);
}

void SH1106Adapter::displayOn() { sendCommand(SH1106_DISPLAYON); }
void SH1106Adapter::displayOff() { sendCommand(SH1106_DISPLAYOFF); }

/* ── Internal ─────────────────────────────────────────────────────────────── */

void SH1106Adapter::sendCommand(uint8_t cmd) {
#ifndef VN_NATIVE_BUILD
    if (useSPI_) {
        if (csPin_ >= 0) digitalWrite(csPin_, LOW);
        if (dcPin_ >= 0) digitalWrite(dcPin_, LOW);
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

void SH1106Adapter::hardwareReset() {
#ifndef VN_NATIVE_BUILD
    if (rstPin_ >= 0) {
        pinMode(rstPin_, OUTPUT);
        digitalWrite(rstPin_, HIGH); delay(1);
        digitalWrite(rstPin_, LOW);  delay(10);
        digitalWrite(rstPin_, HIGH); delay(10);
    }
#endif
}

void SH1106Adapter::sendInitSequence() {
    sendCommand(SH1106_DISPLAYOFF);
    sendCommand(SH1106_SETDISPLAYCLOCKDIV); sendCommand(0x80);
    sendCommand(SH1106_SETMULTIPLEX); sendCommand(static_cast<uint8_t>(height_ - 1));
    sendCommand(SH1106_SETDISPLAYOFFSET); sendCommand(0x00);
    sendCommand(SH1106_SETSTARTLINE | 0x00);
    sendCommand(SH1106_SEGREMAP | 0x01);
    sendCommand(SH1106_COMSCANDEC);
    sendCommand(SH1106_SETCOMPINS); sendCommand(0x12);
    sendCommand(SH1106_SETCONTRAST); sendCommand(0xCF);
    sendCommand(SH1106_SETPRECHARGE); sendCommand(0xF1);
    sendCommand(SH1106_SETVCOMDETECT); sendCommand(0x40);
    sendCommand(SH1106_DISPLAYALLON_RESUME);
    sendCommand(SH1106_NORMALDISPLAY);
    sendCommand(SH1106_DISPLAYON);
}
