# 4-CHANNEL THERMAL PROFILING SYSTEM

**Complete Project Documentation**

Student: Gonugondla Veera Manikanta (230906450)
Course: ELE 3162 - Measurements & Instrumentation Laboratory
Institution: Manipal Institute of Technology
Date: February 2025

---

## QUICK START

### 1. Order Components
See: `hardware/SHOPPING_LINKS.md`
**Cost:** ~₹1,412
**Time:** 3-5 days delivery

### 2. Assemble Hardware
See: `hardware/WIRING_DIAGRAM.md`
**Time:** 30 minutes
**Critical:** Don't forget 4.7kΩ pull-up resistor!

### 3. Upload Firmware
See: `firmware/README.md`
**Time:** 15 minutes
**File:** `firmware/thermal_profiling.ino`

### 4. Install Python SCADA
See: `python_scada/README.md`
**Time:** 10 minutes
```bash
cd python_scada
pip3 install -r requirements.txt
python3 main.py
```

### 5. Test System
See: `documentation/TESTING_CHECKLIST.md`
**Time:** 2 hours
Complete all test phases

### 6. Run Demonstration
See: `PROJECT_SUMMARY.md` → Demo Scenarios
**Time:** 30-60 minutes
Choose: Heat source tracking OR Raspberry Pi profiling

### 7. Write Report
See: `reports/REPORT_OUTLINE.md`
**Time:** 1 week
**Pages:** 15-20 (LaTeX format)

---

## PROJECT OVERVIEW

### What It Does

A multi-point temperature monitoring system that:
- Measures temperature at 8 locations simultaneously
- Creates real-time 2D thermal heatmaps
- Detects hotspots and analyzes thermal gradients
- Logs data to CSV files
- Communicates via industrial MODBUS protocol

### Key Features

**Hardware:**
- 8× DS18B20 digital temperature sensors (±0.5°C accuracy)
- 1-Wire protocol (all sensors on single GPIO pin)
- ESP32-S3 microcontroller
- MODBUS RTU over RS485 communication

**Software:**
- Python SCADA interface (macOS native, PyQt5)
- Real-time 2D heatmap visualization
- 8 individual trend plots (60-second window)
- Thermal analysis (max, min, avg, gradient, hotspot detection)
- CSV data logging with timestamps

### Why This Project

**Advantages:**
- Visually impressive (heatmaps look professional)
- Simple hardware (digital sensors, minimal circuitry)
- Safe (low voltage, no AC mains)
- macOS compatible (no LabVIEW issues)
- Industry relevant (predictive maintenance, thermal management)
- Unique (8-channel profiling rarely done in student projects)

**Course Coverage:**
- CO2: DS18B20 transducer characteristics (25%)
- CO3: 1-Wire digital signal conditioning (35%)
- CO4: MODBUS RTU industrial protocol (30%)
- CO5: Optional PLC-style control logic (10%)
- CO6: Thermal management applications (bonus)

---

## FILE ORGANIZATION

```
Thermal_Profiling_Project/
│
├── README.md                    ← YOU ARE HERE
├── PROJECT_SUMMARY.md           ← Quick reference
│
├── firmware/                    ← ESP32 Arduino code
│   ├── thermal_profiling.ino    ← Main firmware file
│   ├── README.md                ← Firmware setup guide
│   └── libraries_needed.txt     ← Library list
│
├── python_scada/                ← Python SCADA application
│   ├── main.py                  ← Application entry point
│   ├── modbus_client.py         ← MODBUS communication
│   ├── thermal_dashboard.py     ← GUI dashboard
│   ├── data_logger.py           ← CSV logging
│   ├── requirements.txt         ← Python dependencies
│   └── README.md                ← Python setup guide
│
├── hardware/                    ← Component info & wiring
│   ├── COMPONENTS_LIST.md       ← Detailed component specs
│   ├── SHOPPING_LINKS.md        ← Where to buy (India)
│   ├── WIRING_DIAGRAM.md        ← Connection guide
│   └── SENSOR_GRID_LAYOUT.md    ← Physical arrangement
│
├── documentation/               ← Technical documentation
│   ├── MODBUS_REGISTER_MAP.md   ← Register definitions
│   ├── TESTING_CHECKLIST.md     ← Verification procedure
│   └── CALIBRATION_PROCEDURE.md ← Sensor calibration
│
├── testing/                     ← Test data storage
│   ├── test_results/            ← CSV log files
│   └── screenshots/             ← Heatmap captures
│
└── reports/                     ← Report writing
    ├── REPORT_OUTLINE.md        ← LaTeX structure
    └── latex_template/          ← Template files
```

---

## SYSTEM ARCHITECTURE

```
┌─────────────────────────────────────────────────────────┐
│                   SENSORS (Physical)                     │
│  ┌────┐ ┌────┐ ┌────┐ ┌────┐ ┌────┐ ┌────┐ ┌────┐ ┌────┐│
│  │ S0 │ │ S1 │ │ S2 │ │ S3 │ │ S4 │ │ S5 │ │ S6 │ │ S7 ││
│  └─┬──┘ └─┬──┘ └─┬──┘ └─┬──┘ └─┬──┘ └─┬──┘ └─┬──┘ └─┬──┘│
│    └──────┴──────┴──────┴──────┴──────┴──────┴──────┘  │
│         DS18B20 Digital Temperature Sensors             │
└──────────────────────────┬──────────────────────────────┘
                           │ 1-Wire Protocol
                           │ (GPIO 4 + 4.7kΩ pull-up)
                           ▼
┌─────────────────────────────────────────────────────────┐
│              ESP32-S3 MICROCONTROLLER                    │
│  ┌─────────────────────────────────────────────────┐   │
│  │  Firmware (Arduino C/C++)                       │   │
│  │  - OneWire library                              │   │
│  │  - DallasTemperature library                    │   │
│  │  - Temperature reading (500ms interval)         │   │
│  │  - Thermal analysis (max/min/avg/gradient)      │   │
│  │  - MODBUS RTU Slave (ID=1, 9600 baud, 8E1)     │   │
│  └─────────────────────────────────────────────────┘   │
└──────────────────────────┬──────────────────────────────┘
                           │ MODBUS RTU
                           │ (UART → MAX485 → RS485 bus)
                           ▼
┌─────────────────────────────────────────────────────────┐
│              MacBook Air M2 (macOS)                      │
│  ┌─────────────────────────────────────────────────┐   │
│  │  Python SCADA Application (PyQt5)               │   │
│  │  - MODBUS RTU Master (minimalmodbus)            │   │
│  │  - Real-time heatmap (matplotlib)               │   │
│  │  - Trend plots (pyqtgraph)                      │   │
│  │  - Thermal analysis display                     │   │
│  │  - CSV data logging (pandas)                    │   │
│  └─────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────┘
```

---

## GETTING STARTED

### Prerequisites

**Hardware You Need:**
- MacBook Air M2 (you have this)
- ESP32-S3 DevKitM-1 (you have this)
- 8× DS18B20 sensors (order from Robu.in)
- 1× MAX485 module (₹60)
- 1× USB-to-RS485 adapter (₹350)
- 1× 4.7kΩ resistor (₹2)
- Breadboard and jumper wires (₹150)

**Software You Need:**
- Arduino IDE 2.x (download from arduino.cc)
- Python 3.11+ (pre-installed on macOS)
- Terminal access (built-in on macOS)

### Installation Steps

**1. Clone/Download Project**
```bash
cd ~/Desktop
# Project folder already exists from our conversation
```

**2. Order Components**
```bash
# See hardware/SHOPPING_LINKS.md for links
# Estimated delivery: 3-5 days
# Cost: ~₹1,412
```

**3. Install Arduino IDE**
```bash
# Download from: https://www.arduino.cc/en/software
# Install ESP32 board support and libraries
# See firmware/README.md for details
```

**4. Install Python Dependencies**
```bash
cd python_scada
pip3 install -r requirements.txt
```

**5. Assemble Hardware**
```bash
# Follow hardware/WIRING_DIAGRAM.md
# CRITICAL: Don't forget 4.7kΩ pull-up resistor!
```

**6. Upload Firmware**
```bash
# Open firmware/thermal_profiling.ino in Arduino IDE
# Select board: ESP32S3 Dev Module
# Click Upload
```

**7. Test**
```bash
# Open Serial Monitor (115200 baud)
# Should see: "Found 8 device(s)"
# Touch a sensor, temperature should rise
```

**8. Run Python SCADA**
```bash
cd python_scada
python3 main.py
# Select COM port, click Connect
# Heatmap should display temperatures
```

---

## EXPECTED RESULTS

### Measurement Performance
- **Accuracy:** ±0.5°C (DS18B20 specification)
- **Resolution:** 0.0625°C (12-bit mode)
- **Update Rate:** 2 Hz (500ms per full scan)
- **Temperature Range:** -55°C to +125°C

### Thermal Analysis
- **Hotspot Detection:** 100% accurate
- **Gradient Measurement:** ±0.1°C
- **Spatial Resolution:** 5cm (sensor spacing)

### Visualization
- **2D Heatmap:** Real-time color-coded grid
- **3D Surface:** Temperature vs position vs time
- **Trend Charts:** 60-second scrolling window
- **Data Logging:** CSV with timestamps

---

## DEMONSTRATION SCENARIOS

### Demo 1: Moving Heat Source (Easiest)
**Setup Time:** 10 minutes
**Equipment:** Soldering iron or hair dryer
**Procedure:**
1. Arrange sensors in 2×4 grid on cardboard
2. Start Python SCADA, open heatmap view
3. Bring heat source near Sensor 0
4. Watch heatmap cell turn red
5. Move heat source across grid
6. Observe hotspot tracking

**Expected:** Heatmap tracks heat source position in real-time

### Demo 2: Raspberry Pi 5 Thermal Profiling (Most Impressive)
**Setup Time:** 30 minutes
**Equipment:** Raspberry Pi 5 (you have this!)
**Procedure:**
1. Mount sensors on RPi5 (CPU, GPU, RAM, power areas)
2. Record idle baseline (~40°C)
3. Run CPU stress: `stress --cpu 4`
4. Monitor temperature rise (CPU → 65-75°C)
5. Observe thermal propagation in heatmap
6. Stop stress, watch cooldown

**Expected:** Clear hotspot identification, thermal gradient analysis

---

## TROUBLESHOOTING

### No Sensors Detected
**Symptom:** Serial Monitor shows "Found 0 devices"
**Solution:**
1. Check 4.7kΩ pull-up resistor present
2. Verify all sensors connected to GPIO 4
3. Test with single sensor first

### MODBUS Connection Fails
**Symptom:** Python shows "Connection failed"
**Solution:**
1. Check RS485 A/B wiring (try swapping)
2. Verify baud rate: 9600, 8E1
3. Ensure correct COM port selected
4. ESP32 must be powered and running

### Heatmap Not Updating
**Symptom:** Display frozen
**Solution:**
1. Check MODBUS connection status
2. Click Disconnect, then Connect again
3. Restart Python application

**For detailed troubleshooting, see individual README files in each folder.**

---

## PROJECT TIMELINE

**Week 1: Procurement & Assembly**
- Day 1-3: Order components, wait for delivery
- Day 4: Assemble hardware on breadboard
- Day 5-7: Upload firmware, test sensor detection

**Week 2: Software Integration**
- Day 1-3: Install Python, test MODBUS communication
- Day 4-5: Run Python SCADA, verify heatmap
- Day 6-7: Data logging tests, calibration

**Week 3: Demonstrations & Data Collection**
- Day 1-3: Moving heat source demo, collect data
- Day 4-5: Raspberry Pi thermal profiling (if doing)
- Day 6-7: Generate plots, take screenshots

**Week 4: Documentation & Report**
- Day 1-3: Write report sections (theory, design)
- Day 4-5: Results analysis, plots in LaTeX
- Day 6-7: Final review, submission preparation

---

## COURSE OUTCOME ALIGNMENT

### CO2: Transducers (25% weightage)
- **DS18B20 digital temperature transducer**
  - Thermoelectric sensing principle
  - Digital vs analog sensor comparison
  - Multi-sensor array characteristics
  - Unique addressing mechanism

### CO3: Signal Conditioning (35% weightage)
- **1-Wire digital protocol**
  - Pull-up resistor design (4.7kΩ calculation)
  - Digital signal conditioning advantages
  - Noise immunity
  - No amplification/filtering needed

### CO4: Industrial Protocols (30% weightage)
- **MODBUS RTU implementation**
  - Master-slave architecture
  - Register mapping (27 holding registers)
  - RS485 physical layer
  - CRC error detection

### CO5: PLCs (Optional 10% weightage)
- **Control logic**
  - Temperature-based alarm generation
  - Threshold monitoring
  - Can add ladder logic in LabVIEW (if required)

### CO6: Applications (Bonus)
- **Thermal management**
  - Electronics thermal profiling
  - Predictive maintenance concepts
  - Data center monitoring principles

---

## SUPPORT & HELP

### Contact Information
- **Student:** Manikanta Gonugondla
- **Email:** mgonugondlamanikanta@gmail.com
- **GitHub:** github.com/Manikanta25055
- **Portfolio:** helicanexplanations.wordpress.com

### Documentation
- **Quick Start:** PROJECT_SUMMARY.md
- **Hardware:** hardware/WIRING_DIAGRAM.md
- **Firmware:** firmware/README.md
- **Python:** python_scada/README.md
- **Testing:** documentation/TESTING_CHECKLIST.md

### Online Resources
- **DS18B20 Datasheet:** Search "DS18B20 Maxim Integrated PDF"
- **1-Wire Protocol:** https://www.maximintegrated.com/en/design/technical-documents/app-notes/1/126.html
- **MODBUS Specification:** https://modbus.org/docs/Modbus_Application_Protocol_V1_1b3.pdf
- **PyQt5 Documentation:** https://doc.qt.io/qtforpython/

---

## LICENSE

This project is for educational purposes as part of the M&I Laboratory course at Manipal Institute of Technology.

All code and documentation created by Manikanta Gonugondla (2026).

Third-party libraries used:
- OneWire (MIT License)
- DallasTemperature (LGPL)
- ModbusRTU (BSD License)
- PyQt5 (GPL/Commercial)
- Python libraries (various open-source licenses)

---

## ACKNOWLEDGMENTS

- **Course Instructor:** Dr. Arun P Parameswaran
- **Institution:** Manipal Institute of Technology
- **Libraries:** OneWire, DallasTemperature, ModbusRTU, PyQt5, matplotlib, pyqtgraph
- **Hardware:** ESP32 (Espressif), DS18B20 (Maxim Integrated)

---

## VERSION HISTORY

- **v1.0** (January 2026) - Initial release
  - 8-channel thermal profiling
  - MODBUS RTU communication
  - Python SCADA with heatmap
  - Complete documentation

---

## WHAT'S NEXT?

After completing this project, consider:
1. **Publish results** - Blog post or YouTube video
2. **Extend functionality** - Add wireless (WiFi/BLE)
3. **ML integration** - Anomaly detection with TensorFlow
4. **Mobile app** - Android/iOS remote monitoring
5. **Multiple devices** - RS485 network with multiple ESP32 nodes

---

**Ready to build? Start with `hardware/SHOPPING_LINKS.md` to order components!**

**Questions? Check individual README files or contact maintainer.**

**Good luck with your project!**
