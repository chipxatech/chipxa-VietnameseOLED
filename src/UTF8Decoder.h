/**
 * @file UTF8Decoder.h
 * @brief DFA-based UTF-8 byte stream decoder declaration.
 *
 * Reconstructs 32-bit Unicode codepoints from multi-byte UTF-8 streams using an allocation-free finite state machine.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#ifndef VN_UTF8_DECODER_H
#define VN_UTF8_DECODER_H

#include "VNConfig.h"
#include "VNTypes.h"

/**
 * @class UTF8Decoder
 * @brief Decodes UTF-8 encoded byte streams into Unicode codepoints.
 *
 * Usage (streaming, byte-at-a-time):
 * @code
 * UTF8Decoder decoder;
 * const char* text = "Xin chào";
 * while (*text) {
 *     if (decoder.decode(static_cast<uint8_t>(*text++)) == UTF8Decoder::Status::ACCEPT) {
 *         uint32_t cp = decoder.getCodepoint();
 *         // use codepoint
 *     }
 * }
 * @endcode
 *
 * Usage (batch):
 * @code
 * uint32_t codepoints[64];
 * uint16_t count = UTF8Decoder::decodeString("Việt Nam", codepoints, 64);
 * @endcode
 */
class UTF8Decoder {
public:
    /**
     * @brief Decoder status after processing a byte.
     */
    enum class Status : uint8_t {
        ACCEPT   = 0,  /**< A complete codepoint is ready (call getCodepoint()) */
        CONTINUE = 1,  /**< More bytes needed to complete the sequence */
        REJECT   = 2   /**< Invalid byte encountered; codepoint set to U+FFFD */
    };

    /** @brief Construct a decoder in the initial (ready) state. */
    UTF8Decoder();

    /**
     * @brief Reset the decoder to its initial state.
     *
     * Call this to discard any partially-decoded sequence.
     */
    void reset();

    /**
     * @brief Feed one byte into the decoder.
     *
     * @param byte The next byte of the UTF-8 stream
     * @return Status::ACCEPT if a codepoint is complete,
     *         Status::CONTINUE if more bytes are expected,
     *         Status::REJECT if the byte is invalid (codepoint set to U+FFFD)
     */
    Status decode(uint8_t byte);

    /**
     * @brief Get the last successfully decoded codepoint.
     *
     * Valid only after decode() returns ACCEPT or REJECT.
     *
     * @return Unicode codepoint, or U+FFFD for invalid sequences
     */
    uint32_t getCodepoint() const;

    /**
     * @brief Decode an entire null-terminated UTF-8 string into codepoints.
     *
     * @param str    Null-terminated UTF-8 input string
     * @param output Array to receive decoded codepoints
     * @param maxLen Maximum number of codepoints to decode
     * @return Number of codepoints written to output
     */
    static uint16_t decodeString(const char* str, uint32_t* output, uint16_t maxLen);

    /**
     * @brief Count the number of Unicode characters in a UTF-8 string.
     *
     * @param utf8Str Null-terminated UTF-8 string
     * @return Number of Unicode characters (not bytes)
     */
    static uint16_t countChars(const char* utf8Str);

    /**
     * @brief Get the byte length of a single UTF-8 character.
     *
     * @param leadByte The first byte of a UTF-8 sequence
     * @return Number of bytes in this character (1–4), or 1 for invalid
     */
    static uint8_t charByteLength(uint8_t leadByte);

private:
    /** @brief Internal decoder states for the DFA. */
    enum class State : uint8_t {
        READY       = 0,  /**< Waiting for a new sequence */
        NEED_1_MORE = 1,  /**< Expecting 1 more continuation byte */
        NEED_2_MORE = 2,  /**< Expecting 2 more continuation bytes */
        NEED_3_MORE = 3   /**< Expecting 3 more continuation bytes */
    };

    State    state_;          /**< Current DFA state */
    uint32_t codepoint_;      /**< Accumulated codepoint value */
    uint8_t  bytesRemaining_; /**< Continuation bytes still expected */
};

#endif /* VN_UTF8_DECODER_H */
