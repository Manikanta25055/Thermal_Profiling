# TESTING CHECKLIST

Complete this checklist to verify your thermal profiling system is working correctly.

---

## PHASE 1: HARDWARE ASSEMBLY (30 minutes)

### Visual Inspection
- [ ] All 8 DS18B20 sensors unpacked and inspected
- [ ] No physical damage to sensor probes or cables
- [ ] ESP32-S3 board intact
- [ ] MAX485 module present
- [ ] 4.7kΩ resistor available (CRITICAL!)

### Power Connections
- [ ] ESP32 3.3V pin connected to breadboard positive rail
- [ ] ESP32 GND pin connected to breadboard negative rail
- [ ] No short circuits between power rails (verify with multimeter)

### 1-Wire Bus Wiring
- [ ] 4.7kΩ resistor connected between GPIO 4 and 3.3V
- [ ] All 8 sensor Yellow wires connected to GPIO 4
- [ ] All 8 sensor Red wires connected to 3.3V rail
- [ ] All 8 sensor Black wires connected to GND rail
- [ ] Connections are firm (not loose)

### MAX485 Wiring
- [ ] MAX485 VCC → ESP32 5V
- [ ] MAX485 GND → ESP32 GND
- [ ] MAX485 DI → ESP32 GPIO 17
- [ ] MAX485 RO → ESP32 GPIO 16
- [ ] MAX485 DE → ESP32 GPIO 5
- [ ] MAX485 RE → ESP32 GPIO 5 (tied with DE)
- [ ] MAX485 A terminal → USB-RS485 A
- [ ] MAX485 B terminal → USB-RS485 B

### Sensor Labeling
- [ ] Each sensor labeled 0-7 with tape or marker
- [ ] Sensor positions documented (which is which)

---

## PHASE 2: FIRMWARE UPLOAD (20 minutes)

### Arduino IDE Setup
- [ ] Arduino IDE 2.x installed on MacBook
- [ ] ESP32 board support installed
- [ ] OneWire library installed (v2.3.7+)
- [ ] DallasTemperature library installed (v3.9.0+)
- [ ] ModbusRTU library installed (v4.1.0+)

### USB Connection
- [ ] ESP32 connected to MacBook via USB-C cable
- [ ] ESP32 power LED illuminated
- [ ] Port visible in Arduino IDE (Tools → Port)

### Code Upload
- [ ] thermal_profiling.ino opened in Arduino IDE
- [ ] Board selected: ESP32S3 Dev Module
- [ ] Correct port selected
- [ ] Code compiles without errors (Verify button)
- [ ] Code uploads successfully (Upload button)
- [ ] Upload completes with "Done uploading" message

---

## PHASE 3: SENSOR DETECTION (15 minutes)

### Serial Monitor Test
- [ ] Serial Monitor opened (Tools → Serial Monitor)
- [ ] Baud rate set to 115200
- [ ] System startup message visible
- [ ] Message shows "Found 8 device(s)" (or expected number)
- [ ] All 8 sensor addresses displayed
- [ ] No "ERROR: No sensors found" message

### Individual Sensor Test
- [ ] Temperature readings displayed in grid format
- [ ] All 8 sensors showing valid readings (not -127.0°C)
- [ ] Room temperature range: ~20-30°C

**Test:** Pinch Sensor 0 with fingers
- [ ] Sensor 0 temperature rises to 30-35°C within 10 seconds
- [ ] Other sensors remain at room temperature
- [ ] Temperature returns to room temp after releasing

**Repeat for all sensors:**
- [ ] Sensor 1 responds to touch
- [ ] Sensor 2 responds to touch
- [ ] Sensor 3 responds to touch
- [ ] Sensor 4 responds to touch
- [ ] Sensor 5 responds to touch
- [ ] Sensor 6 responds to touch
- [ ] Sensor 7 responds to touch

### Thermal Analysis Verification
- [ ] Max/Min temperatures update correctly
- [ ] Hotspot index changes when touching different sensors
- [ ] Average temperature makes sense
- [ ] Gradient increases when creating temperature difference

---

## PHASE 4: PYTHON SETUP (15 minutes)

### Python Environment
- [ ] Python 3.11+ installed on MacBook
- [ ] pip3 command works in Terminal
- [ ] requirements.txt file present

### Library Installation
```bash
pip3 install -r requirements.txt
```
- [ ] PyQt5 installed successfully
- [ ] pyqtgraph installed successfully
- [ ] matplotlib installed successfully
- [ ] numpy installed successfully
- [ ] minimalmodbus installed successfully
- [ ] pyserial installed successfully
- [ ] pandas installed successfully

### Import Test
```bash
python3 -c "import PyQt5, pyqtgraph, minimalmodbus; print('OK')"
```
- [ ] Prints "OK" without errors

---

## PHASE 5: MODBUS COMMUNICATION (20 minutes)

### Port Detection
```bash
ls /dev/cu.*
```
- [ ] USB-RS485 adapter port visible (e.g., /dev/cu.usbserial-XXXX)
- [ ] Port name noted for Python application

### Permission Check
- [ ] Can read from serial port (no permission errors)
- [ ] If needed, permissions set: `sudo chmod 666 /dev/cu.usbserial-XXXX`

### MODBUS Test (Optional: Using QModMaster)
If you have QModMaster installed:
- [ ] QModMaster configured (Slave ID: 1, Baud: 9600, 8E1)
- [ ] Connected to RS485 port
- [ ] Can read registers 40001-40016 (8 sensor temperatures)
- [ ] Values match Serial Monitor readings

### Python SCADA Launch
```bash
python3 main.py
```
- [ ] Dashboard window opens
- [ ] No error messages in Terminal
- [ ] GUI displays correctly (no missing elements)

---

## PHASE 6: PYTHON SCADA VERIFICATION (30 minutes)

### Connection Test
- [ ] COM port dropdown shows RS485 adapter port
- [ ] "Connect" button clickable
- [ ] Click Connect
- [ ] Status changes to "Connected" (green)
- [ ] Temperature displays start updating

### Display Verification
- [ ] All 8 LCD temperature displays showing values
- [ ] Values match Serial Monitor readings (±0.1°C)
- [ ] Displays update every 500ms (approximately)
- [ ] Color coding works (green for normal temps)

### Heatmap Visualization
- [ ] Switch to "2D Heatmap" tab
- [ ] Heatmap displays with color gradient
- [ ] Grid shows 2 rows × 4 columns
- [ ] Numeric values overlaid on heatmap
- [ ] Colors correspond to temperatures

**Test:** Touch sensor in corner (e.g., Sensor 0)
- [ ] That cell turns orange/red in heatmap
- [ ] Surrounding cells remain green/blue
- [ ] Colormap updates in real-time

### Thermal Analysis Display
- [ ] Max temperature shown correctly
- [ ] Min temperature shown correctly
- [ ] Average temperature reasonable
- [ ] Gradient value = max - min
- [ ] Hotspot index matches warmest sensor

### Trend Plots
- [ ] Switch to "Trends" tab
- [ ] 8 individual plots visible
- [ ] Each plot shows scrolling data
- [ ] Lines move from right to left
- [ ] 60-second window displayed

**Test:** Touch sensor again
- [ ] Corresponding plot shows spike
- [ ] Other plots remain flat

---

## PHASE 7: DATA LOGGING (10 minutes)

### Logging Start
- [ ] "Start Logging" button clickable
- [ ] Click button
- [ ] Status changes to "Logging..."
- [ ] Filename displayed (thermal_data_YYYYMMDD_HHMMSS.csv)

### Logging Verification
- [ ] Let run for 60 seconds
- [ ] Click "Stop Logging"
- [ ] Status confirms stop

### CSV File Check
- [ ] Navigate to python_scada directory
- [ ] CSV file present with correct filename
- [ ] Open in Excel or text editor
- [ ] Header row correct (Timestamp, Elapsed_Time_s, Sensor_0_C, etc.)
- [ ] Data rows present (~120 rows for 60 seconds at 2 Hz)
- [ ] Timestamps increment correctly
- [ ] Temperature values make sense

---

## PHASE 8: CALIBRATION (Optional, 60 minutes)

### Ice Water Test (0°C Reference)
**Setup:**
- [ ] Fill container with ice + water
- [ ] Wait 5 minutes for thermal equilibrium
- [ ] Submerge all 8 sensors in ice water
- [ ] Wait 2 minutes for sensors to stabilize

**Measurements:**
- [ ] Record all 8 sensor readings
- [ ] Expected: 0°C ± 1°C
- [ ] Calculate offset error for each sensor

**Results:**
- Sensor 0: ______°C (Error: ______°C)
- Sensor 1: ______°C (Error: ______°C)
- Sensor 2: ______°C (Error: ______°C)
- Sensor 3: ______°C (Error: ______°C)
- Sensor 4: ______°C (Error: ______°C)
- Sensor 5: ______°C (Error: ______°C)
- Sensor 6: ______°C (Error: ______°C)
- Sensor 7: ______°C (Error: ______°C)

### Room Temperature Test (~25°C)
- [ ] Remove sensors from ice water
- [ ] Let sensors return to room temperature (wait 10 minutes)
- [ ] Record all 8 sensor readings
- [ ] Compare with reference thermometer
- [ ] All sensors within ±1°C of each other

### Warm Water Test (40°C)
- [ ] Heat water to 40°C (verify with thermometer)
- [ ] Submerge all sensors
- [ ] Wait for stabilization
- [ ] Record readings
- [ ] Compare with reference thermometer

---

## PHASE 9: DEMONSTRATION SCENARIO (30-60 minutes)

### Choose ONE:

#### Option A: Moving Heat Source
- [ ] Sensors arranged in 2×4 grid on cardboard
- [ ] Heat source available (soldering iron or hair dryer)
- [ ] Heatmap view open
- [ ] Start recording data

**Test:**
- [ ] Bring heat source near Sensor 0 (top-left)
- [ ] Observe heatmap cell turn red
- [ ] Move heat source to Sensor 3 (top-right)
- [ ] Observe hotspot track across grid
- [ ] Remove heat source
- [ ] Observe cooling in heatmap
- [ ] Stop recording
- [ ] Save screenshots of heatmap

#### Option B: Raspberry Pi 5 Thermal Profiling
- [ ] Sensors mounted on RPi5 (CPU, GPU, RAM, power areas)
- [ ] RPi5 idle, sensors stabilized
- [ ] Record baseline temperatures
- [ ] Start data logging

**CPU Stress Test:**
- [ ] Run: `stress --cpu 4 --timeout 300s`
- [ ] Monitor temperature rise in CPU area sensors
- [ ] Observe thermal propagation in heatmap
- [ ] Note peak temperatures
- [ ] Wait for cooldown after test completes
- [ ] Stop logging
- [ ] Save data and screenshots

---

## PHASE 10: FINAL VERIFICATION (10 minutes)

### System Stability
- [ ] Run system for 30 minutes continuously
- [ ] No crashes or freezes
- [ ] Temperature readings remain consistent
- [ ] No sensor disconnections

### Performance
- [ ] GUI responsive (no lag)
- [ ] Update rate steady (~2 Hz)
- [ ] Heatmap animation smooth
- [ ] No excessive CPU usage

### Data Integrity
- [ ] Multiple CSV files generated successfully
- [ ] All files open correctly
- [ ] No corrupted data
- [ ] Timestamps consistent

---

## TROUBLESHOOTING REFERENCE

If any test fails, see:
- Hardware issues: hardware/WIRING_DIAGRAM.md
- Firmware issues: firmware/README.md
- Python issues: python_scada/README.md
- MODBUS issues: documentation/MODBUS_REGISTER_MAP.md

---

## SIGN-OFF

All phases completed successfully:

- [ ] Hardware assembly verified
- [ ] Firmware uploaded and sensors detected
- [ ] Python SCADA running and communicating
- [ ] Data logging functional
- [ ] Calibration performed (optional but recommended)
- [ ] Demonstration scenario completed
- [ ] System ready for project presentation

**Tested by:** ________________________
**Date:** ________________________
**Signature:** ________________________

---

## NEXT STEPS

After completing this checklist:
1. Take photos/videos of working system
2. Collect all data files
3. Generate plots from CSV data
4. Begin writing report (see reports/REPORT_OUTLINE.md)
5. Prepare presentation

**Ready for submission!**
