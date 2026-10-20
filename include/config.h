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
