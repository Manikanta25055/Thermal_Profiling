# ESP32 FIRMWARE SETUP GUIDE

## REQUIRED SOFTWARE

### Arduino IDE 2.x
Download from: https://www.arduino.cc/en/software

**For macOS:**
- Download Arduino IDE 2.x DMG file
- Drag to Applications folder
- Open Arduino IDE

---

## ESP32 BOARD SETUP

### 1. Install ESP32 Board Support

1. Open Arduino IDE
2. Go to **File → Preferences**
3. In "Additional Board Manager URLs", add:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Click OK
5. Go to **Tools → Board → Boards Manager**
6. Search for "esp32"
7. Install **"esp32 by Espressif Systems"** (latest version)
8. Wait for installation to complete

### 2. Select ESP32-S3 Board

1. Go to **Tools → Board → esp32**
2. Select **"ESP32S3 Dev Module"**
3. Configure settings:
   - USB CDC On Boot: Enabled
   - CPU Frequency: 240MHz
   - Flash Size: 8MB
   - Partition Scheme: Default 4MB with spiffs
   - PSRAM: Disabled (or Enabled if you have PSRAM)

---

## INSTALL REQUIRED LIBRARIES

### Method 1: Library Manager (Recommended)

1. Go to **Tools → Manage Libraries** (or Ctrl+Shift+I)
2. Search and install these libraries:

**OneWire by Paul Stoffregen**
- Search: "OneWire"
- Install version 2.3.7 or later
- Purpose: 1-Wire protocol communication

**DallasTemperature by Miles Burton**
- Search: "DallasTemperature"
- Install version 3.9.0 or later
- Purpose: DS18B20 sensor interface

**ModbusRTU by Alexander Emelianov**
- Search: "ModbusRTU"
- Install version 4.1.0 or later
- Purpose: MODBUS RTU protocol

### Method 2: Manual Installation

If Library Manager doesn't work:

1. Download libraries from GitHub:
   - OneWire: https://github.com/PaulStoffregen/OneWire
   - DallasTemperature: https://github.com/milesburton/Arduino-Temperature-Control-Library
   - ModbusRTU: https://github.com/emelianov/modbus-esp8266

2. Extract ZIP files
3. Copy folders to Arduino libraries directory:
   - macOS: `~/Documents/Arduino/libraries/`
4. Restart Arduino IDE

---

## UPLOAD FIRMWARE

### 1. Connect ESP32-S3 to MacBook

1. Use USB-C cable (data + power, not charge-only)
2. Connect ESP32-S3 to MacBook Air M2
3. ESP32 power LED should turn on

### 2. Select Port

1. Go to **Tools → Port**
2. Select port showing ESP32:
   - Typically: `/dev/cu.usbserial-XXXX` or
   - `/dev/cu.usbmodem-XXXX`
3. If no port appears, install drivers (see below)

### 3. Open Sketch

1. In Arduino IDE, go to **File → Open**
2. Navigate to: `Thermal_Profiling_Project/firmware/`
3. Open `thermal_profiling.ino`

### 4. Compile and Upload

1. Click **Verify** button (checkmark) to compile
   - Should see "Compilation complete"
2. Click **Upload** button (arrow)
   - ESP32 will reset and start uploading
   - Progress bar will show upload status
3. Wait for "Done uploading"

**If upload fails:**
- Try holding BOOT button while clicking Upload
- Release BOOT button when upload starts

### 5. Open Serial Monitor

1. Click **Tools → Serial Monitor** (or Ctrl+Shift+M)
2. Set baud rate to **115200**
3. You should see:
   ```
   ========================================
     8-CHANNEL THERMAL PROFILING SYSTEM
   ========================================
   Student: Manikanta Gonugondla
   ...
   Found 8 device(s) on 1-Wire bus.
   ...
   ```

---

## USB DRIVERS (If Needed)

### For CP2102 Chip (Most ESP32 Boards)

**macOS:**
```bash
brew install --cask silicon-labs-vcp-driver
```

Or download from: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers

### For CH340 Chip (Some Clone Boards)

Download from: https://github.com/adrianmihalko/ch340g-ch34g-ch34x-mac-os-x-driver

**Installation:**
1. Download and unzip
2. Run installer
3. Restart MacBook
4. Reconnect ESP32

---

## VERIFY SENSOR DETECTION

After successful upload, Serial Monitor should show:

```
Searching for DS18B20 sensors...
Found 8 device(s) on 1-Wire bus.

Sensor Addresses:
  Sensor 0: 28:AA:BB:CC:DD:EE:FF:00
  Sensor 1: 28:11:22:33:44:55:66:77
  ...
  Sensor 7: 28:77:88:99:AA:BB:CC:DD

========================================
SETUP COMPLETE!
Detected sensors: 8
Starting measurements...
========================================

╔════════════════════════════════════════════════════╗
║        TEMPERATURE GRID (°C)                       ║
╠════════════════════════════════════════════════════╣
║  S0: 25.12   S1: 24.87   S2: 25.31   S3: 25.06   ║
║  S4: 25.18   S5: 24.93   S6: 25.25   S7: 25.00   ║
╠════════════════════════════════════════════════════╣
║        THERMAL ANALYSIS                            ║
╠════════════════════════════════════════════════════╣
║  Max: 25.31 °C (Sensor 2)                        ║
║  Min: 24.87 °C (Sensor 1)                        ║
║  Avg: 25.09 °C                                      ║
║  Gradient: 0.44 °C                                  ║
╚════════════════════════════════════════════════════╝
```

**Test:** Pinch one sensor with your fingers. Temperature should rise to 30-35°C.

---

## TROUBLESHOOTING

### Problem: No sensors found

**Check:**
1. All 8 sensors connected to GPIO 4
2. 4.7kΩ pull-up resistor between GPIO 4 and 3.3V
3. Sensors powered (3.3V or 5V)
4. GND connected

### Problem: Some sensors missing

**Check:**
1. Each sensor's cable for damage/breaks
2. Connections are firm (not loose)
3. Test each sensor individually

### Problem: Upload fails

**Solutions:**
1. Hold BOOT button during upload
2. Check USB cable (use data cable, not charge-only)
3. Install USB drivers
4. Try different USB port
5. Restart Arduino IDE

### Problem: Serial Monitor blank

**Check:**
1. Correct baud rate (115200)
2. Correct port selected
3. Press EN (reset) button on ESP32
4. Close and reopen Serial Monitor

### Problem: Compilation errors

**Solutions:**
1. Verify all libraries installed
2. Check library versions (update if old)
3. Restart Arduino IDE
4. Re-install ESP32 board support

---

## MODBUS REGISTER MAP

For reference when connecting Python SCADA:

| Register | Data Type | Description |
|----------|-----------|-------------|
| 40001-40002 | float | Sensor 0 temperature (°C) |
| 40003-40004 | float | Sensor 1 temperature (°C) |
| 40005-40006 | float | Sensor 2 temperature (°C) |
| 40007-40008 | float | Sensor 3 temperature (°C) |
| 40009-40010 | float | Sensor 4 temperature (°C) |
| 40011-40012 | float | Sensor 5 temperature (°C) |
| 40013-40014 | float | Sensor 6 temperature (°C) |
| 40015-40016 | float | Sensor 7 temperature (°C) |
| 40017-40018 | float | Maximum temperature (°C) |
| 40019-40020 | float | Minimum temperature (°C) |
| 40021-40022 | float | Average temperature (°C) |
| 40023 | uint16 | Hotspot sensor index (0-7) |
| 40024 | uint16 | Coldspot sensor index (0-7) |
| 40025-40026 | float | Temperature gradient (°C) |
| 40027 | uint16 | Number of detected sensors |

**Note:** MODBUS addresses in code are 0-indexed, but protocol uses 1-indexed (40001 = register 0)

---

## CUSTOMIZATION

### Change Sample Rate

In firmware code, find:
```cpp
#define SAMPLE_INTERVAL 500   // Milliseconds between readings
```

Change to:
- 1000 for 1 Hz (slower, less CPU usage)
- 250 for 4 Hz (faster updates)

### Change MODBUS Settings

```cpp
#define MODBUS_SLAVE_ID 1     // Change if using multiple devices
#define MODBUS_BAUD 9600      // 9600, 19200, 38400, 57600, 115200
```

**Note:** Python SCADA must use same baud rate!

### Change Temperature Resolution

```cpp
sensors.setResolution(12);  // 9, 10, 11, or 12 bits
```

| Resolution | Accuracy | Conversion Time |
|------------|----------|-----------------|
| 9-bit | 0.5°C | 93.75 ms |
| 10-bit | 0.25°C | 187.5 ms |
| 11-bit | 0.125°C | 375 ms |
| 12-bit | 0.0625°C | 750 ms |

Higher resolution = more accurate but slower.

---

## NEXT STEPS

1. ✓ Upload firmware to ESP32
2. ✓ Verify sensor detection in Serial Monitor
3. ✓ Test temperature readings (pinch sensor)
4. → Connect MAX485 module
5. → Test MODBUS communication with Python
6. → Run Python SCADA interface

See `python_scada/README.md` for Python setup!
