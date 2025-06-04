# Hệ Thống Đọc Công Tơ Điện Từ Xa

Đây là hệ thống đọc công tơ điện từ xa sử dụng **ESP32-CAM**, **ESP8266**, kết hợp với xử lý ảnh và OCR để tự động thu thập chỉ số điện từ công tơ cơ khí. 
Hệ thống giúp giảm công sức ghi chỉ số thủ công, tăng tính chính xác và có thể tích hợp với hệ thống IoT giám sát năng lượng.

---

## Thành phần hệ thống

### 1. Phần cứng
- **ESP32-CAM**: Dùng để chụp ảnh công tơ điện.
- **ESP8266**: Kết nối Wi-Fi, truyền ảnh về server xử lý.
- **Nguồn điện ổn định**: 5V cho ESP32-CAM và ESP8266.

### 2. Phần mềm
- **Python Flask Backend**: Nhận ảnh và xử lý OCR.
- **OpenCV**: Xử lý ảnh, cắt vùng số.
- **Tesseract OCR**: Đọc ký tự từ ảnh số công tơ.
- **HTML Template**: Giao diện upload và hiển thị ảnh/giá trị đọc được.

## Sơ đồ hoạt động

1. ESP32-CAM chụp ảnh công tơ điện định kỳ.
2. ESP8266 gửi ảnh qua HTTP POST về Flask Server.
3. Server nhận ảnh, xử lý (resize, crop ROI, threshold).
4. Sử dụng **Tesseract OCR** để đọc số điện.
5. Trả về kết quả chỉ số điện hiển thị trên giao diện web.
---

## Cấu trúc thư mục
![image](https://github.com/user-attachments/assets/40767076-b196-49fc-9f49-a201e981b90e)

## 1 số hình ảnh của hệ thống 
![image](https://github.com/user-attachments/assets/e9c86c16-a02e-4e8a-8454-317c38db3345)
![image](https://github.com/user-attachments/assets/a77505af-9f22-476e-a133-0d25e3b6f589)
![image](https://github.com/user-attachments/assets/e4adf229-2c0b-4816-abc5-cfefa18e52aa)
