#include <HX711.h>
#include <EEPROM.h>

const int LOADCELL_DOUT_PIN = 20;
const int LOADCELL_SCK_PIN = 21;
const int EEPROM_SCALE_ADDR = 0;
const int EEPROM_OFFSET_ADDR = EEPROM_SCALE_ADDR + sizeof(float);

HX711 scale;
float scaleFactor = 1.0;
long offset = 0;

void setup() {
  Serial.begin(38400);
  Serial.println("📦 เริ่มต้นระบบชั่งน้ำหนัก...");

  // โหลดจาก EEPROM
  EEPROM.get(EEPROM_SCALE_ADDR, scaleFactor);
  EEPROM.get(EEPROM_OFFSET_ADDR, offset);

  Serial.print("📥 scaleFactor: ");
  Serial.println(scaleFactor, 6);
  Serial.print("📥 offset: ");
  Serial.println(offset);

  if (scaleFactor < 1.0 || scaleFactor > 100000.0) {
    Serial.println("⚠️ ค่า scaleFactor ผิดปกติ กรุณา calibrate ใหม่");
    while (true);
  }

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(scaleFactor);
  scale.set_offset(offset);  // ✅ ใช้ offset เดิม
  Serial.println("📌 เริ่มวัดน้ำหนัก...");
}

void loop() {
  float weight = scale.get_units(10); // อ่านเฉลี่ย 10 ค่า
  Serial.print("⚖️ น้ำหนัก: ");
  Serial.print(weight, 3);
  Serial.println(" kg");
  delay(1000);
}
