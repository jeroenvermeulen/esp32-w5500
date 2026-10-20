#include "mqtt.h"

#include <ETH.h>
#include <Network.h>
#include <PubSubClient.h>

#include "config.h"

static const uint32_t RECONNECT_INTERVAL_MS = 5000;

static NetworkClient netClient;
static PubSubClient mqtt(netClient);
static String clientId;
static String lwtTopic;
static uint32_t lastAttempt = 0;
static bool attempted = false;

static String teleTopic(const char *suffix) {
  return String("tele/") + MQTT_TOPIC + "/" + suffix;
}

static bool connect() {
  Serial.printf("MQTT connecting to %s:%d as %s\n", MQTT_HOST, MQTT_PORT, clientId.c_str());
  const char *user = strlen(MQTT_USER) ? MQTT_USER : nullptr;
  const char *pass = strlen(MQTT_USER) ? MQTT_PASS : nullptr;
  // Like Tasmota: the broker publishes a retained "Offline" on the LWT topic
  // when we disappear, and we overwrite it with "Online" once connected.
  if (!mqtt.connect(clientId.c_str(), user, pass, lwtTopic.c_str(), 1, true, "Offline")) {
    Serial.printf("MQTT connect failed, state %d\n", mqtt.state());
    return false;
  }
  Serial.println("MQTT connected");
  mqtt.publish(lwtTopic.c_str(), "Online", true);
  return true;
}

void mqttBegin() {
  if (!strlen(MQTT_HOST)) {
    Serial.println("MQTT disabled (MQTT_HOST not set)");
    return;
  }
  // Unique client id from the hostname plus the last 3 MAC bytes, similar to
  // Tasmota's DVES_XXXXXX.
  uint8_t mac[6];
  ETH.macAddress(mac);
  char suffix[8];
  snprintf(suffix, sizeof(suffix), "_%02X%02X%02X", mac[3], mac[4], mac[5]);
  clientId = String(HOSTNAME) + suffix;
  lwtTopic = teleTopic("LWT");

  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  mqtt.setBufferSize(512);
  mqtt.setSocketTimeout(5);
}

bool mqttLoop(bool networkUp) {
  if (!strlen(MQTT_HOST)) {
    return false;
  }
  if (mqtt.connected()) {
    mqtt.loop();
    return false;
  }
  if (!networkUp) {
    return false;
  }
  if (attempted && millis() - lastAttempt < RECONNECT_INTERVAL_MS) {
    return false;
  }
  attempted = true;
  lastAttempt = millis();
  return connect();
}

bool mqttConnected() {
  return mqtt.connected();
}

bool mqttPublishTele(const char *suffix, const String &payload, bool retain) {
  if (!mqtt.connected()) {
    return false;
  }
  return mqtt.publish(teleTopic(suffix).c_str(), payload.c_str(), retain);
}
