#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>

// Thông tin WiFi
const char* ssid = "WIFI SINH VIEN";
const char* password = "";

// Địa chỉ server Flask
const char* serverUrl = "http://172.16.21.94:5000/upload";

// Cấu hình chân camera cho board AI-Thinker
void startCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = 5;   config.pin_d1 = 18;
  config.pin_d2 = 19;  config.pin_d3 = 21;
  config.pin_d4 = 36;  config.pin_d5 = 39;
  config.pin_d6 = 34;  config.pin_d7 = 35;  
  config.pin_xclk = 0;
  config.pin_pclk = 22;
  config.pin_vsync = 25;
  config.pin_href = 23;
  config.pin_sscb_sda = 26;
  config.pin_sscb_scl = 27;
  config.pin_pwdn = 32;
  config.pin_reset = -1;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // Độ phân giải và chất lượng ảnh
  config.frame_size = FRAMESIZE_VGA;   // 640x480
  config.jpeg_quality = 9;
  config.fb_count = 2;

  // Khởi tạo camera
  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("❌ Camera init failed");
    return;
  }
}

void setup() {
  Serial.begin(115200);

  // Kết nối WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  } 
  Serial.println("\n✅ WiFi connected");
  Serial.println("📶 IP: " + WiFi.localIP().toString());

  // Cấu hình flash LED (GPIO 4)
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW); // Tắt đèn ban đầu

  // Khởi động camera
  startCamera();
}

void loop() {
  // Bật flash
  digitalWrite(4, HIGH);
  delay(200);  // Chờ ánh sáng ổn định

  // Chụp ảnh
  camera_fb_t * fb = esp_camera_fb_get();
  digitalWrite(4, LOW);  // Tắt flash ngay sau khi chụp

  if (!fb) {
    Serial.println("❌ Capture failed");
    return;
  }

  if (fb->format != PIXFORMAT_JPEG) {
    Serial.println("⚠️ Ảnh không phải JPEG!");
    esp_camera_fb_return(fb);
    return;
  }

  // Gửi ảnh đến server Flask
  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/octet-stream");

  int res = http.POST(fb->buf, fb->len);
  Serial.println("📤 Server response: " + String(res));

  http.end();
  esp_camera_fb_return(fb);

  // Chờ 4 giây trước khi chụp tiếp
  delay(4000);
}
