# Arduino Uno R4 WiFi Connection Guide for 4-Channel DS18B20 Thermal Profiling

## Arduino Uno R4 WiFi Pinout

```
                    Arduino Uno R4 WiFi
    ┌────────────────────────────────────┐
    │  IOREF  RESET  3V3  5V  GND  GND   │
    │                                     │
    │  A0(14) A1(15) A2(16) A3(17) A4(18) A5(19) │
    │                                     │
    │  AREF  GND  D13  D12  D11  D10      │
    │  D9    D8   D7   D6   D5   D4       │
    │  D3    D2   D1   D0   GND   5V      │
    └────────────────────────────────────┘
```

## Recommended GPIO Configuration for This Project

### 1-Wire Data Line (DS18B20 Sensors)
- **Digital Pin: D3 (GPIO3)** - 1-Wire data line for all 4 sensors
- Why D3: Dedicated GPIO, good noise immunity, no conflict with serial

### RS485 Communication (Serial1 on D0/D1)
- **TX Pin: D1** (Serial1 TX to Max485 DI)
- **RX Pin: D0** (Serial1 RX to Max485 RO)
- **Baud Rate: 9600**
- **Format: 8 data bits, Even parity, 1 stop bit (8E1)**

### Power Pins
- **5V**: Positive supply (for sensors and pull-up resistor)
- **GND**: Ground (multiple available)

---

## Complete Wiring Diagram

### DS18B20 Sensor Connections (4 Sensors, All on D3)

```
Sensor 1 (Red wire)    ─────── 5V
Sensor 1 (Black wire)  ─────── GND
Sensor 1 (Yellow wire) ─────┬─ D3 (with 4.7kΩ pull-up to 5V)
                             │
Sensor 2 (Red wire)    ─────── 5V
Sensor 2 (Black wire)  ─────── GND
Sensor 2 (Yellow wire) ─────┤
                             │
Sensor 3 (Red wire)    ─────── 5V
Sensor 3 (Black wire)  ─────── GND
Sensor 3 (Yellow wire) ─────┤
                             │
Sensor 4 (Red wire)    ─────── 5V
Sensor 4 (Black wire)  ─────── GND
Sensor 4 (Yellow wire) ─────┴─ D3
                             │
                        4.7kΩ Resistor
                             │
                            5V
```

### RS485 Connection (Max485 Module to Arduino R4)

```
Max485 Module    Arduino Uno R4 WiFi
─────────────────────────────────
VCC              5V
GND              GND
DI (Data In)     D1 (Serial1 TX)
RO (Data Out)    D0 (Serial1 RX)
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

| Function | Pin | Arduino Name | Purpose |
|----------|-----|--------------|---------|
| 1-Wire Data | D3 | GPIO3 | DS18B20 data line (all 4 sensors) |
| RS485 TX | D1 | Serial1 TX | Serial transmission to RS485 |
| RS485 RX | D0 | Serial1 RX | Serial reception from RS485 |
| Power 5V | 5V | Power | Pull-up resistor + sensor supply |
| Ground | GND | Ground | Reference (multiple pins) |

---

## Important Notes

1. **Pull-up Resistor:**
   - 4.7kΩ resistor between D3 and 5V
   - Essential for 1-Wire protocol timing
   - Arduino R4 uses 5V, not 3.3V like ESP32

2. **Sensor Configuration:**
   - All 4 DS18B20 sensors on **single GPIO (D3)**
   - Uses 1-Wire protocol (single data line)
   - Each sensor has unique 64-bit address for identification

3. **Serial1 (UART1):**
   - D0 = Serial1 RX (receive from RS485)
   - D1 = Serial1 TX (transmit to RS485)
   - Hardware serial port (dedicated pins)
   - D1 and D0 are NOT used for Serial Monitor (which uses D0/D1 internally, but Serial1 uses different pins)

4. **Arduino R4 Advantages:**
   - 5V GPIO (more robust than 3.3V ESP32)
   - 14 GPIO pins (sufficient for this project)
   - Built-in WiFi (bonus feature)
   - Standard Arduino IDE support

5. **Wire Quality:**
   - Use shielded twisted pair for RS485 (A and B lines)
   - Keep 1-Wire line short (<10m recommended)
   - Keep wires away from high-frequency noise sources

---

## Connection Checklist

- [ ] DS18B20 Sensor 1 connected (Red to 5V, Black to GND, Yellow to D3)
- [ ] DS18B20 Sensor 2 connected (Red to 5V, Black to GND, Yellow to D3)
- [ ] DS18B20 Sensor 3 connected (Red to 5V, Black to GND, Yellow to D3)
- [ ] DS18B20 Sensor 4 connected (Red to 5V, Black to GND, Yellow to D3)
- [ ] 4.7kΩ pull-up resistor between D3 and 5V
- [ ] Max485 VCC to 5V, GND to GND
- [ ] Max485 DI to D1 (Serial1 TX)
- [ ] Max485 RO to D0 (Serial1 RX)
- [ ] Max485 A to USB-to-RS485 Converter A
- [ ] Max485 B to USB-to-RS485 Converter B
- [ ] USB-to-RS485 Converter connected to Mac via USB

---

## Verification Steps

1. **Visual Check:** Verify all connections match diagram
2. **Power Check:** 5V and GND properly connected
3. **Continuity Check:** Test with multimeter if available
4. **USB Check:** Converter detected on Mac (/dev/tty.usbserial-*)

Once connections verified, proceed with firmware upload.
