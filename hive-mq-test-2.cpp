// ไม่ต้อง include SoftwareSerial อีก
// ใช้ HardwareSerial แทน

#define MODEM_RX 27
#define MODEM_TX 26

HardwareSerial sim800(1);  // ใช้ Serial1

void setup() {
  Serial.begin(115200);
  sim800.begin(9600, SERIAL_8N1, MODEM_RX, MODEM_TX); // ใช้ TX=26, RX=27

  delay(3000);
  Serial.println("เริ่มต้นการเชื่อมต่อ SIM800...");

  sendAT("AT");
  sendAT("ATE0");
  sendAT("AT+CPIN?");
  sendAT("AT+CSQ");
  sendAT("AT+CREG?");
  sendAT("AT+CGATT=1");

  sendAT("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
  sendAT("AT+SAPBR=3,1,\"APN\",\"internet\""); // แก้ตามซิม: internet / www.dtac.co.th ฯลฯ
  sendAT("AT+SAPBR=1,1");
  delay(3000);
  sendAT("AT+SAPBR=2,1");

  Serial.println("✅ GPRS พร้อมใช้งานแล้ว!");
}

void loop() {
  // ยังไม่ส่งข้อมูล
}

void sendAT(String command) {
  Serial.println("ส่ง: " + command);
  sim800.println(command);
  delay(500);
  while (sim800.available()) {
    String response = sim800.readStringUntil('\n');
    response.trim();
    if (response.length() > 0) {
      Serial.println("↪ " + response);
    }
  }
}
