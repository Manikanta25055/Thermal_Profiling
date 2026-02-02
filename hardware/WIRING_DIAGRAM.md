# WIRING DIAGRAM AND CONNECTION GUIDE

## OVERVIEW

This guide shows how to connect all 8 DS18B20 sensors to the ESP32-S3 using the 1-Wire protocol, plus the MAX485 module for MODBUS communication.

---

## 1-WIRE BUS TOPOLOGY

```
                        ESP32-S3
                          3.3V
                            │
                            ├─── 4.7kΩ Pull-up Resistor
                            │
                        GPIO 4 (Data)
                            │
                            ├─── DS18B20 Sensor 0 (Yellow wire)
                            ├─── DS18B20 Sensor 1 (Yellow wire)
                            ├─── DS18B20 Sensor 2 (Yellow wire)
                            ├─── DS18B20 Sensor 3 (Yellow wire)
                            ├─── DS18B20 Sensor 4 (Yellow wire)
                            ├─── DS18B20 Sensor 5 (Yellow wire)
                            ├─── DS18B20 Sensor 6 (Yellow wire)
                            └─── DS18B20 Sensor 7 (Yellow wire)

All Red wires (VCC) ─────────────────────── 3.3V or 5V
All Black wires (GND) ────────────────────── GND
```

**CRITICAL: The 4.7kΩ resistor between Data line and VCC is MANDATORY!**

---

## COMPLETE CONNECTION TABLE

### DS18B20 Sensors to ESP32-S3

| DS18B20 Wire Color | ESP32-S3 Pin | Notes |
|-------------------|--------------|-------|
| **Red** (VCC) | 3.3V or 5V | All 8 sensors connect here |
| **Black** (GND) | GND | All 8 sensors connect here |
| **Yellow** (Data) | GPIO 4 | All 8 sensors connect here + 4.7kΩ to VCC |

**Wiring Method:**
- Twist all 8 Red wires together → Connect to 3.3V rail on breadboard
- Twist all 8 Black wires together → Connect to GND rail on breadboard
- Twist all 8 Yellow wires together → Connect to GPIO 4
- Solder 4.7kΩ resistor between Yellow wire bundle and Red wire bundle

---

### MAX485 Module to ESP32-S3

| MAX485 Pin | ESP32-S3 Pin | Notes |
|------------|--------------|-------|
| VCC | 5V or 3.3V | Check module spec (most work with both) |
| GND | GND | Common ground |
| DI (Driver Input) | GPIO 17 (TX2) | UART transmit from ESP32 |
| RO (Receiver Output) | GPIO 16 (RX2) | UART receive to ESP32 |
| DE (Driver Enable) | GPIO 5 | Direction control |
| RE (Receiver Enable) | GPIO 5 | Tie with DE (same pin) |
| A | RS485 A wire | To USB-RS485 adapter |
| B | RS485 B wire | To USB-RS485 adapter |

**Note:** DE and RE pins are tied together to GPIO 5. This pin controls transmit/receive direction.

---

### USB-RS485 Adapter to MAX485

| USB-RS485 Terminal | MAX485 Pin | Notes |
|-------------------|------------|-------|
| A (or D+) | A | Positive differential |
| B (or D-) | B | Negative differential |
| USB connector | MacBook Air M2 | May need USB-A to USB-C adapter |

**Cable:** Use twisted pair wire for A and B connection (reduces noise)
**Length:** Can be up to 1200m theoretically, but we use <2m for this project

---

## BREADBOARD LAYOUT

```
                    ESP32-S3 DevKitM-1
    ┌───────────────────────────────────────────┐
    │                                           │
    │  3.3V ●────────────────────┐              │
    │                            │              │
    │  GND  ●────────────────┐   │              │
    │                        │   │              │
    │  GPIO 4 ●──────────────┼───┼──────────┐   │
    │                        │   │          │   │
    │  GPIO 5 ●──────────────┼───┼──────┐   │   │
    │                        │   │      │   │   │
    │  GPIO 16 ●─────────────┼───┼──┐   │   │   │
    │                        │   │  │   │   │   │
    │  GPIO 17 ●─────────────┼───┼──┼───┼───┼───┼──┐
    │                        │   │  │   │   │   │  │
    │  5V ●──────────────────┼───┼──┼───┼───┼───┼──┼──┐
    │                        │   │  │   │   │   │  │  │
    └───────────────────────────┼──┼───┼───┼───┼──┼──┼─┘
                                │  │   │   │   │  │  │
                                │  │   │   │   │  │  │
        Breadboard Power Rails: │  │   │   │   │  │  │
        3.3V ●──────────────────┘  │   │   │   │  │  │
                                   │   │   │   │  │  │
        GND  ●─────────────────────┘   │   │   │  │  │
                                       │   │   │  │  │
        Data Bus ●─────────────────────┘   │   │  │  │
             │                             │   │  │  │
             └─── 4.7kΩ resistor ─── 3.3V │   │  │  │
                                           │   │  │  │
    MAX485 Module:                         │   │  │  │
    ┌──────────────┐                       │   │  │  │
    │  VCC ●───────┼───────────────────────┼───┼──┘  │
    │  GND ●───────┼───────────────────────┼───┘     │
    │  DE  ●───────┼───────────────────────┘         │
    │  RE  ●───────┼───────────────────┐             │
    │  DI  ●───────┼───────────────────┼─────────────┘
    │  RO  ●───────┼───────────────────┘
    │  A   ●───────┼──── to USB-RS485 A
    │  B   ●───────┼──── to USB-RS485 B
    └──────────────┘

DS18B20 Sensors (all 8):
Red wires    → 3.3V rail
Black wires  → GND rail
Yellow wires → Data bus (GPIO 4 via 4.7kΩ)
```

---

## STEP-BY-STEP WIRING PROCEDURE

### Step 1: Prepare the Breadboard

1. Insert ESP32-S3 into breadboard (straddle the center channel)
2. Connect ESP32 3.3V pin to breadboard positive rail (red line)
3. Connect ESP32 GND pin to breadboard negative rail (blue/black line)

### Step 2: Connect the 4.7kΩ Pull-Up Resistor

1. Insert 4.7kΩ resistor on breadboard
2. One leg to breadboard row connected to GPIO 4
3. Other leg to 3.3V positive rail
4. This is the 1-Wire data line

### Step 3: Connect DS18B20 Sensors

**For each sensor (repeat 8 times):**
1. Red wire → 3.3V positive rail
2. Black wire → GND negative rail
3. Yellow wire → GPIO 4 row (same row as the resistor)

**Tip:** Use a terminal block or solder a bus wire for cleaner connections

**Label each sensor 0-7 with tape/marker** so you know which is which!

### Step 4: Connect MAX485 Module

1. MAX485 VCC → ESP32 5V pin
2. MAX485 GND → GND rail
3. MAX485 DI → ESP32 GPIO 17
4. MAX485 RO → ESP32 GPIO 16
5. MAX485 DE → ESP32 GPIO 5
6. MAX485 RE → ESP32 GPIO 5 (connect both DE and RE to same pin)

### Step 5: Connect RS485 Bus

1. MAX485 A terminal → USB-RS485 adapter A terminal
2. MAX485 B terminal → USB-RS485 adapter B terminal
3. Use twisted pair wire if available
4. Keep cable length as short as practical (<2m for this project)

### Step 6: Power Connection

**Option A: USB Power (Recommended for Testing)**
1. Connect ESP32 USB-C port to MacBook using USB-C cable
2. ESP32 powers from USB (5V)
3. USB-RS485 adapter connects to another USB port

**Option B: External Power (Optional)**
1. Use 5V DC adapter for ESP32
2. Connect 5V and GND to ESP32 5V and GND pins
3. USB-RS485 still connects to MacBook

---

## SENSOR GRID PHYSICAL LAYOUT

### 2×4 Grid Arrangement

```
Physical Layout (Top View):

    Column 0    Column 1    Column 2    Column 3
     (5cm)       (5cm)       (5cm)       (5cm)

Row 0:  [S0]────[S1]────[S2]────[S3]
         │       │       │       │
        5cm     5cm     5cm     5cm
         │       │       │       │
Row 1:  [S4]────[S5]────[S6]────[S7]

Grid dimensions: 15cm (width) × 5cm (height)
```

**Mounting Methods:**

**Method 1: Cardboard Grid (Easiest)**
1. Cut cardboard 20cm × 10cm
2. Mark grid points with marker
3. Make small holes for sensor probes
4. Push sensors through holes
5. Secure with hot glue or tape on back

**Method 2: Acrylic Sheet (Professional)**
1. Cut acrylic 20cm × 10cm
2. Drill 6mm holes at grid points
3. Insert sensor probes through holes
4. Secure with cable ties or clips

**Method 3: Raspberry Pi 5 Mounting**
For RPi5 thermal profiling:
```
Position sensors on RPi5:
[S0] [S1]     Over CPU/GPU area
[S2] [S3]     Over RAM chips
[S4] [S5]     Over power supply
[S6] [S7]     Over USB/Ethernet

Use thermal tape/paste for good contact
```

---

## VERIFICATION CHECKLIST

Before powering on, verify:

**Power Connections:**
- [ ] ESP32 3.3V pin connected to breadboard positive rail
- [ ] ESP32 GND pin connected to breadboard negative rail
- [ ] ESP32 5V pin connected to MAX485 VCC
- [ ] No short circuits between 3.3V, 5V, and GND

**DS18B20 Connections:**
- [ ] All 8 sensors connected to power rails
- [ ] All 8 yellow data wires connected to GPIO 4 row
- [ ] 4.7kΩ pull-up resistor present between GPIO 4 and 3.3V
- [ ] No loose connections

**MAX485 Connections:**
- [ ] DI connected to GPIO 17
- [ ] RO connected to GPIO 16
- [ ] DE and RE both connected to GPIO 5
- [ ] A and B terminals connected to USB-RS485 adapter
- [ ] Correct polarity (A to A, B to B)

**USB Connections:**
- [ ] ESP32 USB-C cable connected to MacBook (for programming)
- [ ] USB-RS485 adapter connected to MacBook (for MODBUS)

---

## TROUBLESHOOTING COMMON WIRING ISSUES

### Problem: Sensors Not Detected

**Possible Causes:**
1. Missing or wrong value pull-up resistor
   - Solution: Verify 4.7kΩ resistor between GPIO 4 and 3.3V
2. Wrong GPIO pin
   - Solution: Check firmware uses GPIO 4, verify physical connection
3. Poor connections
   - Solution: Check all yellow wires are firmly connected
4. Power supply issue
   - Solution: Verify 3.3V present at sensor red wires (use multimeter)

### Problem: Some Sensors Missing

**Possible Causes:**
1. Faulty sensor
   - Solution: Test each sensor individually
2. Loose connection on that sensor
   - Solution: Re-seat connections, check cable continuity
3. Wrong sensor power voltage
   - Solution: Ensure all sensors get same voltage (3.3V or 5V, not mixed)

### Problem: MODBUS Communication Fails

**Possible Causes:**
1. Wrong RS485 wiring (A and B swapped)
   - Solution: Try swapping A and B connections
2. DE/RE not connected properly
   - Solution: Ensure both DE and RE tied to GPIO 5
3. Wrong GPIO pins for UART
   - Solution: Verify GPIO 16 (RX) and GPIO 17 (TX)
4. Baud rate mismatch
   - Solution: Check both ESP32 and Python use 9600 baud, 8E1

### Problem: Random Reading Errors

**Possible Causes:**
1. Noisy power supply
   - Solution: Add 100µF capacitor across ESP32 power pins
2. Long unshielded cables
   - Solution: Keep sensor cables <2m, use shielded if possible
3. Pull-up resistor too high/low
   - Solution: Use 4.7kΩ (standard value)

---

## WIRE COLOR CODES

### DS18B20 Standard Colors:
- **Red:** VCC (Power +)
- **Black:** GND (Power -)
- **Yellow/White:** Data (1-Wire signal)

**Note:** Some cheap clones may use different colors. Always verify with multimeter before connecting!

### RS485 Cable:
- **A (D+):** Often red, orange, or green
- **B (D-):** Often black, blue, or white

**Important:** RS485 polarity matters! If communication fails, try swapping A and B.

---

## FINAL ASSEMBLY PHOTO CHECKLIST

Take photos for your report showing:
1. [ ] Complete breadboard top view
2. [ ] ESP32 connections close-up
3. [ ] MAX485 module connections
4. [ ] 4.7kΩ pull-up resistor location
5. [ ] Sensor grid physical layout
6. [ ] USB-RS485 adapter connection
7. [ ] Overall system setup

Good documentation helps with debugging and looks professional in your report!

---

## NEXT STEPS

After wiring complete:
1. Upload firmware (see firmware/README.md)
2. Open Serial Monitor to verify sensor detection
3. Test MODBUS communication (see documentation/TESTING_CHECKLIST.md)
4. Run Python SCADA interface
5. Verify heatmap display

See PROJECT_SUMMARY.md for overall workflow!
