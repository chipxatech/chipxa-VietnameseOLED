# Library Architecture

This document describes the design pattern, structural layout, and runtime pipeline of the **VietnameseOLED** library.

---

## 1. High-Level Subsystem Flow

The library follows a modular design separating the input parsing, indexing, font mapping, caching, layout, rendering, and hardware communication into distinct, low-coupling classes.

Below is the standard workflow when rendering a UTF-8 string:

```
[ UTF-8 String ]
       │
       ▼
[ UTF8Decoder (DFA State Machine) ] ──(Invalid sequence)──► [ Replacement Char ]
       │
       ▼ (Valid Codepoint)
[ UnicodeMapper (PROGMEM binary search) ]
       │
       ▼ (Glyph Index)
[ FontEngine ] ◄─────────────────────────────────┐
       │                                         │
       ▼ (Check cache)                           │
[ GlyphCache (LRU) ] ──(Miss)──► [ Read PROGMEM GlyphEntry ]
       │                                         │
       ▼ (Cached GlyphInfo)                      │
[ LayoutEngine ] ◄───────────────────────────────┘
       │ (Word wrap / Alignment / Margins / Viewports)
       ▼
[ TextEngine (Orchestrator) ]
       │
       ▼
[ GlyphRenderer ] (Transforms: scale, rotate, mirror)
       │
       ▼
[ BitmapRenderer ] (Draw pixels / lines / rectangles)
       │
       ▼ (Framebuffer)
[ DisplayAdapter ] (SSD1306/SH1106 etc. Concrete Implementation)
       │
       ▼ (I2C/SPI commands)
[ OLED Display Screen ]
```

---

## 2. Core Architectural Subsystems

### 2.1 UTF-8 Parsing: `UTF8Decoder`
- **Responsibility**: Processes multi-byte UTF-8 sequences.
- **Design**: Implements a Deterministic Finite Automaton (DFA) state machine. It processes strings byte-by-byte without lookahead, copying, or heap allocation.
- **Error Handling**: Transitions to a `REJECT` state on invalid bytes (e.g. overlong encodings, out-of-order continuation bytes) and outputs the Unicode replacement character `U+FFFD`.

### 2.2 Glyph Mapping: `UnicodeMapper`
- **Responsibility**: Checks character blocks and maps 32-bit codepoints to 16-bit indexes in the font's lookup table.
- **Design**: To save SRAM, all character-to-index translation lists are kept in Flash (`PROGMEM`). It performs an $O(\log n)$ binary search on sorted index entries of the active font.

### 2.3 Font & Metrics: `FontEngine`
- **Responsibility**: Reads bitmap and layout offset metrics from Flash memory.
- **Design**: Leverages platform-specific reader macros (`pgm_read_byte` / direct dereferencing) to retrieve structure fields. Contains methods to calculate overall text dimensions before drawing.

### 2.4 Performance: `GlyphCache`
- **Responsibility**: Avoids repeated slow Flash reads for commonly repeated letters (e.g. spaces, vowels).
- **Design**: Fixed-size Least-Recently-Used (LRU) cache using a stack-allocated lookup array. When the cache is full, the element with the oldest access timestamp is evicted.

### 2.5 Word Wrap & Alignment: `LayoutEngine`
- **Responsibility**: Computes line breaks and offset coordinates.
- **Design**:
  - `WordWrapper` performs a non-destructive scan of the text to locate wrap boundaries (spaces, hyphens, tabs) fitting a maximum pixel width.
  - `TextAlignment` distributes words for justification and offsets text lines for horizontal/vertical alignment.

### 2.6 Renderers: `GlyphRenderer` & `BitmapRenderer`
- **Responsibility**: Renders pixels to the in-memory adapter buffer.
- **Design**:
  - `BitmapRenderer` performs basic line clipping and primitives drawing.
  - `GlyphRenderer` reads the 1-bit glyph bitmap and applies integer scaling, mirror reflections, and 90° rotations in real-time.

### 2.7 Hardware Interface: `DisplayAdapter`
- **Responsibility**: Abstracts display driver operations.
- **Design**: Extends the **Adapter Pattern**. Subclasses implement command transmission for specific chips (SSD1306, SH1106, SH1107, SSD1327) using I2C or SPI protocols. No screen-specific drawing logic lives in the core library.
