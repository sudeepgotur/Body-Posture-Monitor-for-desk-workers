# Body Posture Monitor for Desk Workers

## Overview

The **Body Posture Monitor for Desk Workers** is an embedded system designed to monitor a user's sitting posture in real time and provide a vibration alert when an incorrect posture is maintained for a prolonged period.

The system uses an **MPU6050 IMU sensor** to measure body orientation and angle. An **ESP32 microcontroller** processes the sensor data and compares the measured angle with a predefined normal-posture reference.

To avoid unnecessary alerts caused by temporary movements, the system uses a **40-second delay mechanism**. If the incorrect posture continues beyond this period, a vibration motor is activated to remind the user to correct their posture.

---

## Features

- Real-time posture monitoring
- Body-angle measurement using MPU6050
- ESP32-based sensor processing
- Incorrect-posture detection
- 40-second delay before alert
- Vibration-based feedback
- Portable and low-cost implementation
- Reduced false alerts during temporary movements

---

## System Architecture

```text
                  ┌─────────────────────┐
                  │      MPU6050        │
                  │     IMU Sensor      │
                  │ Accelerometer/Gyro   │
                  └──────────┬──────────┘
                             │
                         I²C (SDA/SCL)
                             │
                             ▼
                  ┌─────────────────────┐
                  │        ESP32        │
                  │    Microcontroller   │
                  └──────────┬──────────┘
                             │
                      Posture Angle
                         Processing
                             │
                             ▼
                  ┌─────────────────────┐
                  │   Posture Logic     │
                  │                     │
                  │ Compare with        │
                  │ posture threshold   │
                  └──────────┬──────────┘
                             │
                    ┌────────┴────────┐
                    │                 │
                  Correct          Incorrect
                    │                 │
                    ▼                 ▼
              ┌───────────┐    ┌─────────────┐
              │ Continue  │    │ Start Timer │
              │ Monitoring│    └──────┬──────┘
              └───────────┘           │
                                      ▼
                              ┌────────────────┐
                              │  > 40 seconds? │
                              └───────┬────────┘
                                      │
                              ┌───────┴───────┐
                              │               │
                             NO              YES
                              │               │
                              ▼               ▼
                       Reset Timer     ┌──────────────┐
                                       │   Vibration  │
                                       │     Motor    │
                                       └──────────────┘
```

---

## Working Principle

The system works through the following sequence:

1. The **MPU6050 IMU sensor** continuously measures the user's body orientation.
2. The sensor provides motion and orientation data to the **ESP32 through the I²C interface**.
3. The ESP32 processes the sensor data and determines the body angle.
4. The measured angle is compared with a predefined normal-posture reference.
5. If the posture is within the acceptable range, the system continues monitoring.
6. If an incorrect posture is detected, the system starts a timer.
7. If the user corrects the posture within 40 seconds, the timer is reset.
8. If the incorrect posture continues for more than 40 seconds, the vibration motor is activated.
9. The vibration provides tactile feedback to remind the user to correct their posture.
10. Once the posture is corrected, the vibration is stopped.

The 40-second timer is used to reduce unnecessary alerts caused by temporary movements.

---

## Hardware Components

| Component | Purpose |
|---|---|
| ESP32 | Main microcontroller and sensor-data processing |
| MPU6050 IMU | Measures body orientation and angle |
| Vibration Motor | Provides tactile posture alert |
| BC547 NPN Transistor | Drives the vibration motor |
| Resistor | Used in the transistor interface |
| Breadboard | Prototype circuit assembly |
| Jumper Wires | Electrical connections |
| USB Cable | ESP32 programming and power |

---

## MPU6050

The **MPU6050** is an inertial measurement unit containing:

- 3-axis accelerometer
- 3-axis gyroscope

In this project, the MPU6050 is used to obtain orientation information from the user's body position.

The sensor communicates with the ESP32 using the **I²C communication interface**.

---

## ESP32

The ESP32 acts as the main controller of the system.

### Its responsibilities include:

- Reading MPU6050 sensor data
- Processing the measured angle
- Comparing the angle with the posture reference
- Managing the 40-second timer
- Controlling the vibration alert

---

## Vibration Alert

A vibration motor is used to provide physical feedback to the user.

The ESP32 controls the vibration motor through a transistor interface. The transistor allows the microcontroller to control the motor without directly driving the motor current from an ESP32 GPIO.

---

## Posture Detection Logic

The basic decision process is:

```text
              Read MPU6050
                    │
                    ▼
             Calculate Angle
                    │
                    ▼
        Compare with Threshold
                    │
          ┌─────────┴─────────┐
          │                   │
       Correct             Incorrect
          │                   │
          ▼                   ▼
 Continue Monitoring      Start Timer
                              │
                              ▼
                       Is posture still
                       incorrect after
                         40 seconds?
                              │
                     ┌────────┴────────┐
                     │                 │
                    NO                YES
                     │                 │
                     ▼                 ▼
               Reset Timer       Start Vibration
                                       │
                                       ▼
                                Correct Posture
                                       │
                                       ▼
                                Stop Vibration
```

---

## Objectives

The project was developed with the following objectives:

- Design a posture monitoring system for desk workers.
- Measure body angle using the MPU6050.
- Process sensor data using the ESP32.
- Detect incorrect posture.
- Provide vibration feedback.
- Reduce unnecessary alerts caused by temporary movements.
- Implement a 40-second delay before activating the vibration alert.
- Improve awareness of proper sitting posture.

---

## Outcomes

The implemented system:

- Monitors posture in real time.
- Detects incorrect body posture using sensor-based angle measurement.
- Provides vibration feedback when incorrect posture persists.
- Uses a delay mechanism to reduce unnecessary alerts.
- Provides a portable implementation suitable for daily use.

---

## Applications

Potential applications include:

- Corporate offices
- Educational institutions
- Call centers and BPO environments
- Co-working spaces
- Rehabilitation and physiotherapy centers
- Workplace posture-awareness programs
- Long reading or writing sessions

---

## Advantages

- Real-time posture feedback
- Simple and easy to operate
- Portable implementation
- Low-cost hardware
- Vibration-based physical feedback
- Reduced unnecessary alerts through timer-based detection

---

## Project Structure

```text
Body-Posture-Monitor-ESP32/
│
├── README.md
│
├── src/
│   └── posture_monitor.ino
│
├── hardware/
│   └── circuit-diagram.jpg
│
├── images/
│   ├── prototype.jpg
│   └── final-setup.jpg
│
└── docs/
    └── project-report.pdf
```

> Update the filenames in this section if your actual files have different names.

---

## Technologies Used

- ESP32
- MPU6050 IMU
- I²C Communication
- Arduino C/C++
- Embedded Systems
- Sensor Interfacing
- Real-Time Monitoring

---

## My Contribution

My contribution to the project was primarily focused on the hardware and embedded-system implementation.

- Worked on ESP32 and MPU6050 interfacing.
- Worked on sensor integration and circuit implementation.
- Worked on posture detection logic.
- Implemented and assisted with the vibration-alert mechanism.
- Worked on the delay mechanism for reducing unnecessary alerts.
- Participated in hardware assembly and testing.

---

## Future Improvements

- Improve posture-angle calibration.
- Add more robust sensor filtering.
- Improve posture classification for different users.
- Add wireless/mobile monitoring.
- Develop a compact wearable enclosure.
- Add data logging for posture history.
- Improve power management.

---

## Project Information

**Project:** Body Posture Monitor for Desk Workers

**Department:** Electronics and Communication Engineering

**Institution:** KLS Vishwanathrao Deshpande Institute of Technology, Haliyal

**Academic Year:** 2024–25

**Guide:** Dr. Nagaraj Bhat

---

## Team Members

- Sudeep T. Gotur
- Srushti Kolekar
- Suhas Rotti
- Sujata S. Waghamode

---

## Author

**Sudeep T. Gotur**

Electronics and Communication Engineering
