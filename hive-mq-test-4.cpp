#define TINY_GSM_MODEM_SIM800
#define TINY_GSM_RX_BUFFER 1024

// TTGO T-Call pins
#define MODEM_RST            5
#define MODEM_PWKEY          4
#define MODEM_POWER_ON       23
#define MODEM_TX             27
#define MODEM_RX             26
#define I2C_SDA              21
#define I2C_SCL              22

// Required Libraries
#include <TinyGSM.h>
#include <TinyGsmClient.h>
#include <PubSubClient.h>
#include <Wire.h>

// Serial
#define SerialMon Serial
#define SerialAT Serial1

// GPRS Settings
const char apn[]      = "internet";
const char gprsUser[] = "";
const char gprsPass[] = "";
const char simPIN[]   = "";

// MQTT Settings (HiveMQ Public Broker)
const char* broker = "broker.hivemq.com";
const int mqttPort = 1883;

// Topics
const char* topicSub1 = "labubu/attack";
const char* topicSub2 = "labubu/back";
const char* topicPub = "labubu/laugh";

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
PubSubClient mqtt(client);

TwoWire I2CPower = TwoWire(0);
#define IP5306_ADDR          0x75
#define IP5306_REG_SYS_CTL0  0x00

bool setPowerBoostKeepOn(int en) {
  I2CPower.beginTransmission(IP5306_ADDR);
  I2CPower.write(IP5306_REG_SYS_CTL0);
  I2CPower.write(en ? 0x37 : 0x35);
  return I2CPower.endTransmission() == 0;
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  SerialMon.print("Message arrived [");
  SerialMon.print(topic);
  SerialMon.print("] ");
  for (int i = 0; i < length; i++) {
    SerialMon.print((char)payload[i]);
  }
  SerialMon.println();
}

void mqttConnect() {
  while (!mqtt.connected()) {
    SerialMon.print("Connecting to MQTT...");
    if (mqtt.connect("ESP32Client")) {
      SerialMon.println("connected");
      mqtt.subscribe(topicSub1);
      mqtt.subscribe(topicSub2);
    } else {
      SerialMon.print("failed, rc=");
      SerialMon.print(mqtt.state());
      SerialMon.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  SerialMon.begin(115200);
  delay(10);

  I2CPower.begin(I2C_SDA, I2C_SCL, 400000);
  bool isOk = setPowerBoostKeepOn(1);
  SerialMon.println(String("IP5306 KeepOn ") + (isOk ? "OK" : "FAIL"));

  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);
  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);

  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(3000);

  SerialMon.println("Initializing modem...");
  modem.restart();
  if (strlen(simPIN)) modem.simUnlock(simPIN);

  SerialMon.print("Connecting to APN: ");
  SerialMon.println(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    SerialMon.println("GPRS connect failed");
    while (true);
  }
  SerialMon.println("GPRS connected");

  mqtt.setServer(broker, mqttPort);
  mqtt.setCallback(mqttCallback);
}

unsigned long lastMsg = 0;
void loop() {
  if (!mqtt.connected()) {
    mqttConnect();
  }
  mqtt.loop();

  unsigned long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    String message = "ho ho ho";
    mqtt.publish(topicPub, message.c_str());
    SerialMon.println("Published: " + message);
  }
}
