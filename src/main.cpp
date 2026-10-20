#include <Arduino.h>
#include <ETH.h>
#include <SPI.h>
#include <WebServer.h>
#include <time.h>

#include "config.h"
#include "mqtt.h"

static bool ethConnected = false;
static bool ethLinkUp = false;
static WebServer server(HTTP_PORT);
static uint32_t lastTele = 0;
static uint32_t lastWaitMsg = 0;

// How often to repeat the "waiting for network" message while offline.
static const uint32_t WAIT_MSG_INTERVAL_MS = 10000;

static void onNetworkEvent(arduino_event_id_t event, arduino_event_info_t info) {
  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      Serial.println("ETH started");
      ETH.setHostname(HOSTNAME);
      break;
    case ARDUINO_EVENT_ETH_CONNECTED:
      Serial.println("ETH link up, requesting IP via DHCP");
      ethLinkUp = true;
      break;
    case ARDUINO_EVENT_ETH_GOT_IP:
      Serial.printf("ETH got IP: %s\n", ETH.localIP().toString().c_str());
      Serial.println(ETH);
      Serial.printf("Starting NTP (%s)\n", NTP_SERVER);
      configTzTime(TIMEZONE, NTP_SERVER);
      ethConnected = true;
      break;
    case ARDUINO_EVENT_ETH_LOST_IP:
      Serial.println("ETH lost IP");
      ethConnected = false;
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      Serial.println("ETH link down");
      ethLinkUp = false;
      ethConnected = false;
      break;
    case ARDUINO_EVENT_ETH_STOP:
      Serial.println("ETH stopped");
      ethLinkUp = false;
      ethConnected = false;
      break;
    default:
      break;
  }
}

// ESP32 internal die temperature in degrees C. It reads well above ambient
// and is only a rough indication.
static float readChipTemperature() {
  return roundf(temperatureRead() * 10) / 10;
}

// Local time as ISO 8601 (e.g. 2026-10-20T21:30:00). Returns false until NTP
// has set the clock (it starts at 1970).
static bool formatLocalTime(char *buf, size_t len) {
  time_t now = time(nullptr);
  struct tm tm;
  if (!localtime_r(&now, &tm) || tm.tm_year + 1900 < 2020) {
    return false;
  }
  strftime(buf, len, "%Y-%m-%dT%H:%M:%S", &tm);
  return true;
}

// Tasmota-style SENSOR payload, e.g.
// {"Time":"2026-10-20T21:30:00","ESP32":{"Internal Temperature":42.1},"TempUnit":"C"}
static String buildSensorJson() {
  String json = "{";
  char buf[24];
  if (formatLocalTime(buf, sizeof(buf))) {
    json += "\"Time\":\"" + String(buf) + "\",";
  }
  json += "\"ESP32\":{\"Internal Temperature\":" + String(readChipTemperature(), 1) + "}";
  json += ",\"TempUnit\":\"C\"}";
  return json;
}

static void publishSensors() {
  String json = buildSensorJson();
  if (mqttPublishTele("SENSOR", json)) {
    Serial.printf("MQTT SENSOR %s\n", json.c_str());
  }
}

static void handleRoot() {
  String body = "ESP32 + W5500\n";
  body += "IP:     " + ETH.localIP().toString() + "\n";
  body += "MAC:    " + ETH.macAddress() + "\n";
  char timeBuf[24];
  body += "Time:   " + String(formatLocalTime(timeBuf, sizeof(timeBuf)) ? timeBuf : "not set") + "\n";
  body += "Uptime: " + String(millis() / 1000) + " s\n";
  body += "Temp:   " + String(readChipTemperature(), 1) + " C (chip)\n";
  body += "MQTT:   " + String(mqttConnected() ? "connected" : "disconnected") + "\n";
  server.send(200, "text/plain", body);
}

void setup() {
  Serial.begin(115200);
  // The S2 Mini uses native USB CDC: give the host a moment to open the port
  // so early boot messages are not lost.
  for (uint32_t start = millis(); !Serial && millis() - start < 2000;) {
    delay(10);
  }
  Serial.println("\nBooting ESP32 + W5500");

  Network.onEvent(onNetworkEvent);

  Serial.println("Starting SPI");
  SPI.begin(ETH_SPI_SCK, ETH_SPI_MISO, ETH_SPI_MOSI);
  Serial.println("Starting W5500 (ETH.begin)");
  if (!ETH.begin(ETH_PHY_W5500, ETH_PHY_ADDR, ETH_PHY_CS, ETH_PHY_IRQ, ETH_PHY_RST, SPI)) {
    Serial.println("ETH.begin() failed - check W5500 wiring");
  }

  Serial.println("Starting MQTT client");
  mqttBegin();

  Serial.printf("Starting web server on port %d\n", HTTP_PORT);
  server.on("/", handleRoot);
  server.begin();

  // Link and DHCP are handled asynchronously by onNetworkEvent(); loop()
  // reports progress while we wait.
  Serial.println("Setup done, waiting for Ethernet link");
  lastWaitMsg = millis();
}

// Link and DHCP have no timeout (they wait forever), so say periodically
// what we are still waiting for.
static void reportWaiting() {
  if (ethConnected) {
    return;
  }
  if (millis() - lastWaitMsg < WAIT_MSG_INTERVAL_MS) {
    return;
  }
  lastWaitMsg = millis();
  if (ethLinkUp) {
    Serial.println("Still waiting for an IP address from DHCP");
  } else {
    Serial.println("Still waiting for Ethernet link - is the cable plugged in?");
  }
}

void loop() {
  server.handleClient();
  reportWaiting();

  if (mqttLoop(ethConnected)) {
    // Publish right after (re)connecting, then every MQTT_TELEPERIOD seconds.
    publishSensors();
    lastTele = millis();
  } else if (mqttConnected() && millis() - lastTele >= MQTT_TELEPERIOD * 1000UL) {
    publishSensors();
    lastTele = millis();
  }
}
