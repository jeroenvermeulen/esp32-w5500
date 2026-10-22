# ESP32 + W5500 Boilerplate

> **Disclaimer:** The code in this project is AI generated, and tested by humans.
> The prompts used to generate it are listed in [PROMPTS.md](PROMPTS.md).

PlatformIO starter project for an ESP32 with a WIZnet W5500 SPI Ethernet module.
It uses the native W5500 driver in Arduino-ESP32 core 3.x (`ETH.h`), so the
standard network stack (`WiFiClient`, `HTTPClient`, `WebServer`, mDNS, …) works
over Ethernet. On boot it gets an address over DHCP and serves a small status
page on port 80. It also publishes sensor data over MQTT.

Used hardware for testing:
- WEMOS LOLIN S2 Mini clone V1.0.0 (ESP32-S2)
- W5500 Lite USR-ES1 V455

## Wiring (default)

| W5500 | S2 Mini |
|-------|---------|
| SCK   | GPIO36 |
| MISO  | GPIO37 |
| MOSI  | GPIO35 |
| CS    | GPIO34 |
| INT   | GPIO33 |
| RST   | not connected |
| 3V3   | 3V3    |
| GND   | GND    |

To change the pins, edit [include/config.h](include/config.h).

## Build & flash

```sh
pio run                    # build
pio run -t upload          # flash
pio device monitor         # serial console, 115200 baud
```

The S2 Mini has no USB-to-serial chip; it uses the ESP32-S2's native USB. If
an upload fails (or the board is fresh out of the box), put it in download mode
by holding **0**, pressing **RST**, then releasing **0**, and upload again.
After that first flash, uploads normally work without the buttons. Press **RST** after a
download-mode upload to start the firmware.

After it boots, open `http://<ip-from-serial-log>/`.

## MQTT

The firmware publishes telemetry over plain MQTT (no TLS) using
[PubSubClient](https://github.com/knolleary/pubsubclient), with Tasmota-style
topics. `<topic>` defaults to the hostname (`esp32-w5500`).

| Topic                  | Payload | Retained |
|------------------------|---------|----------|
| `tele/<topic>/LWT`     | `Online` once connected, `Offline` (sent by the broker as Last Will) when the device drops off | yes |
| `tele/<topic>/SENSOR`  | `{"Time":"2026-10-20T21:30:00","ESP32":{"Internal Temperature":42.1},"TempUnit":"C"}` | no |

SENSOR is sent right after connecting and then every `MQTT_TELEPERIOD` seconds
(60 by default). `Time` is local time from NTP (Europe/Brussels
by default) and is left out until the clock has been set. The temperature comes from the ESP32's internal sensor, so it measures the chip, not the room.

To configure the broker, copy `include/secrets.example.h` to `include/secrets.h`
(gitignored) and set `MQTT_HOST`, `MQTT_PORT`, `MQTT_USER` and `MQTT_PASS`.
You can also override `MQTT_TOPIC`, `MQTT_TELEPERIOD` and `TIMEZONE` (a POSIX TZ string) there.
MQTT stays off while `MQTT_HOST` is empty.

## ESPHome alternative

[esphome/esp32-w5500.yaml](esphome/esp32-w5500.yaml) is an example ESPHome
config that does the same as this firmware: same wiring, DHCP, a web page on
port 80, and the same Tasmota-style `LWT` and `SENSOR` MQTT messages. See
[esphome/README.md](esphome/README.md) for setup and usage.

## Setup on macOS (from scratch)

These steps assume a fresh Mac with nothing installed.

1. **Install the Xcode Command Line Tools** (provides `git` and a compiler):

   ```sh
   xcode-select --install
   ```

2. **Install [Homebrew](https://brew.sh)**, then follow the "Next steps" it
   prints to add `brew` to your `PATH`:

   ```sh
   /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
   ```

3. **Install PlatformIO Core and `jq`** (`jq` is only needed for the prompt-log
   hook, see below):

   ```sh
   brew install platformio jq
   pio --version
   ```

4. **Get the code:**

   ```sh
   git clone https://github.com/jeroenvermeulen/esp32-w5500.git
   cd esp32-w5500
   ```

5. **Optional: configure MQTT** by creating `include/secrets.h` (see
   [MQTT](#mqtt)). The firmware builds and runs without it.

   ```sh
   cp include/secrets.example.h include/secrets.h
   ```

6. **Connect the S2 Mini** with a USB-C *data* cable (charge-only cables won't
   work). No driver is needed on macOS. A fresh board must be put in download
   mode for the first upload (see below). Check that it shows up:

   ```sh
   pio device list            # look for /dev/cu.usbmodem*
   ```

7. **Build and flash** with the commands in [Build & flash](#build--flash). The
   first `pio run` downloads the ESP32 toolchain and Arduino core (several
   hundred MB), so it takes a while.

### Using VS Code instead (optional)

Install [Visual Studio Code](https://code.visualstudio.com) (or
`brew install --cask visual-studio-code`), open this folder, and accept the
recommended **PlatformIO IDE** extension. Build, Upload and Serial Monitor are
then available from the PlatformIO toolbar at the bottom of the window. The
extension installs its own copy of PlatformIO Core, so step 3 is optional if
you only use VS Code.

## AI prompt log

All AI prompts used on this project are recorded in [PROMPTS.md](PROMPTS.md).
When you work in Claude Code, a `UserPromptSubmit` hook
([.claude/hooks/log-prompt.sh](.claude/hooks/log-prompt.sh)) appends each prompt
automatically. The hook needs `jq`. Add prompts from other tools by hand.
