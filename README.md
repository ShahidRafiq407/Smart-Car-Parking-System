# 🚗 Smart Car Parking System

### By [SMB Robotics](https://github.com/shahidRafiq407)

> An Arduino-based automated car parking system using **2 IR Sensors**, a **Servo Motor** (barrier gate), and a **16x2 I2C LCD Display** to manage vehicle entry/exit and track available parking slots in real-time.

![Project Photo](car%20parking%20system.jpg)

---

## 📋 Table of Contents

- [Features](#-features)
- [Components Required](#-components-required)
- [Circuit Diagram](#-circuit-diagram)
- [Pin Connections](#-pin-connections)
- [How It Works](#-how-it-works)
- [Installation & Upload](#-installation--upload)
- [Libraries Required](#-libraries-required)
- [Serial Monitor Output](#-serial-monitor-output)
- [Customization](#-customization)


---

## Features

| Feature | Description |
|---------|-------------|
| **Automatic Gate Control** | Servo-driven barrier opens/closes on vehicle detection |
| **Dual IR Sensor Detection** | Entry & Exit detection using 2 separate IR sensors |
| **Real-Time Slot Tracking** | Tracks occupied & available slots (configurable up to N slots) |
| **LCD Status Display** | 16x2 I2C LCD shows live parking status |
| **Parking Full Alert** | Denies entry when all slots are occupied |
| **Timeout Protection** | Auto-closes gate if vehicle doesn't cross within 4 seconds |
| **Serial Monitor Logging** | Full debug output via Serial Monitor at 9600 baud |

---

## 🧰 Components Required

| # | Component | Quantity | Description |
|---|-----------|----------|-------------|
| 1 | Arduino Uno | 1 | Microcontroller board |
| 2 | IR Sensor Module | 2 | FC-51 / obstacle avoidance module |
| 3 | SG90 Servo Motor | 1 | 9g micro servo for barrier gate |
| 4 | 16x2 LCD Display (I2C) | 1 | With PCF8574 I2C backpack |
| 5 | Jumper Wires | ~15 | Male-to-Female, Male-to-Male |
| 6 | Breadboard | 1 | Optional, for prototyping |
| 7 | USB Cable (Type-B) | 1 | For Arduino programming |
| 8 | Cardboard / Foam Board | 1 | For parking lot model |

---

## 🔌 Circuit Diagram

### Zone 1 — Sensor Inputs (IR Sensors → Arduino)

```mermaid
graph LR
    IR1["IR Sensor 1\n(Entry)"]
    IR2["IR Sensor 2\n(Exit)"]
    UNO["Arduino Uno"]

    IR1 -- "OUT → Digital Pin 3" --> UNO
    IR1 -. "VCC → 5V" .-> UNO
    IR1 -. "GND → GND" .-> UNO

    IR2 -- "OUT → Digital Pin 4" --> UNO
    IR2 -. "VCC → 5V" .-> UNO
    IR2 -. "GND → GND" .-> UNO

    style IR1 fill:#e65100,stroke:#bf360c,color:#fff,stroke-width:2px
    style IR2 fill:#e65100,stroke:#bf360c,color:#fff,stroke-width:2px
    style UNO fill:#1565c0,stroke:#0d47a1,color:#fff,stroke-width:2px
```

### Zone 2 — Actuator Output (Arduino → Servo Gate)

```mermaid
graph LR
    UNO["Arduino Uno"]
    SERVO["SG90 Servo Motor\n(Barrier Gate)"]

    UNO -- "Digital Pin 9 (PWM) → Signal (Orange)" --> SERVO
    UNO -. "5V → VCC (Red)" .-> SERVO
    UNO -. "GND → GND (Brown)" .-> SERVO

    style UNO fill:#1565c0,stroke:#0d47a1,color:#fff,stroke-width:2px
    style SERVO fill:#2e7d32,stroke:#1b5e20,color:#fff,stroke-width:2px
```

### Zone 3 — Display Output (Arduino → LCD via I2C)

```mermaid
graph LR
    UNO["Arduino Uno"]
    LCD["16x2 I2C LCD\n(PCF8574)"]

    UNO -- "A4 (SDA) → SDA" --> LCD
    UNO -- "A5 (SCL) → SCL" --> LCD
    UNO -. "5V → VCC" .-> LCD
    UNO -. "GND → GND" .-> LCD

    style UNO fill:#1565c0,stroke:#0d47a1,color:#fff,stroke-width:2px
    style LCD fill:#6a1b9a,stroke:#4a148c,color:#fff,stroke-width:2px
```

### Full System Overview

```mermaid
graph LR
    IR1["IR Sensor 1\n(Entry)"]
    IR2["IR Sensor 2\n(Exit)"]
    UNO["Arduino Uno\n(ATmega328P)"]
    SERVO["SG90 Servo\n(Gate)"]
    LCD["16x2 I2C LCD"]

    IR1 -- "OUT → Pin 3" --> UNO
    IR2 -- "OUT → Pin 4" --> UNO
    UNO -- "Pin 9 (PWM) → Signal" --> SERVO
    UNO -- "A4 (SDA)" --> LCD
    UNO -- "A5 (SCL)" --> LCD

    style IR1 fill:#e65100,stroke:#bf360c,color:#fff,stroke-width:2px
    style IR2 fill:#e65100,stroke:#bf360c,color:#fff,stroke-width:2px
    style UNO fill:#1565c0,stroke:#0d47a1,color:#fff,stroke-width:2px
    style SERVO fill:#2e7d32,stroke:#1b5e20,color:#fff,stroke-width:2px
    style LCD fill:#6a1b9a,stroke:#4a148c,color:#fff,stroke-width:2px
```

> **Note:** All components share common **5V** and **GND** rails from the Arduino Uno.

---

### System Flow — Entry & Exit Logic

```mermaid
flowchart LR
    START(["System Start"]) --> INIT["Init LCD\nServo\nSensors"]
    INIT --> READ["Read IR1 & IR2"]

    READ --> E_CHECK{"IR1 = LOW\nIR2 = HIGH"}
    READ --> X_CHECK{"IR2 = LOW\nIR1 = HIGH"}

    E_CHECK -->|"Entry\nDetected"| SLOT{"Slots\nAvailable?"}
    SLOT -->|"Yes"| GATE_E["Open Gate 90°\nPLEASE ENTER"]
    SLOT -->|"No"| FULL["PARKING FULL\nENTRY DENIED"]

    GATE_E --> WAIT_E{"IR2 triggered\nwithin 4s?"}
    WAIT_E -->|"Yes"| INC["slots++\nCAR ENTERED"]
    WAIT_E -->|"No"| TOUT1["TIMEOUT"]

    INC --> CLOSE1["Close Gate 0°"]
    TOUT1 --> CLOSE1
    CLOSE1 --> UPDATE1["Update LCD\nDisplay"]
    UPDATE1 --> READ
    FULL --> READ

    X_CHECK -->|"Exit\nDetected"| CAR{"Cars\nInside?"}
    CAR -->|"Yes"| GATE_X["Open Gate 90°\nPLEASE EXIT"]
    CAR -->|"No"| EMPTY["PARKING EMPTY\nNO CAR INSIDE"]

    GATE_X --> WAIT_X{"IR1 triggered\nwithin 4s?"}
    WAIT_X -->|"Yes"| DEC["slots--\nCAR LEFT"]
    WAIT_X -->|"No"| TOUT2["TIMEOUT"]

    DEC --> CLOSE2["Close Gate 0°"]
    TOUT2 --> CLOSE2
    CLOSE2 --> UPDATE2["Update LCD\nDisplay"]
    UPDATE2 --> READ
    EMPTY --> READ

    style START fill:#1565c0,stroke:#0d47a1,color:#fff
    style FULL fill:#c62828,stroke:#b71c1c,color:#fff
    style EMPTY fill:#e65100,stroke:#bf360c,color:#fff
    style INC fill:#2e7d32,stroke:#1b5e20,color:#fff
    style DEC fill:#2e7d32,stroke:#1b5e20,color:#fff
    style TOUT1 fill:#f57f17,stroke:#e65100,color:#000
    style TOUT2 fill:#f57f17,stroke:#e65100,color:#000
    style GATE_E fill:#00838f,stroke:#006064,color:#fff
    style GATE_X fill:#00838f,stroke:#006064,color:#fff
```

---

## 📌 Pin Connections

### IR Sensor 1 (Entry)

| IR Sensor 1 Pin | Arduino Pin |
|-----------------|-------------|
| VCC | 5V |
| GND | GND |
| OUT | Digital Pin 3 |

### IR Sensor 2 (Exit)

| IR Sensor 2 Pin | Arduino Pin |
|-----------------|-------------|
| VCC | 5V |
| GND | GND |
| OUT | Digital Pin 4 |

### SG90 Servo Motor (Gate)

| Servo Wire | Arduino Pin |
|------------|-------------|
| Red (VCC) | 5V |
| Brown (GND) | GND |
| Orange (Signal) | Digital Pin 9 |

### 16x2 I2C LCD Display

| LCD Pin | Arduino Pin |
|---------|-------------|
| VCC | 5V |
| GND | GND |
| SDA | A4 |
| SCL | A5 |

> **Note:** If your LCD doesn't show anything, try changing the I2C address from `0x27` to `0x3F` in the code.

---

## ⚙️ How It Works

### Entry Process
1. **Vehicle approaches** → IR Sensor 1 (Pin 3) detects the car (`LOW` signal)
2. System checks if parking slots are **available**
3. If available → **Servo opens gate** to 90° and LCD shows `PLEASE ENTER`
4. System waits **4 seconds** for IR Sensor 2 (Pin 4) to trigger (vehicle crossed)
5. If crossed → `occupiedSlots++`, LCD shows `CAR ENTERED SUCCESSFULLY`
6. If not crossed (timeout) → LCD shows `TIMEOUT / CANCEL`
7. Gate closes back to 0° and LCD updates slot count

### Exit Process
1. **Vehicle approaches from inside** → IR Sensor 2 (Pin 4) detects the car (`LOW` signal)
2. System checks if there are **cars inside** (`occupiedSlots > 0`)
3. If yes → **Servo opens gate** to 90° and LCD shows `PLEASE EXIT`
4. System waits **4 seconds** for IR Sensor 1 (Pin 3) to trigger (vehicle crossed)
5. If crossed → `occupiedSlots--`, LCD shows `CAR LEFT SUCCESSFULLY`
6. If not crossed (timeout) → LCD shows `TIMEOUT / CANCEL`
7. Gate closes back to 0° and LCD updates slot count

### Special Cases
- **Parking Full** → Entry denied, LCD shows `PARKING FULL - ENTRY DENIED`
- **Parking Empty** → Exit denied, LCD shows `PARKING EMPTY - NO CAR INSIDE`

---

## Installation & Upload

### Prerequisites
- [Arduino IDE](https://www.arduino.cc/en/software) (v1.8+ or v2.x)
- USB Type-B cable for Arduino Uno

### Steps

1. **Clone this repository**
   ```bash
   git clone https://github.com/shahidrafiq407/smart-car-parking-system.git
   cd smart-car-parking-system
   ```

2. **Install required libraries** (see [Libraries Required](#-libraries-required))

3. **Open the sketch**
   ```
   Open car_parking_system/car_parking_system.ino in Arduino IDE
   ```

4. **Select Board & Port**
   - Board: `Arduino Uno`
   - Port: Select the correct COM port

5. **Upload** → Click the Upload button (→)

6. **Open Serial Monitor** at `9600` baud to see live logs

---

## 📚 Libraries Required

| Library | Install via Arduino IDE |
|---------|------------------------|
| `Servo.h` | ✅ Built-in (no install needed) |
| `Wire.h` | ✅ Built-in (no install needed) |
| `LiquidCrystal_I2C.h` | Install from Library Manager → Search "LiquidCrystal I2C" by Frank de Brabander |

### Installing LiquidCrystal_I2C
1. Open Arduino IDE
2. Go to **Sketch** → **Include Library** → **Manage Libraries**
3. Search for `LiquidCrystal I2C`
4. Install the one by **Frank de Brabander**

---

## Serial Monitor Output

```
--- SMART PARKING SYSTEM READY ---
Total Slots: 4 | Occupied: 0 | Remaining Slots: 4
-------------------------------------------

[ENTRY] First Detection at D3 -> Gate Open!
[SUCCESS] Car Entered successfully!
Total Slots: 4 | Occupied: 1 | Remaining Slots: 3
-------------------------------------------

[EXIT] First Detection at D4 -> Gate Open!
[SUCCESS] Car Left successfully!
Total Slots: 4 | Occupied: 0 | Remaining Slots: 4
-------------------------------------------

[ALERT] Parking FULL! Entry Denied.
```

---

## Customization

| Parameter | Default | Location | Description |
|-----------|---------|----------|-------------|
| `totalSlots` | `4` | Line 14 | Total number of parking slots |
| `irSensor1` | `3` | Line 6 | Entry IR sensor pin |
| `irSensor2` | `4` | Line 7 | Exit IR sensor pin |
| `servoPin` | `9` | Line 8 | Servo motor pin |
| LCD I2C Address | `0x27` | Line 12 | Change to `0x3F` if LCD doesn't work |
| Gate open angle | `90°` | Lines 57, 118 | Servo angle when gate is open |
| Timeout duration | `4000ms` | Lines 63, 124 | Time to wait for vehicle to cross |



---

---

## 👨‍💻 Author

**SMB Robotics**
- Linkedin: [@SMBRobotics](https://www.linkedin.com/in/shahid407)
- Facebook: [@SMBRobotics](https://web.facebook.com/smbrobotics)
- instagram: [@SMBRobotics](https://www.instagram.com/smbrobotics)
- Reddit: [@SMBRobotics](https://www.reddit.com/user/SMB_ROBOTICS)
- youtube: [@SMBRobotics](https://youtube.com/shahidrafiq407)

Made with ❤️ by **SMB Robotics** — Building smart solutions with Arduino & Robotics.

---

> ⭐ If you found this project helpful, please give it a star!
