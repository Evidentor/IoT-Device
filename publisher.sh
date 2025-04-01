#!/bin/sh

# Usage example: ./publisher.sh v1/devices/me/telemetry A1_TEST_TOKEN "{cardId: "47-32-68-49", room: "room-1"}"

topic=$1
access=$2
data=$3

mosquitto_pub -d -q 1 -h 0.0.0.0 -p 1883 -t "$topic" -u "$access" -m "$data"
