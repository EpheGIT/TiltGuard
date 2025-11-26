// TiltGuard – Feather HUZZAH ESP8266 Final Version with Local HTML Status Page
// States:
//   IDLE (blue) → MONITORING (green) → ALARM (red)
//
// Button toggles IDLE ↔ MONITORING and clears ALARM.
// MPU6050 detects tilt and triggers alarm.
// ESP8266 runs as WiFi Access Point and hosts a simple HTML page
// showing the current status (IDLE / MONITORING / ALARM).

#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

Adafruit_MPU6050 mpu;

// ---------- WiFi AP (Feather as hotspot) ----------
const char* apSsid     = "group16SES";   // WiFi name shown on phone
const char* apPassword = "group16SES";   // WiFi password

ESP8266WebServer server(80);

// ---------- Pins ----------
const int PIN_BUTTON = 2;    // button: one leg to pin 2, other leg to GND
const int PIN_LED_R  = 14;
const int PIN_LED_G  = 12;
const int PIN_LED_B  = 13;

// ---------- States ----------
enum SystemState { STATE_IDLE, STATE_MONITORING, STATE_ALARM };
SystemState state = STATE_IDLE;

// ---------- Baseline ----------
float baseAx = 0, baseAy = 0, baseAz = 0;
const float MOVEMENT_THRESHOLD = 1.5;   // tune this if needed

// ---------- Timers ----------
unsigned long lastSensorTime = 0;

// ---------- Alarm info for HTML ----------
bool alarmActive = false;
float lastAlarmMagnitude = 0.0;

// ---------- LED Helper ----------
void setLED(int r, int g, int b) {
  // ESP8266 analogWrite: 0–1023
  analogWrite(PIN_LED_R, r);
  analogWrite(PIN_LED_G, g);
  analogWrite(PIN_LED_B, b);
}

void ledIdle()       { setLED(0,    0,    1023); }   // blue
void ledMonitoring() { setLED(0,    1023, 0   ); }   // green
void ledAlarm()      { setLED(1023, 0,    0   ); }   // red
void ledOff()        { setLED(0,    0,    0   ); }

// ---------- Baseline Calibration ----------
void calibrateBaseline() {
  Serial.println("Calibrating baseline... keep board still");
  float ax = 0, ay = 0, az = 0;

  for (int i = 0; i < 100; i++) {
    sensors_event_t a, g, t;
    mpu.getEvent(&a, &g, &t);
    ax += a.acceleration.x;
    ay += a.acceleration.y;
    az += a.acceleration.z;
    delay(10);
  }

  baseAx = ax / 100.0;
  baseAy = ay / 100.0;
  baseAz = az / 100.0;

  Serial.println("Baseline set.");
}

// ---------- Button Handling ----------
bool buttonPressed() {
  static int lastState = HIGH;
  int current = digitalRead(PIN_BUTTON);

  if (current == LOW && lastState == HIGH) {
    lastState = LOW;
    delay(20);   // simple debounce
    return true;
  }
  if (current == HIGH) lastState = HIGH;
  return false;
}

// ---------- HTML Page ----------
String getHTML() {
  String html = "<!DOCTYPE html><html><head><meta charset='utf-8'>";
  html += "<meta http-equiv='refresh' content='0.5'>"; // auto refresh every 0.5s
  html += "<title>TiltGuard Status</title></head><body>";
  html += "<h1>TiltGuard – Anti-Theft Monitor</h1>";

  html += "<p><strong>Current state: </strong>";

  if (state == STATE_IDLE)            html += "IDLE (disarmed)</p>";
  else if (state == STATE_MONITORING) html += "MONITORING (armed)</p>";
  else if (state == STATE_ALARM)      html += "ALARM</p>";

  if (alarmActive) {
    html += "<p style='color:red; font-size:20px;'><strong>ALARM! ";
    html += "Movement magnitude: ";
    html += String(lastAlarmMagnitude, 2);
    html += "</strong></p>";
  } else {
    html += "<p>No alarm at the moment.</p>";
  }

  html += "<p>Refresh rate: 0.5 seconds.</p>";
  html += "</body></html>";
  return html;
}

void handleRoot() {
  server.send(200, "text/html", getHTML());
}

// ---------- Setup ----------
void setup() {
  Serial.begin(115200);
  delay(300);

  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_LED_R, OUTPUT);
  pinMode(PIN_LED_G, OUTPUT);
  pinMode(PIN_LED_B, OUTPUT);

  ledIdle();

  if (!mpu.begin()) {
    Serial.println("MPU6050 not found!");
    while (1) { ledAlarm(); delay(200); ledOff(); delay(200); }
  }

  Serial.println("Starting WiFi Access Point...");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(apSsid, apPassword);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);   // usually 192.168.4.1

  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started.");
}

// ---------- Loop ----------
void loop() {
  // Handle web requests
  server.handleClient();

  // Handle button presses
  if (buttonPressed()) {
    if (state == STATE_IDLE) {
      // Go to monitoring immediately (turn LED green), then calibrate
      state = STATE_MONITORING;
      alarmActive = false;
      ledMonitoring();
      Serial.println("State → MONITORING");
      calibrateBaseline();
    } else if (state == STATE_MONITORING) {
      // Stop monitoring
      state = STATE_IDLE;
      alarmActive = false;
      ledIdle();
      Serial.println("State → IDLE");
    } else if (state == STATE_ALARM) {
      // Clear alarm and go back to idle
      state = STATE_IDLE;
      alarmActive = false;
      ledIdle();
      Serial.println("Alarm cleared → IDLE");
    }
  }

  // Motion check while monitoring
  if (state == STATE_MONITORING) {
    if (millis() - lastSensorTime > 20) {
      lastSensorTime = millis();

      sensors_event_t a, g, t;
      mpu.getEvent(&a, &g, &t);

      float dx = a.acceleration.x - baseAx;
      float dy = a.acceleration.y - baseAy;
      float dz = a.acceleration.z - baseAz;

      float magnitude = sqrt(dx*dx + dy*dy + dz*dz);

      Serial.print("Movement: ");
      Serial.println(magnitude);

      if (magnitude > MOVEMENT_THRESHOLD) {
        state = STATE_ALARM;
        alarmActive = true;
        lastAlarmMagnitude = magnitude;
        ledAlarm();
        Serial.println("State → ALARM");
      }
    }
  }
}
