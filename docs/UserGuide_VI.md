# HƯỚNG DẪN SỬ DỤNG CHI TIẾT - VIETNAMESEOLED

Chào mừng bạn đến với **VietnameseOLED**! Đây là tài liệu hướng dẫn từng bước một, được thiết kế đặc biệt dành cho những người mới bắt đầu từ con số 0. Chỉ cần làm theo hướng dẫn này, bạn chắc chắn sẽ hiển thị được tiếng Việt có dấu lên màn hình OLED của mình.

---

## 1. Giới thiệu sơ lược
Thư viện **VietnameseOLED** giúp hiển thị tiếng Việt có dấu chuẩn UTF-8 trực tiếp trên các màn hình OLED thông dụng (như SSD1306, SH1106, SH1107, SSD1327) bằng các vi điều khiển phổ biến như:
* Arduino UNO, Nano, Mega.
* ESP8266, ESP32.
* STM32, CH32.
* Raspberry Pi Pico (RP2040/RP2350).

---

## 2. Hướng dẫn đấu nối phần cứng (Wiring)
Màn hình OLED thông dụng nhất thường giao tiếp qua giao thức **I2C** (chỉ cần 4 dây nối: VCC, GND, SDA, SCL). Hãy kết nối các chân theo bảng dưới đây tùy thuộc vào vi điều khiển bạn đang sử dụng:

| Vi điều khiển | Chân VCC OLED | Chân GND OLED | Chân SDA OLED | Chân SCL OLED |
| :--- | :--- | :--- | :--- | :--- |
| **Arduino UNO / Nano** | 5V hoặc 3.3V | GND | **A4** | **A5** |
| **Arduino Mega 2560** | 5V hoặc 3.3V | GND | **Pin 20** | **Pin 21** |
| **ESP8266 (NodeMCU)** | 3V3 | G (GND) | **D2** (GPIO4) | **D1** (GPIO5) |
| **ESP32 (WROOM)** | 3V3 | GND | **GPIO21** | **GPIO22** |
| **STM32 Blue Pill** | 3.3 | GND | **PB7** | **PB6** |
| **Raspberry Pi Pico** | 3V3 | GND | **GP4** | **GP5** |

> [!NOTE]
> Nếu bạn sử dụng màn hình chuẩn **SPI**, hãy tham khảo thêm sơ đồ đấu nối chi tiết trong driver của màn hình tương ứng. Tuy nhiên, khuyến khích người mới bắt đầu nên sử dụng loại **I2C** vì đấu nối đơn giản nhất.

---

## 3. Cách cài đặt thư viện vào Arduino IDE

### Bước 1: Tải thư viện
Bạn có thể cài đặt thư viện bằng hai cách:
* **Cách 1 (Khuyên dùng):** Mở Arduino IDE -> Chọn menu **Sketch** -> **Include Library** -> **Manage Libraries...** Gõ tìm kiếm từ khóa `VietnameseOLED` rồi nhấn **Install**.
* **Cách 2:** Tải file `.zip` của thư viện -> Mở Arduino IDE -> Chọn **Sketch** -> **Include Library** -> **Add .ZIP Library...** -> Chọn file zip vừa tải.

### Bước 2: Bắt buộc cài đặt định dạng file UTF-8 trong Arduino IDE
Để viết được tiếng Việt trực tiếp trong code, Arduino IDE cần phải nhận diện được bảng mã UTF-8. 
* **Arduino IDE 2.x trở lên:** Mặc định đã hỗ trợ UTF-8. Bạn chỉ việc viết tiếng Việt bình thường.
* **Arduino IDE 1.8.x cũ:** Hãy chắc chắn rằng bạn đã vào **File** -> **Preferences** và bật tính năng lưu file dưới dạng UTF-8.

---

## 4. Chương trình mẫu đầu tiên (Quick Start)

Hãy copy đoạn code dưới đây, dán vào Arduino IDE và nạp thử vào board của bạn (Ví dụ này mặc định dùng cho màn hình OLED SSD1306 kích thước 128x64 giao tiếp I2C):

```cpp
#include <VietnameseOLED.h>
#include <drivers/SSD1306Adapter.h> // Driver cho màn hình SSD1306
#include <fonts/Font_8x16_VN.h>       // Font chữ tiếng Việt trung bình (dễ nhìn)

// Khởi tạo adapter màn hình kích thước 128x64, địa chỉ I2C mặc định là 0x3C
SSD1306Adapter display(128, 64, 0x3C);

// Gộp adapter vào thư viện VietnameseOLED
VietnameseOLED oled(display);

void setup() {
    // 1. Khởi động màn hình và bộ nhớ đệm
    oled.begin();
    
    // 2. Xóa sạch màn hình trước khi vẽ
    oled.clear();
    
    // 3. Cài đặt font chữ hiển thị
    oled.setFont(Font_8x16_VN);
    
    // 4. Chọn màu vẽ (Màu trắng sáng)
    oled.setColor(VNColor::WHITE);
    
    // 5. Viết chữ tại tọa độ X=0, Y=0 (Góc trên cùng bên trái)
    oled.drawUTF8(0, 0, "Xin chào Việt Nam!");
    
    // 6. Viết thêm một dòng nữa tại tọa độ X=0, Y=20 (cách dòng trên 20 pixel)
    oled.drawUTF8(0, 20, "Học lập trình nhúng");
    
    // 7. Cập nhật dữ liệu từ bộ đệm lên màn hình vật lý (BẮT BUỘC phải có lệnh này)
    oled.update();
}

void loop() {
    // Không làm gì trong vòng lặp chính
}
```

---

## 5. Giải thích các tính năng và hàm quan trọng

### 5.1. Khởi tạo và cập nhật màn hình
* `oled.begin()`: Bắt đầu giao tiếp và cấu hình màn hình.
* `oled.clear()`: Xóa sạch toàn bộ những gì đang hiển thị trên màn hình.
* `oled.update()`: Đẩy dữ liệu chữ đã vẽ lên màn hình thật. Nếu quên lệnh này, màn hình sẽ chỉ có một màu tối đen.

### 5.2. Các font chữ đi kèm thư viện
Thư viện cung cấp sẵn 2 loại font chữ tiếng Việt chất lượng cao:
1. `Font_8x16_VN`: Phông chữ cỡ vừa (mỗi ký tự rộng 8 pixel, cao 16 pixel), đây là phông chữ mặc định tốt nhất cho mọi hiển thị thông thường.
2. `Font_16x16_VN`: Phông chữ cỡ lớn, thích hợp vẽ các tiêu đề hoặc thông số nhiệt độ/độ ẩm to rõ ràng.

Cách đổi font chữ:
```cpp
#include <fonts/Font_16x16_VN.h> // Import font lớn
// ...
oled.setFont(Font_16x16_VN); // Cài đặt sử dụng font lớn
```

### 5.3. Viết chữ tự động xuống dòng (Word Wrap)
Khi bạn in một đoạn văn bản dài, nếu vượt quá độ rộng của màn hình, chữ sẽ bị tràn ra ngoài. Bạn có thể bật tính năng tự động xuống dòng:
```cpp
oled.setTextWrap(VNTextWrap::WORD); // Tự động xuống dòng khi hết hàng (cắt ở khoảng trắng giữa các từ)
oled.setCursor(0, 10);              // Đặt con trỏ tại vị trí mong muốn
oled.printVN("Đây là một đoạn văn bản rất dài tự động xuống dòng thông minh.");
```

### 5.4. Căn lề chữ (Text Alignment)
Bạn có thể căn chữ ra giữa màn hình, căn sang phải hoặc căn trái cực kỳ chuyên nghiệp:
* `oled.setAlign(VNAlign::CENTER)`: Căn giữa.
* `oled.setAlign(VNAlign::RIGHT)`: Căn phải.
* `oled.setAlign(VNAlign::LEFT)`: Căn trái.
* `oled.setAlign(VNAlign::JUSTIFY)`: Căn đều hai bên lề.

Ví dụ căn chữ ra giữa màn hình:
```cpp
oled.setAlign(VNAlign::CENTER);
oled.drawUTF8(64, 20, "Ở GIỮA"); // Tọa độ X=64 (giữa màn hình 128) làm tâm căn lề
```

### 5.5. Phóng to, xoay chữ và lật chữ (Transforms)
* **Phóng to chữ:**
  ```cpp
  oled.setScale(2); // Phóng to font chữ lên gấp 2 lần (Hỗ trợ từ 1 đến 4)
  ```
* **Xoay màn hình hiển thị:**
  ```cpp
  oled.setRotation(90);  // Xoay dọc màn hình 90 độ (Hỗ trợ 0, 90, 180, 270)
  ```
* **Lật ngược chữ:**
  ```cpp
  oled.setMirror(true, false); // Lật ngang (hiển thị gương)
  ```

---

## 6. Khắc phục các lỗi thường gặp (Troubleshooting)

### Lỗi 1: Màn hình không sáng (Màn hình tối đen)
* **Kiểm tra dây nối:** Hãy chắc chắn các chân SDA và SCL đã được cắm đúng vào các chân I2C tương ứng của board vi điều khiển.
* **Địa chỉ I2C không đúng:** Hầu hết màn hình OLED có địa chỉ I2C là `0x3C`, nhưng một số loại sử dụng `0x3D`. Hãy thử sửa dòng khai báo:
  `SSD1306Adapter display(128, 64, 0x3D);`
* **Thiếu lệnh update:** Kiểm tra xem cuối phần code vẽ đã có dòng `oled.update();` chưa.

### Lỗi 2: Chữ hiển thị bị lỗi font (Hiện dấu chấm hỏi hoặc ký tự lạ)
* **Định dạng file không đúng:** File code của bạn chưa được lưu ở định dạng mã hóa **UTF-8**. Hãy tạo một file mới hoàn toàn trong Arduino IDE phiên bản mới, viết lại chữ tiếng Việt có dấu và nạp lại.
* **Chưa set font tiếng Việt:** Hãy chắc chắn bạn đã chạy lệnh `oled.setFont(Font_8x16_VN);` trước khi in chữ.

Chúc các bạn thực hiện thành công các dự án thú vị của mình!
