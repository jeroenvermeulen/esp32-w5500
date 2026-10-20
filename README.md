# ESP32 + W5500 Boilerplate

> **Disclaimer:** The code in this project is AI generated, and tested by humans.
> The prompts used to generate it are listed in [PROMPTS.md](PROMPTS.md).

PlatformIO starter project for a WEMOS LOLIN S2 Mini (ESP32-S2) with a WIZnet W5500 SPI Ethernet module.
It uses the native W5500 driver in Arduino-ESP32 core 3.x (`ETH.h`), so the
standard network stack (`WiFiClient`, `HTTPClient`, `WebServer`, mDNS, …) works
over Ethernet. On boot it gets an address over DHCP and serves a small status
page on port 80.

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

## AI prompt log

All AI prompts used on this project are recorded in [PROMPTS.md](PROMPTS.md).
When you work in Claude Code, a `UserPromptSubmit` hook
([.claude/hooks/log-prompt.sh](.claude/hooks/log-prompt.sh)) appends each prompt
automatically. The hook needs `jq`. Add prompts from other tools by hand.
