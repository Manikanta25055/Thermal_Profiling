# ESP32-S3 Mini 1 Connection Guide for 4-Channel DS18B20 Thermal Profiling

## ESP32-S3 Mini 1 Pinout Overview

```
                    ESP32-S3 Mini 1
    ┌─────────────────────────────────────┐
    │  USB    GND  3V3  GND  IO46  IO45   │
    │                                      │
    │  IO44  IO43  IO42  IO41  IO40  IO39 │
    │                                      │
    │  IO38  IO37  IO36  IO35  IO34  IO33 │
    │                                      │
    │  IO32  IO31  IO30  IO29  IO28  IO27 │
    │                                      │
    │  IO26  IO25  IO24  IO23  IO22  IO21 │
    │                                      │
    │  IO20  IO19  IO18  IO17  IO16  IO15 │
    │                                      │
    │  IO14  IO13  IO12  IO11  IO10  IO9  │
    │                                      │
    │  IO8   IO7   IO6   IO5   IO4   IO3  │
    │                                      │
    │  IO2   IO1   IO0   RX   TX   GND    │
    └─────────────────────────────────────┘
```

## Recommended GPIO Configuration for This Project

### 1-Wire Data Line (DS18B20 Sensors)
- **GPIO Pin: IO42** (1-Wire data line for all 4 sensors)
- Why IO42: Reliable, good noise immunity, easy accessibility

### RS485 Communication (UART2)
- **TX Pin: IO17** (RS485 DI - Data In)
- **RX Pin: IO18** (RS485 RO - Data Out)
- **Baud Rate: 9600**
- **Format: 8 data bits, Even parity, 1 stop bit (8E1)**

### Power Pins
- **3V3**: Positive supply (for DS18B20 pull-up resistor)
- **GND**: Ground (multiple available)

---

## Complete Wiring Diagram

### DS18B20 Sensor Connections (4 Sensors, All on IO42)

```
Sensor 1 (Red wire)    ─────── 3V3
Sensor 1 (Black wire)  ─────── GND
Sensor 1 (Yellow wire) ─────┬─ IO42 (with 4.7kΩ pull-up to 3V3)
                             │
Sensor 2 (Red wire)    ─────── 3V3
Sensor 2 (Black wire)  ─────── GND
Sensor 2 (Yellow wire) ─────┤
                             │
Sensor 3 (Red wire)    ─────── 3V3
Sensor 3 (Black wire)  ─────── GND
Sensor 3 (Yellow wire) ─────┤
                             │
Sensor 4 (Red wire)    ─────── 3V3
Sensor 4 (Black wire)  ─────── GND
Sensor 4 (Yellow wire) ─────┴─ IO42
                             │
                        4.7kΩ Resistor
                             │
                            3V3
```

### RS485 Connection (Max485 Module to ESP32-S3 Mini 1)

```
Max485 Module    ESP32-S3 Mini 1
─────────────────────────────────
VCC              3V3
GND              GND
DI (Data In)     IO17 (TX)
RO (Data Out)    IO18 (RX)
A (Signal A)     ─── to USB-to-RS485 Converter (A)
B (Signal B)     ─── to USB-to-RS485 Converter (B)
```

### USB-to-RS485 Converter Connection

```
USB Connector    Max485 Module
────────────────────────────
Power            VCC
GND              GND
A                ─── A
B                ─── B
```

---

## GPIO Summary Table

| Function | GPIO | Pin Type | Purpose |
|----------|------|----------|---------|
| 1-Wire Data | **IO42** | Digital I/O | DS18B20 data line (all 4 sensors) |
| RS485 TX | **IO17** | UART2 TX | Serial transmission to RS485 |
| RS485 RX | **IO18** | UART2 RX | Serial reception from RS485 |
| Power 3V3 | 3V3 | Power | Pull-up resistor supply |
| Ground | GND | Ground | Reference (multiple pins) |

---

## Important Notes

1. **Pull-up Resistor:**
   - 4.7kΩ resistor between IO42 and 3V3
   - Essential for 1-Wire protocol timing

2. **Sensor Configuration:**
   - All 4 DS18B20 sensors on **single GPIO (IO42)**
   - Uses 1-Wire protocol (single data line)
   - Each sensor has unique 64-bit address for identification

3. **UART2 (RS485):**
   - IO17 = UART2_TX
   - IO18 = UART2_RX
   - Hardware serial port (not software serial)

4. **Wire Quality:**
   - Use shielded twisted pair for RS485 (A and B lines)
   - Keep 1-Wire line short (<10m recommended)
   - Keep wires away from high-frequency noise sources

5. **Breadboard Layout:**
   - Place pull-up resistor close to IO42
   - Group sensor connections together
   - Separate power and signal grounds if possible

---

## Connection Checklist

- [ ] DS18B20 Sensor 1 connected (Red to 3V3, Black to GND, Yellow to IO42)
- [ ] DS18B20 Sensor 2 connected (Red to 3V3, Black to GND, Yellow to IO42)
- [ ] DS18B20 Sensor 3 connected (Red to 3V3, Black to GND, Yellow to IO42)
- [ ] DS18B20 Sensor 4 connected (Red to 3V3, Black to GND, Yellow to IO42)
- [ ] 4.7kΩ pull-up resistor between IO42 and 3V3
- [ ] Max485 VCC to 3V3, GND to GND
- [ ] Max485 DI to IO17 (TX)
- [ ] Max485 RO to IO18 (RX)
- [ ] Max485 A to USB-to-RS485 Converter A
- [ ] Max485 B to USB-to-RS485 Converter B
- [ ] USB-to-RS485 Converter connected to Mac via USB

---

## Verification Steps

1. **Visual Check:** Verify all connections match diagram
2. **Power Check:** 3V3 and GND properly connected
3. **Continuity Check:** Test with multimeter if available
4. **USB Check:** Converter detected on Mac (/dev/tty.usbserial-*)

Once connections verified, proceed with firmware upload.
