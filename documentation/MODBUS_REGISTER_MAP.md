# MODBUS REGISTER MAP

## OVERVIEW

The ESP32 operates as a MODBUS RTU **Slave Device** (ID = 1) with 27 holding registers containing temperature data and thermal analysis results.

**Protocol:** MODBUS RTU over RS485
**Baud Rate:** 9600 bps
**Data Format:** 8 bits, Even parity, 1 stop bit (8E1)
**Slave ID:** 1
**Function Codes:** 0x03 (Read Holding Registers)

---

## REGISTER MAP TABLE

| MODBUS Address | Register Offset | Data Type | Size (regs) | Description | Units | Range |
|----------------|-----------------|-----------|-------------|-------------|-------|-------|
| 40001-40002 | 0-1 | float | 2 | Sensor 0 Temperature | °C | -55 to +125 |
| 40003-40004 | 2-3 | float | 2 | Sensor 1 Temperature | °C | -55 to +125 |
| 40005-40006 | 4-5 | float | 2 | Sensor 2 Temperature | °C | -55 to +125 |
| 40007-40008 | 6-7 | float | 2 | Sensor 3 Temperature | °C | -55 to +125 |
| 40009-40010 | 8-9 | float | 2 | Sensor 4 Temperature | °C | -55 to +125 |
| 40011-40012 | 10-11 | float | 2 | Sensor 5 Temperature | °C | -55 to +125 |
| 40013-40014 | 12-13 | float | 2 | Sensor 6 Temperature | °C | -55 to +125 |
| 40015-40016 | 14-15 | float | 2 | Sensor 7 Temperature | °C | -55 to +125 |
| 40017-40018 | 16-17 | float | 2 | Maximum Temperature | °C | -55 to +125 |
| 40019-40020 | 18-19 | float | 2 | Minimum Temperature | °C | -55 to +125 |
| 40021-40022 | 20-21 | float | 2 | Average Temperature | °C | -55 to +125 |
| 40023 | 22 | uint16 | 1 | Hotspot Sensor Index | - | 0 to 7 |
| 40024 | 23 | uint16 | 1 | Coldspot Sensor Index | - | 0 to 7 |
| 40025-40026 | 24-25 | float | 2 | Temperature Gradient | °C | 0 to 180 |
| 40027 | 26 | uint16 | 1 | Detected Sensor Count | - | 0 to 8 |

**Total Registers:** 27 (40001-40027)

---

## DATA TYPES

### Float (32-bit IEEE 754)
Occupies **2 consecutive registers** (16-bit each):
- **First register:** Low word (bits 0-15)
- **Second register:** High word (bits 16-31)

**Example:** Temperature = 25.31°C
- Binary: 0x41CA7AE1
- Register[n]: 0x7AE1
- Register[n+1]: 0x41CA

### Uint16 (16-bit Unsigned Integer)
Occupies **1 register**:
- Range: 0 to 65535
- Used for indices and counts

---

## SENSOR GRID MAPPING

Physical sensor arrangement to register addresses:

```
Grid Layout (2×4):

    Column 0      Column 1      Column 2      Column 3
Row 0: Sensor 0   Sensor 1      Sensor 2      Sensor 3
       40001-2    40003-4       40005-6       40007-8

Row 1: Sensor 4   Sensor 5      Sensor 6      Sensor 7
       40009-10   40011-12      40013-14      40015-16
```

---

## MODBUS FRAME EXAMPLES

### Reading Single Sensor (Sensor 0)

**Request (Master → Slave):**
```
[01] [03] [00 00] [00 02] [CRC_L] [CRC_H]
 │    │     │       │
 │    │     │       └─ Quantity: 2 registers (float)
 │    │     └───────── Start Address: 0 (40001)
 │    └─────────────── Function Code: 03 (Read Holding)
 └──────────────────── Slave ID: 1
```

**Response (Slave → Master):**
```
[01] [03] [04] [7A E1] [41 CA] [CRC_L] [CRC_H]
 │    │    │     │       │
 │    │    │     │       └─ High word
 │    │    │     └───────── Low word
 │    │    └─────────────── Byte count: 4
 │    └──────────────────── Function Code: 03
 └───────────────────────── Slave ID: 1

Temperature = 0x41CA7AE1 = 25.31°C
```

### Reading All 8 Sensors

**Request:**
```
[01] [03] [00 00] [00 10] [CRC_L] [CRC_H]
                    │
                    └─ Quantity: 16 registers (8 floats)
```

**Response:**
```
[01] [03] [20] [data: 32 bytes] [CRC_L] [CRC_H]
             │
             └─ Byte count: 32 (16 regs × 2 bytes)
```

### Reading Thermal Analysis

**Request (Max, Min, Avg temperatures):**
```
[01] [03] [00 10] [00 06] [CRC_L] [CRC_H]
            │       │
            │       └─ Quantity: 6 registers (3 floats)
            └───────── Start Address: 16 (40017)
```

---

## ERROR CODES

If MODBUS request fails, slave responds with exception:

| Exception Code | Name | Description |
|----------------|------|-------------|
| 0x01 | ILLEGAL_FUNCTION | Unsupported function code |
| 0x02 | ILLEGAL_DATA_ADDRESS | Invalid register address |
| 0x03 | ILLEGAL_DATA_VALUE | Invalid data value |
| 0x04 | SLAVE_DEVICE_FAILURE | Internal error |

**Exception Response Format:**
```
[Slave ID] [Function Code + 0x80] [Exception Code] [CRC]
```

**Example:** Invalid register address
```
[01] [83] [02] [CRC_L] [CRC_H]
      │    │
      │    └─ Exception: Illegal Data Address
      └────── Function 0x03 + 0x80 = 0x83
```

---

## CRC-16 CALCULATION

MODBUS RTU uses CRC-16 (Polynomial: 0xA001)

**Algorithm:**
```c
uint16_t calculateCRC(uint8_t *data, uint8_t length) {
    uint16_t crc = 0xFFFF;
    for (uint8_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}
```

**Important:** CRC bytes are transmitted **LOW byte first, then HIGH byte**

---

## PYTHON ACCESS EXAMPLE

Using `minimalmodbus` library:

```python
import minimalmodbus

# Initialize instrument
instrument = minimalmodbus.Instrument('/dev/cu.usbserial-0001', 1)
instrument.serial.baudrate = 9600
instrument.serial.parity = minimalmodbus.serial.PARITY_EVEN

# Read Sensor 0 (register 0, 2 registers for float)
temp_sensor_0 = instrument.read_float(0, functioncode=3)
print(f"Sensor 0: {temp_sensor_0:.2f}°C")

# Read all 8 sensors (register 0-15, 16 registers total)
# Note: minimalmodbus doesn't support reading multiple floats directly
# Read individually or use register access

# Read hotspot index (register 22, single uint16)
hotspot = instrument.read_register(22, functioncode=3)
print(f"Hotspot at Sensor {hotspot}")
```

---

## TESTING WITH MODBUS TOOLS

### QModMaster (macOS)

1. Download from: https://sourceforge.net/projects/qmodmaster/
2. Install and open
3. Configure:
   - Slave ID: 1
   - Function: Read Holding Registers (0x03)
   - Start Address: 0
   - Quantity: 27
   - Port: /dev/cu.usbserial-XXXX
   - Baud: 9600, 8E1
4. Click "Read"
5. View register values

### ModScan (Windows/Wine)

1. Download from Witte Software
2. Configure serial settings
3. Add devices with register map
4. Monitor in real-time

---

## UPDATE RATE

- **ESP32 samples sensors:** Every 500ms (2 Hz)
- **MODBUS registers updated:** Immediately after sampling
- **Recommended polling rate:** 500ms to 1000ms
- **Maximum polling rate:** 100ms (avoid flooding)

---

## REGISTER PERSISTENCE

- **Volatile:** Registers reset to default on ESP32 reboot
- **No EEPROM storage:** Values not saved across power cycles
- **Initialization values:**
  - All temperatures: -127.0°C (error value)
  - Indices: 0
  - Count: 0 (until sensors detected)

---

## TROUBLESHOOTING

### No Response from Slave

**Check:**
1. RS485 A/B wiring correct
2. Slave ID matches (default: 1)
3. Baud rate matches (9600)
4. Parity setting (Even parity required)
5. ESP32 powered and firmware running

### CRC Errors

**Check:**
1. Cable quality (use shielded twisted pair)
2. Termination resistors (120Ω on long cables)
3. Grounding (common ground between master/slave)

### Invalid Data (NaN or -127.0°C)

**Indicates:**
- Sensor not detected
- Sensor disconnected
- 1-Wire communication failure

**Solution:**
- Check sensor wiring
- Verify 4.7kΩ pull-up present
- Test sensors individually

---

## ADVANCED: MULTIPLE SLAVES

To add more ESP32 devices on same RS485 bus:

1. **Change Slave ID in firmware:**
   ```cpp
   #define MODBUS_SLAVE_ID 2  // or 3, 4, etc.
   ```
2. **Upload to each device**
3. **Query different IDs from Python:**
   ```python
   instrument1 = minimalmodbus.Instrument(port, 1)
   instrument2 = minimalmodbus.Instrument(port, 2)
   ```

**Note:** Only one master polls at a time!

---

## REFERENCES

- **MODBUS Protocol Specification:** https://modbus.org/docs/Modbus_Application_Protocol_V1_1b3.pdf
- **MODBUS RTU Serial Implementation:** https://modbus.org/docs/Modbus_over_serial_line_V1_02.pdf
- **IEEE 754 Float Format:** Standard for binary floating-point arithmetic

---

**For implementation details, see:**
- Firmware code: `firmware/thermal_profiling.ino`
- Python client: `python_scada/modbus_client.py`
