#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6fL12pH69"
#define BLYNK_TEMPLATE_NAME "Quickstart Device"
 // Default heartbeat interval for GSM is 60
 // #define BLYNK_HEARTBEAT 30
 
 // Please select the corresponding model
 
 // #define SIM800L_IP5306_VERSION_20190610
 // #define SIM800L_AXP192_VERSION_20200327
 // #define SIM800C_AXP192_VERSION_20200609
#define SIM800L_IP5306_VERSION_20200811
#define TINY_GSM_MODEM_SIM800

#include "utilities.h"
#include <TinyGsmClient.h>
#include <BlynkSimpleTinyGSM.h>
#include <DHT.h>

 // Select your modem:
 
 // Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial
 
 // Set serial for AT commands (to the module)
 // Use Hardware Serial on Mega, Leonardo, Micro
#define SerialAT Serial1
 
 // See all AT commands, if wanted
 //#define DUMP_AT_COMMANDS
 
 // Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon
 
 // set GSM PIN, if any
#define GSM_PIN ""
#define DHTPIN 26
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

 // Your GPRS credentials, if any
const char apn[] = "internet";
const char gprsUser[] = "";
const char gprsPass[] = "";
const char auth[] = "U8Be_VVoGLdzkvRZAzPAWCloOvIag_nN";


#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif
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
 // Set console baud rate
  SerialMon.begin(115200);
  delay(10);

  setupModem();

  SerialMon.println("Wait...");

  // Set GSM module baud rate and UART pins
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(6000);

  SerialMon.println("Initializing modem...");
  modem.restart();
    // modem.init();

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
