#include <Wire.h>
#include <MPU6050.h>

// ============================================================
// MPU6050 Object
// ============================================================
MPU6050 mpu;

// ============================================================
// Pin Configuration
// ============================================================
const int BUZZER_PIN = 4;

// ============================================================
// Posture Detection Parameters
// ============================================================
const float POSTURE_THRESHOLD = 20.0;
const int CALIBRATION_SAMPLES = 100;

// ============================================================
// Angle and Calibration Variables
// ============================================================
float angleX = 0.0;
float angleY = 0.0;

float offsetX = 0.0;
float offsetY = 0.0;


// ============================================================
// SETUP
// Runs once when the ESP32 is powered on or reset
// ============================================================
void setup() {

  // Start Serial Communication
  Serial.begin(115200);

  // Initialize I2C
  // ESP32:
  // SDA = GPIO 21
  // SCL = GPIO 22
  Wire.begin(21, 22);

  // Configure buzzer pin as output
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // ----------------------------------------------------------
  // Initialize MPU6050
  // ----------------------------------------------------------
  Serial.println("Initializing MPU6050...");

  mpu.initialize();

  // Check MPU6050 connection
  if (!mpu.testConnection()) {

    Serial.println("MPU6050 connection failed!");

    // Stop program if sensor is not connected
    while (1);
  }

  Serial.println("MPU6050 connected successfully!");

  // ----------------------------------------------------------
  // Calibration
  // ----------------------------------------------------------
  Serial.println("Calibrating...");
  Serial.println("Keep the device flat and still.");

  for (int i = 0; i < CALIBRATION_SAMPLES; i++) {

    int16_t ax, ay, az;
    int16_t gx, gy, gz;

    // Read accelerometer and gyroscope data
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // Calculate accelerometer-based angles
    float currentAngleX =
        atan2(ay, az) * 180.0 / PI;

    float currentAngleY =
        atan2(-ax, sqrt((float)ay * ay + (float)az * az))
        * 180.0 / PI;

    // Add readings to calculate average offset
    offsetX += currentAngleX;
    offsetY += currentAngleY;

    delay(10);
  }

  // Calculate average offsets
  offsetX /= CALIBRATION_SAMPLES;
  offsetY /= CALIBRATION_SAMPLES;

  Serial.println("Calibration done!");

  Serial.print("X Offset: ");
  Serial.println(offsetX);

  Serial.print("Y Offset: ");
  Serial.println(offsetY);
}


// ============================================================
// LOOP
// Runs continuously after setup()
// ============================================================
void loop() {

  // ----------------------------------------------------------
  // Variables for sensor readings
  // ----------------------------------------------------------
  int16_t ax, ay, az;
  int16_t gx, gy, gz;

  // Read MPU6050 data
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // ----------------------------------------------------------
  // Calculate Tilt Angles
  // ----------------------------------------------------------

  // Roll / X-axis tilt
  angleX =
      atan2(ay, az) * 180.0 / PI - offsetX;

  // Pitch / Y-axis tilt
  angleY =
      atan2(-ax, sqrt((float)ay * ay + (float)az * az))
      * 180.0 / PI - offsetY;

  // ----------------------------------------------------------
  // Display Angle Information
  // ----------------------------------------------------------
  Serial.print("Angle X: ");
  Serial.print(angleX);

  Serial.print("\tAngle Y: ");
  Serial.println(angleY);

  // ----------------------------------------------------------
  // Posture Detection
  // ----------------------------------------------------------

  if (abs(angleX) > POSTURE_THRESHOLD ||
      abs(angleY) > POSTURE_THRESHOLD) {

    // Bad posture detected
    Serial.println("Bad Posture!");

    // Turn buzzer ON
    digitalWrite(BUZZER_PIN, HIGH);

  } else {

    // Good posture
    Serial.println("Good Posture!");

    // Turn buzzer OFF
    digitalWrite(BUZZER_PIN, LOW);
  }

  // Wait before taking the next reading
  delay(300);
}