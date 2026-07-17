/**
 * @file SH1107Adapter.h
 * @brief SH1107 128x128 high-resolution OLED controller adapter.
 *
 * Configures and manages high-density monochrome displays over I2C/SPI.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#ifndef VN_SH1107_ADAPTER_H
#define VN_SH1107_ADAPTER_H

#include "DisplayAdapter.h"

class SH1107Adapter : public DisplayAdapter {
public:
    SH1107Adapter(uint16_t width, uint16_t height,
                   uint8_t address = VN_DEFAULT_I2C_ADDRESS);
    SH1107Adapter(uint16_t width, uint16_t height,
                   int8_t dcPin, int8_t csPin, int8_t rstPin = -1);
    ~SH1107Adapter() override = default;

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
    /** @brief 128×128 / 8 = 2048 bytes. */
    static constexpr uint16_t MAX_BUFFER_SIZE = 2048;

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
    void hardwareReset();
    void sendInitSequence();
};

#endif /* VN_SH1107_ADAPTER_H */
