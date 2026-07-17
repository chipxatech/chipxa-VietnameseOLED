/**
 * @file SH1107Adapter.cpp
 * @brief SH1107 controller communication implementation.
 *
 * Sends vertical memory maps and coordinates buffer uploads on 128x128 panels.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#include "SH1107Adapter.h"

#ifndef VN_NATIVE_BUILD
    #include <Wire.h>
    #include <SPI.h>
#endif

static constexpr uint8_t SH1107_DISPLAYOFF     = 0xAE;
static constexpr uint8_t SH1107_DISPLAYON      = 0xAF;
static constexpr uint8_t SH1107_SETCONTRAST    = 0x81;
static constexpr uint8_t SH1107_NORMALDISPLAY  = 0xA6;
static constexpr uint8_t SH1107_DISPLAYALLON_R = 0xA4;
static constexpr uint8_t SH1107_SETMULTIPLEX   = 0xA8;
static constexpr uint8_t SH1107_DCDC           = 0xAD;
static constexpr uint8_t SH1107_SETPAGEADDR    = 0xB0;
static constexpr uint8_t SH1107_SETLOWCOL      = 0x00;
static constexpr uint8_t SH1107_SETHIGHCOL     = 0x10;
static constexpr uint8_t SH1107_SETSTARTLINE   = 0xDC;
static constexpr uint8_t SH1107_SETDISPOFFSET  = 0xD3;
static constexpr uint8_t SH1107_SETCLKDIV      = 0xD5;
static constexpr uint8_t SH1107_SETPRECHARGE   = 0xD9;
static constexpr uint8_t SH1107_SETVCOM        = 0xDB;
static constexpr uint8_t SH1107_SEGREMAPNORM   = 0xA0;
static constexpr uint8_t SH1107_SEGREMAPINV    = 0xA1;
static constexpr uint8_t SH1107_COMSCANINC     = 0xC0;
static constexpr uint8_t SH1107_COMSCANDEC     = 0xC8;

static constexpr uint8_t I2C_COMMAND = 0x00;
static constexpr uint8_t I2C_DATA    = 0x40;

SH1107Adapter::SH1107Adapter(uint16_t width, uint16_t height, uint8_t address)
    : width_(width), height_(height), address_(address)
    , dcPin_(-1), csPin_(-1), rstPin_(-1), useSPI_(false)
    , bufferSize_(static_cast<uint16_t>((width * height) / 8))
{
    if (bufferSize_ > MAX_BUFFER_SIZE) bufferSize_ = MAX_BUFFER_SIZE;
}

SH1107Adapter::SH1107Adapter(uint16_t width, uint16_t height,
                               int8_t dcPin, int8_t csPin, int8_t rstPin)
    : width_(width), height_(height), address_(0)
    , dcPin_(dcPin), csPin_(csPin), rstPin_(rstPin), useSPI_(true)
    , bufferSize_(static_cast<uint16_t>((width * height) / 8))
{
    if (bufferSize_ > MAX_BUFFER_SIZE) bufferSize_ = MAX_BUFFER_SIZE;
}

bool SH1107Adapter::init() {
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

void SH1107Adapter::sendBuffer() {
#ifndef VN_NATIVE_BUILD
    uint8_t pages = static_cast<uint8_t>(height_ / 8);
    for (uint8_t page = 0; page < pages; page++) {
        sendCommand(SH1107_SETPAGEADDR | page);
        sendCommand(SH1107_SETLOWCOL | 0x00);
        sendCommand(SH1107_SETHIGHCOL | 0x00);

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
                uint16_t end = col + 16;
                if (end > width_) end = width_;
                for (uint16_t c = col; c < end; c++) {
                    Wire.write(buffer_[offset + c]);
                }
                Wire.endTransmission();
            }
        }
    }
#endif
}

void SH1107Adapter::clearBuffer() {
    for (uint16_t i = 0; i < bufferSize_; i++) buffer_[i] = 0x00;
}

void SH1107Adapter::setPixel(int16_t x, int16_t y, uint8_t color) {
    if (x < 0 || x >= (int16_t)width_ || y < 0 || y >= (int16_t)height_) return;
    uint16_t idx = (uint16_t)x + ((uint16_t)y / 8) * width_;
    uint8_t mask = 1 << ((uint8_t)y & 0x07);
    switch (color) {
        case 0: buffer_[idx] &= ~mask; break;
        case 1: buffer_[idx] |= mask;  break;
        case 2: buffer_[idx] ^= mask;  break;
    }
}

uint8_t SH1107Adapter::getPixel(int16_t x, int16_t y) const {
    if (x < 0 || x >= (int16_t)width_ || y < 0 || y >= (int16_t)height_) return 0;
    uint16_t idx = (uint16_t)x + ((uint16_t)y / 8) * width_;
    return (buffer_[idx] & (1 << ((uint8_t)y & 0x07))) ? 1 : 0;
}

uint16_t SH1107Adapter::getWidth() const { return width_; }
uint16_t SH1107Adapter::getHeight() const { return height_; }
uint8_t* SH1107Adapter::getBuffer() { return buffer_; }
const uint8_t* SH1107Adapter::getBuffer() const { return buffer_; }
uint16_t SH1107Adapter::getBufferSize() const { return bufferSize_; }

void SH1107Adapter::setContrast(uint8_t value) {
    sendCommand(SH1107_SETCONTRAST); sendCommand(value);
}
void SH1107Adapter::displayOn()  { sendCommand(SH1107_DISPLAYON); }
void SH1107Adapter::displayOff() { sendCommand(SH1107_DISPLAYOFF); }

void SH1107Adapter::sendCommand(uint8_t cmd) {
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

void SH1107Adapter::hardwareReset() {
#ifndef VN_NATIVE_BUILD
    if (rstPin_ >= 0) {
        pinMode(rstPin_, OUTPUT);
        digitalWrite(rstPin_, HIGH); delay(1);
        digitalWrite(rstPin_, LOW);  delay(10);
        digitalWrite(rstPin_, HIGH); delay(10);
    }
#endif
}

void SH1107Adapter::sendInitSequence() {
    sendCommand(SH1107_DISPLAYOFF);
    sendCommand(SH1107_SETCLKDIV);   sendCommand(0x51);
    sendCommand(SH1107_SETMULTIPLEX); sendCommand(0x7F); /* 128 MUX */
    sendCommand(SH1107_SETDISPOFFSET); sendCommand(0x00);
    sendCommand(SH1107_SETSTARTLINE);  sendCommand(0x00);
    sendCommand(SH1107_DCDC);         sendCommand(0x8B); /* DC-DC on */
    sendCommand(SH1107_SEGREMAPINV);
    sendCommand(SH1107_COMSCANDEC);
    sendCommand(SH1107_SETCONTRAST);  sendCommand(0x80);
    sendCommand(SH1107_SETPRECHARGE); sendCommand(0x22);
    sendCommand(SH1107_SETVCOM);      sendCommand(0x35);
    sendCommand(SH1107_DISPLAYALLON_R);
    sendCommand(SH1107_NORMALDISPLAY);
    sendCommand(SH1107_DISPLAYON);
}
