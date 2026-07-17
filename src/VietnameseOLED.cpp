/**
 * @file VietnameseOLED.cpp
 * @brief Implementation of the primary VietnameseOLED API facade.
 *
 * Coordinates lifecycle initialization, property setting, string print pipelines, and geometric primitives drawing.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#include "VietnameseOLED.h"

#ifndef VN_NATIVE_BUILD
    #include <Arduino.h>
#endif

#include <stdarg.h>
#include <stdio.h>

/* ─────────────────────────────────────────────────────────────────────────────
 * Constructor
 * ────────────────────────────────────────────────────────────────────────── */

VietnameseOLED::VietnameseOLED(DisplayAdapter& display)
    : display_(display)
    , color_(VNColor::WHITE)
{
    printfBuf_[0] = '\0';
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Lifecycle
 * ────────────────────────────────────────────────────────────────────────── */

bool VietnameseOLED::begin() {
    /* Initialize display hardware */
    bool ok = display_.init();
    if (!ok) return false;

    /* Wire up internal subsystems */
    bitmapRenderer_.setTarget(&display_);
    glyphRenderer_.setTarget(&display_);
    glyphRenderer_.setBitmapRenderer(&bitmapRenderer_);

    textEngine_.setDisplayAdapter(&display_);
    textEngine_.setFontEngine(&fontEngine_);
    textEngine_.setLayoutEngine(&layoutEngine_);
    textEngine_.setGlyphRenderer(&glyphRenderer_);
    textEngine_.setBitmapRenderer(&bitmapRenderer_);
    textEngine_.setGlyphCache(&glyphCache_);

    /* Set default viewport to full display */
    layoutEngine_.setViewport(VNRect(0, 0, display_.getWidth(), display_.getHeight()));

    return true;
}

void VietnameseOLED::clear() {
    display_.clearBuffer();
    textEngine_.setCursor(0, 0);
}

void VietnameseOLED::update() {
    display_.sendBuffer();
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Font Selection
 * ────────────────────────────────────────────────────────────────────────── */

void VietnameseOLED::setFont(const VNFont& font) {
    fontEngine_.setFont(&font);
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Drawing Properties
 * ────────────────────────────────────────────────────────────────────────── */

void VietnameseOLED::setColor(VNColor color)           { color_ = color; }
void VietnameseOLED::setAlign(VNAlign align)            { layoutEngine_.setAlignment(align, layoutEngine_.getVAlign()); }
void VietnameseOLED::setVAlign(VNVAlign align)          { layoutEngine_.setAlignment(layoutEngine_.getHAlign(), align); }
void VietnameseOLED::setTextWrap(VNTextWrap wrap)       { layoutEngine_.setTextWrap(wrap); }
void VietnameseOLED::setLineSpacing(uint8_t pixels)     { layoutEngine_.setLineSpacing(pixels); }
void VietnameseOLED::setParagraphSpacing(uint8_t px)    { layoutEngine_.setParagraphSpacing(px); }
void VietnameseOLED::setRenderMode(VNRenderMode mode)   { glyphRenderer_.setRenderMode(mode); }
void VietnameseOLED::setScale(uint8_t scale)            { glyphRenderer_.setScale(scale, scale); }
void VietnameseOLED::setRotation(uint16_t degrees)      { glyphRenderer_.setRotation(degrees); }
void VietnameseOLED::setMirror(bool h, bool v)          { glyphRenderer_.setMirror(h, v); }

/* ─────────────────────────────────────────────────────────────────────────────
 * Cursor
 * ────────────────────────────────────────────────────────────────────────── */

void VietnameseOLED::setCursor(int16_t x, int16_t y)   { textEngine_.setCursor(x, y); }
int16_t VietnameseOLED::getCursorX() const              { return textEngine_.getCursorX(); }
int16_t VietnameseOLED::getCursorY() const              { return textEngine_.getCursorY(); }

/* ─────────────────────────────────────────────────────────────────────────────
 * Text Drawing
 * ────────────────────────────────────────────────────────────────────────── */

void VietnameseOLED::drawUTF8(int16_t x, int16_t y, const char* text) {
    textEngine_.drawText(x, y, text, color_);
}

void VietnameseOLED::printVN(const char* text) {
    textEngine_.print(text, color_);
}

void VietnameseOLED::printlnVN(const char* text) {
    textEngine_.println(text, color_);
}

void VietnameseOLED::printfVN(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(printfBuf_, VN_PRINTF_BUFFER_SIZE, fmt, args);
    va_end(args);

    printfBuf_[VN_PRINTF_BUFFER_SIZE - 1] = '\0';
    textEngine_.print(printfBuf_, color_);
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Text Measurement
 * ────────────────────────────────────────────────────────────────────────── */

uint16_t VietnameseOLED::getTextWidth(const char* text) const {
    return fontEngine_.getTextWidth(text);
}

uint8_t VietnameseOLED::getTextHeight() const {
    return fontEngine_.getLineHeight();
}

VNRect VietnameseOLED::getTextBounds(const char* text) const {
    return textEngine_.measureText(text);
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Primitive Drawing
 * ────────────────────────────────────────────────────────────────────────── */

void VietnameseOLED::drawPixel(int16_t x, int16_t y, VNColor color) {
    bitmapRenderer_.drawPixel(x, y, color);
}

void VietnameseOLED::drawBitmap(int16_t x, int16_t y, const uint8_t* bitmap,
                                  uint16_t w, uint16_t h)
{
    bitmapRenderer_.drawBitmap(x, y, bitmap, w, h, color_, VNRenderMode::TRANSPARENT);
}

void VietnameseOLED::fillRect(int16_t x, int16_t y, uint16_t w, uint16_t h,
                                VNColor color)
{
    bitmapRenderer_.fillRect(x, y, w, h, color);
}

void VietnameseOLED::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                                VNColor color)
{
    bitmapRenderer_.drawLine(x0, y0, x1, y1, color);
}

void VietnameseOLED::drawRect(int16_t x, int16_t y, uint16_t w, uint16_t h,
                                VNColor color)
{
    bitmapRenderer_.drawRect(x, y, w, h, color);
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Display Control
 * ────────────────────────────────────────────────────────────────────────── */

void VietnameseOLED::setContrast(uint8_t value)  { display_.setContrast(value); }
void VietnameseOLED::displayOn()                  { display_.displayOn(); }
void VietnameseOLED::displayOff()                 { display_.displayOff(); }
uint16_t VietnameseOLED::getWidth() const         { return display_.getWidth(); }
uint16_t VietnameseOLED::getHeight() const        { return display_.getHeight(); }
