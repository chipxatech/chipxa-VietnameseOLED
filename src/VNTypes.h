/**
 * @file VNTypes.h
 * @brief Common data structures, structures, definitions, and geometric primitives.
 *
 * Declares HSL-tailored colors, alignment enumerations, point/rect shapes, and font/glyph PROGMEM structures.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#ifndef VN_TYPES_H
#define VN_TYPES_H

#include "VNConfig.h"

#ifndef VN_NATIVE_BUILD
    #include <Arduino.h>
#else
    #include <cstdint>
    #include <cstddef>
#endif

/* ─────────────────────────────────────────────────────────────────────────────
 * Color
 * ────────────────────────────────────────────────────────────────────────── */

/**
 * @brief Pixel color values for monochrome and grayscale displays.
 */
enum class VNColor : uint8_t {
    BLACK   = 0,  /**< Pixel off */
    WHITE   = 1,  /**< Pixel on (full intensity for grayscale) */
    INVERSE = 2   /**< Toggle pixel state (XOR) */
};

/* ─────────────────────────────────────────────────────────────────────────────
 * Text Alignment
 * ────────────────────────────────────────────────────────────────────────── */

/**
 * @brief Horizontal text alignment modes.
 */
enum class VNAlign : uint8_t {
    LEFT    = 0,  /**< Align text to the left edge */
    CENTER  = 1,  /**< Center text horizontally */
    RIGHT   = 2,  /**< Align text to the right edge */
    JUSTIFY = 3   /**< Distribute words evenly across the line */
};

/**
 * @brief Vertical text alignment modes.
 */
enum class VNVAlign : uint8_t {
    TOP    = 0,   /**< Align text to the top edge */
    MIDDLE = 1,   /**< Center text vertically */
    BOTTOM = 2    /**< Align text to the bottom edge */
};

/* ─────────────────────────────────────────────────────────────────────────────
 * Render Mode
 * ────────────────────────────────────────────────────────────────────────── */

/**
 * @brief Rendering modes for glyph/bitmap drawing.
 */
enum class VNRenderMode : uint8_t {
    TRANSPARENT = 0,  /**< Only set foreground pixels (OR blend) */
    OVERWRITE   = 1,  /**< Set foreground and clear background pixels */
    INVERT      = 2   /**< Toggle pixels (XOR blend) */
};

/* ─────────────────────────────────────────────────────────────────────────────
 * Text Wrapping
 * ────────────────────────────────────────────────────────────────────────── */

/**
 * @brief Text wrapping modes.
 */
enum class VNTextWrap : uint8_t {
    NONE      = 0,  /**< No wrapping — text may extend beyond display */
    WORD      = 1,  /**< Wrap at word boundaries (space characters) */
    CHARACTER = 2   /**< Wrap at character boundaries */
};

/* ─────────────────────────────────────────────────────────────────────────────
 * Communication Interface
 * ────────────────────────────────────────────────────────────────────────── */

/**
 * @brief Communication interface type for display adapters.
 */
enum class VNCommInterface : uint8_t {
    I2C = 0,  /**< I2C (TWI) interface */
    SPI = 1   /**< SPI interface */
};

/* ─────────────────────────────────────────────────────────────────────────────
 * Geometry Types
 * ────────────────────────────────────────────────────────────────────────── */

/**
 * @brief A 2D point with signed coordinates.
 */
struct VNPoint {
    int16_t x;  /**< X coordinate */
    int16_t y;  /**< Y coordinate */

    /** @brief Default constructor, initializes to origin. */
    VNPoint() : x(0), y(0) {}

    /**
     * @brief Construct a point with given coordinates.
     * @param px X coordinate
     * @param py Y coordinate
     */
    VNPoint(int16_t px, int16_t py) : x(px), y(py) {}
};

/**
 * @brief A 2D axis-aligned rectangle.
 */
struct VNRect {
    int16_t  x;       /**< Left edge X coordinate */
    int16_t  y;       /**< Top edge Y coordinate */
    uint16_t width;   /**< Width in pixels */
    uint16_t height;  /**< Height in pixels */

    /** @brief Default constructor, initializes to empty rectangle at origin. */
    VNRect() : x(0), y(0), width(0), height(0) {}

    /**
     * @brief Construct a rectangle with given position and size.
     * @param rx Left edge X
     * @param ry Top edge Y
     * @param rw Width
     * @param rh Height
     */
    VNRect(int16_t rx, int16_t ry, uint16_t rw, uint16_t rh)
        : x(rx), y(ry), width(rw), height(rh) {}

    /** @brief Check if a point is inside this rectangle. */
    bool contains(int16_t px, int16_t py) const {
        return px >= x && px < x + static_cast<int16_t>(width) &&
               py >= y && py < y + static_cast<int16_t>(height);
    }

    /** @brief Get the right edge X coordinate. */
    int16_t right() const { return x + static_cast<int16_t>(width); }

    /** @brief Get the bottom edge Y coordinate. */
    int16_t bottom() const { return y + static_cast<int16_t>(height); }

    /** @brief Check if the rectangle has zero area. */
    bool isEmpty() const { return width == 0 || height == 0; }
};

/**
 * @brief Edge insets (padding or margin) for layout.
 */
struct VNEdgeInsets {
    uint8_t top;     /**< Top inset in pixels */
    uint8_t right;   /**< Right inset in pixels */
    uint8_t bottom;  /**< Bottom inset in pixels */
    uint8_t left;    /**< Left inset in pixels */

    /** @brief Default constructor, zero insets. */
    VNEdgeInsets() : top(0), right(0), bottom(0), left(0) {}

    /**
     * @brief Construct with uniform insets.
     * @param all Inset value applied to all four sides
     */
    explicit VNEdgeInsets(uint8_t all) : top(all), right(all), bottom(all), left(all) {}

    /**
     * @brief Construct with individual insets.
     * @param t Top
     * @param r Right
     * @param b Bottom
     * @param l Left
     */
    VNEdgeInsets(uint8_t t, uint8_t r, uint8_t b, uint8_t l)
        : top(t), right(r), bottom(b), left(l) {}

    /** @brief Sum of horizontal insets. */
    uint16_t horizontal() const { return static_cast<uint16_t>(left) + right; }

    /** @brief Sum of vertical insets. */
    uint16_t vertical() const { return static_cast<uint16_t>(top) + bottom; }
};

/* ─────────────────────────────────────────────────────────────────────────────
 * Font & Glyph Types (PROGMEM-compatible structures)
 * ────────────────────────────────────────────────────────────────────────── */

/**
 * @brief Single entry in the Unicode-to-glyph-index mapping table.
 *
 * Stored in PROGMEM as a sorted array for binary search lookup.
 */
struct VNUnicodeEntry {
    uint16_t codepoint;   /**< Unicode codepoint */
    uint16_t glyphIndex;  /**< Index into the glyph metadata table */
};

/**
 * @brief Metadata for a single glyph in a font.
 *
 * Stored in PROGMEM alongside the bitmap data.
 */
struct VNGlyphEntry {
    uint16_t bitmapOffset;  /**< Byte offset into the bitmap data array */
    uint8_t  width;         /**< Glyph bitmap width in pixels */
    uint8_t  height;        /**< Glyph bitmap height in pixels */
    int8_t   xOffset;       /**< Horizontal offset from cursor to glyph left edge */
    int8_t   yOffset;       /**< Vertical offset from baseline to glyph top edge */
    uint8_t  xAdvance;      /**< Horizontal cursor advance after drawing this glyph */
};

/**
 * @brief Font descriptor — ties together all font data arrays.
 *
 * The font descriptor itself is stored in RAM and points to PROGMEM data.
 */
struct VNFont {
    const uint8_t*        bitmapData;     /**< Pointer to PROGMEM bitmap data */
    const VNGlyphEntry*   glyphTable;     /**< Pointer to PROGMEM glyph metadata */
    const VNUnicodeEntry* unicodeMap;      /**< Pointer to PROGMEM unicode mapping */
    uint16_t              glyphCount;     /**< Total number of glyphs */
    uint16_t              unicodeMapSize; /**< Number of entries in unicode map */
    uint8_t               lineHeight;     /**< Line height in pixels */
    uint8_t               baseline;       /**< Baseline offset from top of line */
    uint8_t               defaultWidth;   /**< Default character width */
    uint8_t               defaultHeight;  /**< Default character height */
};

/**
 * @brief Runtime glyph information — decoded from PROGMEM for rendering.
 *
 * This struct lives in RAM and is populated by the FontEngine.
 */
struct VNGlyphInfo {
    uint32_t       codepoint;  /**< Unicode codepoint of this glyph */
    const uint8_t* bitmap;     /**< Pointer to bitmap data (in PROGMEM) */
    uint8_t        width;      /**< Bitmap width in pixels */
    uint8_t        height;     /**< Bitmap height in pixels */
    int8_t         xOffset;    /**< Horizontal offset from cursor */
    int8_t         yOffset;    /**< Vertical offset from baseline */
    uint8_t        xAdvance;   /**< Cursor advance */
    bool           valid;      /**< True if glyph was found in font */

    /** @brief Default constructor — marks glyph as invalid. */
    VNGlyphInfo()
        : codepoint(0), bitmap(nullptr), width(0), height(0),
          xOffset(0), yOffset(0), xAdvance(0), valid(false) {}
};

/* ─────────────────────────────────────────────────────────────────────────────
 * Constants — named alternatives for C-style API compatibility
 * ────────────────────────────────────────────────────────────────────────── */

/** @name Color constants (for use without enum class syntax) */
/** @{ */
static constexpr VNColor VN_COLOR_BLACK   = VNColor::BLACK;
static constexpr VNColor VN_COLOR_WHITE   = VNColor::WHITE;
static constexpr VNColor VN_COLOR_INVERSE = VNColor::INVERSE;
/** @} */

/** @name Alignment constants */
/** @{ */
static constexpr VNAlign VN_ALIGN_LEFT    = VNAlign::LEFT;
static constexpr VNAlign VN_ALIGN_CENTER  = VNAlign::CENTER;
static constexpr VNAlign VN_ALIGN_RIGHT   = VNAlign::RIGHT;
static constexpr VNAlign VN_ALIGN_JUSTIFY = VNAlign::JUSTIFY;
/** @} */

/** @name Vertical alignment constants */
/** @{ */
static constexpr VNVAlign VN_VALIGN_TOP    = VNVAlign::TOP;
static constexpr VNVAlign VN_VALIGN_MIDDLE = VNVAlign::MIDDLE;
static constexpr VNVAlign VN_VALIGN_BOTTOM = VNVAlign::BOTTOM;
/** @} */

/** @name Render mode constants */
/** @{ */
static constexpr VNRenderMode VN_RENDER_TRANSPARENT = VNRenderMode::TRANSPARENT;
static constexpr VNRenderMode VN_RENDER_OVERWRITE   = VNRenderMode::OVERWRITE;
static constexpr VNRenderMode VN_RENDER_INVERT      = VNRenderMode::INVERT;
/** @} */

/** @name Text wrap constants */
/** @{ */
static constexpr VNTextWrap VN_WRAP_NONE      = VNTextWrap::NONE;
static constexpr VNTextWrap VN_WRAP_WORD      = VNTextWrap::WORD;
static constexpr VNTextWrap VN_WRAP_CHARACTER = VNTextWrap::CHARACTER;
/** @} */

/** @name Communication interface constants */
/** @{ */
static constexpr VNCommInterface VN_COMM_I2C = VNCommInterface::I2C;
static constexpr VNCommInterface VN_COMM_SPI = VNCommInterface::SPI;
/** @} */

/** @brief Sentinel value indicating a glyph was not found. */
static constexpr uint16_t VN_GLYPH_NOT_FOUND = 0xFFFF;

#endif /* VN_TYPES_H */
