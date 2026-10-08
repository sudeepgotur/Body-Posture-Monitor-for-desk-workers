#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include "FS.h"
#include "LittleFS.h"

// ========= WiFi credentials =========
const char* ssid     = "YOUR_HOTSPOT_NAME";
const char* password = "YOUR_PASSAWORD";

// ========= Web server =========
AsyncWebServer server(80);

// ========= MPU6050 =========
const int MPU_ADDR = 0x68;
int16_t AcX, AcY, AcZ;
float pitch;

// ========= Posture logic =========
float refPitch = 0;
bool calibrated = false;
unsigned long calibStartTime = 0;
const unsigned long CALIB_DURATION = 5000;   // 5s for good posture at start

float deviation = 0;
const float PITCH_TOLERANCE = 15.0;          // allowed deviation in degrees

// 🔴 CHANGED: 30s -> 10s
const unsigned long BAD_DELAY = 40000;       // 10s continuous bad posture

bool isCurrentlyBad = false;
bool postureBadConfirmed = false;
unsigned long badStartTime = 0;
unsigned long badSeconds = 0;

// ========= VIBRATOR (transistor driver) =========
// 🔧 CHANGE THIS to the GPIO you actually use (e.g. 4 or 14)
const int VIBRATOR_PIN = 19;   // example: GPIO4 (D4)

// ========= MPU helper =========
void readMPU() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true);

  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();

  float Ax = AcX / 16384.0;
  float Ay = AcY / 16384.0;
  float Az = AcZ / 16384.0;

  // pitch from accelerometer (same as your original code)
  pitch = atan2(Ay, sqrt(Ax * Ax + Az * Az)) * 180.0 / PI;  // degrees
}

// ========= HTTP handlers =========
void handlePosture(AsyncWebServerRequest *request) {
  // decide status string
  const char* statusStr;
  if (postureBadConfirmed)      statusStr = "BAD";
  else if (isCurrentlyBad)      statusStr = "POTENTIALLY_BAD";
  else                          statusStr = "GOOD";

  StaticJsonDocument<256> doc;
  doc["pitch"]      = pitch;
  doc["refPitch"]   = refPitch;
  doc["deviation"]  = deviation;
  doc["badSeconds"] = badSeconds;      // always a number
  doc["status"]     = statusStr;

  String json;
  serializeJson(doc, json);
  request->send(200, "application/json", json);
}

// ========= Setup =========
void setup() {
  Serial.begin(115200);
  delay(1000);

  // I2C + MPU6050
  Wire.begin(21, 22);  // SDA, SCL on ESP32
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);   // wake up MPU6050
  Wire.endTransmission();

  // Vibrator pin setup
  pinMode(VIBRATOR_PIN, OUTPUT);
  digitalWrite(VIBRATOR_PIN, LOW);  // make sure it's OFF at start

  // LittleFS
  if (!LittleFS.begin(true)) {   // true = format if mount fails
    Serial.println("LittleFS Mount Failed");
    while (true) delay(1000);
  }
  Serial.println("LittleFS mounted successfully");

  // WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi connected");
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());

  // Web routes
  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
  server.on("/posture", HTTP_GET, handlePosture);

  server.begin();
  Serial.println("Async WebServer started");

  // Start calibration
  calibStartTime = millis();
  Serial.println("Sit in GOOD posture for 5 seconds...");
}

// ========= Calibration loop =========
void loopCalibration() {
  static float sum = 0;
  static int count = 0;

  readMPU();

  if (millis() - calibStartTime < CALIB_DURATION) {
    sum += pitch;
    count++;
    Serial.print("Calibrating... pitch=");
    Serial.println(pitch);
  } else if (!calibrated && count > 0) {
    refPitch = sum / count;
    calibrated = true;
    Serial.println("\nCalibration DONE!");
    Serial.print("refPitch = ");
    Serial.println(refPitch);
    Serial.println("---------------------------");
  }
}

// ========= Posture loop =========
void loopPosture() {
  readMPU();
  unsigned long now = millis();

  deviation = pitch - refPitch;
  float absDev = fabs(deviation);
  bool currentlyBad = (absDev > PITCH_TOLERANCE);

  if (currentlyBad) {
    if (!isCurrentlyBad) {
      isCurrentlyBad = true;
      badStartTime = now;
      postureBadConfirmed = false;
    } else if (!postureBadConfirmed && (now - badStartTime >= BAD_DELAY)) {
      postureBadConfirmed = true;
    }
  } else {
    isCurrentlyBad = false;
    postureBadConfirmed = false;
  }

  if (isCurrentlyBad) badSeconds = (now - badStartTime) / 1000;
  else                badSeconds = 0;

  // ========= Vibrator control =========
  // Vibrate ONCE for 1–3 seconds when BAD posture is confirmed (after 10s)
  static bool buzzedOnce = false;
  const unsigned long BUZZ_DURATION = 3000; // 2s vibration (change 1000–3000 if you want)
  static unsigned long buzzStartTime = 0;

  if (postureBadConfirmed) {
    if (!buzzedOnce) {
      buzzedOnce = true;
      buzzStartTime = now;
      digitalWrite(VIBRATOR_PIN, HIGH); // start buzzing
      Serial.println("Buzz started");
    }

    // stop buzzing after BUZZ_DURATION
    if (buzzedOnce && (now - buzzStartTime >= BUZZ_DURATION)) {
      digitalWrite(VIBRATOR_PIN, LOW);
    }

  } else {
    // Reset when posture returns to GOOD / short deviation
    buzzedOnce = false;
    digitalWrite(VIBRATOR_PIN, LOW);
  }

  // Debug
  Serial.print("Pitch=");
  Serial.print(pitch);
  Serial.print(" ref=");
  Serial.print(refPitch);
  Serial.print(" dev=");
  Serial.print(deviation);
  Serial.print(" absDev=");
  Serial.print(absDev);
  Serial.print(" status=");
  if (postureBadConfirmed)      Serial.println("BAD");
  else if (isCurrentlyBad)      Serial.println("POTENTIALLY_BAD");
  else                          Serial.println("GOOD");
}

// ========= Main loop =========
void loop() {
  if (!calibrated) {
    loopCalibration();
  } else {
    loopPosture();
  }

  delay(200);  // ~5 Hz updates
}
