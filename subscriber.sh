#!/bin/sh

# Usage example: ./subscriber.sh v1/devices/me/telemetry A1_TEST_TOKEN client1

topic=$1
access=$2
client=$3

mosquitto_sub -d -q 1 -h 0.0.0.0 -p 1883 -t "$topic" -u "$access"  -i "$client"
