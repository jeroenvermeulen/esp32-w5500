# AGENTS.md

Notes for AI agents working on this repo. Read this first; it should save you
from re-exploring the codebase.

## What this is

PlatformIO / Arduino firmware for a **WEMOS LOLIN S2 Mini (ESP32-S2)** with a
**WIZnet W5500** SPI Ethernet module. Ethernet only (no WiFi use). It gets an
address over DHCP, serves a plain-text status page on port 80, and publishes
Tasmota-style telemetry over plain MQTT (no TLS).

The whole codebase is ~230 lines; reading every file under `src/` and
`include/` costs less than searching.

## Layout

| Path | Purpose |
|------|---------|
| `src/main.cpp` | `setup()`/`loop()`, network event handler, web server (`/`), SENSOR JSON, NTP time formatting |
| `src/mqtt.cpp`, `src/mqtt.h` | PubSubClient wrapper: connect/reconnect (5 s backoff), LWT, `mqttPublishTele()` |
| `include/config.h` | Pins, hostname, and all tunables, each as `#ifndef` default |
| `include/secrets.example.h` | Template for `include/secrets.h` (gitignored), which overrides `config.h` |
| `platformio.ini` | Single env `lolin_s2_mini`; pinned platform and libs |
| `.vscode/tasks.json` | PIO build/upload/monitor tasks |
| `.claude/hooks/log-prompt.sh` | Appends every user prompt to `PROMPTS.md` |
| `lib/`, `test/` | Empty |

## Build and verify

```sh
pio run          # build; ~2 s incremental. Must succeed before you report done.
```

- `pio` is on PATH (Homebrew). The toolchain is already installed.
- Always run `pio run` after code changes and report RAM/Flash usage when it
  changes noticeably (current baseline: RAM 17.0%, Flash 22.4% of a 3 MB app
  partition).
- You cannot test on hardware. Do not run `pio run -t upload` or
  `pio device monitor` unless asked: the board may not be connected, and
  monitor never exits. The user flashes and tests.
- There are no unit tests.

## Platform specifics (avoid stale examples)

- `platform = espressif32 @ 8.0.0` → **Arduino-ESP32 core 3.3.x** (ESP-IDF 5).
  Many online examples target core 2.x and will not compile or are wrong:
  - W5500 is native in `ETH.h`: `ETH.begin(ETH_PHY_W5500, addr, cs, irq, rst, SPI)`.
    Do not add the `Ethernet` / `EthernetESP32` / WIZnet libraries.
  - Use `Network.onEvent(...)` and `ARDUINO_EVENT_ETH_*` events.
  - Use `NetworkClient` (not `WiFiClient`/`EthernetClient`) for TCP sockets.
- ESP32-S2: single core, native USB CDC serial (no UART bridge), no Bluetooth.
  `Serial` is USB; `setup()` waits up to 2 s for the host to open the port.
- `CORE_DEBUG_LEVEL=0` strips core logging; use `Serial.printf` for diagnostics.
- Partition table `huge_app.csv`: no OTA, by the user's choice. Don't add OTA
  without asking.
- Flash size matters to the user (it was once 86% full). Avoid pulling in
  heavy libraries (e.g. ArduinoJson, WiFi, AsyncWebServer) for small features;
  the SENSOR JSON is built by hand with `String` on purpose.

## Conventions

- **Config:** new tunables go in `include/config.h` as
  `#ifndef X / #define X default / #endif`, placed *after* the
  `secrets.h` include so they can be overridden. Credentials or site-specific
  values also get a line in `include/secrets.example.h`. Never create or
  commit `include/secrets.h` with real values.
- **MQTT:** Tasmota topic scheme `tele/<MQTT_TOPIC>/<suffix>`; LWT is
  `tele/<topic>/LWT`, retained, `Online`/`Offline`. JSON keys follow Tasmota
  naming (`"Time"`, `"ESP32":{"Internal Temperature":…}`, `"TempUnit"`).
  `MQTT_HOST` empty means MQTT is disabled; the code must keep working then.
- **Time:** NTP is started on `ARDUINO_EVENT_ETH_GOT_IP`; `formatLocalTime()`
  returns false until the clock is set, and callers omit/flag the time.
  Default TZ is Europe/Brussels (POSIX string).
- **Code style:** 2-space indent, file-local functions and globals are
  `static`, camelCase functions, UPPER_CASE macros, short comments explaining
  *why*. Non-blocking `loop()`: no `delay()` there; use `millis()` timers.

## Keep in sync when you change behaviour

- `README.md`: wiring table (when pins change), MQTT topic/payload table and
  defaults (e.g. `MQTT_TELEPERIOD`), setup steps.
- `include/secrets.example.h` when adding override-worthy settings.
- `PROMPTS.md` is written automatically by the hook; don't edit or reformat
  existing entries. Only add prompts by hand if they came from another tool.

## User preferences

- The user tests on real hardware and reports back; keep changes small and
  explain what to look for in the serial log.
- README states the code is AI generated and tested by humans; keep that
  disclaimer.
