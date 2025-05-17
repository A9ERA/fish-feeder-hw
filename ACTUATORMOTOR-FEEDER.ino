int ENA = 11;  // PWM ควบคุมความเร็ว
int IN1 = 12;  // ควบคุมทิศทาง
int IN2 = 13;  // ควบคุมทิศทาง

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  Serial.begin(9600);
  Serial.println("กด 1 = ดันออก | 2 = ดึงกลับ | 0 = หยุด");
}

void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();

    if (cmd == '1') {
      digitalWrite(IN1, HIGH);     // ดันออก
      digitalWrite(IN2, LOW);
      analogWrite(ENA, 255);       // ความเร็วสูงสุด
      Serial.println("ดันออก");
    }
    else if (cmd == '2') {
      digitalWrite(IN1, LOW);      // ดึงกลับ
      digitalWrite(IN2, HIGH);
      analogWrite(ENA, 255);
      Serial.println("ดึงกลับ");
    }
    else if (cmd == '0') {
      analogWrite(ENA, 0);         // หยุด
      Serial.println("หยุด");
    }
  }
}
