#include <Arduino.h>
#include <ETH.h>
#include <SPI.h>
#include <WebServer.h>

#include "config.h"

static bool ethConnected = false;
static WebServer server(HTTP_PORT);

static void onNetworkEvent(arduino_event_id_t event, arduino_event_info_t info) {
  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      Serial.println("ETH started");
      ETH.setHostname(HOSTNAME);
      break;
    case ARDUINO_EVENT_ETH_CONNECTED:
      Serial.println("ETH link up");
      break;
    case ARDUINO_EVENT_ETH_GOT_IP:
      Serial.printf("ETH got IP: %s\n", ETH.localIP().toString().c_str());
      Serial.println(ETH);
      ethConnected = true;
      break;
    case ARDUINO_EVENT_ETH_LOST_IP:
      Serial.println("ETH lost IP");
      ethConnected = false;
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      Serial.println("ETH link down");
      ethConnected = false;
      break;
    case ARDUINO_EVENT_ETH_STOP:
      Serial.println("ETH stopped");
      ethConnected = false;
      break;
    default:
      break;
  }
}

static void handleRoot() {
  String body = "ESP32 + W5500\n";
  body += "IP:     " + ETH.localIP().toString() + "\n";
  body += "MAC:    " + ETH.macAddress() + "\n";
  body += "Uptime: " + String(millis() / 1000) + " s\n";
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

  SPI.begin(ETH_SPI_SCK, ETH_SPI_MISO, ETH_SPI_MOSI);
  if (!ETH.begin(ETH_PHY_W5500, ETH_PHY_ADDR, ETH_PHY_CS, ETH_PHY_IRQ, ETH_PHY_RST, SPI)) {
    Serial.println("ETH.begin() failed - check W5500 wiring");
  }

  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();
}
