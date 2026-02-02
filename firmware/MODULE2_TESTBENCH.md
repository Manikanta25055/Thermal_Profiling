# Module 2 Testbench: MODBUS RTU Protocol Verification

## Overview
This testbench verifies that the Arduino R4 can:
1. Read all 4 DS18B20 sensors (Module 1 functionality)
2. Pack data into MODBUS holding registers
3. Accept MODBUS RTU requests over RS485
4. Send valid MODBUS RTU responses
5. Transmit temperature and thermal analysis data via MODBUS

## Pre-Upload Checklist

Before uploading Module 2 firmware, verify:

- [ ] Module 1 firmware previously worked (sensors detected and readings valid)
- [ ] Arduino Uno R4 WiFi selected in IDE
- [ ] D3 still has 1-Wire connection with all 4 sensors
- [ ] D0 and D1 connected to Max485 module (RX and TX)
- [ ] Max485 A and B lines connected to USB-to-RS485 converter
- [ ] USB-to-RS485 converter connected to Mac
- [ ] 4.7kΩ pull-up resistor still between D3 and 5V

## Upload Instructions

1. **Copy Module 2 firmware code** to Arduino IDE
2. **Verify code** (Ctrl+Shift+V)
3. **Upload** (Ctrl+U)
4. **Wait** for "Upload complete"

## Serial Monitor Test (Step 1: Verify Sensors Still Working)

**Step 1: Open Serial Monitor**
- Tools → Serial Monitor
- Baud Rate: **115200**

**Step 2: Expected Startup Output**

```
========================================
Thermal Profiling System - Arduino Uno R4 WiFi
Module 2: MODBUS RTU Implementation
========================================

Scanning for DS18B20 sensors on D3...

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
Waiting for MODBUS requests on RS485...
```

**Step 3: Expected Continuous Output Every 500ms**

```
================== THERMAL DATA ==================
Timestamp: 10s

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

MODBUS Holding Registers:
  Reg 0-1: Sensor 0 = 25.50
  Reg 2-3: Sensor 1 = 24.75
  Reg 4-5: Sensor 2 = 26.25
  Reg 6-7: Sensor 3 = 25.00
  Reg 8: Max Temp = 2650
  Reg 9: Min Temp = 2475
  Reg 10: Avg Temp = 2538
  Reg 11: Gradient = 150
  Reg 12: Hotspot = 2
==================================================
```

**If sensors are NOT detected:**
- Check Module 1 connections (same as before)
- Verify 1-Wire communication still working

## MODBUS Test (Step 2: Verify RS485 Communication)

### Test 2.1: Test with Modbus Poll (Recommended for Mac)

**Install Modbus Poll (Free trial available)**

1. Download from: https://www.modbustools.com/modbus_poll.html
2. Install on Mac
3. Configure:
   - Connection: Serial (USB-to-RS485 port)
   - Slave ID: 1
   - Baud Rate: 9600
   - Data Bits: 8
   - Stop Bits: 1
   - Parity: Even
   - Function: Read Holding Registers (03)
   - Start Address: 0
   - Quantity: 13 registers

4. Click "Connect"
5. You should see:
   - Register 0-1: Sensor 0 temperature (float)
   - Register 2-3: Sensor 1 temperature (float)
   - Register 4-5: Sensor 2 temperature (float)
   - Register 6-7: Sensor 3 temperature (float)
   - Register 8: Max temperature × 100
   - Register 9: Min temperature × 100
   - Register 10: Avg temperature × 100
   - Register 11: Thermal gradient × 100
   - Register 12: Hotspot sensor index

### Test 2.2: Test with Python (Alternative)

**Create a test script on Mac:**

```python
#!/usr/bin/env python3
import minimalmodbus
import time

# Configure modbus client
instrument = minimalmodbus.Instrument('/dev/tty.usbserial-XXXXX', 1)
instrument.serial.baudrate = 9600
instrument.serial.bytesize = 8
instrument.serial.parity = minimalmodbus.serial.PARITY_EVEN
instrument.serial.stopbits = 1

print("Reading MODBUS registers...")

while True:
    try:
        # Read registers 0-12 (13 registers)
        registers = instrument.read_registers(0, 13, functioncode=3)

        print("\n=== MODBUS Data ===")
        print(f"Register 8 (Max Temp): {registers[8] / 100:.2f}°C")
        print(f"Register 9 (Min Temp): {registers[9] / 100:.2f}°C")
        print(f"Register 10 (Avg Temp): {registers[10] / 100:.2f}°C")
        print(f"Register 11 (Gradient): {registers[11] / 100:.2f}°C")
        print(f"Register 12 (Hotspot): Sensor {registers[12]}")

        time.sleep(1)

    except Exception as e:
        print(f"Error: {e}")
        time.sleep(2)
```

**Run the script:**
```bash
python3 modbus_test.py
```

## Test Cases

### Test 1: Sensor Detection ✓
- **Action:** Upload and check Serial Monitor startup
- **Expected:** All 4 sensors found with unique addresses
- **Pass Criteria:** "Found 4 sensor(s)" message

### Test 2: Temperature Reading ✓
- **Action:** Observe temperature values every 500ms
- **Expected:** Valid temperature readings (not 0 or error)
- **Pass Criteria:** All sensors show valid °C values

### Test 3: MODBUS Register Update ✓
- **Action:** Watch "MODBUS Holding Registers" section
- **Expected:** Register values update every 500ms
- **Pass Criteria:** Values change as temperatures change

### Test 4: MODBUS Communication ✓
- **Action:** Connect Modbus Poll or Python script
- **Expected:** Successful read of registers
- **Pass Criteria:** Can read all 13 registers without error

### Test 5: MODBUS Data Accuracy ✓
- **Action:** Compare Serial Monitor values with MODBUS register values
- **Expected:**
  - Registers 8-11 are displayed values × 100
  - Register 12 matches hotspot location
- **Pass Criteria:** All values match between Serial Monitor and MODBUS

### Test 6: MODBUS CRC Validation ✓
- **Action:** Monitor Arduino Serial Monitor for CRC errors
- **Expected:** No "MODBUS: CRC Error" messages
- **Pass Criteria:** Clean communication without CRC issues

## Troubleshooting

### Problem: No MODBUS response from Arduino
**Solutions:**
1. Check USB-to-RS485 converter is detected: `ls /dev/tty.*`
2. Verify D0/D1 connections to Max485 (RX/TX)
3. Check Max485 A/B connected to USB converter
4. Verify power to Max485 (VCC to 5V, GND to GND)
5. Try different USB port or USB converter

### Problem: MODBUS CRC errors
**Solutions:**
1. Check baud rate is 9600 in both Arduino and Modbus Poll
2. Verify parity is Even (8E1)
3. Check RS485 cable quality and connections
4. Try shorter cable between Max485 and USB converter

### Problem: Temperature values not updating in MODBUS
**Solutions:**
1. Verify sensors still reading on Serial Monitor
2. Check MODBUS registers updating (visible in Serial Monitor)
3. Verify register addresses (start at 0, read quantity 13)

## MODBUS Register Map

| Register | Name | Data Type | Range | Notes |
|----------|------|-----------|-------|-------|
| 0-1 | Sensor 0 Temp | Float | -10 to +85°C | Two 16-bit registers |
| 2-3 | Sensor 1 Temp | Float | -10 to +85°C | Two 16-bit registers |
| 4-5 | Sensor 2 Temp | Float | -10 to +85°C | Two 16-bit registers |
| 6-7 | Sensor 3 Temp | Float | -10 to +85°C | Two 16-bit registers |
| 8 | Max Temperature | Int16 × 100 | 0-8500 | Divide by 100 for °C |
| 9 | Min Temperature | Int16 × 100 | 0-8500 | Divide by 100 for °C |
| 10 | Avg Temperature | Int16 × 100 | 0-8500 | Divide by 100 for °C |
| 11 | Thermal Gradient | Int16 × 100 | 0-8500 | Divide by 100 for °C |
| 12 | Hotspot Index | Int16 | 0-3 | Sensor number with highest temp |
| 13-15 | Reserved | - | - | For future use |

## Next Steps (Module 3)

Once Module 2 testbench passes:
1. Create Python SCADA application
2. Read MODBUS data from RS485 converter
3. Generate CSV logs
4. Prepare for MATLAB visualization

---

## Success Criteria ✓

Module 2 is complete when:
- [ ] All 4 sensors detected and reading
- [ ] MODBUS registers update every 500ms
- [ ] Can connect to MODBUS slave (Modbus Poll or Python)
- [ ] Can read all 13 registers without errors
- [ ] Register values match Serial Monitor readings
- [ ] No CRC errors in communication
- [ ] System runs stable for 5+ minutes

**Status:** Ready for Module 3 (Python SCADA Development)
