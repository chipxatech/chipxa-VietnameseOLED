/**
 * @file DisplayAdapter.h
 * @brief Hardware-abstracted display adapter interface.
 *
 * Declares pure virtual functions to initialize, clear, contrast-tune, and upload buffers.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#ifndef VN_DISPLAY_ADAPTER_H
#define VN_DISPLAY_ADAPTER_H

#include "../VNConfig.h"
#include "../VNTypes.h"

/**
 * @class DisplayAdapter
 * @brief Abstract interface for display hardware drivers.
 *
 * Implementors must provide:
 * - Hardware initialization
 * - Framebuffer management (clear, set pixel, get pixel)
 * - Buffer transmission to hardware
 * - Display power and contrast control
 *
 * All rendering is done to an in-memory framebuffer. Call sendBuffer()
 * to push the framebuffer to the physical display.
 */
class DisplayAdapter {
public:
    /** @brief Virtual destructor for proper cleanup of derived classes. */
    virtual ~DisplayAdapter() = default;

    /* ── Lifecycle ─────────────────────────────────────────────────────── */

    /**
     * @brief Initialize the display hardware.
     *
     * Must be called once before any other operations. Sets up communication
     * (I2C/SPI), sends initialization commands, and allocates the framebuffer.
     *
     * @return true if initialization succeeded
     */
    virtual bool init() = 0;

    /* ── Framebuffer Operations ────────────────────────────────────────── */

    /**
     * @brief Send the entire framebuffer to the display hardware.
     *
     * This is the only operation that communicates with the physical display.
     */
    virtual void sendBuffer() = 0;

    /**
     * @brief Clear the framebuffer (fill with black/off pixels).
     */
    virtual void clearBuffer() = 0;

    /**
     * @brief Set a single pixel in the framebuffer.
     *
     * @param x     X coordinate (0 = left)
     * @param y     Y coordinate (0 = top)
     * @param color 0=BLACK, 1=WHITE, 2=INVERSE
     */
    virtual void setPixel(int16_t x, int16_t y, uint8_t color) = 0;

    /**
     * @brief Get the color of a pixel in the framebuffer.
     *
     * @param x X coordinate
     * @param y Y coordinate
     * @return 0=BLACK, 1=WHITE
     */
    virtual uint8_t getPixel(int16_t x, int16_t y) const = 0;

    /* ── Display Properties ────────────────────────────────────────────── */

    /** @brief Get display width in pixels. */
    virtual uint16_t getWidth() const = 0;

    /** @brief Get display height in pixels. */
    virtual uint16_t getHeight() const = 0;

    /** @brief Get raw pointer to framebuffer. */
    virtual uint8_t* getBuffer() = 0;

    /** @brief Get const pointer to framebuffer. */
    virtual const uint8_t* getBuffer() const = 0;

    /** @brief Get framebuffer size in bytes. */
    virtual uint16_t getBufferSize() const = 0;

    /* ── Display Control ───────────────────────────────────────────────── */

    /**
     * @brief Set display contrast/brightness.
     * @param value 0 (dimmest) to 255 (brightest)
     */
    virtual void setContrast(uint8_t value) = 0;

    /** @brief Turn the display on (exit sleep). */
    virtual void displayOn() = 0;

    /** @brief Turn the display off (enter sleep). */
    virtual void displayOff() = 0;
};

#endif /* VN_DISPLAY_ADAPTER_H */
