# EVBox Component for ESPHome (Work In Progress)

Author: A.J. van de Werken  
GitHub: ajvdw

## Description

This is a custom ESPHome component for controlling and monitoring EVBox EV chargers via RS485 communication.

## Features

- Read L1, L2, L3 phase currents
- Read total energy consumption
- Set charging current (6-32A)
- RS485 communication with flow control
- JSON API endpoints
- Home Assistant integration

## Installation

1. Copy `evbox.h` and `evbox.cpp` to your ESPHome configuration directory
2. Use the `evbox.yaml` as a template for your configuration
3. Adjust GPIO pins for your hardware setup

## Hardware Requirements

- ESP32 or ESP8266
- RS485 transceiver module (e.g., MAX485)
- Connection to EVBox charger RS485 bus

## Wiring

- TX Pin: Connect to DI (Data In) of RS485 transceiver
- RX Pin: Connect to RO (Receive Out) of RS485 transceiver
- Flow Control Pin: Connect to DE and RE pins of RS485 transceiver
- RS485 A/B: Connect to EVBox charger RS485 terminals

## API Endpoints
```bash
Set Charging Current
curl http://device-ip/number/charging_current/set?value=16

Get Status (JSON)
curl http://device-ip/text_sensor/evbox_status_json
curl http://device-ip/sensors
```
## License
This component is provided as-is for use with EVBox chargers.
