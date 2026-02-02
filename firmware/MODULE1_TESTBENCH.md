# Module 1 Testbench: DS18B20 Sensor Reading Verification

## Overview
This testbench verifies that the ESP32-S3 Mini can:
1. Detect all 4 DS18B20 sensors on GPIO42
2. Read temperature values correctly
3. Perform thermal analysis (max/min/avg/gradient)
4. Output results via Serial Monitor

## Pre-Upload Checklist

Before uploading firmware, verify:

- [ ] Arduino IDE installed
- [ ] ESP32 board support installed (Tools → Boards Manager → search "ESP32" → install)
- [ ] ESP32-S3-MINI-1 board selected (Tools → Board → ESP32-S3-MINI-1)
- [ ] COM port detected (Tools → Port → should show /dev/tty.usbserial-* or similar)
- [ ] All 4 sensors connected to GPIO42
- [ ] 4.7kΩ pull-up resistor between GPIO42 and 3V3
- [ ] Sensors have power (Red to 3V3, Black to GND)

## Upload Instructions

1. **Copy firmware code** to Arduino IDE
2. **Verify code** (Ctrl+Shift+V or Sketch → Verify)
3. **Upload** (Ctrl+U or Sketch → Upload)
4. **Wait** for "Upload complete" message

## Serial Monitor Test

**Step 1: Open Serial Monitor**
- Tools → Serial Monitor
- Baud Rate: **115200**
- Click "Open"

**Step 2: Expected Output on Startup**

```
========================================
Thermal Profiling System - ESP32-S3 Mini 1
Module 1: DS18B20 Sensor Reading
========================================

Scanning for DS18B20 sensors on GPIO42...

Found 4 sensor(s):

Sensor 0: 28XXXXXXXXXXXXXXXX
  Resolution: 12 bits
Sensor 1: 28YYYYYYYYYYYYYYYY
  Resolution: 12 bits
Sensor 2: 28ZZZZZZZZZZZZZZZZ
  Resolution: 12 bits
Sensor 3: 28AAAAAAAAAAAAAAAA
  Resolution: 12 bits

Sensor discovery complete.

System initialization complete.
```

**If you see "Found 0 sensor(s)":**
- Check GPIO42 connection to sensor data lines
- Verify 4.7kΩ pull-up resistor is present
- Check sensor power (3V3 and GND)
- Try pressing ESP32 reset button

## Real-Time Output Test

**Expected continuous output every 500ms:**

```
================== THERMAL DATA ==================
Timestamp: 5s

Individual Sensor Readings:
  Sensor 0: 25.50°C
  Sensor 1: 24.75°C
  Sensor 2: 26.25°C [HOTSPOT]
  Sensor 3: 25.00°C

Thermal Analysis:
  Max Temperature:  26.25°C
  Min Temperature:  24.75°C
  Avg Temperature:  25.38°C
  Thermal Gradient: 1.50°C
  Hotspot Location:  Sensor 2
==================================================
```

## Test Cases

### Test 1: Sensor Detection ✓
- **Action:** Upload firmware and check initial startup output
- **Expected:** All 4 sensors detected with unique addresses
- **Pass Criteria:** Sees "Found 4 sensor(s)" message

### Test 2: Temperature Reading ✓
- **Action:** Observe sensor readings over 30 seconds
- **Expected:** Stable temperature values (within ±0.5°C variation)
- **Pass Criteria:** All sensors show valid readings (not -127°C or 85°C)

### Test 3: Thermal Analysis ✓
- **Action:** Heat one sensor with your finger/hot water
- **Expected:**
  - Max temperature increases
  - Hotspot location shows that sensor
  - Thermal gradient increases
- **Pass Criteria:** Values change appropriately when heating

### Test 4: Sensor Disconnection ✓
- **Action:** Unplug one sensor while running
- **Expected:**
  - Sensor shows "ERROR: Not responding!"
  - Other sensors continue reading normally
- **Pass Criteria:** System handles disconnection gracefully

### Test 5: Pull-up Verification ✓
- **Action:** Check 1-Wire signal quality
- **Expected:** Clean square wave on GPIO42 (if viewing with scope)
- **Pass Criteria:** Sensors respond within 100ms

## Troubleshooting

### Problem: "Found 0 sensor(s)"
**Solutions:**
1. Verify GPIO42 has all 4 yellow wires connected
2. Check 4.7kΩ resistor between GPIO42 and 3V3
3. Measure voltage on GPIO42: should be ~3.3V at rest
4. Try different GPIO pin (update #define ONE_WIRE_BUS to GPIO41, etc.)

### Problem: Sensor readings stuck at 0°C or -127°C
**Solutions:**
1. Check sensor power connections (3V3 and GND)
2. Verify red and black wires to correct power rails
3. Try sensor in isolation (one sensor first)
4. Check if sensor is Dallas DS18B20 (not clone)

### Problem: Intermittent sensor failures
**Solutions:**
1. Shorten data line (keep GPIO42 to sensors <1m)
2. Add capacitor (0.1µF) between GPIO42 and GND for noise filtering
3. Check for nearby high-frequency noise sources
4. Increase pull-up resistor contact pressure on breadboard

### Problem: Temperature readings changing rapidly
**Solutions:**
1. Add delay in loop (already 500ms interval)
2. Implement moving average (filter last N readings)
3. Check for electrical noise on 3V3 rail
4. Verify breadboard contact quality

## Data Collection for Next Module

Once all tests pass, record this data:

```
Sensor 0 (Location: _____):  Average Temperature: ___°C
Sensor 1 (Location: _____):  Average Temperature: ___°C
Sensor 2 (Location: _____):  Average Temperature: ___°C
Sensor 3 (Location: _____):  Average Temperature: ___°C

Thermal Gradient Range: ___°C to ___°C
Hotspot Frequency: ___% (which sensor most often)
```

## Next Steps (Module 2)

Once Module 1 testbench passes:
1. Implement MODBUS RTU protocol
2. Add RS485 transmission
3. Connect to USB-to-RS485 converter
4. Verify data reception on Mac

---

## Success Criteria ✓

Module 1 is complete when:
- [ ] All 4 sensors detected at startup
- [ ] All sensor readings valid (not error values)
- [ ] Thermal analysis (max/min/avg/gradient) computed correctly
- [ ] Sensor disconnection handled gracefully
- [ ] Output stable over 5+ minutes
- [ ] Hotspot detection works when heating sensors

**Status:** Ready for Module 2 (MODBUS RTU Implementation)
