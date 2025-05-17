#define RPWM 5
#define LPWM 6

int speedLevel = 170;  // เริ่มที่ระดับกลาง
bool isRunning = false;
bool isReversed = false;

void setup() {
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);

  Serial.begin(9600);
  Serial.println("📌 คำสั่งควบคุมพัดลม:");
  Serial.println("1 = เริ่มหมุน | 2 = หยุด");
  Serial.println("3 = ปรับความเร็ว (ช้า/กลาง/เร็ว)");
  Serial.println("4 = กลับทิศทาง | 5 = หมุนปกติ");
}

void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();

    switch (cmd) {
      case '1': // เริ่มหมุน
        isRunning = true;
        runMotor();
        Serial.println("🟢 เริ่มหมุนพัดลม");
        break;

      case '2': // หยุด
        isRunning = false;
        stopMotor();
        Serial.println("⛔ หยุดพัดลม");
        break;

      case '3': // ปรับความเร็ว
        if (speedLevel == 85) {
          speedLevel = 170;
          Serial.println("⚙️ ความเร็ว: กลาง");
        } else if (speedLevel == 170) {
          speedLevel = 255;
          Serial.println("⚙️ ความเร็ว: เร็ว");
        } else {
          speedLevel = 85;
          Serial.println("⚙️ ความเร็ว: ช้า");
        }
        if (isRunning) runMotor(); // รีเซ็ตความเร็วถ้ากำลังทำงาน
        break;

      case '4': // กลับทิศทาง
        isReversed = true;
        if (isRunning) runMotor();
        Serial.println("🔁 กลับทิศทาง (CCW)");
        break;

      case '5': // ทิศทางปกติ
        isReversed = false;
        if (isRunning) runMotor();
        Serial.println("➡️ ทิศทางปกติ (CW)");
        break;
    }
  }
}

void runMotor() {
  if (isReversed) {
    analogWrite(RPWM, 0);
    analogWrite(LPWM, speedLevel);
  } else {
    analogWrite(RPWM, speedLevel);
    analogWrite(LPWM, 0);
  }
}

void stopMotor() {
  analogWrite(RPWM, 0);
  analogWrite(LPWM, 0);
}
