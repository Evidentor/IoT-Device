#include <SPI.h>
#include <MFRC522.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <time.h>
#include <WiFiUdp.h>
#include "config.h"

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
MFRC522 mfrc522(SS_PIN, RST_PIN);

void connectToWiFi(const char* ssid, const char* password, int retryInterval, bool reconnecting) {
  Serial.printf("%s to WiFi %s", reconnecting ? "Reconnecting" : "Connecting", ssid);

  // Try to connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(WIFI_RETRY_INTERVAL);
    Serial.print(".");
  }
  Serial.print("\nWiFi connected!\n");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void connectToMQTT(const char* clientId, int retryInterval, bool reconnecting) {
  while (!mqttClient.connected()) {
    Serial.printf("%s to MQTT (%s:%d) ...\n", reconnecting ? "Reconnecting" : "Connecting", MQTT_SERVER, MQTT_SERVER_PORT);

    // Try to connect to MQTT
    if (mqttClient.connect(clientId, DEVICE_ACCESS_TOKEN, NULL)) {
      Serial.println("Connected to MQTT!");
    } else {
      Serial.printf("Failed (status code = %d)\n", mqttClient.state());
      Serial.printf("Try again in %d milliseconds.\n", retryInterval);
      delay(retryInterval);
    }
  }
}

String nuidToString(byte *nuid) {
  String nuidStr = "";
  for (byte i = 0; i < 4; i++) {
    nuidStr += String(nuid[i]);
    if (i < 3) {
      nuidStr += "-";
    }
  }
  return nuidStr;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(100);
  }

  // WiFi Setup
  connectToWiFi(WIFI_SSID, WIFI_PASSWORD, WIFI_RETRY_INTERVAL, false);

  // Setup MQTT
  mqttClient.setServer(MQTT_SERVER, MQTT_SERVER_PORT);
  connectToMQTT(MQTT_CLIENT_ID, MQTT_RETRY_INTERVAL, false);

  SPI.begin();  // Init SPI bus
  mfrc522.PCD_Init();  // Init MFRC522

  Serial.println("RC522 Initialized. Bring a card near...");
}

void loop() {
  // Check WiFi
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi(WIFI_SSID, WIFI_PASSWORD, WIFI_RETRY_INTERVAL, true);
  }

  // Check MQTT; 0 - connected
  if (mqttClient.connected() != 0) {
    connectToMQTT(MQTT_CLIENT_ID, MQTT_RETRY_INTERVAL, true);
  }

  // Check for a new card
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.print("Card UID: ");

    for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i], HEX);
        Serial.print(" ");
    }

    String nuidStr = nuidToString(mfrc522.uid.uidByte);

    StaticJsonDocument<300> JSONbuffer;
    JsonObject JSONencoder = JSONbuffer.to<JsonObject>();

    JSONencoder["cardId"] = nuidStr;
    JSONencoder["roomId"] = ROOM_ID;

    char JSONmessageBuffer[200];
    serializeJson(JSONencoder, JSONmessageBuffer);
    Serial.println("Sending message to MQTT topic..");
    Serial.println(JSONmessageBuffer);

    if (mqttClient.publish(MQTT_TELEMETRY_TOPIC, JSONmessageBuffer)) {
      Serial.println("Success sending message");
    } else {
      Serial.println("Error sending message");
    }

    Serial.println();
    mfrc522.PICC_HaltA();
  }
}
