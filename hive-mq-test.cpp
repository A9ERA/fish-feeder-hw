#include <TinyGsmClient.h>
#include <PubSubClient.h>

// ใส่ APN ของเครือข่ายมือถือของคุณ (AIS/TRUE/DTAC)
const char apn[] = "internet";
const char user[] = "";
const char pass[] = "";

// กำหนดขา Serial ของโมดูล SIM800L หรือ SIM7600
#define MODEM_RX 26
#define MODEM_TX 27
#define MODEM_RST 5

#define BAUD_RATE 9600

HardwareSerial SerialAT(1);
TinyGsm modem(SerialAT);
TinyGsmClient gsmClient;
PubSubClient mqtt(gsmClient);

const char* mqtt_server = "bb158e5dcd76426d9cab5ae648dcfbaa.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_topic_pub = "esp32/sim800/message";
const char* mqtt_topic_sub = "esp32/sim800/control";

void mqttCallback(char* topic, byte* payload, unsigned int len) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  for (int i = 0; i < len; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  delay(10);

  SerialAT.begin(BAUD_RATE, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(3000);

  Serial.println("Initializing modem...");
  modem.restart();
  delay(1000);

  Serial.print("Connecting to GPRS...");
  if (!modem.gprsConnect(apn, user, pass)) {
    Serial.println(" fail");
    while (true);
  }
  Serial.println(" success");

  // ตั้งค่า MQTT
  mqtt.setServer(mqtt_server, mqtt_port);
  mqtt.setCallback(mqttCallback);

  // เชื่อมต่อ MQTT
  connectMQTT();
}

void connectMQTT() {
  while (!mqtt.connected()) {
    Serial.print("Connecting to MQTT...");
    if (mqtt.connect("ESP32Client_SIM")) {
      Serial.println("connected");
      mqtt.subscribe(mqtt_topic_sub); // Subscribe เพื่อรับคำสั่ง
    } else {
      Serial.print(" failed, rc=");
      Serial.print(mqtt.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  if (!mqtt.connected()) {
    connectMQTT();
  }

  mqtt.loop();

  // ทดสอบส่งข้อความทุก 10 วินาที
  static unsigned long lastSend = 0;
  if (millis() - lastSend > 10000) {
    String msg = "HELLO from ESP32 via SIM!";
    mqtt.publish(mqtt_topic_pub, msg.c_str());
    Serial.println("Published: " + msg);
    lastSend = millis();
  }
}
