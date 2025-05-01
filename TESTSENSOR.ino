// ✅ โค้ดสมบูรณ์: ESP32 แสดงค่าทาง Serial Monitor เท่านั้น ไม่มี Web Server
// รองรับ: DHT22, HX711, ACS712, Soil Sensor, Voltage Sensor, Relay, Motor Control

#include "DHT.h"
#include "HX711.h"

// ===== GPIO Mapping =====
#define DHTPIN      26
#define DHTTYPE     DHT22
#define SOIL_PIN    39
#define DT          32
#define SCK         27
#define CURRENT_PIN 35
#define VOLT_PIN    34
#define R_EN   12
#define L_EN   14
#define R_PWM  25
#define L_PWM  15
#define RELAY_IN1 4
#define RELAY_IN2 5

DHT dht(DHTPIN, DHTTYPE);
HX711 scale;
float scaleFactor = 2280.0;
float currentOffset = 0;

void setup() {
  Serial.begin(115200);
  dht.begin();
  scale.begin(DT, SCK);
  scale.set_scale(scaleFactor);
  scale.tare();

  pinMode(R_EN, OUTPUT);
  pinMode(L_EN, OUTPUT);
  pinMode(R_PWM, OUTPUT);
  pinMode(L_PWM, OUTPUT);
  pinMode(RELAY_IN1, OUTPUT);
  pinMode(RELAY_IN2, OUTPUT);
  digitalWrite(RELAY_IN1, LOW);
  digitalWrite(RELAY_IN2, LOW);
  stopMotor();

  // อ่าน Offset กระแส
  int rawSum = 0;
  for (int i = 0; i < 100; i++) {
    rawSum += analogRead(CURRENT_PIN);
    delay(5);
  }
  currentOffset = rawSum / 100.0 * (3.3 / 4095.0);
  Serial.printf("📐 Current Sensor Offset: %.3f V\n", currentOffset);

  Serial.println("พิมพ์คำสั่งเพื่อควบคุม:");
  Serial.println("1 = มอเตอร์ขึ้น | 2 = มอเตอร์ลง");
  Serial.println("3 = อ่าน DHT22 | 4 = Soil | 5 = Weight");
  Serial.println("6 = Voltage/Current | 7 = Calibrate Load Cell");
  Serial.println("8 = เปิดรีเลย์ IN1 | 9 = เปิดรีเลย์ IN2 | 0 = ปิดรีเลย์ทั้งสอง\n");
}

void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();
    switch (cmd) {
      case '1':
        Serial.println("▶ มอเตอร์ขึ้น 2 วินาที");
        runMotor(true); delay(2000); stopMotor();
        break;
      case '2':
        Serial.println("▶ มอเตอร์ลง 2 วินาที");
        runMotor(false); delay(2000); stopMotor();
        break;
      case '3': readDHT(); break;
      case '4': readSoil(); break;
      case '5': readWeight(); break;
      case '6': readVoltageCurrent(); break;
      case '7': calibrateLoadCell(); break;
      case '8': toggleRelay(RELAY_IN1, true, "IN1"); break;
      case '9': toggleRelay(RELAY_IN2, true, "IN2"); break;
      case '0':
        toggleRelay(RELAY_IN1, false, "IN1");
        toggleRelay(RELAY_IN2, false, "IN2");
        break;
    }
  }
}

void runMotor(bool up) {
  digitalWrite(R_EN, up);
  digitalWrite(L_EN, !up);
  analogWrite(R_PWM, up ? 200 : 0);
  analogWrite(L_PWM, up ? 0 : 200);
}

void stopMotor() {
  digitalWrite(R_EN, LOW);
  digitalWrite(L_EN, LOW);
  analogWrite(R_PWM, 0);
  analogWrite(L_PWM, 0);
}

void readDHT() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  Serial.printf("🌡️ Temp: %.1f °C\t💧 Humidity: %.1f %%\n", t, h);
}

void readSoil() {
  int soilRaw = analogRead(SOIL_PIN);
  float soil = map(soilRaw, 3000, 1000, 0, 100);
  soil = constrain(soil, 0, 100);
  Serial.printf("🌱 Soil Moisture: %.0f %%\n", soil);
}

void readWeight() {
  float weight = scale.get_units(5);
  Serial.printf("⚖️ Weight: %.2f g\n", weight);
}

void calibrateLoadCell() {
  Serial.println("⏳ Calibrate... วางภาชนะเปล่า");
  delay(3000);
  scale.tare();
  Serial.println("✅ ตั้งศูนย์น้ำหนักสำเร็จ\n");
}

void readVoltageCurrent() {
  int voltRaw = analogRead(VOLT_PIN);
  float voltage = voltRaw * (3.3 / 4095.0) * 5.0;
  int currentRaw = analogRead(CURRENT_PIN);
  float currentADC = currentRaw * (3.3 / 4095.0);
  float current = (currentADC - currentOffset) / 0.066;
  Serial.printf("🔋 Voltage: %.2f V\t⚡ Current: %.2f A\n", voltage, current);
}

void toggleRelay(int pin, bool on, const char* label) {
  digitalWrite(pin, on ? HIGH : LOW);
  Serial.printf("🔌 รีเลย์ %s %s\n", label, on ? "เปิด" : "ปิด");
}
