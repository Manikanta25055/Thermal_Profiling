# 8-CHANNEL THERMAL PROFILING SYSTEM - PROJECT SUMMARY



---

## PROJECT OVERVIEW

A multi-point temperature monitoring system that creates real-time thermal maps, detects hotspots, and analyzes thermal gradients using 8 digital temperature sensors arranged in a 2D grid.

### Key Features:
- 8× DS18B20 digital temperature sensors (±0.5°C accuracy)
- 1-Wire protocol communication (all sensors on single GPIO)
- ESP32-S3 microcontroller with MODBUS RTU slave
- Python SCADA interface (macOS native - PyQt5)
- Real-time 2D heatmap visualization
- 3D surface plot capabilities
- Individual sensor trend plots
- Thermal gradient analysis
- Hotspot/coldspot detection
- CSV data logging with timestamps

---

## WHY THIS PROJECT IS UNIQUE

1. Multi-sensor integration - 8 simultaneous temperature measurements
2. Visual impact - Heatmaps and 3D thermal visualization
3. Simple hardware - Digital sensors, no complex analog conditioning
4. Safe - Low voltage, no AC mains
5. macOS compatible - Python-based, no LabVIEW issues
6. Practical applications - PCB thermal testing, RPi5 profiling, motor monitoring

---

## TOTAL COST: ₹1,412

### Components Breakdown:
- DS18B20 waterproof sensors (8×) = ₹800
- MAX485 RS485 module = ₹60
- USB-to-RS485 adapter = ₹350
- 4.7kΩ resistor = ₹2
- Breadboard/connectors = ₹200

**You already have:** ESP32-S3, MacBook Air M2

---

## COURSE OUTCOME MAPPING

- CO2: Transducers (25%) - DS18B20 digital temperature sensor
- CO3: Signal Conditioning (35%) - 1-Wire protocol, digital conditioning
- CO4: Industrial Protocols (30%) - MODBUS RTU implementation
- CO5: PLCs (Optional 10%) - Temperature-based control logic
- CO6: Applications (Bonus) - Thermal management applications

---

## TIMELINE: 3-4 WEEKS

Week 1: Hardware setup and testing
Week 2: Firmware and MODBUS communication
Week 3: Python SCADA development
Week 4: Testing, calibration, and report writing

---

## DEMONSTRATION SCENARIOS

1. Moving Heat Source Tracking (easiest)
2. Raspberry Pi 5 Thermal Profiling (most impressive)
3. Temperature Gradient Analysis

---

See individual folders for detailed documentation and code files.
