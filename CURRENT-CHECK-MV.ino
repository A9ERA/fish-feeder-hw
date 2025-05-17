void setup() {
  Serial.begin(9600);
  Serial.println("📟 เริ่มตรวจสอบค่าแรงดันจากเซ็นเซอร์กระแส...");
}

void loop() {
  int raw = analogRead(A0);  // อ่านค่าจากเซ็นเซอร์
  double voltage_mV = (raw / 1024.0) * 5000.0;  // แปลงเป็น mV (สำหรับ Arduino 5V)

  Serial.print("📈 analogRead: ");
  Serial.print(raw);
  Serial.print(" | 🔌 Voltage: ");
  Serial.print(voltage_mV, 2);
  Serial.println(" mV");

  delay(1000);
}
