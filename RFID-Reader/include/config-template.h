#pragma once

#define SS_PIN                  5
#define RST_PIN                 0

#define WIFI_SSID               "ssid"
#define WIFI_PASSWORD           "password"
#define WIFI_RETRY_INTERVAL     1000                      // milliseconds

#define MQTT_SERVER             "mgtt_server"
#define MQTT_SERVER_PORT        1883
#define MQTT_CLIENT_ID          "clientID"                // example: ESP32-room1
#define MQTT_TELEMETRY_TOPIC    "topic"                   // example: room1/motions
#define MQTT_PING_TOPIC         "ping"                    // example: pings
#define MQTT_RETRY_INTERVAL     5000                      // milliseconds
#define MQTT_PING_INTERVAL      50000                     // milliseconds
#define MQTT_PUBLISH_MAX_TRIES  12

#define CET_OFFSET              3600
#define DEVICE_ACCESS_TOKEN     "device_access_token"
#define ROOM_ID                 "room-1"