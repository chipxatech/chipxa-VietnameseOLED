/**
 * @file mock_display_adapter.h
 * @brief Mock display adapter for testing framebuffer operations.
 *
 * Implements DisplayAdapter to allow unit testing of rendering classes.
 *
 * @author VietnameseOLED Contributors
 * @version 1.0.0
 * @copyright MIT License
 */

#ifndef MOCK_DISPLAY_ADAPTER_H
#define MOCK_DISPLAY_ADAPTER_H

#include "drivers/DisplayAdapter.h"

class MockDisplayAdapter : public DisplayAdapter {
public:
    MockDisplayAdapter(uint16_t width, uint16_t height)
        : width_(width)
        , height_(height)
        , bufferSize_((width * height) / 8)
    {
        buffer_ = new uint8_t[bufferSize_];
        clearBuffer();
    }

    ~MockDisplayAdapter() override {
        delete[] buffer_;
    }

    bool init() override {
        clearBuffer();
        return true;
    }

    void sendBuffer() override {
        // No-op for mock
    }

    void clearBuffer() override {
        for (uint16_t i = 0; i < bufferSize_; i++) {
            buffer_[i] = 0x00;
        }
    }

    void setPixel(int16_t x, int16_t y, uint8_t color) override {
        if (x < 0 || x >= static_cast<int16_t>(width_) ||
            y < 0 || y >= static_cast<int16_t>(height_)) {
            return;
        }
        uint16_t byteIndex = x + (y / 8) * width_;
        uint8_t bitMask = 1 << (y % 8);

        if (color == 0) {
            buffer_[byteIndex] &= ~bitMask;
        } else if (color == 1) {
            buffer_[byteIndex] |= bitMask;
        } else if (color == 2) {
            buffer_[byteIndex] ^= bitMask;
        }
    }

    uint8_t getPixel(int16_t x, int16_t y) const override {
        if (x < 0 || x >= static_cast<int16_t>(width_) ||
            y < 0 || y >= static_cast<int16_t>(height_)) {
            return 0;
        }
        uint16_t byteIndex = x + (y / 8) * width_;
        uint8_t bitMask = 1 << (y % 8);
        return (buffer_[byteIndex] & bitMask) ? 1 : 0;
    }

    uint16_t getWidth() const override { return width_; }
    uint16_t getHeight() const override { return height_; }
    uint8_t* getBuffer() override { return buffer_; }
    const uint8_t* getBuffer() const override { return buffer_; }
    uint16_t getBufferSize() const override { return bufferSize_; }

    void setContrast(uint8_t value) override { (void)value; }
    void displayOn() override {}
    void displayOff() override {}

private:
    uint16_t width_;
    uint16_t height_;
    uint16_t bufferSize_;
    uint8_t* buffer_;
};

#endif /* MOCK_DISPLAY_ADAPTER_H */
