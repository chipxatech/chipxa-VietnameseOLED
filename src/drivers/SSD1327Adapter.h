/**
 * @file SSD1327Adapter.h
 * @brief SSD1327 4-bit grayscale OLED controller adapter.
 *
 * Manages 16-level gray framebuffers for 128x128 displays.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#ifndef VN_SSD1327_ADAPTER_H
#define VN_SSD1327_ADAPTER_H

#include "DisplayAdapter.h"

class SSD1327Adapter : public DisplayAdapter {
public:
    SSD1327Adapter(uint16_t width, uint16_t height,
                    uint8_t address = VN_DEFAULT_I2C_ADDRESS);
    SSD1327Adapter(uint16_t width, uint16_t height,
                    int8_t dcPin, int8_t csPin, int8_t rstPin = -1);
    ~SSD1327Adapter() override = default;

    bool     init() override;
    void     sendBuffer() override;
    void     clearBuffer() override;
    void     setPixel(int16_t x, int16_t y, uint8_t color) override;
    uint8_t  getPixel(int16_t x, int16_t y) const override;
    uint16_t getWidth() const override;
    uint16_t getHeight() const override;
    uint8_t* getBuffer() override;
    const uint8_t* getBuffer() const override;
    uint16_t getBufferSize() const override;
    void     setContrast(uint8_t value) override;
    void     displayOn() override;
    void     displayOff() override;

private:
    /**
     * @brief 4-bit grayscale: 128×128 pixels, 2 pixels per byte = 8192 bytes.
     * Note: This is large; only suitable for MCUs with sufficient RAM (ESP32/STM32).
     */
    static constexpr uint16_t MAX_BUFFER_SIZE = 8192;

    uint16_t width_;
    uint16_t height_;
    uint8_t  address_;
    int8_t   dcPin_;
    int8_t   csPin_;
    int8_t   rstPin_;
    bool     useSPI_;
    uint16_t bufferSize_;
    uint8_t  buffer_[MAX_BUFFER_SIZE];

    void sendCommand(uint8_t cmd);
    void sendCommandArg(uint8_t cmd, uint8_t arg);
    void hardwareReset();
    void sendInitSequence();
};

#endif /* VN_SSD1327_ADAPTER_H */
