# ESPHome version

> **Disclaimer:** This config is partly AI generated, and tested by humans.

[esp32-w5500.yaml](esp32-w5500.yaml) is an [ESPHome](https://esphome.io)
version of the PlatformIO firmware in the repository root:

- WEMOS LOLIN S2 Mini (ESP32-S2) with a W5500 Ethernet module, same
  [wiring](../README.md#wiring-default)
- Address over DHCP, time over NTP (Europe/Brussels)
- Status web page on port 80 (IP, MAC, time, uptime, chip temperature, status)
- Home Assistant native API (encrypted) and ESPHome OTA updates
- MQTT (plain, no TLS), with Home Assistant MQTT discovery

## MQTT topics

Tasmota-style:

| Topic                     | Payload | Retained |
|---------------------------|---------|----------|
| `tele/esp32-w5500/LWT`    | `Online` once connected, `Offline` (Last Will) when the device drops off | yes |
| `tele/esp32-w5500/SENSOR` | `{"Time":"2026-10-20T21:30:00","ESP32":{"Internal Temperature":42.1},"TempUnit":"C"}` | no |

SENSOR is sent right after connecting and then every 60 s. `Time` is left out
until NTP has set the clock.

ESPHome-style, one retained topic per entity, plus the log:

| Topic                                           | Payload |
|-------------------------------------------------|---------|
| `esp32-w5500/sensor/internal_temperature/state` | `42.1` |
| `esp32-w5500/sensor/uptime/state`               | `3600` |
| `esp32-w5500/text_sensor/ip/state`              | `192.168.1.50` |
| `esp32-w5500/text_sensor/mac/state`             | `AA:BB:CC:DD:EE:FF` |
| `esp32-w5500/binary_sensor/mqtt/state`          | `ON` |
| `esp32-w5500/debug`                             | log lines |

The Time entity is internal: it is shown on the web page only, not sent to
Home Assistant or MQTT.

## Home Assistant

The device announces itself over mDNS, so Home Assistant shows it under
**Settings → Devices & services → Discovered**. Enter the
`ota_encryption_key` from `secrets.yaml` when asked. If it isn't discovered,
add the ESPHome integration by hand with the device's IP and port 6053.

## Setup (macOS)

1. Install [Homebrew](https://brew.sh), then ESPHome and the Mosquitto
   clients:

   ```sh
   brew install esphome mosquitto
   ```

2. Get the code and create your secrets (`secrets.yaml` is gitignored):

   ```sh
   git clone https://github.com/jeroenvermeulen/esp32-w5500.git
   cd esp32-w5500/esphome
   cp secrets.example.yaml secrets.yaml
   ```

   Fill in the MQTT broker settings (use `""` for user and password without
   authentication) and a key from `openssl rand -base64 32`.

3. Connect the S2 Mini with a USB-C *data* cable. For the first upload, put
   it in download mode: hold **0**, press **RST**, release **0**. Then build
   and flash:

   ```sh
   esphome run esp32-w5500.yaml
   ```

   Pick the `/dev/cu.usbmodem*` port and press **RST** afterwards. The first
   build downloads the ESP-IDF toolchain and takes a while. Later updates can
   go over the network (OTA).

4. Watch the log with `esphome logs esp32-w5500.yaml`: look for an IP
   address, `SNTP` time sync and `MQTT` connected. Then open
   `http://<ip>/`.

To watch the MQTT messages:

```sh
mosquitto_sub -h <broker> -u <user> -P '<password>' -v -t 'tele/esp32-w5500/#' -t 'esp32-w5500/#'
```

## Changing settings

The `substitutions:` block at the top of [esp32-w5500.yaml](esp32-w5500.yaml)
sets the device name, MQTT topic, SENSOR interval and Ethernet pins. If INT
is not connected, remove the `interrupt_pin:` line. The NTP server and
timezone are in the `time:` block.
