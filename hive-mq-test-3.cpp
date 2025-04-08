#define MODEM_RX 27
#define MODEM_TX 26

HardwareSerial sim800(1);

void setup() {
  Serial.begin(115200);
  sim800.begin(9600, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(3000);
  Serial.println("เริ่มต้นการเชื่อมต่อ SIM800...");

  sendAT("AT");
  sendAT("ATE0");
  sendAT("AT+CPIN?");
  sendAT("AT+CSQ");
  sendAT("AT+CREG?");
  sendAT("AT+CGATT=1");
  sendAT("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
  sendAT("AT+SAPBR=3,1,\"APN\",\"internet\"");  // เปลี่ยน APN ตามซิม
  sendAT("AT+SAPBR=1,1");
  delay(3000);
  sendAT("AT+SAPBR=2,1");

  Serial.println("GPRS พร้อมใช้งานแล้ว");
  sendHTTP();
}

void loop() {
  delay(120000);
  sendHTTP();
}

void sendHTTP() {
  String payload = "{\"temp\":25.0,\"hum\":60.0}";
  Serial.println("กำลังส่งข้อมูล: " + payload);

  sendAT("AT+HTTPINIT");
  sendAT("AT+HTTPPARA=\"CID\",1");
  sendAT("AT+HTTPPARA=\"URL\",\"http://9465-2001-fb1-14f-9d4a-35a9-5380-c80a-34fa.ngrok-free.app/webhook\"");  // เปลี่ยน URL
  sendAT("AT+HTTPPARA=\"CONTENT\",\"application/json\"");
  sendAT("AT+HTTPDATA=" + String(payload.length()) + ",10000");
  delay(500);
  sim800.print(payload);
  delay(1000);
  sendAT("AT+HTTPACTION=1");
  delay(5000);
  sendAT("AT+HTTPREAD");
  sendAT("AT+HTTPTERM");
}

void sendAT(String command) {
  Serial.println("ส่ง: " + command);
  sim800.println(command);
  delay(500);
  while (sim800.available()) {
    String response = sim800.readStringUntil('\n');
    response.trim();
    if (response.length() > 0) {
      Serial.println(response);
    }
  }
}
