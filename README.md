# TiltGuard – Anti-Theft Tilt Monitoring System

TiltGuard is a small embedded anti-theft device built using the Adafruit Feather HUZZAH ESP8266 and the MPU6050 motion sensor. It detects tilt or movement of a valuable item, triggers an alarm state, and displays live status on a phone through a local WiFi HTML page.

## Features
- Creates its own WiFi hotspot (`group16SES`)
- Live status page accessible from any phone browser
- Three system states shown via RGB LED:
  - **Idle (Blue)** – System disarmed
  - **Monitoring (Green)** – Tilt detection active
  - **Alarm (Red)** – Significant movement detected
- Movement detection using MPU6050 accelerometer
- One button for arming, disarming, and resetting alarm
- Baseline calibration after arming

## Hardware Used
- Adafruit Feather HUZZAH ESP8266
- MPU6050 accelerometer/gyro
- RGB LED (pins 12, 13, 14)
- Push button (GPIO 2 → GND)
- Breadboard and jumper wires

## How to Use
1. Power the Feather and wait for WiFi hotspot to start.
2. Connect your phone to `group16SES` (password: `group16SES`).
3. Open a browser and go to: `http://192.168.4.1`
4. Press the button to arm the system.
5. Move or tilt the board to trigger the alarm.
6. Press the button again to clear the alarm.

## Project Purpose
This project demonstrates a simple embedded security system using sensor input, WiFi communication, and human–device interaction via HTML.
