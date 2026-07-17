#!/usr/bin/env python3
"""
Generate Vietnamese bitmap font C headers for VietnameseOLED library.

This script generates bitmap font data from pixel patterns stored as
strings, producing C headers with PROGMEM arrays suitable for embedded use.

It generates three fonts: 5x8, 8x16, and 16x16 covering all ASCII
printable characters and 134 Vietnamese diacritical characters.
"""

import os
import sys
import struct

# ──────────────────────────────────────────────────────────────────────────
# Vietnamese codepoints (complete list of 134 characters)
# ──────────────────────────────────────────────────────────────────────────

VIETNAMESE_CODEPOINTS = [
    # Latin-1 Supplement - uppercase (16)
    0x00C0, 0x00C1, 0x00C2, 0x00C3,  # À Á Â Ã
    0x00C8, 0x00C9, 0x00CA,           # È É Ê
    0x00CC, 0x00CD,                    # Ì Í
    0x00D2, 0x00D3, 0x00D4, 0x00D5,  # Ò Ó Ô Õ
    0x00D9, 0x00DA,                    # Ù Ú
    0x00DD,                            # Ý
    # Latin-1 Supplement - lowercase (16)
    0x00E0, 0x00E1, 0x00E2, 0x00E3,  # à á â ã
    0x00E8, 0x00E9, 0x00EA,           # è é ê
    0x00EC, 0x00ED,                    # ì í
    0x00F2, 0x00F3, 0x00F4, 0x00F5,  # ò ó ô õ
    0x00F9, 0x00FA,                    # ù ú
    0x00FD,                            # ý
    # Latin Extended-A (8)
    0x0102, 0x0103,  # Ă ă
    0x0110, 0x0111,  # Đ đ
    0x0128, 0x0129,  # Ĩ ĩ
    0x0168, 0x0169,  # Ũ ũ
    # Latin Extended-B (4)
    0x01A0, 0x01A1,  # Ơ ơ
    0x01AF, 0x01B0,  # Ư ư
    # Latin Extended Additional (90: U+1EA0..U+1EF9)
    0x1EA0, 0x1EA1, 0x1EA2, 0x1EA3, 0x1EA4, 0x1EA5, 0x1EA6, 0x1EA7,
    0x1EA8, 0x1EA9, 0x1EAA, 0x1EAB, 0x1EAC, 0x1EAD, 0x1EAE, 0x1EAF,
    0x1EB0, 0x1EB1, 0x1EB2, 0x1EB3, 0x1EB4, 0x1EB5, 0x1EB6, 0x1EB7,
    0x1EB8, 0x1EB9, 0x1EBA, 0x1EBB, 0x1EBC, 0x1EBD, 0x1EBE, 0x1EBF,
    0x1EC0, 0x1EC1, 0x1EC2, 0x1EC3, 0x1EC4, 0x1EC5, 0x1EC6, 0x1EC7,
    0x1EC8, 0x1EC9, 0x1ECA, 0x1ECB, 0x1ECC, 0x1ECD, 0x1ECE, 0x1ECF,
    0x1ED0, 0x1ED1, 0x1ED2, 0x1ED3, 0x1ED4, 0x1ED5, 0x1ED6, 0x1ED7,
    0x1ED8, 0x1ED9, 0x1EDA, 0x1EDB, 0x1EDC, 0x1EDD, 0x1EDE, 0x1EDF,
    0x1EE0, 0x1EE1, 0x1EE2, 0x1EE3, 0x1EE4, 0x1EE5, 0x1EE6, 0x1EE7,
    0x1EE8, 0x1EE9, 0x1EEA, 0x1EEB, 0x1EEC, 0x1EED, 0x1EEE, 0x1EEF,
    0x1EF0, 0x1EF1, 0x1EF2, 0x1EF3, 0x1EF4, 0x1EF5, 0x1EF6, 0x1EF7,
    0x1EF8, 0x1EF9,
]

ASCII_RANGE = list(range(0x20, 0x7F))  # 95 printable ASCII chars

# ──────────────────────────────────────────────────────────────────────────
# 5x8 Base Letter Bitmaps (row-major, 5 pixels left-aligned in byte)
# Each glyph: 8 rows, each row 1 byte (bits 7-3 = pixels, bits 2-0 unused)
# ──────────────────────────────────────────────────────────────────────────

# Classic 5x7 font data (row-major, MSB-left)
FONT5X8_ASCII = {
    0x20: [0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00],  # (space)
    0x21: [0x20,0x20,0x20,0x20,0x20,0x00,0x20,0x00],  # !
    0x22: [0x50,0x50,0x50,0x00,0x00,0x00,0x00,0x00],  # "
    0x23: [0x50,0x50,0xF8,0x50,0xF8,0x50,0x50,0x00],  # #
    0x24: [0x20,0x78,0xA0,0x70,0x28,0xF0,0x20,0x00],  # $
    0x25: [0xC0,0xC8,0x10,0x20,0x40,0x98,0x18,0x00],  # %
    0x26: [0x60,0x90,0xA0,0x40,0xA8,0x90,0x68,0x00],  # &
    0x27: [0x30,0x30,0x20,0x40,0x00,0x00,0x00,0x00],  # '
    0x28: [0x10,0x20,0x40,0x40,0x40,0x20,0x10,0x00],  # (
    0x29: [0x40,0x20,0x10,0x10,0x10,0x20,0x40,0x00],  # )
    0x2A: [0x00,0x20,0xA8,0x70,0xA8,0x20,0x00,0x00],  # *
    0x2B: [0x00,0x20,0x20,0xF8,0x20,0x20,0x00,0x00],  # +
    0x2C: [0x00,0x00,0x00,0x00,0x30,0x30,0x20,0x40],  # ,
    0x2D: [0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0x00],  # -
    0x2E: [0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00],  # .
    0x2F: [0x00,0x08,0x10,0x20,0x40,0x80,0x00,0x00],  # /
    0x30: [0x70,0x88,0x98,0xA8,0xC8,0x88,0x70,0x00],  # 0
    0x31: [0x20,0x60,0x20,0x20,0x20,0x20,0x70,0x00],  # 1
    0x32: [0x70,0x88,0x08,0x10,0x20,0x40,0xF8,0x00],  # 2
    0x33: [0xF8,0x10,0x20,0x10,0x08,0x88,0x70,0x00],  # 3
    0x34: [0x10,0x30,0x50,0x90,0xF8,0x10,0x10,0x00],  # 4
    0x35: [0xF8,0x80,0xF0,0x08,0x08,0x88,0x70,0x00],  # 5
    0x36: [0x30,0x40,0x80,0xF0,0x88,0x88,0x70,0x00],  # 6
    0x37: [0xF8,0x08,0x08,0x10,0x20,0x20,0x20,0x00],  # 7
    0x38: [0x70,0x88,0x88,0x70,0x88,0x88,0x70,0x00],  # 8
    0x39: [0x70,0x88,0x88,0x78,0x08,0x10,0x60,0x00],  # 9
    0x3A: [0x00,0x30,0x30,0x00,0x30,0x30,0x00,0x00],  # :
    0x3B: [0x00,0x30,0x30,0x00,0x30,0x30,0x20,0x40],  # ;
    0x3C: [0x08,0x10,0x20,0x40,0x20,0x10,0x08,0x00],  # <
    0x3D: [0x00,0x00,0xF8,0x00,0xF8,0x00,0x00,0x00],  # =
    0x3E: [0x80,0x40,0x20,0x10,0x20,0x40,0x80,0x00],  # >
    0x3F: [0x70,0x88,0x08,0x10,0x20,0x00,0x20,0x00],  # ?
    0x40: [0x70,0x88,0x08,0x68,0xA8,0xA8,0x70,0x00],  # @
    0x41: [0x70,0x88,0x88,0x88,0xF8,0x88,0x88,0x00],  # A
    0x42: [0xF0,0x88,0x88,0xF0,0x88,0x88,0xF0,0x00],  # B
    0x43: [0x70,0x88,0x80,0x80,0x80,0x88,0x70,0x00],  # C
    0x44: [0xE0,0x90,0x88,0x88,0x88,0x90,0xE0,0x00],  # D
    0x45: [0xF8,0x80,0x80,0xF0,0x80,0x80,0xF8,0x00],  # E
    0x46: [0xF8,0x80,0x80,0xF0,0x80,0x80,0x80,0x00],  # F
    0x47: [0x70,0x88,0x80,0xB8,0x88,0x88,0x78,0x00],  # G
    0x48: [0x88,0x88,0x88,0xF8,0x88,0x88,0x88,0x00],  # H
    0x49: [0x70,0x20,0x20,0x20,0x20,0x20,0x70,0x00],  # I
    0x4A: [0x38,0x10,0x10,0x10,0x10,0x90,0x60,0x00],  # J
    0x4B: [0x88,0x90,0xA0,0xC0,0xA0,0x90,0x88,0x00],  # K
    0x4C: [0x80,0x80,0x80,0x80,0x80,0x80,0xF8,0x00],  # L
    0x4D: [0x88,0xD8,0xA8,0xA8,0x88,0x88,0x88,0x00],  # M
    0x4E: [0x88,0x88,0xC8,0xA8,0x98,0x88,0x88,0x00],  # N
    0x4F: [0x70,0x88,0x88,0x88,0x88,0x88,0x70,0x00],  # O
    0x50: [0xF0,0x88,0x88,0xF0,0x80,0x80,0x80,0x00],  # P
    0x51: [0x70,0x88,0x88,0x88,0xA8,0x90,0x68,0x00],  # Q
    0x52: [0xF0,0x88,0x88,0xF0,0xA0,0x90,0x88,0x00],  # R
    0x53: [0x78,0x80,0x80,0x70,0x08,0x08,0xF0,0x00],  # S
    0x54: [0xF8,0x20,0x20,0x20,0x20,0x20,0x20,0x00],  # T
    0x55: [0x88,0x88,0x88,0x88,0x88,0x88,0x70,0x00],  # U
    0x56: [0x88,0x88,0x88,0x88,0x88,0x50,0x20,0x00],  # V
    0x57: [0x88,0x88,0x88,0xA8,0xA8,0xA8,0x50,0x00],  # W
    0x58: [0x88,0x88,0x50,0x20,0x50,0x88,0x88,0x00],  # X
    0x59: [0x88,0x88,0x88,0x50,0x20,0x20,0x20,0x00],  # Y
    0x5A: [0xF8,0x08,0x10,0x20,0x40,0x80,0xF8,0x00],  # Z
    0x5B: [0x70,0x40,0x40,0x40,0x40,0x40,0x70,0x00],  # [
    0x5C: [0x00,0x80,0x40,0x20,0x10,0x08,0x00,0x00],  # backslash
    0x5D: [0x70,0x10,0x10,0x10,0x10,0x10,0x70,0x00],  # ]
    0x5E: [0x20,0x50,0x88,0x00,0x00,0x00,0x00,0x00],  # ^
    0x5F: [0x00,0x00,0x00,0x00,0x00,0x00,0xF8,0x00],  # _
    0x60: [0x40,0x20,0x10,0x00,0x00,0x00,0x00,0x00],  # `
    0x61: [0x00,0x00,0x70,0x08,0x78,0x88,0x78,0x00],  # a
    0x62: [0x80,0x80,0xB0,0xC8,0x88,0x88,0xF0,0x00],  # b
    0x63: [0x00,0x00,0x70,0x80,0x80,0x88,0x70,0x00],  # c
    0x64: [0x08,0x08,0x68,0x98,0x88,0x88,0x78,0x00],  # d
    0x65: [0x00,0x00,0x70,0x88,0xF8,0x80,0x70,0x00],  # e
    0x66: [0x30,0x48,0x40,0xE0,0x40,0x40,0x40,0x00],  # f
    0x67: [0x00,0x00,0x78,0x88,0x78,0x08,0x70,0x00],  # g (with descender implied)
    0x68: [0x80,0x80,0xB0,0xC8,0x88,0x88,0x88,0x00],  # h
    0x69: [0x20,0x00,0x60,0x20,0x20,0x20,0x70,0x00],  # i
    0x6A: [0x10,0x00,0x30,0x10,0x10,0x90,0x60,0x00],  # j
    0x6B: [0x80,0x80,0x90,0xA0,0xC0,0xA0,0x90,0x00],  # k
    0x6C: [0x60,0x20,0x20,0x20,0x20,0x20,0x70,0x00],  # l
    0x6D: [0x00,0x00,0xD0,0xA8,0xA8,0x88,0x88,0x00],  # m
    0x6E: [0x00,0x00,0xB0,0xC8,0x88,0x88,0x88,0x00],  # n
    0x6F: [0x00,0x00,0x70,0x88,0x88,0x88,0x70,0x00],  # o
    0x70: [0x00,0x00,0xF0,0x88,0xF0,0x80,0x80,0x00],  # p
    0x71: [0x00,0x00,0x68,0x98,0x78,0x08,0x08,0x00],  # q
    0x72: [0x00,0x00,0xB0,0xC8,0x80,0x80,0x80,0x00],  # r
    0x73: [0x00,0x00,0x78,0x80,0x70,0x08,0xF0,0x00],  # s
    0x74: [0x40,0x40,0xE0,0x40,0x40,0x48,0x30,0x00],  # t
    0x75: [0x00,0x00,0x88,0x88,0x88,0x98,0x68,0x00],  # u
    0x76: [0x00,0x00,0x88,0x88,0x88,0x50,0x20,0x00],  # v
    0x77: [0x00,0x00,0x88,0x88,0xA8,0xA8,0x50,0x00],  # w
    0x78: [0x00,0x00,0x88,0x50,0x20,0x50,0x88,0x00],  # x
    0x79: [0x00,0x00,0x88,0x88,0x78,0x08,0x70,0x00],  # y
    0x7A: [0x00,0x00,0xF8,0x10,0x20,0x40,0xF8,0x00],  # z
    0x7B: [0x18,0x20,0x20,0x40,0x20,0x20,0x18,0x00],  # {
    0x7C: [0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x00],  # |
    0x7D: [0xC0,0x20,0x20,0x10,0x20,0x20,0xC0,0x00],  # }
    0x7E: [0x40,0xA8,0x10,0x00,0x00,0x00,0x00,0x00],  # ~
}

# ──────────────────────────────────────────────────────────────────────────
# Diacritical mark overlays for 5x8 font
# These are applied to base letter bitmaps to create Vietnamese characters
# ──────────────────────────────────────────────────────────────────────────

# Marks applied to the TOP of the glyph (rows 0-1)
MARK_ACUTE_TOP    = [0x10, 0x20]   # ´  rising stroke top-right to top-left
MARK_GRAVE_TOP    = [0x40, 0x20]   # `  falling stroke top-left to top-right
MARK_HOOK_TOP     = [0x30, 0x08]   # ̉   small hook above
MARK_TILDE_TOP    = [0x48, 0x30]   # ~  wave above
MARK_CIRCUM_TOP   = [0x20, 0x50]   # ^  circumflex above
MARK_BREVE_TOP    = [0x50, 0x20]   # ˘  breve above

# Marks applied to the BOTTOM of the glyph (row 7)
MARK_DOT_BELOW    = [0x20]         # .  dot below

# Combined marks (circumflex/breve + tone) - uses rows 0-2
MARK_CIRCUM_ACUTE  = [0x20, 0x50, 0x08]  # ấ
MARK_CIRCUM_GRAVE  = [0x20, 0x50, 0x40]  # ầ
MARK_CIRCUM_HOOK   = [0x20, 0x50, 0x08]  # ẩ (similar to acute at small size)
MARK_CIRCUM_TILDE  = [0x20, 0x50, 0x00]  # ẫ (just circumflex at small size)

MARK_BREVE_ACUTE   = [0x50, 0x20, 0x08]  # ắ
MARK_BREVE_GRAVE   = [0x50, 0x20, 0x40]  # ằ
MARK_BREVE_HOOK    = [0x50, 0x20, 0x08]  # ẳ
MARK_BREVE_TILDE   = [0x50, 0x20, 0x00]  # ẵ

# Horn mark for ơ/ư (small hook to upper right of letter)
MARK_HORN = 0x08  # bit added to top-right area


def get_base_letter_upper(cp):
    """Determine the base uppercase letter for a Vietnamese codepoint."""
    mapping = {
        # A-based
        0x00C0: 'A', 0x00C1: 'A', 0x00C2: 'A', 0x00C3: 'A',
        0x0102: 'A', 0x1EA0: 'A', 0x1EA2: 'A',
        0x1EA4: 'A', 0x1EA6: 'A', 0x1EA8: 'A', 0x1EAA: 'A', 0x1EAC: 'A',
        0x1EAE: 'A', 0x1EB0: 'A', 0x1EB2: 'A', 0x1EB4: 'A', 0x1EB6: 'A',
        # E-based
        0x00C8: 'E', 0x00C9: 'E', 0x00CA: 'E',
        0x1EB8: 'E', 0x1EBA: 'E', 0x1EBC: 'E',
        0x1EBE: 'E', 0x1EC0: 'E', 0x1EC2: 'E', 0x1EC4: 'E', 0x1EC6: 'E',
        # I-based
        0x00CC: 'I', 0x00CD: 'I', 0x0128: 'I',
        0x1EC8: 'I', 0x1ECA: 'I',
        # O-based
        0x00D2: 'O', 0x00D3: 'O', 0x00D4: 'O', 0x00D5: 'O',
        0x01A0: 'O',
        0x1ECC: 'O', 0x1ECE: 'O',
        0x1ED0: 'O', 0x1ED2: 'O', 0x1ED4: 'O', 0x1ED6: 'O', 0x1ED8: 'O',
        0x1EDA: 'O', 0x1EDC: 'O', 0x1EDE: 'O', 0x1EE0: 'O', 0x1EE2: 'O',
        # U-based
        0x00D9: 'U', 0x00DA: 'U', 0x0168: 'U',
        0x01AF: 'U',
        0x1EE4: 'U', 0x1EE6: 'U',
        0x1EE8: 'U', 0x1EEA: 'U', 0x1EEC: 'U', 0x1EEE: 'U', 0x1EF0: 'U',
        # Y-based
        0x00DD: 'Y',
        0x1EF2: 'Y', 0x1EF4: 'Y', 0x1EF6: 'Y', 0x1EF8: 'Y',
        # D-based
        0x0110: 'D',
    }
    return mapping.get(cp)


def get_base_letter_lower(cp):
    """Determine the base lowercase letter for a Vietnamese codepoint."""
    mapping = {
        # a-based
        0x00E0: 'a', 0x00E1: 'a', 0x00E2: 'a', 0x00E3: 'a',
        0x0103: 'a', 0x1EA1: 'a', 0x1EA3: 'a',
        0x1EA5: 'a', 0x1EA7: 'a', 0x1EA9: 'a', 0x1EAB: 'a', 0x1EAD: 'a',
        0x1EAF: 'a', 0x1EB1: 'a', 0x1EB3: 'a', 0x1EB5: 'a', 0x1EB7: 'a',
        # e-based
        0x00E8: 'e', 0x00E9: 'e', 0x00EA: 'e',
        0x1EB9: 'e', 0x1EBB: 'e', 0x1EBD: 'e',
        0x1EBF: 'e', 0x1EC1: 'e', 0x1EC3: 'e', 0x1EC5: 'e', 0x1EC7: 'e',
        # i-based
        0x00EC: 'i', 0x00ED: 'i', 0x0129: 'i',
        0x1EC9: 'i', 0x1ECB: 'i',
        # o-based
        0x00F2: 'o', 0x00F3: 'o', 0x00F4: 'o', 0x00F5: 'o',
        0x01A1: 'o',
        0x1ECD: 'o', 0x1ECF: 'o',
        0x1ED1: 'o', 0x1ED3: 'o', 0x1ED5: 'o', 0x1ED7: 'o', 0x1ED9: 'o',
        0x1EDB: 'o', 0x1EDD: 'o', 0x1EDF: 'o', 0x1EE1: 'o', 0x1EE3: 'o',
        # u-based
        0x00F9: 'u', 0x00FA: 'u', 0x0169: 'u',
        0x01B0: 'u',
        0x1EE5: 'u', 0x1EE7: 'u',
        0x1EE9: 'u', 0x1EEB: 'u', 0x1EED: 'u', 0x1EEF: 'u', 0x1EF1: 'u',
        # y-based
        0x00FD: 'y',
        0x1EF3: 'y', 0x1EF5: 'y', 0x1EF7: 'y', 0x1EF9: 'y',
        # d-based
        0x0111: 'd',
    }
    return mapping.get(cp)


def get_diacritic_type(cp):
    """Return a tuple (base_modifier, tone_mark, has_dot_below, has_horn) for a codepoint."""
    # base_modifier: None, 'circumflex', 'breve', 'horn'
    # tone_mark: None, 'acute', 'grave', 'hook', 'tilde'
    # has_dot_below: bool
    # has_horn: bool (for ơ/ư)

    info = {
        # Simple accented vowels - Latin-1 Supplement
        0x00C0: (None, 'grave', False, False),      # À
        0x00C1: (None, 'acute', False, False),       # Á
        0x00C2: ('circumflex', None, False, False),  # Â
        0x00C3: (None, 'tilde', False, False),       # Ã
        0x00C8: (None, 'grave', False, False),       # È
        0x00C9: (None, 'acute', False, False),       # É
        0x00CA: ('circumflex', None, False, False),  # Ê
        0x00CC: (None, 'grave', False, False),       # Ì
        0x00CD: (None, 'acute', False, False),       # Í
        0x00D2: (None, 'grave', False, False),       # Ò
        0x00D3: (None, 'acute', False, False),       # Ó
        0x00D4: ('circumflex', None, False, False),  # Ô
        0x00D5: (None, 'tilde', False, False),       # Õ
        0x00D9: (None, 'grave', False, False),       # Ù
        0x00DA: (None, 'acute', False, False),       # Ú
        0x00DD: (None, 'acute', False, False),       # Ý

        0x00E0: (None, 'grave', False, False),       # à
        0x00E1: (None, 'acute', False, False),       # á
        0x00E2: ('circumflex', None, False, False),  # â
        0x00E3: (None, 'tilde', False, False),       # ã
        0x00E8: (None, 'grave', False, False),       # è
        0x00E9: (None, 'acute', False, False),       # é
        0x00EA: ('circumflex', None, False, False),  # ê
        0x00EC: (None, 'grave', False, False),       # ì
        0x00ED: (None, 'acute', False, False),       # í
        0x00F2: (None, 'grave', False, False),       # ò
        0x00F3: (None, 'acute', False, False),       # ó
        0x00F4: ('circumflex', None, False, False),  # ô
        0x00F5: (None, 'tilde', False, False),       # õ
        0x00F9: (None, 'grave', False, False),       # ù
        0x00FA: (None, 'acute', False, False),       # ú
        0x00FD: (None, 'acute', False, False),       # ý

        # Latin Extended-A
        0x0102: ('breve', None, False, False),       # Ă
        0x0103: ('breve', None, False, False),       # ă
        0x0110: ('stroke', None, False, False),      # Đ (stroke through D)
        0x0111: ('stroke', None, False, False),      # đ (stroke through d)
        0x0128: (None, 'tilde', False, False),       # Ĩ
        0x0129: (None, 'tilde', False, False),       # ĩ
        0x0168: (None, 'tilde', False, False),       # Ũ
        0x0169: (None, 'tilde', False, False),       # ũ

        # Latin Extended-B
        0x01A0: (None, None, False, True),           # Ơ (horn)
        0x01A1: (None, None, False, True),           # ơ
        0x01AF: (None, None, False, True),           # Ư
        0x01B0: (None, None, False, True),           # ư

        # Latin Extended Additional - A variants
        0x1EA0: (None, None, True, False),           # Ạ
        0x1EA1: (None, None, True, False),           # ạ
        0x1EA2: (None, 'hook', False, False),        # Ả
        0x1EA3: (None, 'hook', False, False),        # ả
        0x1EA4: ('circumflex', 'acute', False, False),  # Ấ
        0x1EA5: ('circumflex', 'acute', False, False),  # ấ
        0x1EA6: ('circumflex', 'grave', False, False),  # Ầ
        0x1EA7: ('circumflex', 'grave', False, False),  # ầ
        0x1EA8: ('circumflex', 'hook', False, False),   # Ẩ
        0x1EA9: ('circumflex', 'hook', False, False),   # ẩ
        0x1EAA: ('circumflex', 'tilde', False, False),  # Ẫ
        0x1EAB: ('circumflex', 'tilde', False, False),  # ẫ
        0x1EAC: ('circumflex', None, True, False),      # Ậ
        0x1EAD: ('circumflex', None, True, False),      # ậ
        0x1EAE: ('breve', 'acute', False, False),       # Ắ
        0x1EAF: ('breve', 'acute', False, False),       # ắ
        0x1EB0: ('breve', 'grave', False, False),       # Ằ
        0x1EB1: ('breve', 'grave', False, False),       # ằ
        0x1EB2: ('breve', 'hook', False, False),        # Ẳ
        0x1EB3: ('breve', 'hook', False, False),        # ẳ
        0x1EB4: ('breve', 'tilde', False, False),       # Ẵ
        0x1EB5: ('breve', 'tilde', False, False),       # ẵ
        0x1EB6: ('breve', None, True, False),           # Ặ
        0x1EB7: ('breve', None, True, False),           # ặ

        # E variants
        0x1EB8: (None, None, True, False),              # Ẹ
        0x1EB9: (None, None, True, False),              # ẹ
        0x1EBA: (None, 'hook', False, False),           # Ẻ
        0x1EBB: (None, 'hook', False, False),           # ẻ
        0x1EBC: (None, 'tilde', False, False),          # Ẽ
        0x1EBD: (None, 'tilde', False, False),          # ẽ
        0x1EBE: ('circumflex', 'acute', False, False),  # Ế
        0x1EBF: ('circumflex', 'acute', False, False),  # ế
        0x1EC0: ('circumflex', 'grave', False, False),  # Ề
        0x1EC1: ('circumflex', 'grave', False, False),  # ề
        0x1EC2: ('circumflex', 'hook', False, False),   # Ể
        0x1EC3: ('circumflex', 'hook', False, False),   # ể
        0x1EC4: ('circumflex', 'tilde', False, False),  # Ễ
        0x1EC5: ('circumflex', 'tilde', False, False),  # ễ
        0x1EC6: ('circumflex', None, True, False),      # Ệ
        0x1EC7: ('circumflex', None, True, False),      # ệ

        # I variants
        0x1EC8: (None, 'hook', False, False),           # Ỉ
        0x1EC9: (None, 'hook', False, False),           # ỉ
        0x1ECA: (None, None, True, False),              # Ị
        0x1ECB: (None, None, True, False),              # ị

        # O variants
        0x1ECC: (None, None, True, False),              # Ọ
        0x1ECD: (None, None, True, False),              # ọ
        0x1ECE: (None, 'hook', False, False),           # Ỏ
        0x1ECF: (None, 'hook', False, False),           # ỏ
        0x1ED0: ('circumflex', 'acute', False, False),  # Ố
        0x1ED1: ('circumflex', 'acute', False, False),  # ố
        0x1ED2: ('circumflex', 'grave', False, False),  # Ồ
        0x1ED3: ('circumflex', 'grave', False, False),  # ồ
        0x1ED4: ('circumflex', 'hook', False, False),   # Ổ
        0x1ED5: ('circumflex', 'hook', False, False),   # ổ
        0x1ED6: ('circumflex', 'tilde', False, False),  # Ỗ
        0x1ED7: ('circumflex', 'tilde', False, False),  # ỗ
        0x1ED8: ('circumflex', None, True, False),      # Ộ
        0x1ED9: ('circumflex', None, True, False),      # ộ
        0x1EDA: (None, 'acute', False, True),           # Ớ
        0x1EDB: (None, 'acute', False, True),           # ớ
        0x1EDC: (None, 'grave', False, True),           # Ờ
        0x1EDD: (None, 'grave', False, True),           # ờ
        0x1EDE: (None, 'hook', False, True),            # Ở
        0x1EDF: (None, 'hook', False, True),            # ở
        0x1EE0: (None, 'tilde', False, True),           # Ỡ
        0x1EE1: (None, 'tilde', False, True),           # ỡ
        0x1EE2: (None, None, True, True),               # Ợ
        0x1EE3: (None, None, True, True),               # ợ

        # U variants
        0x1EE4: (None, None, True, False),              # Ụ
        0x1EE5: (None, None, True, False),              # ụ
        0x1EE6: (None, 'hook', False, False),           # Ủ
        0x1EE7: (None, 'hook', False, False),           # ủ
        0x1EE8: (None, 'acute', False, True),           # Ứ
        0x1EE9: (None, 'acute', False, True),           # ứ
        0x1EEA: (None, 'grave', False, True),           # Ừ
        0x1EEB: (None, 'grave', False, True),           # ừ
        0x1EEC: (None, 'hook', False, True),            # Ử
        0x1EED: (None, 'hook', False, True),            # ử
        0x1EEE: (None, 'tilde', False, True),           # Ữ
        0x1EEF: (None, 'tilde', False, True),           # ữ
        0x1EF0: (None, None, True, True),               # Ự
        0x1EF1: (None, None, True, True),               # ự

        # Y variants
        0x1EF2: (None, 'grave', False, False),          # Ỳ
        0x1EF3: (None, 'grave', False, False),          # ỳ
        0x1EF4: (None, None, True, False),              # Ỵ
        0x1EF5: (None, None, True, False),              # ỵ
        0x1EF6: (None, 'hook', False, False),           # Ỷ
        0x1EF7: (None, 'hook', False, False),           # ỷ
        0x1EF8: (None, 'tilde', False, False),          # Ỹ
        0x1EF9: (None, 'tilde', False, False),          # ỹ
    }
    return info.get(cp, (None, None, False, False))


def compose_5x8_glyph(cp):
    """Compose a 5x8 Vietnamese glyph from base letter + diacritics."""
    base_upper = get_base_letter_upper(cp)
    base_lower = get_base_letter_lower(cp)
    base_char = base_upper or base_lower

    if base_char is None:
        return [0x00] * 8  # fallback empty glyph

    is_upper = base_upper is not None
    base_code = ord(base_char)
    base_bitmap = list(FONT5X8_ASCII.get(base_code, [0x00]*8))

    modifier, tone, dot_below, horn = get_diacritic_type(cp)

    # Handle Đ/đ specially (stroke through the letter)
    if modifier == 'stroke':
        if is_upper:
            # D with horizontal stroke through middle
            result = list(base_bitmap)
            result[3] |= 0xF0  # add stroke at row 3
            return result
        else:
            # d with stroke through ascender
            result = list(base_bitmap)
            result[1] |= 0x18  # stroke through top of d
            return result

    result = list(base_bitmap)

    # Apply horn (small curve on top-right for ơ/ư)
    if horn:
        if is_upper:
            result[0] |= 0x08
            result[1] |= 0x10
        else:
            result[2] |= 0x08

    # Apply base modifier (circumflex/breve) - goes to top area
    if modifier == 'circumflex' and tone is not None:
        # Combined: circumflex + tone in rows 0-1
        if tone == 'acute':
            if is_upper:
                result[0] = result[0] | 0x28  # ^+´ compressed
            else:
                result[0] = MARK_CIRCUM_TOP[0]
                result[1] = MARK_CIRCUM_TOP[1] | 0x08
        elif tone == 'grave':
            if is_upper:
                result[0] = result[0] | 0x68
            else:
                result[0] = MARK_CIRCUM_TOP[0]
                result[1] = MARK_CIRCUM_TOP[1] | 0x40
        elif tone == 'hook':
            if is_upper:
                result[0] = result[0] | 0x28
            else:
                result[0] = MARK_CIRCUM_TOP[0]
                result[1] = MARK_CIRCUM_TOP[1]
        elif tone == 'tilde':
            if is_upper:
                result[0] = result[0] | 0x68
            else:
                result[0] = MARK_CIRCUM_TOP[0]
                result[1] = MARK_CIRCUM_TOP[1]
    elif modifier == 'circumflex' and tone is None:
        if is_upper:
            result[0] = MARK_CIRCUM_TOP[0] | result[0]
        else:
            result[0] = MARK_CIRCUM_TOP[0]
            result[1] = MARK_CIRCUM_TOP[1]
    elif modifier == 'breve' and tone is not None:
        if tone == 'acute':
            if is_upper:
                result[0] = result[0] | 0x58
            else:
                result[0] = MARK_BREVE_TOP[0]
                result[1] = MARK_BREVE_TOP[1] | 0x08
        elif tone == 'grave':
            if is_upper:
                result[0] = result[0] | 0xD0
            else:
                result[0] = MARK_BREVE_TOP[0]
                result[1] = MARK_BREVE_TOP[1] | 0x40
        elif tone == 'hook':
            if is_upper:
                result[0] = result[0] | 0x58
            else:
                result[0] = MARK_BREVE_TOP[0]
                result[1] = MARK_BREVE_TOP[1]
        elif tone == 'tilde':
            if is_upper:
                result[0] = result[0] | 0xD0
            else:
                result[0] = MARK_BREVE_TOP[0]
                result[1] = MARK_BREVE_TOP[1]
    elif modifier == 'breve' and tone is None:
        if is_upper:
            result[0] = MARK_BREVE_TOP[0] | result[0]
        else:
            result[0] = MARK_BREVE_TOP[0]
            result[1] = MARK_BREVE_TOP[1]
    elif tone is not None and modifier is None:
        # Simple tone mark only (no base modifier)
        if tone == 'acute':
            if is_upper:
                result[0] = MARK_ACUTE_TOP[1]  # dot above center
            else:
                result[0] = MARK_ACUTE_TOP[0]
                result[1] = MARK_ACUTE_TOP[1]
        elif tone == 'grave':
            if is_upper:
                result[0] = MARK_GRAVE_TOP[1]
            else:
                result[0] = MARK_GRAVE_TOP[0]
                result[1] = MARK_GRAVE_TOP[1]
        elif tone == 'hook':
            if is_upper:
                result[0] = MARK_HOOK_TOP[0]
            else:
                result[0] = MARK_HOOK_TOP[0]
                result[1] = MARK_HOOK_TOP[1]
        elif tone == 'tilde':
            if is_upper:
                result[0] = MARK_TILDE_TOP[0]
            else:
                result[0] = MARK_TILDE_TOP[0]
                result[1] = MARK_TILDE_TOP[1]

    # Apply dot below
    if dot_below:
        result[7] = MARK_DOT_BELOW[0]

    return result


def scale_5x8_to_8x16(glyph_5x8):
    """Scale a 5x8 glyph to 8x16 by doubling each pixel."""
    result = []
    for row in glyph_5x8:
        # Expand 5 pixels (bits 7-3) to 8 pixels
        expanded = 0
        for bit in range(5):
            if row & (0x80 >> bit):
                # Map bit position: 0->0,1  1->1,2  2->3,4  3->5,6  4->6,7
                p = int(bit * 1.6)
                expanded |= (0x80 >> p)
                if p + 1 < 8:
                    expanded |= (0x80 >> (p + 1))
        # Each row is doubled vertically
        result.append(expanded)
        result.append(expanded)
    return result


def scale_5x8_to_16x16(glyph_5x8):
    """Scale a 5x8 glyph to 16x16 by quadrupling each pixel."""
    result = []
    for row in glyph_5x8:
        # Expand 5 pixels to 16 pixels (each pixel becomes ~3 pixels wide)
        high = 0
        low = 0
        for bit in range(5):
            if row & (0x80 >> bit):
                # Map each of 5 bits to ~3 bits across 16
                start = int(bit * 3.2)
                for p in range(3):
                    pos = start + p
                    if pos < 8:
                        high |= (0x80 >> pos)
                    elif pos < 16:
                        low |= (0x80 >> (pos - 8))
        # Each row doubled vertically
        result.extend([high, low])
        result.extend([high, low])
    return result


def generate_font_header(font_name, width, height, all_codepoints, glyph_generator):
    """Generate a complete C header for a font."""
    lines = []
    lines.append(f'/**')
    lines.append(f' * @file {font_name}.h')
    lines.append(f' * @brief {width}x{height} Vietnamese bitmap font for VietnameseOLED.')
    lines.append(f' *')
    lines.append(f' * Auto-generated font data covering ASCII + full Vietnamese Unicode.')
    lines.append(f' * Total glyphs: {len(all_codepoints)}')
    lines.append(f' * Format: Row-major, MSB-first, {width} pixels wide, {height} pixels tall.')
    lines.append(f' *')
    lines.append(f' * @author VietnameseOLED Font Generator')
    lines.append(f' * @version 1.0.0')
    lines.append(f' * @copyright MIT License')
    lines.append(f' */')
    lines.append(f'')
    lines.append(f'#ifndef VN_{font_name.upper()}_H')
    lines.append(f'#define VN_{font_name.upper()}_H')
    lines.append(f'')
    lines.append(f'#include "../VNConfig.h"')
    lines.append(f'#include "../VNTypes.h"')
    lines.append(f'')

    # Generate bitmap data
    bytes_per_row = (width + 7) // 8
    bytes_per_glyph = bytes_per_row * height

    lines.append(f'/** @brief Bitmap data for {font_name} ({len(all_codepoints)} glyphs, {bytes_per_glyph} bytes each). */')
    lines.append(f'static const uint8_t {font_name}_bitmaps[] VN_PROGMEM = {{')

    all_glyphs = {}
    bitmap_offset = 0

    for cp in all_codepoints:
        glyph_data = glyph_generator(cp)
        all_glyphs[cp] = {
            'data': glyph_data,
            'offset': bitmap_offset,
            'width': width,
            'height': height,
        }
        bitmap_offset += len(glyph_data)

        # Format the glyph data
        char_repr = chr(cp) if (cp >= 0x20 and cp < 0x7F) else f'U+{cp:04X}'
        hex_str = ', '.join(f'0x{b:02X}' for b in glyph_data)
        lines.append(f'    {hex_str},  /* {char_repr} */')

    lines.append(f'}};')
    lines.append(f'')

    # Generate glyph metadata table
    lines.append(f'/** @brief Glyph metadata for {font_name}. */')
    lines.append(f'static const VNGlyphEntry {font_name}_glyphs[] VN_PROGMEM = {{')
    lines.append(f'    /* {{bitmapOffset, width, height, xOffset, yOffset, xAdvance}} */')

    glyph_index = 0
    for cp in all_codepoints:
        g = all_glyphs[cp]
        x_advance = width + 1  # 1 pixel spacing
        if width <= 5:
            x_advance = 6  # standard 5x8 advance
        elif width <= 8:
            x_advance = width + 1
        else:
            x_advance = width + 1

        char_repr = chr(cp) if (cp >= 0x20 and cp < 0x7F) else f'U+{cp:04X}'
        lines.append(f'    {{{g["offset"]}, {g["width"]}, {g["height"]}, 0, 0, {x_advance}}},  /* [{glyph_index}] {char_repr} */')
        glyph_index += 1

    lines.append(f'}};')
    lines.append(f'')

    # Generate Unicode mapping table (sorted by codepoint)
    sorted_cps = sorted(all_codepoints)
    lines.append(f'/** @brief Unicode-to-glyph-index mapping for {font_name} (sorted by codepoint). */')
    lines.append(f'static const VNUnicodeEntry {font_name}_unicode_map[] VN_PROGMEM = {{')
    lines.append(f'    /* {{codepoint, glyphIndex}} */')

    for cp in sorted_cps:
        glyph_idx = all_codepoints.index(cp)
        char_repr = chr(cp) if (cp >= 0x20 and cp < 0x7F) else f'U+{cp:04X}'
        lines.append(f'    {{0x{cp:04X}, {glyph_idx}}},  /* {char_repr} */')

    lines.append(f'}};')
    lines.append(f'')

    # Generate font descriptor
    line_height = height
    baseline = height - 1 if height <= 8 else height - 2

    lines.append(f'/** @brief Font descriptor for {font_name}. */')
    lines.append(f'static const VNFont {font_name} = {{')
    lines.append(f'    {font_name}_bitmaps,       /* bitmapData */')
    lines.append(f'    {font_name}_glyphs,        /* glyphTable */')
    lines.append(f'    {font_name}_unicode_map,   /* unicodeMap */')
    lines.append(f'    {len(all_codepoints)},  /* glyphCount */')
    lines.append(f'    {len(sorted_cps)},  /* unicodeMapSize */')
    lines.append(f'    {line_height},    /* lineHeight */')
    lines.append(f'    {baseline},    /* baseline */')
    lines.append(f'    {width},     /* defaultWidth */')
    lines.append(f'    {height},    /* defaultHeight */')
    lines.append(f'}};')
    lines.append(f'')
    lines.append(f'#endif /* VN_{font_name.upper()}_H */')
    lines.append(f'')

    return '\n'.join(lines)


def gen_5x8(cp):
    """Generate 5x8 glyph data for a codepoint."""
    if cp in FONT5X8_ASCII:
        return FONT5X8_ASCII[cp]
    else:
        return compose_5x8_glyph(cp)


def gen_8x16(cp):
    """Generate 8x16 glyph data by scaling up 5x8."""
    base = gen_5x8(cp)
    return scale_5x8_to_8x16(base)


def gen_16x16(cp):
    """Generate 16x16 glyph data by scaling up 5x8."""
    base = gen_5x8(cp)
    return scale_5x8_to_16x16(base)


def main():
    # Build complete codepoint list: ASCII + Vietnamese
    all_codepoints = ASCII_RANGE + sorted(VIETNAMESE_CODEPOINTS)

    script_dir = os.path.dirname(os.path.abspath(__file__))
    fonts_dir = os.path.join(script_dir, '..', '..', 'src', 'fonts')
    os.makedirs(fonts_dir, exist_ok=True)



    # Generate 8x16 font
    print(f"Generating Font_8x16_VN ({len(all_codepoints)} glyphs)...")
    header_8x16 = generate_font_header('Font_8x16_VN', 8, 16, all_codepoints, gen_8x16)
    with open(os.path.join(fonts_dir, 'Font_8x16_VN.h'), 'w', encoding='utf-8') as f:
        f.write(header_8x16)
    print(f"  -> Font_8x16_VN.h written")

    # Generate 16x16 font
    print(f"Generating Font_16x16_VN ({len(all_codepoints)} glyphs)...")
    header_16x16 = generate_font_header('Font_16x16_VN', 16, 16, all_codepoints, gen_16x16)
    with open(os.path.join(fonts_dir, 'Font_16x16_VN.h'), 'w', encoding='utf-8') as f:
        f.write(header_16x16)
    print(f"  -> Font_16x16_VN.h written")

    print(f"\nDone! Generated 2 font headers with {len(all_codepoints)} glyphs each.")
    print(f"  ASCII: {len(ASCII_RANGE)} characters (U+0020..U+007E)")
    print(f"  Vietnamese: {len(VIETNAMESE_CODEPOINTS)} characters")
    print(f"  Total: {len(all_codepoints)} glyphs per font")


if __name__ == '__main__':
    main()
