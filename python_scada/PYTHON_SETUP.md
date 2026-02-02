# Module 3: Python SCADA Setup & Installation

## Overview
The Python SCADA application reads MODBUS data from Arduino R4 via USB-to-RS485 converter and logs to CSV file.

## Prerequisites
- Python 3.7 or higher
- macOS with USB port available
- USB-to-RS485 converter connected to Mac

## Installation Steps

### Step 1: Install Python Dependencies

```bash
cd /Users/manikantagonugondla/Desktop/MIT/MIT/3rd\ Year/6th\ sem/M\&I\ Lab/Thermal_Profiling_Project/python_scada
```

Install required packages:

```bash
pip3 install minimalmodbus==2.1.1
pip3 install pyserial==3.5
pip3 install pandas==1.5.3
pip3 install numpy==1.24.3
pip3 install matplotlib==3.7.1
```

Or install all at once:

```bash
pip3 install minimalmodbus pyserial pandas numpy matplotlib
```

### Step 2: Identify USB Serial Port

Before running the script, identify your USB-to-RS485 converter port:

```bash
ls /dev/tty.* /dev/cu.*
```

Look for:
- `/dev/tty.usbserial-XXXXX` (most common)
- `/dev/cu.usbserial-XXXXX`
- `/dev/tty.CH340-XXXXX` (if CH340C chipset)

Note the device name - you'll need it if the script can't auto-detect.

### Step 3: Verify Arduino R4 is Running Module 2 Firmware

- Arduino R4 must have MODBUS RTU firmware uploaded
- Serial Monitor should show "Waiting for MODBUS requests on RS485..."
- Sensors should be detected and temperatures updating

## Running the Python SCADA

### Basic Usage

```bash
python3 modbus_master.py
```

### Expected Output

```
==================================================
Thermal Profiling System - Python SCADA
Module 3: MODBUS Data Acquisition
==================================================

Available serial ports:
  /dev/tty.usbserial-14240: USB Serial Device

Using port: /dev/tty.usbserial-14240

MODBUS connected to /dev/tty.usbserial-14240 (Slave ID: 1)
Logging started: thermal_data_20250129_143025.csv

Starting data acquisition (press Ctrl+C to stop)...

[Sample 2] Temps: 25.50°C, 24.75°C, 26.25°C, 25.00°C
  Max: 26.25°C | Min: 24.75°C | Avg: 25.38°C | Gradient: 1.50°C | Hotspot: Sensor 2

[Sample 4] Temps: 25.48°C, 24.78°C, 26.23°C, 25.02°C
  Max: 26.23°C | Min: 24.78°C | Avg: 25.38°C | Gradient: 1.45°C | Hotspot: Sensor 2

...
```

Press **Ctrl+C** to stop the application.

### Output

A CSV file is created with timestamp: `thermal_data_YYYYMMDD_HHMMSS.csv`

**CSV Format:**
```
Timestamp,Elapsed_Time,Sensor_0,Sensor_1,Sensor_2,Sensor_3,Max_Temp,Min_Temp,Avg_Temp,Gradient,Hotspot
2025-01-29 14:30:25,0.00,25.50,24.75,26.25,25.00,26.25,24.75,25.38,1.50,2
2025-01-29 14:30:25,0.50,25.48,24.78,26.23,25.02,26.23,24.78,25.38,1.45,2
```

## Troubleshooting

### Problem: "No serial ports found"
**Solutions:**
1. Check USB-to-RS485 converter is plugged in
2. Run: `ls /dev/tty.*` to verify it appears
3. Try different USB port or cable
4. On Mac, may need to install CH340 driver if using that chipset

### Problem: "ERROR: Failed to connect to MODBUS"
**Solutions:**
1. Verify Arduino R4 has Module 2 firmware uploaded
2. Check Serial Monitor shows sensor discovery message
3. Verify D0/D1 connections to Max485 module
4. Check Max485 A/B connected to USB converter
5. Try manually specifying port in script

### Problem: "ERROR: Failed to read temperatures"
**Solutions:**
1. Check RS485 communication cables
2. Verify baud rate is 9600
3. Check Arduino is still running (no crashes)
4. Try disconnecting and reconnecting USB converter

### Problem: CSV file created but no data written
**Solutions:**
1. Check write permissions on directory
2. Verify MODBUS communication is working
3. Check file not already open in another application
4. Try creating log in different directory

## Modifying the Script

### Change Serial Port Manually

Edit `modbus_master.py` and modify the `port` variable in `main()` function:

```python
# Instead of auto-detection
port = "/dev/tty.usbserial-14240"  # Manually specify port
```

### Change Read Interval

Default is 500ms (2 reads per second). To change:

```python
READ_INTERVAL = 0.5  # Change this value (in seconds)
```

### Change Log Filename

```python
logger = DataLogger("my_custom_filename.csv")
```

## Next Steps (Module 4)

Once Python SCADA is logging data successfully:
1. Review CSV file with sample data
2. Create MATLAB script to read and visualize CSV data
3. Implement ML anomaly detection
4. Create real-time MATLAB visualization

---

## System Architecture

```
Arduino R4 (MODBUS Slave)
    ↓
    [Module 2 Firmware]
    ↓
RS485 Communication
    ↓
USB-to-RS485 Converter
    ↓
Mac USB Port
    ↓
Python SCADA (MODBUS Master)
    ↓
    [modbus_master.py]
    ↓
CSV Data File
    ↓
MATLAB/Python Analysis
```

---

## Success Criteria ✓

Module 3 is working when:
- [ ] Script runs without errors
- [ ] MODBUS successfully connects
- [ ] Reads all 4 temperature values
- [ ] CSV file created with data
- [ ] Data samples update every 500ms
- [ ] Thermal analysis values computed correctly
- [ ] Can run for 5+ minutes continuously

**Ready for Module 4 (MATLAB Visualization)**
