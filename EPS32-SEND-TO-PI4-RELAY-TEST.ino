#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

// === WiFi & MQTT Config ===
const char* ssid = "B65IEE02";
const char* password = "123456789";
const char* mqtt_server = "192.168.0.5";

WiFiClient espClient;
PubSubClient client(espClient);

// === DHT Config ===
#define DHTPIN 26
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// === Soil Moisture Config ===
#define SOIL_PIN 39

// === Relay Config ===
#define RELAY_IN1 19
#define RELAY_IN2 21

// === ฟังก์ชันรับคำสั่งจาก MQTT ===
void callback(char* topic, byte* payload, unsigned int length) {
  // ตรวจสอบว่า topic ตรงกับ "server/message" เท่านั้น
  // if (String(topic) != "server/message") return;
  Serial.println(topic);

  payload[length] = '\0'; // แปลง payload เป็น string
  String msg = String((char*)payload);
  Serial.printf("📥 Received command from topic [%s]: %s\n", topic, msg.c_str());

  if (msg == "on_in1") {
    digitalWrite(RELAY_IN1, HIGH);
    Serial.println("✅ เปิดรีเลย์ IN1");
  } else if (msg == "on_in2") {
    digitalWrite(RELAY_IN2, HIGH);
    Serial.println("✅ เปิดรีเลย์ IN2");
  } else if (msg == "off_all") {
    digitalWrite(RELAY_IN1, LOW);
    digitalWrite(RELAY_IN2, LOW);
    Serial.println("✅ ปิดรีเลย์ IN1 และ IN2");
  }
}

void setup_wifi() {
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  int retry = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (++retry > 20) {
      Serial.println("\nFailed to connect to WiFi");
      return;
    }
  }
  Serial.println("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      client.subscribe("esp32/control");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(RELAY_IN1, OUTPUT);
  pinMode(RELAY_IN2, OUTPUT);
  digitalWrite(RELAY_IN1, LOW);
  digitalWrite(RELAY_IN2, LOW);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // === อ่านค่า DHT22 ===
  // float temp = dht.readTemperature();
  // float hum = dht.readHumidity();

  // === อ่านค่า Soil Moisture ===
  // int soilRaw = analogRead(SOIL_PIN);
  // float soilPercent = map(soilRaw, 3000, 1000, 0, 100);
  // soilPercent = constrain(soilPercent, 0, 100);

  // === สร้าง JSON ข้อมูล
  // String payload = "{";
  // payload += "\"temperature\":" + String(temp, 1) + ",";
  // payload += "\"humidity\":" + String(hum, 1) + ",";
  // payload += "\"soil\":" + String(soilPercent, 0);
  // payload += "}";

  // === ส่ง MQTT
  // Serial.print("Publishing sensor data: ");
  // Serial.println(payload);
  // client.publish("sensor/data", payload.c_str());

  // delay(3000);
}
