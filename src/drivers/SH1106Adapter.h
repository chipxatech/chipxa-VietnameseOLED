/**
 * @file SH1106Adapter.h
 * @brief SH1106 1.3-inch OLED display driver adapter.
 *
 * Manages internal 132-column RAM addressing using a 2-column offset shift.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#ifndef VN_SH1106_ADAPTER_H
#define VN_SH1106_ADAPTER_H

#include "DisplayAdapter.h"

/**
 * @class SH1106Adapter
 * @brief Concrete adapter for SH1106-based 1.3" OLED displays.
 */
class SH1106Adapter : public DisplayAdapter {
public:
    SH1106Adapter(uint16_t width, uint16_t height,
                   uint8_t address = VN_DEFAULT_I2C_ADDRESS);

    SH1106Adapter(uint16_t width, uint16_t height,
                   int8_t dcPin, int8_t csPin, int8_t rstPin = -1);

    ~SH1106Adapter() override = default;

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
    static constexpr uint16_t MAX_BUFFER_SIZE = 1024;
    /** @brief SH1106 has 132-column RAM; display starts at column 2. */
    static constexpr uint8_t  COLUMN_OFFSET = 2;

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

#endif /* VN_SH1106_ADAPTER_H */
