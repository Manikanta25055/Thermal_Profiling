# PYTHON SCADA SETUP GUIDE (macOS)

## SYSTEM REQUIREMENTS

- macOS Sequoia 15.x (your system)
- MacBook Air M2 (Apple Silicon)
- Python 3.11 or later
- USB-to-RS485 adapter connected

---

## INSTALLATION

### Step 1: Install Python Dependencies

Open Terminal and navigate to this directory:

```bash
cd "/Users/manikantagonugondla/Desktop/MIT/MIT/3rd Year/6th sem/M&I Lab/Thermal_Profiling_Project/python_scada"
```

Install required libraries:

```bash
pip3 install -r requirements.txt
```

Wait for all libraries to install (~2-3 minutes).

### Step 2: Verify Installation

Test imports:

```bash
python3 -c "import PyQt5; import pyqtgraph; import minimalmodbus; print('All libraries installed!')"
```

Should print: `All libraries installed!`

---

## RUNNING THE APPLICATION

### Step 1: Connect Hardware

1. ESP32 USB-C cable connected to MacBook (for power)
2. USB-RS485 adapter connected to MacBook
3. ESP32 firmware uploaded and running

### Step 2: Find Serial Port

```bash
ls /dev/cu.*
```

Look for:
- `/dev/cu.usbserial-XXXX` (USB-RS485 adapter)
- `/dev/cu.usbmodem-YYYY` (ESP32 programming port)

**Note the RS485 adapter port name!**

### Step 3: Run Application

```bash
python3 main.py
```

The SCADA dashboard window should open.

### Step 4: Connect to ESP32

1. In the dashboard, select COM port from dropdown (the RS485 port)
2. Click "Connect" button
3. Status should change to green "Connected"
4. Temperature displays should start updating

---

## TROUBLESHOOTING

### Problem: "No module named PyQt5"

**Solution:**
```bash
pip3 install PyQt5
```

### Problem: "Permission denied" on serial port

**Solution:**
```bash
sudo chmod 666 /dev/cu.usbserial-XXXX
```
Replace XXXX with your actual port.

### Problem: No ports showing in dropdown

**Solution:**
1. Check USB-RS485 adapter is connected
2. Install drivers if needed
3. Click "Refresh" button in dashboard

### Problem: Connection fails

**Check:**
1. ESP32 is powered on (LED lit)
2. ESP32 firmware uploaded successfully
3. MAX485 A/B wires connected correctly
4. Correct COM port selected
5. Baud rate matches (9600)

---

## FILE DESCRIPTIONS

- **main.py** - Application entry point
- **thermal_dashboard.py** - PyQt5 GUI (main window)
- **modbus_client.py** - MODBUS RTU communication
- **data_logger.py** - CSV data logging
- **requirements.txt** - Python dependencies

---

## FEATURES

### Real-Time Display
- 8 numeric temperature indicators (LCD style)
- Color-coded by temperature (green/orange/red)
- Update rate: 2 Hz (500ms)

### Heatmap Visualization
- 2D color-coded grid (2×4 sensors)
- Auto-scaling colormap
- Numeric values overlaid on heatmap

### Thermal Analysis
- Maximum temperature and hotspot location
- Minimum temperature and coldspot location
- Average temperature
- Temperature gradient (max - min)

### Trend Plotting
- 8 individual sensor trend charts
- Scrolling window (last 60 seconds)
- Real-time updates

### Data Logging
- CSV format with timestamps
- All 8 sensor readings
- Thermal analysis parameters
- Start/stop recording on demand

---

## USAGE TIPS

### Calibration Mode
Before first use, verify accuracy:
1. Place all sensors in ice water (0°C)
2. Record readings
3. Check against reference thermometer

### Demo Mode
For presentations:
1. Touch one sensor with fingers
2. Watch heatmap change color in real-time
3. Observe hotspot detection

### Performance
- For smooth operation, close other heavy applications
- If visualization lags, increase update interval

---

## CUSTOMIZATION

### Change Update Rate

In `thermal_dashboard.py`, find:
```python
self.update_interval = 500  # milliseconds
```

Change to:
- 1000 for slower updates (less CPU)
- 250 for faster updates (more CPU)

### Change Heatmap Colors

In `thermal_dashboard.py`, find:
```python
im = self.heatmap_ax.imshow(..., cmap='hot', ...)
```

Change `cmap` to:
- 'hot' (red/orange/yellow)
- 'coolwarm' (blue to red)
- 'viridis' (purple to yellow)
- 'plasma' (purple to orange)

### Temperature Range

Find:
```python
vmin=20, vmax=60
```

Adjust based on expected temperature range.

---

## DATA LOGGING

### CSV File Format

Generated files: `thermal_data_YYYYMMDD_HHMMSS.csv`

Columns:
- Timestamp (date/time)
- Elapsed_Time_s (seconds from start)
- Sensor_0_C through Sensor_7_C (temperatures)
- Max_Temp_C, Min_Temp_C, Avg_Temp_C
- Hotspot_Index, Coldspot_Index
- Gradient_C

### Opening in Excel

1. Open Excel
2. File → Import → CSV
3. Select logged file
4. Delimiter: Comma
5. All columns should parse correctly

### Plotting in MATLAB

```matlab
data = readtable('thermal_data_20260123_143020.csv');
plot(data.Elapsed_Time_s, data.Sensor_0_C);
xlabel('Time (s)');
ylabel('Temperature (°C)');
title('Sensor 0 Temperature vs Time');
```

---

## KEYBOARD SHORTCUTS

- **Space** - Toggle connect/disconnect
- **L** - Toggle logging
- **Q** - Quit application
- **R** - Refresh port list

---

## NEXT STEPS

1. ✓ Install Python dependencies
2. ✓ Run main.py
3. ✓ Connect to ESP32
4. ✓ Verify temperature readings
5. → Perform calibration tests
6. → Run demonstration scenarios
7. → Collect data for report

See `documentation/TESTING_CHECKLIST.md` for full testing procedure!
