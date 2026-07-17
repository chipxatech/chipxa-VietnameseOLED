# VietnameseOLED Font Converter

A Python utility to convert TrueType/OpenType vector fonts (`.ttf`, `.otf`) into C++ header files compatible with the **VietnameseOLED** library.

The converter automatically extracts the 95 printable ASCII characters and the 134 Vietnamese diacritical characters (total 229 glyphs) and packages them into memory-efficient, binary-searchable tables.

## Requirements

The utility requires Python 3.x and the **Pillow** (PIL) image processing library.

### Installation

Install the required library using `pip`:

```bash
pip install -r requirements.txt
```

Or install it directly:

```bash
pip install Pillow
```

## Usage

```bash
python font_converter.py -i <input_font> -s <size_in_pixels> -o <output_header> [options]
```

### Command Line Arguments

| Argument | Long Option | Description | Required |
|---|---|---|---|
| `-i` | `--input` | Path to the TrueType/OpenType font file (`.ttf`, `.otf`) | Yes |
| `-s` | `--size` | Target font height/size in pixels | Yes |
| `-o` | `--output` | Path to the output C++ header file to generate | Yes |
| `-n` | `--name` | Custom name for the font descriptor variable (defaults to filename) | No |
| `-p` | `--preview` | Print an ASCII rendering of generated Vietnamese characters to terminal | No |

### Example

Generate a 12px Vietnamese header from `Roboto-Regular.ttf`:

```bash
python font_converter.py -i Roboto-Regular.ttf -s 12 -o Font_Roboto_12.h -p
```

This will create a C++ file `Font_Roboto_12.h` containing:
- Bounding-box-clipped 1-bit bitmaps.
- Transformed metrics table (`VNGlyphEntry`).
- Unicode-sorted index mapping (`VNUnicodeEntry`).
- The `VNFont` font descriptor `Font_Roboto_12_VN`.

## Integration into Skethes

1. Copy the generated `.h` file to your Arduino sketch folder or your project's custom fonts folder.
2. Include the header in your sketch.
3. Pass the font descriptor variable to `oled.setFont()`:

```cpp
#include <VietnameseOLED.h>
#include <drivers/SSD1306Adapter.h>
#include "Font_Roboto_12.h" // Your generated custom font

SSD1306Adapter display(128, 64);
VietnameseOLED oled(display);

void setup() {
    oled.begin();
    oled.clear();
    
    // Set custom font
    oled.setFont(Font_Roboto_12_VN);
    
    oled.drawUTF8(0, 0, "Xin chào phông chữ mới!");
    oled.update();
}
```
