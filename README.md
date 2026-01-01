# TiltGuard – Smart Anti-Theft Motion Monitoring System

TiltGuard is a compact, item-level anti-theft monitoring system developed as part of a Small Embedded Systems coursework project.  
It uses motion sensing, visual indicators, and a local Wi-Fi status interface to detect unauthorised item movement in real time.

The system is designed to be low-power, event-driven, and self-contained, making it suitable for protecting individual high-value or theft-prone items.

---

## Features

- Finite State Machine–based control logic  
  - IDLE (disarmed)  
  - MONITORING (armed)  
  - ALARM (unauthorised movement detected)

- Motion-based theft detection using an MPU6050 accelerometer  
- RGB LED status indication  
  - Blue: IDLE  
  - Green: MONITORING  
  - Red: ALARM  

- Local Wi-Fi status page hosted on the ESP8266  
- Single-button control for arming, disarming, and alarm reset  

---

## Hardware Used

- Microcontroller: Adafruit Feather HUZZAH (ESP8266)  
- Motion sensor: MPU6050 accelerometer  
- Output: RGB LED  
- Input: Push button  
- Power: USB / regulated supply  

---

## System Behaviour

1. On power-up, the system enters the IDLE state.  
2. Pressing the button arms the system, calibrates the accelerometer baseline, and enters MONITORING mode.  
3. While monitoring, motion exceeding a predefined threshold triggers the ALARM state.  
4. The alarm can be cleared using the button, returning the system to IDLE.

---

## Wi-Fi Status Page

- SSID: `group16SES`  
- Password: `group16SES`  
- URL: `http://192.168.4.1`  

The web interface displays the current system state and alarm status and refreshes automatically.

---

## Software Design Notes

- The firmware follows an event-driven architecture to minimise power consumption.  
- Motion detection is delegated to the accelerometer, reducing continuous polling by the microcontroller.  
- System behaviour is modelled using a finite state machine (FSM) for clarity and reliability.  

---

## Future Improvements

- Interrupt-based motion detection  
- RFID-based authorised reset  
- Remote notifications via Wi-Fi  
- Migration to a lower-power microcontroller  

---

## Author

Developed by Group 16  
Small Embedded Systems Coursework
