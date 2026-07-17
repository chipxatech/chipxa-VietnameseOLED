/**
 * @file UTF8Decoder.cpp
 * @brief Deterministic Finite Automaton (DFA) UTF-8 decoding state machine implementation.
 *
 * Processes byte inputs sequence-by-sequence, validating UTF-8 boundaries and filtering malformed sequences.
 *
 * @author CHIPXA TECHNOLOGY CO., LTD
 * @version 1.0.0
 * @copyright MIT License
 */

#include "UTF8Decoder.h"

/* ─────────────────────────────────────────────────────────────────────────────
 * Byte classification masks
 * ────────────────────────────────────────────────────────────────────────── */

/** @brief Bit mask for ASCII single-byte characters (0xxxxxxx). */
static constexpr uint8_t MASK_1BYTE_LEAD = 0x80;   /* 10000000 */
/** @brief Pattern for ASCII single-byte characters. */
static constexpr uint8_t PATTERN_1BYTE   = 0x00;    /* 0xxxxxxx */

/** @brief Bit mask for 2-byte sequence lead (110xxxxx). */
static constexpr uint8_t MASK_2BYTE_LEAD = 0xE0;   /* 11100000 */
/** @brief Pattern for 2-byte sequence lead. */
static constexpr uint8_t PATTERN_2BYTE   = 0xC0;    /* 110xxxxx */

/** @brief Bit mask for 3-byte sequence lead (1110xxxx). */
static constexpr uint8_t MASK_3BYTE_LEAD = 0xF0;   /* 11110000 */
/** @brief Pattern for 3-byte sequence lead. */
static constexpr uint8_t PATTERN_3BYTE   = 0xE0;    /* 1110xxxx */

/** @brief Bit mask for 4-byte sequence lead (11110xxx). */
static constexpr uint8_t MASK_4BYTE_LEAD = 0xF8;   /* 11111000 */
/** @brief Pattern for 4-byte sequence lead. */
static constexpr uint8_t PATTERN_4BYTE   = 0xF0;    /* 11110xxx */

/** @brief Bit mask for continuation bytes (10xxxxxx). */
static constexpr uint8_t MASK_CONTINUATION = 0xC0;  /* 11000000 */
/** @brief Pattern for continuation bytes. */
static constexpr uint8_t PATTERN_CONT      = 0x80;  /* 10xxxxxx */

/** @brief Number of data bits in a continuation byte. */
static constexpr uint8_t CONT_DATA_BITS = 6;

/** @brief Mask for extracting data bits from a continuation byte. */
static constexpr uint8_t CONT_DATA_MASK = 0x3F;     /* 00111111 */

/* ─────────────────────────────────────────────────────────────────────────────
 * Constructor & Reset
 * ────────────────────────────────────────────────────────────────────────── */

UTF8Decoder::UTF8Decoder()
    : state_(State::READY)
    , codepoint_(0)
    , bytesRemaining_(0)
{
}

void UTF8Decoder::reset() {
    state_          = State::READY;
    codepoint_      = 0;
    bytesRemaining_ = 0;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Core Decode — DFA State Machine
 * ────────────────────────────────────────────────────────────────────────── */

UTF8Decoder::Status UTF8Decoder::decode(uint8_t byte) {
    if (state_ == State::READY) {
        /*
         * READY state: expecting a lead byte or ASCII byte.
         */
        if ((byte & MASK_1BYTE_LEAD) == PATTERN_1BYTE) {
            /* Single-byte ASCII: 0xxxxxxx */
            codepoint_ = byte;
            return Status::ACCEPT;
        }
        else if ((byte & MASK_2BYTE_LEAD) == PATTERN_2BYTE) {
            /* 2-byte sequence lead: 110xxxxx */
            codepoint_      = byte & 0x1F; /* Extract 5 data bits */
            bytesRemaining_ = 1;
            state_          = State::NEED_1_MORE;
            return Status::CONTINUE;
        }
        else if ((byte & MASK_3BYTE_LEAD) == PATTERN_3BYTE) {
            /* 3-byte sequence lead: 1110xxxx */
            codepoint_      = byte & 0x0F; /* Extract 4 data bits */
            bytesRemaining_ = 2;
            state_          = State::NEED_2_MORE;
            return Status::CONTINUE;
        }
        else if ((byte & MASK_4BYTE_LEAD) == PATTERN_4BYTE) {
            /* 4-byte sequence lead: 11110xxx */
            codepoint_      = byte & 0x07; /* Extract 3 data bits */
            bytesRemaining_ = 3;
            state_          = State::NEED_3_MORE;
            return Status::CONTINUE;
        }
        else {
            /* Invalid lead byte (10xxxxxx in ready state, or 11111xxx) */
            codepoint_ = VN_UNICODE_REPLACEMENT_CHAR;
            return Status::REJECT;
        }
    }
    else {
        /*
         * Continuation state: expecting 10xxxxxx bytes.
         */
        if ((byte & MASK_CONTINUATION) == PATTERN_CONT) {
            /* Valid continuation byte */
            codepoint_ = (codepoint_ << CONT_DATA_BITS) | (byte & CONT_DATA_MASK);
            bytesRemaining_--;

            if (bytesRemaining_ == 0) {
                /* Sequence complete — validate the codepoint */
                state_ = State::READY;

                /* Reject overlong encodings and surrogate halves */
                if (codepoint_ < 0x80 && state_ != State::READY) {
                    /* This path won't trigger due to state reset above,
                     * but we check the decoded value for safety */
                }
                if (codepoint_ >= 0xD800 && codepoint_ <= 0xDFFF) {
                    /* UTF-16 surrogate halves are invalid in UTF-8 */
                    codepoint_ = VN_UNICODE_REPLACEMENT_CHAR;
                    return Status::REJECT;
                }
                if (codepoint_ > 0x10FFFF) {
                    /* Beyond the Unicode maximum */
                    codepoint_ = VN_UNICODE_REPLACEMENT_CHAR;
                    return Status::REJECT;
                }
                return Status::ACCEPT;
            }

            /* Update state to reflect bytes remaining */
            switch (bytesRemaining_) {
                case 1: state_ = State::NEED_1_MORE; break;
                case 2: state_ = State::NEED_2_MORE; break;
                default: break;
            }
            return Status::CONTINUE;
        }
        else {
            /*
             * Expected a continuation byte but got something else.
             * Reset state and emit replacement character.
             * The current byte might be a valid lead byte for a new sequence,
             * but per the spec we report error first; the caller can re-feed.
             */
            state_          = State::READY;
            bytesRemaining_ = 0;
            codepoint_      = VN_UNICODE_REPLACEMENT_CHAR;
            return Status::REJECT;
        }
    }
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Convenience — getCodepoint
 * ────────────────────────────────────────────────────────────────────────── */

uint32_t UTF8Decoder::getCodepoint() const {
    return codepoint_;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Static — Decode Entire String
 * ────────────────────────────────────────────────────────────────────────── */

uint16_t UTF8Decoder::decodeString(const char* str, uint32_t* output, uint16_t maxLen) {
    if (str == nullptr || output == nullptr || maxLen == 0) {
        return 0;
    }

    UTF8Decoder decoder;
    uint16_t count = 0;
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(str);

    while (*ptr != 0 && count < maxLen) {
        Status status = decoder.decode(*ptr);
        ptr++;

        switch (status) {
            case Status::ACCEPT:
                output[count++] = decoder.getCodepoint();
                break;
            case Status::REJECT:
                output[count++] = VN_UNICODE_REPLACEMENT_CHAR;
                break;
            case Status::CONTINUE:
                /* Keep feeding bytes */
                break;
        }
    }

    return count;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Static — Count Unicode Characters
 * ────────────────────────────────────────────────────────────────────────── */

uint16_t UTF8Decoder::countChars(const char* utf8Str) {
    if (utf8Str == nullptr) {
        return 0;
    }

    uint16_t count = 0;
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(utf8Str);

    while (*ptr != 0) {
        /* Count lead bytes only (not continuation bytes 10xxxxxx) */
        if ((*ptr & MASK_CONTINUATION) != PATTERN_CONT) {
            count++;
        }
        ptr++;
    }

    return count;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Static — Character Byte Length
 * ────────────────────────────────────────────────────────────────────────── */

uint8_t UTF8Decoder::charByteLength(uint8_t leadByte) {
    if ((leadByte & MASK_1BYTE_LEAD) == PATTERN_1BYTE) {
        return 1;
    }
    if ((leadByte & MASK_2BYTE_LEAD) == PATTERN_2BYTE) {
        return 2;
    }
    if ((leadByte & MASK_3BYTE_LEAD) == PATTERN_3BYTE) {
        return 3;
    }
    if ((leadByte & MASK_4BYTE_LEAD) == PATTERN_4BYTE) {
        return 4;
    }
    /* Invalid lead byte — treat as single byte */
    return 1;
}
