# Body Posture Monitor for Desk Workers

An ESP32-based embedded system for real-time posture monitoring and
prolonged bad-posture detection using an MPU6050 IMU sensor, with
vibration-based feedback and an integrated web-based monitoring
interface.

---

## 📌 Overview

The **Body Posture Monitor for Desk Workers** is an embedded system
developed to monitor a user's sitting posture in real time and provide
tactile feedback when an incorrect posture is maintained for a prolonged
period.

The system uses an **MPU6050 inertial measurement unit (IMU)** to obtain
body-orientation information. An **ESP32 microcontroller** processes the
sensor data, performs an initial posture calibration, calculates the
current pitch angle, and compares it with the calibrated reference
posture.

To prevent unnecessary alerts caused by short-term movements, the system
uses a **40-second bad-posture confirmation delay**. If the measured
posture remains outside the allowed range for the complete delay period,
the ESP32 activates a vibration motor to remind the user to correct their
posture.

The ESP32 also hosts a **web server** using its internal Wi-Fi
connectivity. The web interface is stored in the ESP32's **LittleFS
filesystem** and provides real-time posture information through a JSON
API.

A custom PCB was also designed as part of the hardware implementation.

---

## ✨ Features

- Real-time posture monitoring
- MPU6050-based posture sensing
- ESP32-based embedded processing
- Automatic 5-second posture calibration
- Reference posture angle generation
- Pitch-angle calculation
- 15° posture deviation tolerance
- Prolonged bad-posture detection
- 40-second confirmation delay
- Three posture states:
  - `GOOD`
  - `POTENTIALLY_BAD`
  - `BAD`
- Vibration-based tactile feedback
- 3-second vibration alert
- ESP32 Wi-Fi connectivity
- ESP32-hosted asynchronous web server
- LittleFS-based web interface
- Real-time JSON posture API
- Custom PCB design
- Low-cost embedded implementation
- Camera-free posture monitoring

---

# 🏗️ System Architecture

```text
                    ┌─────────────────────┐
                    │       MPU6050       │
                    │     IMU Sensor      │
                    │  Accelerometer/Gyro │
                    └──────────┬──────────┘
                               │
                           I²C / SDA-SCL
                               │
                               ▼
                    ┌─────────────────────┐
                    │        ESP32        │
                    │   Microcontroller   │
                    └──────────┬──────────┘
                               │
                 ┌─────────────┼─────────────┐
                 │             │             │
                 ▼             ▼             ▼
          Posture Logic    Vibrator      Wi-Fi
                 │          Control         │
                 │             │             │
                 ▼             ▼             ▼
          GOOD / BAD      Vibration     Web Server
          Detection          Alert           │
                                             ▼
                                       ┌─────────────┐
                                       │   LittleFS  │
                                       │             │
                                       │ index.html  │
                                       │ script.js   │
                                       │ style.css   │
                                       └──────┬──────┘
                                              │
                                              ▼
                                      Web Monitoring UI
```
---
## ⚙️ Working Principle

The system operates through the following sequence.

### 1. Initialization

When the ESP32 starts, it initializes:
- Serial communication
- I²C communication
- MPU6050
- Vibration motor control GPIO
- LittleFS
- Wi-Fi
- Asynchronous web server
The MPU6050 is connected to the ESP32 using the I²C interface.


### 2. Automatic Posture Calibration

After initialization, the system performs an automatic calibration.
The user is expected to sit in a normal posture for approximately
5 seconds.

During this period, the ESP32 continuously reads the MPU6050 pitch
values and calculates their average.
The resulting average is stored as the reference posture:

```text
              Normal Posture
                    │
                    ▼
             5-second calibration
                    │
                    ▼
              Read pitch values
                    │
                    ▼
              Calculate average
                    │
                    ▼
                 refPitch
```

This reference value is then used for subsequent posture monitoring.


### 3. Pitch Calculation

The ESP32 reads the raw accelerometer values from the MPU6050.

The accelerometer readings are converted into acceleration values and
used to calculate the pitch angle.

The implemented calculation is based on:

`pitch = atan2(Ay, √(Ax² + Az²))`

The resulting angle is expressed in degrees.


### 4. Posture Deviation

After calibration, the current pitch is compared with the calibrated
reference pitch.

The firmware calculates:

`deviation = pitch - refPitch`

The absolute deviation is then compared with the configured posture
tolerance.

The current firmware uses:

`PITCH_TOLERANCE = 15°`

Therefore:

```text
Absolute deviation ≤ 15°
        ↓
     GOOD

Absolute deviation > 15°
        ↓
Potentially incorrect posture
```
---


## 🚦 Posture States

The firmware provides three posture states.

| State | Description |
|---|---|
| `GOOD` | Posture is within the allowed deviation |
| `POTENTIALLY_BAD` | Posture has exceeded the allowed deviation, but the confirmation delay has not expired |
| `BAD` | Incorrect posture has remained beyond the 40-second confirmation period |

The state is provided to the web interface through the `/posture`
endpoint.

---

## ⏱️ Bad-Posture Confirmation

The system does not immediately activate the vibrator when the user's
posture becomes incorrect.

Instead, a timer is started.

```text
                Posture Reading
                      │
                      ▼
              Calculate Deviation
                      │
                      ▼
               Deviation > 15°?
                  │          │
                 NO         YES
                  │          │
                  ▼          ▼
                GOOD    Start Timer
                            │
                            ▼
                      Continue Monitoring
                            │
                            ▼
                      40 Seconds Reached?
                        │            │
                       NO           YES
                        │            │
                        ▼            ▼
               POTENTIALLY_BAD      BAD
                                    │
                                    ▼
                               Vibrator ON
```

If the user returns to the acceptable posture before the 40-second
period expires, the timer is reset.

If the incorrect posture continues for the complete 40 seconds, the
firmware confirms the posture as BAD.

---
                   
## 📳 Vibration Alert

The vibration motor is controlled through a transistor driver connected
to the ESP32.

When bad posture is confirmed:

```text
BAD posture confirmed
        │
        ▼
Vibrator activated
        │
        ▼
Approximately 3 seconds
        │
        ▼
Vibrator switched OFF
```

The current firmware uses a vibration duration of:

`BUZZ_DURATION = 3000 ms`

The vibration provides tactile feedback without requiring a visual or
audible alarm.

---

## 🌐 ESP32 Web Server

One of the important features of this project is that the ESP32
itself acts as the web server.

The firmware uses:

- ESPAsyncWebServer
- AsyncTCP
- WiFi
- ArduinoJson
- LittleFS

The ESP32 connects to a Wi-Fi network and starts an asynchronous web
server on port 80.

The web interface is stored in the ESP32's internal LittleFS
filesystem.

```text
ESP32
 │
 ├── Wi-Fi
 │
 ├── Async Web Server
 │
 └── LittleFS
      │
      ├── index.html
      ├── script.js
      └── style.css
```

---

## 📡 Posture JSON API

The firmware provides a dedicated endpoint:

`/posture`

This endpoint returns posture information in JSON format.

```text
Example structure:
{
    "pitch": 12.5,
    "refPitch": 10.2,
    "deviation": 2.3,
    "badSeconds": 0,
    "status": "GOOD"
}
```

The API provides:

- Current pitch
- Reference pitch
- Pitch deviation
- Duration of the current bad-posture condition
- Current posture status

The web interface can use this information to display the current
posture state and sensor values.

---

## 💾 LittleFS

The project uses LittleFS on the ESP32 to store and serve the web
interface files.

The firmware mounts LittleFS during startup:

```text
LittleFS
   │
   ├── index.html
   ├── script.js
   └── style.css
```

The ESP32 serves these files directly through its web server.

This allows the project to operate as a self-contained embedded web
application without requiring a separate computer or backend server for
the interface.

---

## 💻 Web Interface

The frontend of the project is organized as:

```text
software/
├── index.html
├── script.js
└── style.css
```

### index.html

Defines the structure of the web interface.

### script.js

Handles client-side interaction and communication with the ESP32 posture API.

### style.css

Provides the visual styling of the web interface.

Dashboard screenshots are available in:

```text
images/dashboard/
```

---

## 🔧 Hardware Components

| Component | Purpose |
|---|---|
| **ESP32** | Main microcontroller and processing unit |
| **MPU6050** | Accelerometer and gyroscope for posture sensing |
| **Vibration Motor** | Provides tactile feedback |
| **BC547 Transistor** | Drives the vibration motor |
| **Diode** | Protects the circuit from motor back-EMF |
| **Resistor** | Used in the transistor interface |
| **Capacitor** | Helps reduce power fluctuations and electrical noise |
| **USB Cable** | Programming and power |
| **Custom PCB** | Hardware integration |

---



## 🧩 MPU6050

The MPU6050 is a 6-axis inertial measurement unit containing:

- 3-axis accelerometer
- 3-axis gyroscope

In this project, the sensor is used to obtain body-orientation
information for posture monitoring.

The sensor communicates with the ESP32 using:
I²C

The ESP32 uses the sensor readings to calculate the user's pitch angle
and determine posture deviation from the calibrated reference.

---

## 🔌 Circuit and Vibration Driver

The vibration motor is not driven directly from the ESP32 GPIO.
Instead, a transistor driver stage is used.

```text
             ESP32 GPIO
                  │
                  ▼
             Base Resistor
                  │
                  ▼
              BC547 NPN
                  │
                  ▼
           Vibration Motor
                  │
                  ▼
               GND

```

A protection diode is included to reduce the effects of the motor's
back-EMF.

The circuit diagram is available at:
hardware/circuit_diagram/circuit_diagram.jpeg

---

## 🖥️ Firmware

The main ESP32 firmware is located at:

```text
firmware/
└── ESP_code_for_monitor.ino
```

The firmware handles:

- MPU6050 initialization
- I²C communication
- 5-second calibration
- Pitch calculation
- Posture deviation calculation
- 15° tolerance comparison
- 40-second bad-posture confirmation
- Vibration control
- Wi-Fi connection
- LittleFS
- Web server
- JSON API
- Serial debugging

---

## 🔌 PCB Design

A custom PCB was designed as part of the project to integrate the
required hardware circuitry.

The PCB resources are organized as:

```text
hardware/
└── pcb/
    ├── BOM/
    ├── Final_PCB/
    ├── Gerber_Posture-Monitor-PCB-1_PCB_Posture-Monitor-PCB-1_2026-10-07/
    └── Schematic/
```

Included PCB Resources

- Bill of Materials
- Schematic
- Final PCB design
- Gerber manufacturing files

The PCB files document the hardware design and manufacturing workflow
of the project.

---

## 📷 Project Images

**Final Setup and Prototype**
Images of the physical implementation are available in:

`images/final_setup_and_prototype/`

Contents include:

- Final setup
- Hardware prototype
- Project implementation photographs

**Web Dashboard**
Screenshots of the monitoring interface are available in:

`images/dashboard/`

---

## 📊 Posture Detection Logic

The overall detection process can be summarized as:

```text
                  Start
                    │
                    ▼
              Initialize ESP32
                    │
                    ▼
              Initialize MPU6050
                    │
                    ▼
            5-second Calibration
                    │
                    ▼
              Store refPitch
                    │
                    ▼
              Read MPU6050
                    │
                    ▼
             Calculate Pitch
                    │
                    ▼
          Calculate Deviation
                    │
                    ▼
           Deviation > 15°?
              │           │
             NO          YES
              │           │
              ▼           ▼
            GOOD    POTENTIALLY_BAD
                          │
                          ▼
                      Start Timer
                          │
                          ▼
                    40 Seconds?
                     │        │
                    NO       YES
                     │        │
                     ▼        ▼
                 Continue     BAD
                              │
                              ▼
                         Vibrator ON
                              │
                              ▼
                          ~3 Seconds
                              │
                              ▼
                         Vibrator OFF
                              │
                              ▼
                       Continue Monitoring

```

---

## 🎯 Objectives

The project was developed with the following objectives:

- Design a posture monitoring system for desk workers.
- Measure body orientation using an MPU6050.
- Process sensor data using an ESP32.
- Detect incorrect posture.
- Provide vibration-based feedback.
- Reduce unnecessary alerts caused by temporary movements.
- Implement a delayed alert mechanism.
- Improve awareness of proper sitting posture.

---

## 📈 Project Outcomes

The implemented system demonstrates:

- Real-time posture monitoring.
- MPU6050-based angle measurement.
- Automatic posture calibration.
- Reference-based posture deviation detection.
- 15° posture tolerance.
- 40-second delayed bad-posture confirmation.
- Vibration-based tactile feedback.
- ESP32 Wi-Fi connectivity.
- Embedded web-server functionality.
- LittleFS-based web interface.
- Real-time posture data through a JSON endpoint.
- Custom PCB implementation.

---

## 🛠️ Technologies Used

Hardware

- ESP32
- MPU6050
- Vibration Motor
- BC547 Transistor
- Diode
- Resistors
- Capacitor
- Custom PCB
Embedded Software
- Arduino IDE
- Arduino C/C++
- ESP32 Board Package
- Wire.h
- WiFi.h
- ESPAsyncWebServer
- AsyncTCP
- ArduinoJson
- LittleFS
Communication
- I²C
- Wi-Fi
- HTTP
- JSON
Frontend
- HTML
- CSS
- JavaScript

---

## 📁 Repository Structure

```text
Body-Posture-Monitor-Desk-Workers/
│
├── README.md
├── LICENSE
├── .gitignore
│
├── docs/
│
├── firmware/
│   └── ESP_code_for_monitor.ino
│
├── hardware/
│   ├── circuit_diagram/
│   │   └── circuit_diagram.jpeg
│   │
│   └── pcb/
│       ├── BOM/
│       ├── Final_PCB/
│       ├── Gerber_Posture-Monitor-PCB-1_PCB_Posture-Monitor-PCB-1_2026-10-07/
│       └── Schematic/
│
├── images/
│   ├── dashboard/
│   │   ├── Posture_image1.jpeg
│   │   ├── Posture_image2.jpeg
│   │   ├── Posture_image3.jpeg
│   │   └── Posture_image4.jpeg
│   │
│   └── final_setup_and_prototype/
│       ├── final_setup.jpeg
│       ├── final_setup2.jpeg
│       └── prototype.jpeg
│
└── software/
    ├── index.html
    ├── script.js
    └── style.css

```

---

# 📚 Documentation

The `docs/` directory contains the project's detailed documentation
and presentation materials.

### Project Report

The complete project report contains detailed information about the
project, including the problem statement, objectives, methodology,
hardware and software implementation, results, and project outcomes.

### Project Presentation

The project presentation provides a concise overview of the system,
its architecture, implementation, and results.

```text
docs/
├── Body Posture Monitor For Desk Workers (4) [1].pptx
└── Project_report_final(BPM)[1](AutoRecovered)[1].pdf
```

---

## 🏫 Project Information

**Project Title**: Body Posture Monitor for Desk Workers
**Project Type**: Mini Project
**Department**: Electronics and Communication Engineering
**Institution**: KLS Vishwanrao Deshpande Institute of Technology, Haliyal
**University**: Visvesvaraya Technological University (VTU)
**Academic Year**: 2025–2026
**Guide**: Dr. Nagaraj Bhat

---

## 👥 Team Members

- **Srushti Kolekar** — 2VD23EC106
- **Sudeep T. Gotur** — 2VD23EC107
- **Suhas Rotti** — 2VD23EC108
- **Sujata S. Waghamode** — 2VD23EC110

---

## 👨‍💻 My Contribution

My primary contribution to the project focused on the hardware and
embedded-system implementation.

Hardware
- ESP32 and MPU6050 interfacing
- Sensor integration
- Circuit implementation
- Hardware assembly
- Testing and debugging
- Completed the PCB development

Embedded System
- ESP32 firmware development
- MPU6050 sensor data acquisition
- Posture calibration
- Posture-angle calculation
- Posture detection logic
- Bad-posture timing mechanism
- Vibration-alert control

Web Interface
- LittleFS integration
- Posture JSON API
- Integration of the embedded system with the web interface

---

## 📜 License

This project is released under the MIT License.
See the LICENSE file for details.

---

## 👤 Author

Sudeep T. Gotur
Bachelor of Engineering
Electronics and Communication Engineering
KLS Vishwanrao Deshpande Institute of Technology, Haliyal
