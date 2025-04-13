#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6fL12pH69"
#define BLYNK_TEMPLATE_NAME "Quickstart Device"
 // Default heartbeat interval for GSM is 60
 // #define BLYNK_HEARTBEAT 30
#define SIM800L_IP5306_VERSION_20200811
#define TINY_GSM_MODEM_SIM800

#include "utilities.h"
#include <TinyGsmClient.h>
#include <BlynkSimpleTinyGSM.h>
#include <DHT.h>

// ตั้ง Serial และ DHT
#define SerialMon Serial
#define SerialAT Serial1
#define TINY_GSM_DEBUG SerialMon
#define GSM_PIN ""
#define DHTPIN 26
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// กำหนดค่า Blynk และ SIM
const char apn[] = "internet";
const char gprsUser[] = "";
const char gprsPass[] = "";
const char auth[] = "U8Be_VVoGLdzkvRZAzPAWCloOvIag_nN";

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);

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

  dht.begin(); // เริ่มต้นเซนเซอร์ DHT
  Blynk.begin(auth, modem, apn, gprsUser, gprsPass); // เริ่มเชื่อมต่อ Blynk ผ่าน GPRS
}

void loop()
{
  Blynk.run();
  sendDHT();
  delay(5000);
}
