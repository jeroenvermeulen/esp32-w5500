#pragma once

#include <Arduino.h>

// Call once after ETH.begin().
void mqttBegin();

// Call from loop(). Keeps the broker connection alive and reconnects when the
// network is up. Returns true right after a (re)connect, so the caller can
// publish fresh state immediately.
bool mqttLoop(bool networkUp);

bool mqttConnected();

// Publish to tele/<MQTT_TOPIC>/<suffix>.
bool mqttPublishTele(const char *suffix, const String &payload, bool retain = false);
