#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "WIFI SINH VIEN";
const char* password = "";

// Server URL (kiểm tra địa chỉ IP + PORT chính xác từ Flask)
const char* serverUrl = "http://172.16.21.94:5000/data"; 

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("🔌 Đang kết nối WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ Kết nối WiFi thành công!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    http.begin(client, serverUrl);
    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<200> jsonDoc;
    jsonDoc["status"] = "OK";
    jsonDoc["voltage"] = 220;
    jsonDoc["current"] = 1.5;
    jsonDoc["alert"] = false;

    String requestBody;
    serializeJson(jsonDoc, requestBody);

    int httpCode = http.POST(requestBody);
    if (httpCode > 0) {
      String payload = http.getString();
      Serial.printf("📨 Gửi thành công! Mã phản hồi: %d\n", httpCode);
      Serial.println("📥 Phản hồi từ server: " + payload);
    } else {
      Serial.printf("❌ Gửi thất bại. Lỗi: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  } else {
    Serial.println("🚫 Mất kết nối WiFi.");
  }

  delay(4000);  // gửi mỗi 4 giây
}
