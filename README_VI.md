# VietnameseOLED

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[[English Version](README.md)]

**VietnameseOLED** là thư viện C++17 mã nguồn mở, tối ưu và sẵn sàng cho môi trường sản xuất (production-ready) được phát triển bởi **CÔNG TY TNHH CÔNG NGHỆ CHIPXA**. Thư viện được thiết kế nhằm cung cấp giải pháp hiển thị tiếng Việt có dấu Unicode (UTF-8) một cách dễ dàng, chính xác và hiệu quả trên các màn hình OLED đơn sắc (monochrome) và grayscale (thang độ xám). Thư viện được tối ưu hóa cho các vi điều khiển hạn chế về bộ nhớ (AVR) cho đến các hệ thống hiệu năng cao (ESP32, ESP8266, STM32, CH32, RP2040, RP2350).

Hiện tại, hầu hết các thư viện hiển thị phổ biến trên Arduino và hệ thống nhúng như Adafruit GFX, SSD1306 hoặc SH1106 chỉ hỗ trợ bảng mã ASCII tiêu chuẩn. Điều này khiến việc hiển thị tiếng Việt có dấu gặp rất nhiều khó khăn, yêu cầu lập trình viên phải tự thiết kế font chữ tùy chỉnh hoặc ghép ký tự thủ công. VietnameseOLED được phát triển để giải quyết triệt để hạn chế này.

Thư viện tích hợp sẵn bộ giải mã **UTF-8 decoder**, cơ chế ánh xạ Unicode sang font bitmap, giúp hiển thị đầy đủ bảng chữ cái tiếng Việt trên nhiều nền tảng phần cứng khác nhau bao gồm **Arduino, ESP8266, ESP32, STM32, CH32, Raspberry Pi Pico**, và các vi điều khiển tương thích khác. Ngoài ra, thư viện được thiết kế với kiến trúc mở, dễ dàng mở rộng để hỗ trợ thêm nhiều loại màn hình và driver đồ họa khác trong tương lai.

---

## Mục tiêu Dự án

* **Hỗ trợ Tiếng Việt Unicode Toàn diện:** Hiển thị trọn vẹn văn bản tiếng Việt có dấu chuẩn Unicode (UTF-8).
* **API Tích hợp Đơn giản:** Cung cấp API trực quan, đơn giản và dễ sử dụng cho các dự án nhúng.
* **Tương thích Driver Rộng rãi:** Đảm bảo tương thích với nhiều loại màn hình OLED và các thư viện đồ họa phổ biến.
* **Tỉ lệ Font và Mở rộng Linh hoạt:** Hỗ trợ nhiều kích thước font và khả năng mở rộng font tùy chỉnh.
* **Phát triển Giao diện Nhanh chóng:** Rút ngắn thời gian phát triển giao diện người dùng tiếng Việt trên các sản phẩm nhúng.
* **Đóng góp Cộng đồng Mã nguồn mở:** Xây dựng một thư viện mã nguồn mở chất lượng cao phục vụ cộng đồng Maker, IoT, giáo dục STEM và phát triển thiết bị điện tử thương mại.

---

## Đối tượng Hướng tới

VietnameseOLED phù hợp cho:
* **Nhà phát triển Arduino & ESP** đang tìm kiếm giải pháp bản địa hóa giao diện nhanh chóng.
* **Kỹ sư Phần mềm Nhúng** yêu cầu logic vẽ văn bản tối ưu hóa cao.
* **Nhà phát triển Thiết bị IoT** xây dựng bộ điều khiển trung tâm (smart hub), trạm thời tiết hoặc màn hình giám sát công nghiệp.
* **Nhà sản xuất Thiết bị Điện tử Thương mại** thiết kế sản phẩm cho thị trường Việt Nam.
* **Trung tâm STEM & Robot** hướng dẫn học sinh và maker cách hiển thị dữ liệu cảm biến nội địa hóa.
* **Học sinh, Giáo viên và Cộng đồng Maker** xây dựng các dự án cá nhân/sở thích.

---

## Lộ trình Phát triển tương lai

VietnameseOLED không chỉ dừng lại ở một thư viện hiển thị tiếng Việt đơn thuần, mà hướng tới trở thành một nền tảng quản lý font và dựng văn bản chuẩn cho hệ thống nhúng. Trong tương lai, dự án sẽ mở rộng hỗ trợ thêm nhiều driver màn hình, các bộ font chữ mới, nhiều ngôn ngữ Unicode khác, và các công cụ chuyển đổi font chuyên nghiệp nhằm phục vụ tối đa nhu cầu của các nhà phát triển phần cứng trên toàn thế giới.

---

## Các Tính năng Nổi bật

- **Hỗ trợ Tiếng Việt Đầy đủ:** Hỗ trợ toàn bộ 134 ký tự tiếng Việt viết hoa/viết thường có dấu và bảng mã ASCII tiêu chuẩn.
- **Tiết kiệm Bộ nhớ Cực hạn:** Hoàn toàn không sử dụng cấp phát động (`new`/`malloc`), thay vào đó dựa trên các cấu trúc tĩnh. Toàn bộ bảng glyph font và danh sách ánh xạ được lưu trực tiếp trong bộ nhớ Flash thông qua cơ chế trừu tượng `PROGMEM`.
- **Độc lập Phần cứng (Adapter Pattern):** Giao diện driver màn hình được tách biệt hoàn toàn, cho phép dễ dàng tích hợp màn hình mới mà không cần chỉnh sửa mã nguồn cốt lõi của thư viện.
- **Bộ giải mã UTF-8 dựa trên DFA:** Bộ giải mã kiểm tra tính hợp lệ UTF-8 bằng máy trạng thái hữu hạn (DFA) một lượt, không cần cấp phát bộ nhớ động, độ ổn định cực cao.
- **Bộ nhớ đệm LRU Glyph Cache:** Cơ chế cache thông minh giúp giảm thiểu số lần đọc bộ nhớ Flash chậm khi dựng bố cục ký tự.
- **Bố cục Văn bản Nâng cao (Advanced Text Layout):** Tích hợp hệ thống quản lý bố cục hoàn chỉnh hỗ trợ căn lề (margins, padding), khoảng cách dòng (line spacing), căn chỉnh ngang (trái, giữa, phải, đều hai bên), căn chỉnh dọc (trên, giữa, dưới) và tự động xuống dòng (không xuống dòng, ngắt theo ký tự, ngắt theo từ).
- **Phép biến đổi hình học (Transforms):** Thu phóng thời gian thực (từ 1x đến 4x), xoay văn bản (các bước 90 độ), và lật/phản chiếu hình ảnh.
- **Bộ Font tích hợp sẵn:** Bao gồm 2 bộ font bitmap tiếng Việt được thiết kế thủ công tỉ mỉ:
  - `Font_8x16_VN` (kích thước trung bình, hiển thị sắc nét, dễ đọc)
  - `Font_16x16_VN` (kích thước lớn)
- **Khả năng Mở rộng:** Cung cấp công cụ dòng lệnh viết bằng Python để chuyển đổi các font vector chuẩn TrueType (`.ttf`) hoặc OpenType (`.otf`) thành định dạng font tương thích của thư viện.

---

## Cấu trúc Thư mục

```
VietnameseOLED/
├── CMakeLists.txt              # Cấu hình build cho unit test & IDE
├── Doxyfile                    # File cấu hình Doxygen
├── LICENSE                     # Giấy phép MIT
├── library.properties          # Thông số thư viện Arduino
├── keywords.txt                # Cấu hình highlight cú pháp
├── CHANGELOG.md                # Nhật ký thay đổi phiên bản
├── src/                        # Mã nguồn cốt lõi của thư viện
│   ├── VietnameseOLED.h/.cpp   # API chính giao tiếp với người dùng
│   ├── VNConfig.h              # Các macro cấu hình biên dịch và cài đặt cache
│   ├── VNTypes.h               # Cấu trúc dữ liệu core và kiểu enum
│   ├── UTF8Decoder.h/.cpp      # Máy trạng thái UTF-8 dựa trên DFA
│   ├── UnicodeMapper.h/.cpp    # Tra cứu ánh xạ codepoint-sang-glyph-index bằng tìm kiếm nhị phân
│   ├── FontEngine.h/.cpp       # Bộ nạp glyph từ bộ nhớ PROGMEM mức thấp
│   ├── FontManager.h/.cpp      # Bộ chọn phông chữ khi chạy (runtime)
│   ├── GlyphCache.h/.cpp       # Bộ nhớ đệm LRU
│   ├── BitmapRenderer.h/.cpp   # Các hàm vẽ pixel cơ bản lên Framebuffer
│   ├── GlyphRenderer.h/.cpp    # Vẽ glyph với các phép biến đổi scale, xoay, phản chiếu
│   ├── WordWrapper.h/.cpp      # Bộ xác định ranh giới xuống dòng
│   ├── TextAlignment.h/.cpp    # Bộ tính toán căn giữa và căn đều hai bên
│   ├── TextEngine.h/.cpp       # Bộ điều phối luồng dựng hình & layout pipeline
│   ├── drivers/                # Các driver màn hình adapter
│   │   ├── DisplayAdapter.h    # Giao diện cơ sở trừu tượng cho driver màn hình
│   │   ├── SSD1306Adapter.h    # Driver SSD1306 I2C/SPI (128x64 & 128x32)
│   │   ├── SH1106Adapter.h     # Driver SH1106 I2C/SPI hỗ trợ dịch cột RAM
│   │   ├── SH1107Adapter.h     # Driver SH1107 I2C/SPI (128x128)
│   │   └── SSD1327Adapter.h    # Driver SSD1327 I2C/SPI (Grayscale 4-bit)
│   └── fonts/                  # Phông chữ bitmap tiếng Việt thiết kế sẵn
│       ├── Font_8x16_VN.h
│       └── Font_16x16_VN.h
├── examples/                   # 10 ví dụ chạy trên các vi điều khiển thực tế
├── extras/                     # Công cụ chuyển đổi phông chữ Python
│   └── font_converter/
│       ├── font_converter.py
│       ├── requirements.txt
│       └── README.md
├── test/                       # Bộ kiểm thử đơn vị (unit test) C++ native
└── docs/                       # Các bài viết tài liệu kỹ thuật chi tiết
```

---

## Phần cứng Hỗ trợ

Thư viện hỗ trợ sẵn các driver màn hình OLED phổ biến (giao tiếp qua I2C hoặc SPI):

1. **SSD1306:** Cấu hình 128x64 và 128x32 (đơn sắc).
2. **SH1106:** OLED 1.3" 128x64 với ánh xạ RAM 132 cột bên trong.
3. **SH1107:** OLED độ phân giải cao 128x128.
4. **SSD1327:** Màn hình grayscale 4-bit 128x128 (16 mức xám).

---

## Hướng dẫn Cài đặt

### Qua Thư viện Arduino (Library Manager)
1. Mở Arduino IDE.
2. Truy cập **Sketch** -> **Include Library** -> **Manage Libraries...**
3. Tìm kiếm từ khóa `VietnameseOLED` và nhấn **Install**.

### Cài đặt thủ công bằng file ZIP
1. Tải về file ZIP của repository này.
2. Trong Arduino IDE, truy cập **Sketch** -> **Include Library** -> **Add .ZIP Library...**
3. Chọn file ZIP bạn vừa tải về.

---

## Ví dụ Bắt đầu Nhanh

Dưới đây là một ví dụ đơn giản hiển thị tiếng Việt có dấu trên màn hình OLED SSD1306 I2C:

```cpp
#include <VietnameseOLED.h>
#include <drivers/SSD1306Adapter.h>
#include <fonts/Font_8x16_VN.h>

// Khởi tạo adapter màn hình: SSD1306 128x64 với địa chỉ mặc định I2C 0x3C
SSD1306Adapter display(128, 64, 0x3C);

// Bọc adapter màn hình vào đối tượng điều khiển VietnameseOLED
VietnameseOLED oled(display);

void setup() {
    // Khởi tạo màn hình và các phân hệ dựng hình
    oled.begin();
    
    // Xóa bộ đệm màn hình
    oled.clear();
    
    // Đặt font chữ hoạt động
    oled.setFont(Font_8x16_VN);
    
    // Đặt màu vẽ là màu trắng
    oled.setColor(VNColor::WHITE);
    
    // Vẽ dòng chữ tiếng Việt có dấu UTF-8
    oled.drawUTF8(0, 0, "Xin chào Việt Nam!");
    oled.drawUTF8(0, 20, "Chào mừng bạn!");
    
    // Đẩy dữ liệu từ bộ đệm lên phần cứng màn hình
    oled.update();
}

void loop() {
    // Không làm gì cả
}
```

---

## Chỉ mục Tài liệu

Để tìm hiểu sâu hơn về cấu hình, kiến trúc và hướng dẫn tích hợp chi tiết, vui lòng tham khảo các tài liệu trong thư mục [docs/](file:///c:/Users/Admin/Desktop/VietnameseOLED/docs):

- [API Reference](file:///c:/Users/Admin/Desktop/VietnameseOLED/docs/API_Reference.md): Đặc tả chi tiết các class và method C++ public.
- [Architecture](file:///c:/Users/Admin/Desktop/VietnameseOLED/docs/Architecture.md): Giải thích thiết kế kiến trúc và cách thức điều phối giữa các module.
- [Migration Guide](file:///c:/Users/Admin/Desktop/VietnameseOLED/docs/MigrationGuide.md): Hướng dẫn chuyển đổi từ Adafruit GFX, U8g2 hoặc các thư viện cũ hơn.
- [Performance & Optimization](file:///c:/Users/Admin/Desktop/VietnameseOLED/docs/Performance.md): Thông tin chi tiết về tỷ lệ cache hit, tối ưu flash và cách sử dụng SPI phần cứng.
- [Troubleshooting](file:///c:/Users/Admin/Desktop/VietnameseOLED/docs/Troubleshooting.md): Khắc phục các lỗi phổ biến về I2C, SPI và lỗi hiển thị font.
- [FAQ](file:///c:/Users/Admin/Desktop/VietnameseOLED/docs/FAQ.md): Câu hỏi thường gặp về vi điều khiển, màn hình grayscale và mức sử dụng bộ nhớ.

---

## Tác giả & Hỗ trợ

Thư viện được phát triển và bảo trì bởi **CÔNG TY TNHH CÔNG NGHỆ CHIPXA (CHIPXA TECHNOLOGY CO., LTD.)**.

* **Trụ sở:** Đông Lũ, Hưng Đạo, Hà Nội
* **Mã số thuế (MST):** 0111399950
* **Hotline:** +(84) 971 399 950 *(Zalo / WhatsApp / WeChat)*
* **Hỗ trợ kỹ thuật:** +(84) 928 000 084 *(Zalo / WhatsApp / WeChat)*
* **Email:** [info@chipxa.vn](mailto:info@chipxa.vn)
* **Website:** [chipxa.vn](https://chipxa.vn)

---

## Giấy phép (License)

Thư viện này phát hành dưới giấy phép **MIT License**. Xem thêm chi tiết tại file [LICENSE](file:///c:/Users/Admin/Desktop/VietnameseOLED/LICENSE).

---

© CÔNG TY TNHH CÔNG NGHỆ CHIPXA. Bảo lưu mọi quyền.
