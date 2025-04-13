#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6fL12pH69"
#define BLYNK_TEMPLATE_NAME "Quickstart Device"

// เลือกเวอร์ชันของ SIM800L ที่ใช้งาน
#define SIM800L_IP5306_VERSION_20200811
#define TINY_GSM_MODEM_SIM800

#include "utilities.h"
#include <TinyGsmClient.h>
#include <BlynkSimpleTinyGSM.h>
#include <DHT.h>

// ตั้ง Serial และ DHT
#define SerialMon Serial
#define SerialAT Serial1
#define DHTPIN 26
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// กำหนดค่า Blynk และ SIM
const char auth[] = "U8Be_VVoGLdzkvRZAzPAWCloOvIag_nN";
const char apn[]  = "internet";
const char gprsUser[] = "";
const char gprsPass[] = "";
const char GSM_PIN[] = "";

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
BlynkTimer timer;

// อ่านและส่งค่าเซนเซอร์
void sendDHT()
{
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (isnan(t) || isnan(h)) {
    SerialMon.println("❌ Failed to read from DHT22");
    return;
  }

  SerialMon.print("🌡 Temp: ");
  SerialMon.print(t);
  SerialMon.print("°C | 💧 Humidity: ");
  SerialMon.print(h);
  SerialMon.println("%");

  Blynk.virtualWrite(V4, t);
  Blynk.virtualWrite(V5, h);
}

void setup()
{
  SerialMon.begin(115200);
  delay(10);

  dht.begin(); // เริ่มต้นเซนเซอร์ DHT

  setupModem();

  SerialMon.println("Wait...");

  // เริ่มต้นการสื่อสารกับ SIM800L
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(6000);

  SerialMon.println("Initializing modem...");
  modem.restart(); // หรือใช้ modem.init();

  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);

  // เริ่มเชื่อมต่อ Blynk ผ่าน GPRS
  Blynk.begin(auth, modem, apn, gprsUser, gprsPass);

  // ตั้งเวลาอ่านค่า DHT ทุก 5 วินาที
  timer.setInterval(5000L, sendDHT);
}

void loop()
{
  Blynk.run();
  timer.run();
}
