/**
 * @file SSD1306Adapter.h
 * @brief SSD1306 monochrome OLED driver adapter.
 *
 * Supports 128x64/128x32 resolutions over standard I2C buses or high-speed SPI interfaces.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#ifndef VN_SSD1306_ADAPTER_H
#define VN_SSD1306_ADAPTER_H

#include "DisplayAdapter.h"

/**
 * @class SSD1306Adapter
 * @brief Concrete adapter for SSD1306-based OLED displays.
 */
class SSD1306Adapter : public DisplayAdapter {
public:
    /**
     * @brief Construct an I2C SSD1306 adapter.
     *
     * @param width   Display width (128)
     * @param height  Display height (32 or 64)
     * @param address I2C address (default 0x3C)
     */
    SSD1306Adapter(uint16_t width, uint16_t height,
                    uint8_t address = VN_DEFAULT_I2C_ADDRESS);

    /**
     * @brief Construct a SPI SSD1306 adapter.
     *
     * @param width  Display width (128)
     * @param height Display height (32 or 64)
     * @param dcPin  Data/Command pin
     * @param csPin  Chip Select pin
     * @param rstPin Reset pin (-1 if not used)
     */
    SSD1306Adapter(uint16_t width, uint16_t height,
                    int8_t dcPin, int8_t csPin, int8_t rstPin = -1);

    ~SSD1306Adapter() override = default;

    /* ── DisplayAdapter Interface ─────────────────────────────────────── */
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
    /** @brief Maximum framebuffer size: 128×64 / 8 = 1024 bytes. */
    static constexpr uint16_t MAX_BUFFER_SIZE = 1024;

    uint16_t width_;             /**< Display width */
    uint16_t height_;            /**< Display height */
    uint8_t  address_;           /**< I2C address */
    int8_t   dcPin_;             /**< SPI Data/Command pin (-1 for I2C) */
    int8_t   csPin_;             /**< SPI Chip Select pin (-1 for I2C) */
    int8_t   rstPin_;            /**< Reset pin (-1 if unused) */
    bool     useSPI_;            /**< true=SPI, false=I2C */
    uint16_t bufferSize_;        /**< Actual framebuffer size */
    uint8_t  buffer_[MAX_BUFFER_SIZE]; /**< Framebuffer (static) */

    /** @brief Send a single command byte to the display. */
    void sendCommand(uint8_t cmd);

    /** @brief Send multiple command bytes. */
    void sendCommandList(const uint8_t* cmds, uint8_t count);

    /** @brief Hardware reset via reset pin (if connected). */
    void hardwareReset();

    /** @brief Send initialization command sequence. */
    void sendInitSequence();
};

#endif /* VN_SSD1306_ADAPTER_H */
