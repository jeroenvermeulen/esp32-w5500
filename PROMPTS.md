# AI Prompt Log

Every prompt given to an AI assistant while building this project, oldest first.
New entries are appended automatically by the Claude Code hook in
`.claude/hooks/log-prompt.sh`. Prompts sent to other tools can be added by hand
in the same format.

## 2026-10-20 19:40 · Jeroen Vermeulen

> This is a boilerplate project for an ESP32 + W5500 LAN module. I would like to use PlatformIO.
> I would like to register all used AI prompts in a human readable file.

## 2026-10-20 19:50 · Jeroen Vermeulen

> Can you add a disclaimer to the README the code is AI generated but tested by humans.

## 2026-10-20 20:22 · Jeroen Vermeulen

> Please create a tasks.json for VSCode to build, flash, monitor etc

## 2026-10-20 20:26 · Jeroen Vermeulen

> My board is an ESP32 S2 Mini. I have already updated the pins in config.h. Can you change the project to match this board?

## 2026-10-20 20:32 · Jeroen Vermeulen

> Can you make sure wifi is disabled on the ESP32?

## 2026-10-20 21:19 · Jeroen Vermeulen

> I would like to publish sensor values to MQTT.
> Let's start with the internal temperature of the ESP.
> I would like to be able to configure MQTT host, port, user, password.
> We will use the normal MQTT protocol, no need for TLS/SSL implementation.
> Please use the MQTT paths like Tasmota does. I would also like an online/offline status sent to MQTT.

## 2026-10-20 21:21 · Jeroen Vermeulen

> Please add instructions in the readme how to get this project working on MacOS, assuming it is brand new so nothing installed yet.

## 2026-10-20 21:25 · Jeroen Vermeulen

> Please rename the "Temperature" field on MQTT to "Internal Temperature"

## 2026-10-20 21:29 · Jeroen Vermeulen

> Flash is 86% full. Please drop the time and wifi functions. Wifi is off by default anyway.
> How can we save more memory?

## 2026-10-20 21:33 · Jeroen Vermeulen

> Please do 1 and 2, drop OTA.
> Please bring back the time for MQTT. Default timezone should be Europe/Brussels.

## 2026-10-20 21:35 · Jeroen Vermeulen

> Can you add the current timestamp to the webserver output

## 2026-10-20 21:44 · Jeroen Vermeulen

> Please create an AGENTS.md with anything useful for future agents working on this project.
> The goal is to make it the AI faster and more efficient.

## 2026-10-20 22:06 · Jeroen Vermeulen

> During startup please send a serial message what the next step is because when something is wrong it takes very long and you don't know what it is doing.
> I disconnected the ethernet cable, will there be a timeout?
