#pragma once

// W5500 wiring on a WEMOS LOLIN S2 Mini (ESP32-S2).
// Adjust to match your hardware.
#define ETH_PHY_ADDR  1
#define ETH_SPI_SCK   36
#define ETH_SPI_MISO  37
#define ETH_SPI_MOSI  35
#define ETH_PHY_CS    34
#define ETH_PHY_IRQ   33   // set to -1 if INT is not connected
#define ETH_PHY_RST   -1  // set to a GPIO if RST is connected

#define HOSTNAME      "esp32-w5500"
#define HTTP_PORT     80

// Local overrides (MQTT credentials etc.) live in include/secrets.h, which is
// not committed. Copy include/secrets.example.h to get started.
#if __has_include("secrets.h")
#include "secrets.h"
#endif

// MQTT broker. Leave MQTT_HOST empty to disable MQTT, and MQTT_USER empty to
// connect without authentication. Plain MQTT only (no TLS).
#ifndef MQTT_HOST
#define MQTT_HOST     ""
#endif
#ifndef MQTT_PORT
#define MQTT_PORT     1883
#endif
#ifndef MQTT_USER
#define MQTT_USER     ""
#endif
#ifndef MQTT_PASS
#define MQTT_PASS     ""
#endif

// Tasmota-style topics: tele/<MQTT_TOPIC>/<suffix>, e.g.
// tele/esp32-w5500/SENSOR and tele/esp32-w5500/LWT.
#ifndef MQTT_TOPIC
#define MQTT_TOPIC    HOSTNAME
#endif
#ifndef MQTT_TELEPERIOD
#define MQTT_TELEPERIOD 60  // seconds between SENSOR messages
#endif

// NTP is used for the "Time" field in telemetry. TIMEZONE is a POSIX TZ
// string; the default is Europe/Brussels (CET/CEST).
#ifndef NTP_SERVER
#define NTP_SERVER    "pool.ntp.org"
#endif
#ifndef TIMEZONE
#define TIMEZONE      "CET-1CEST,M3.5.0,M10.5.0/3"
#endif
