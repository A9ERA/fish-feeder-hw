#include <HX711.h>
#include <EEPROM.h>

const int LOADCELL_DOUT_PIN = 20;
const int LOADCELL_SCK_PIN = 21;
const int EEPROM_SCALE_ADDR = 0;
const int EEPROM_OFFSET_ADDR = EEPROM_SCALE_ADDR + sizeof(float); // ต่อจาก scaleFactor

HX711 scale;

String inputString = "";
bool inputComplete = false;

float knownWeight = 0.0;
float scaleFactor = 1.0;

void setup() {
  Serial.begin(38400);
  Serial.println("📏 HX711 Calibration with EEPROM Save");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(1.0);  // ค่าเริ่มต้นสำหรับ get_value()
  scale.tare();          // ต้องแน่ใจว่าไม่มีน้ำหนักอยู่

  Serial.println("📌 วางของที่รู้น้ำหนักบน Load Cell หลัง tare()");
  Serial.println("💬 พิมพ์ค่าน้ำหนักจริง (kg) เช่น 1.0 หรือพิมพ์ 'reset' เพื่อเคลียร์ EEPROM");
}

void loop() {
  if (inputComplete) {
    inputComplete = false;
    inputString.trim();

    if (inputString.equalsIgnoreCase("reset")) {
      float zeroF = 0.0;
      long zeroL = 0;
      EEPROM.put(EEPROM_SCALE_ADDR, zeroF);
      EEPROM.put(EEPROM_OFFSET_ADDR, zeroL);
      Serial.println("🧹 ล้าง EEPROM แล้ว (scaleFactor = 0.0, offset = 0)");
    } else {
      knownWeight = inputString.toFloat();
      float reading = scale.get_value(10); // ค่าดิบ

      if (knownWeight > 0 && reading != 0) {
        scaleFactor = reading / knownWeight;
        long offset = scale.get_offset();

        EEPROM.put(EEPROM_SCALE_ADDR, scaleFactor);
        EEPROM.put(EEPROM_OFFSET_ADDR, offset);

        Serial.println("✅ Calibration สำเร็จ:");
        Serial.print("   ↪ น้ำหนักจริง: "); Serial.print(knownWeight, 3); Serial.println(" kg");
        Serial.print("   ↪ ค่าอ่านดิบ: "); Serial.println(reading, 3);
        Serial.print("   ↪ scaleFactor: "); Serial.println(scaleFactor, 6);
        Serial.print("   ↪ offset: "); Serial.println(offset);
        Serial.println("💾 บันทึกลง EEPROM เรียบร้อย");
      } else {
        Serial.println("❌ น้ำหนักต้อง > 0 และค่าดิบต้องไม่เป็น 0");
      }
    }

    inputString = "";
  }
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n' || inChar == '\r') {
      inputComplete = true;
    } else {
      inputString += inChar;
    }
  }
}
