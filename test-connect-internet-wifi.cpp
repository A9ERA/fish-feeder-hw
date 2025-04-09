#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* serverURL = "http://f49c-2001-fb1-14f-9d4a-58b3-cdef-672e-18f8.ngrok-free.app/webhook";  // ใส่ URL ปลายทาง

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi");

  // เมื่อเชื่อมต่อแล้ว ให้ส่งค่า
  sendHello();
}

void loop() {
  // ไม่ทำอะไรใน loop
}

void sendHello() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverURL);             // เริ่มการเชื่อมต่อ
    http.addHeader("Content-Type", "application/json");  // หรือ "text/plain"

    String payload = "{\"message\":\"HELLO\"}";
    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      Serial.print("Response code: ");
      Serial.println(httpResponseCode);
      String response = http.getString();
      Serial.println("Response: " + response);
    } else {
      Serial.print("Error sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();  // ปิดการเชื่อมต่อ
  }
}
