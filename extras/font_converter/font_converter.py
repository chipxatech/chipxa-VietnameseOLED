#!/usr/bin/env python3
/**
 * @file font_converter.py
 * @brief Converts TTF/OTF fonts to VietnameseOLED compatible C++ header files.
 *
 * Uses Pillow (PIL) to render glyphs and extract metrics.
 *
 * @author VietnameseOLED Contributors
 * @version 1.0.0
 * @copyright MIT License
 */

import os
import sys
import argparse
import math
from PIL import Image, ImageFont, ImageDraw

# Complete list of 134 Vietnamese codepoints
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

def get_glyph_data(font, codepoint):
    """Renders a single codepoint and extracts its bitmap and metrics."""
    char = chr(codepoint)
    
    # Handle space character specially
    if codepoint == 0x20:
        # Get space width using getlength or getsize
        try:
            space_width = int(round(font.getlength(char)))
        except AttributeError:
            space_width = font.getsize(char)[0]
        
        # If space_width is 0 or negative, default to size/3
        if space_width <= 0:
            space_width = font.size // 3
            
        return {
            'bitmap': [],
            'width': 0,
            'height': 0,
            'xOffset': 0,
            'yOffset': 0,
            'xAdvance': space_width
        }

    # Get bounding box and advance width
    try:
        bbox = font.getbbox(char)
        x_advance = int(round(font.getlength(char)))
    except AttributeError:
        # Fallback for older PIL versions
        w, h = font.getsize(char)
        bbox = (0, 0, w, h)
        x_advance = w

    if bbox is None:
        return {
            'bitmap': [],
            'width': 0,
            'height': 0,
            'xOffset': 0,
            'yOffset': 0,
            'xAdvance': x_advance
        }

    left, top, right, bottom = bbox
    width = right - left
    height = bottom - top

    if width <= 0 or height <= 0:
        return {
            'bitmap': [],
            'width': 0,
            'height': 0,
            'xOffset': left,
            'yOffset': top,
            'xAdvance': x_advance
        }

    # Render glyph onto a mask image
    mask = font.getmask(char, mode='1')
    
    # Convert mask pixels to a list of bytes
    # Bitmap layout: row-major, MSB-first, padded to byte boundary per row
    bytes_per_row = (width + 7) // 8
    bitmap_bytes = []
    
    for r in range(height):
        current_byte = 0
        bit_count = 0
        for c in range(width):
            # Read pixel from mask (0 = black, 255 = white)
            pixel = mask.getpixel((c, r))
            pixel_bit = 1 if pixel > 0 else 0
            
            current_byte = (current_byte << 1) | pixel_bit
            bit_count += 1
            
            if bit_count == 8:
                bitmap_bytes.append(current_byte)
                current_byte = 0
                bit_count = 0
                
        if bit_count > 0:
            # Pad remaining bits of the row byte with zeros
            current_byte <<= (8 - bit_count)
            bitmap_bytes.append(current_byte)

    return {
        'bitmap': bitmap_bytes,
        'width': width,
        'height': height,
        'xOffset': left,
        'yOffset': top,
        'xAdvance': x_advance
    }

def print_glyph_preview(glyph):
    """Prints a simple ASCII preview of the glyph to stdout."""
    bitmap = glyph['bitmap']
    w = glyph['width']
    h = glyph['height']
    if w == 0 or h == 0:
        return

    bytes_per_row = (w + 7) // 8
    for r in range(h):
        line = ""
        for c in range(w):
            byte_idx = r * bytes_per_row + (c // 8)
            bit_idx = 7 - (c % 8)
            val = (bitmap[byte_idx] >> bit_idx) & 1
            line += "#" if val else "."
        print("  " + line)

def main():
    parser = argparse.ArgumentParser(description="Convert TrueType/OpenType fonts to VietnameseOLED C++ headers.")
    parser.add_argument("-i", "--input", required=True, help="Path to input TTF/OTF font file")
    parser.add_argument("-s", "--size", type=int, required=True, help="Font size in pixels")
    parser.add_argument("-o", "--output", required=True, help="Path to output C++ header file")
    parser.add_argument("-n", "--name", help="Custom font class name (e.g. MyFont12)")
    parser.add_argument("-p", "--preview", action="store_true", help="Print ASCII preview of Vietnamese characters")
    
    args = parser.parse_args()

    if not os.path.exists(args.input):
        print(f"Error: Input font file not found: {args.input}", file=sys.stderr)
        sys.exit(1)

    # Load font using Pillow
    try:
        font = ImageFont.truetype(args.input, args.size)
    except Exception as e:
        print(f"Error loading font: {e}", file=sys.stderr)
        sys.exit(1)

    # Automatically derive font name if not specified
    if args.name:
        font_name = args.name
    else:
        base = os.path.splitext(os.path.basename(args.input))[0]
        # Clean base name
        base = "".join(c if c.isalnum() else "_" for c in base)
        font_name = f"Font_{base}_{args.size}_VN"

    print(f"Processing font '{args.input}' at size {args.size}px...")

    # Assemble all codepoints (ASCII + Vietnamese diacritics)
    all_codepoints = ASCII_RANGE + sorted(VIETNAMESE_CODEPOINTS)
    
    # Process all glyphs
    glyphs = {}
    total_bitmap_bytes = 0
    
    for cp in all_codepoints:
        glyph = get_glyph_data(font, cp)
        glyphs[cp] = glyph
        total_bitmap_bytes += len(glyph['bitmap'])

        if args.preview and cp in VIETNAMESE_CODEPOINTS:
            char_repr = chr(cp)
            print(f"Preview U+{cp:04X} ({char_repr}) size={glyph['width']}x{glyph['height']} offset=({glyph['xOffset']},{glyph['yOffset']}) advance={glyph['xAdvance']}:")
            print_glyph_preview(glyph)

    # Calculate global metrics
    # Line height is generally font.getmetrics() ascent + descent
    try:
        ascent, descent = font.getmetrics()
        line_height = ascent + descent
        baseline = ascent
    except AttributeError:
        line_height = args.size
        baseline = int(args.size * 0.8)

    # Default width: check 'A' or use size/2
    default_width = glyphs.get(0x41, {}).get('width', args.size // 2)
    default_height = glyphs.get(0x41, {}).get('height', args.size)

    # Generate C++ Header Content
    lines = []
    lines.append(f'/**')
    lines.append(f' * @file {font_name}.h')
    lines.append(f' * @brief Auto-generated Vietnamese bitmap font from vector font.')
    lines.append(f' *')
    lines.append(f' * Font source: {os.path.basename(args.input)}')
    lines.append(f' * Font size: {args.size}px')
    lines.append(f' * Total glyphs: {len(all_codepoints)}')
    lines.append(f' *')
    lines.append(f' * @author VietnameseOLED Font Converter')
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

    # Bitmaps data
    lines.append(f'/** @brief Bitmap data for {font_name} ({total_bitmap_bytes} bytes). */')
    lines.append(f'static const uint8_t {font_name}_bitmaps[] VN_PROGMEM = {{')
    
    bitmap_offset = 0
    glyph_offsets = {}
    
    for cp in all_codepoints:
        glyph = glyphs[cp]
        glyph_offsets[cp] = bitmap_offset
        
        char_repr = chr(cp) if (cp >= 0x20 and cp < 0x7F) else f'U+{cp:04X}'
        if len(glyph['bitmap']) > 0:
            hex_str = ", ".join(f"0x{b:02X}" for b in glyph['bitmap'])
            lines.append(f'    {hex_str},  /* {char_repr} */')
            bitmap_offset += len(glyph['bitmap'])
        else:
            lines.append(f'    /* {char_repr} (empty) */')
            
    lines.append(f'}};')
    lines.append(f'')

    # Glyph metadata table
    lines.append(f'/** @brief Glyph metadata table for {font_name}. */')
    lines.append(f'static const VNGlyphEntry {font_name}_glyphs[] VN_PROGMEM = {{')
    lines.append(f'    /* {{bitmapOffset, width, height, xOffset, yOffset, xAdvance}} */')

    for idx, cp in enumerate(all_codepoints):
        g = glyphs[cp]
        offset = glyph_offsets[cp]
        char_repr = chr(cp) if (cp >= 0x20 and cp < 0x7F) else f'U+{cp:04X}'
        lines.append(f'    {{{offset}, {g["width"]}, {g["height"]}, {g["xOffset"]}, {g["yOffset"]}, {g["xAdvance"]}}},  /* [{idx}] {char_repr} */')

    lines.append(f'}};')
    lines.append(f'')

    # Unicode to glyph index mapping (sorted by codepoint)
    sorted_cps = sorted(all_codepoints)
    lines.append(f'/** @brief Unicode mapping table for {font_name} (sorted by codepoint). */')
    lines.append(f'static const VNUnicodeEntry {font_name}_unicode_map[] VN_PROGMEM = {{')
    lines.append(f'    /* {{codepoint, glyphIndex}} */')

    for cp in sorted_cps:
        glyph_idx = all_codepoints.index(cp)
        char_repr = chr(cp) if (cp >= 0x20 and cp < 0x7F) else f'U+{cp:04X}'
        lines.append(f'    {{0x{cp:04X}, {glyph_idx}}},  /* {char_repr} */')

    lines.append(f'}};')
    lines.append(f'')

    # Font descriptor
    lines.append(f'/** @brief Font descriptor for {font_name}. */')
    lines.append(f'static const VNFont {font_name} = {{')
    lines.append(f'    {font_name}_bitmaps,       /* bitmapData */')
    lines.append(f'    {font_name}_glyphs,        /* glyphTable */')
    lines.append(f'    {font_name}_unicode_map,   /* unicodeMap */')
    lines.append(f'    {len(all_codepoints)},  /* glyphCount */')
    lines.append(f'    {len(sorted_cps)},  /* unicodeMapSize */')
    lines.append(f'    {line_height},    /* lineHeight */')
    lines.append(f'    {baseline},    /* baseline */')
    lines.append(f'    {default_width},     /* defaultWidth */')
    lines.append(f'    {default_height},    /* defaultHeight */')
    lines.append(f'}};')
    lines.append(f'')
    lines.append(f'#endif /* VN_{font_name.upper()}_H */')
    lines.append(f'')

    # Write to output file
    try:
        os.makedirs(os.path.dirname(os.path.abspath(args.output)), exist_ok=True)
        with open(args.output, "w", encoding="utf-8") as f:
            f.write("\n".join(lines))
        print(f"Successfully generated C++ font header: {args.output}")
        print(f"  Total glyphs: {len(all_codepoints)}")
        print(f"  Total bitmap bytes: {total_bitmap_bytes}")
        print(f"  Metrics: height={line_height}px, baseline={baseline}px")
    except Exception as e:
        print(f"Error writing output file: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
