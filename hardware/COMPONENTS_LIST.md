# COMPLETE COMPONENTS LIST

## SENSORS

### DS18B20 Digital Temperature Sensor - 8 pieces
- **Type:** Waterproof probe version (recommended)
- **Specifications:**
  - Voltage: 3.0V - 5.5V
  - Accuracy: ±0.5°C from -10°C to +85°C
  - Resolution: 9 to 12-bit configurable (0.0625°C at 12-bit)
  - Range: -55°C to +125°C
  - Conversion time: 750ms (12-bit mode)
  - Protocol: 1-Wire (Dallas/Maxim)
  - Package: Waterproof stainless steel probe (6mm × 50mm)
  - Cable: 1 meter with 3 wires (Red=VCC, Black=GND, Yellow=Data)
  - Unique 64-bit address per sensor
- **Quantity:** 8
- **Unit Price:** ₹100
- **Total:** ₹800

**Why waterproof probe version?**
- Easy to mount and position
- Can be used for liquid temperature measurement
- More robust than bare TO-92 package
- Pre-wired with color-coded cables

---

## ELECTRONICS COMPONENTS

### 1. Resistors

**4.7kΩ Resistor (1/4W, 5% tolerance) - 1 piece**
- **Purpose:** Pull-up resistor for 1-Wire bus (CRITICAL!)
- **Value:** 4.7kΩ (standard for 1-Wire protocol)
- **Rating:** 1/4W or higher
- **Tolerance:** 5% acceptable
- **Price:** ₹2

**Important:** This resistor is MANDATORY. Without it, 1-Wire communication will fail!

**Optional: Status LEDs (10kΩ resistors) - 8 pieces**
- For visual feedback of which sensor is being read
- 10kΩ current-limiting resistors for LEDs
- Price: ₹1 each = ₹8

---

### 2. Microcontroller

**ESP32-S3 DevKitM-1 - 1 piece**
- **Status:** You already have this!
- **Specifications:**
  - MCU: ESP32-S3 (Xtensa dual-core 32-bit LX7, 240MHz)
  - Flash: 8MB
  - PSRAM: 2MB (optional)
  - GPIO: 36 pins
  - ADC: 2× 12-bit SAR ADCs
  - UART: 3 ports
  - I2C: 2 ports
  - SPI: 3 ports
  - 1-Wire: Supported via GPIO
  - WiFi: 2.4GHz 802.11 b/g/n
  - Bluetooth: BLE 5.0
  - Operating voltage: 3.3V
  - USB: USB-C for programming and power

**GPIO Assignment for this project:**
- GPIO 4: 1-Wire data bus (all 8 sensors)
- GPIO 17: UART TX (MAX485 DI)
- GPIO 16: UART RX (MAX485 RO)
- GPIO 5: RS485 direction control (MAX485 DE/RE)
- 3.3V: Power for sensors and MAX485
- GND: Common ground

---

### 3. Communication Modules

**MAX485 RS485 Module - 1 piece**
- **Purpose:** MODBUS RTU communication (ESP32 ↔ MacBook)
- **Specifications:**
  - IC: MAX485 or equivalent (SN75176)
  - Logic levels: 3.3V/5V compatible
  - Differential outputs: A and B terminals
  - Direction control: DE and RE pins (tie together)
  - Baud rate: Up to 2.5 Mbps (we use 9600 bps)
  - Distance: Up to 1200 meters (theoretically)
- **Pinout:**
  - VCC: 5V power input
  - GND: Ground
  - DI (Driver Input): Connect to ESP32 TX
  - RO (Receiver Output): Connect to ESP32 RX
  - DE (Driver Enable): Connect to ESP32 GPIO 5
  - RE (Receiver Enable): Connect to ESP32 GPIO 5 (tie with DE)
  - A: RS485 A terminal (to USB-RS485 adapter)
  - B: RS485 B terminal (to USB-RS485 adapter)
- **Price:** ₹60

**USB-to-RS485 Adapter - 1 piece**
- **Purpose:** Connect RS485 bus to MacBook Air M2
- **Specifications:**
  - Interface: USB Type-A or USB-C (check MacBook compatibility)
  - Chipset: CH340, FTDI, or CP2102 (macOS drivers available)
  - Terminals: Screw terminals for A and B connections
  - Automatic flow control (some models)
  - Status LEDs: TX/RX indicators
- **macOS Compatibility:** Ensure drivers available
- **Price:** ₹300-400

**Note for MacBook Air M2:**
- M2 has only USB-C ports
- If USB-RS485 adapter has USB-A connector, you'll need USB-A to USB-C adapter (₹150-200)
- OR buy USB-C native RS485 adapter directly

---

### 4. Power Supply Components

**5V Power Supply**
- **Source:** USB cable (use ESP32's USB-C port)
- **Current requirement:**
  - ESP32-S3: ~500mA (with WiFi active)
  - DS18B20 (8×): 8 × 1.5mA = 12mA (idle), ~4mA active each = 32mA
  - MAX485: ~5mA
  - Total: ~550mA
- **USB power (5V/1A) is sufficient**

**Optional: External 5V/1A Power Adapter**
- If using ESP32 standalone (not connected to laptop via USB)
- Barrel jack or USB power supply
- Price: ₹100-150

---

### 5. Prototyping & Mounting

**Breadboard or Prototype PCB**
- **Option A: Breadboard (830 points)**
  - Easy to modify
  - Good for initial testing
  - Price: ₹100

- **Option B: Prototype PCB (7×9 cm)**
  - Permanent soldering
  - More reliable connections
  - Professional appearance
  - Price: ₹50-80

**Recommendation:** Start with breadboard, move to PCB if time permits

**Jumper Wires**
- Male-to-male: 20 pieces
- Male-to-female: 10 pieces (for sensor connections)
- Price: ₹50

**Terminal Blocks (Screw type)**
- For neat sensor wire connections
- 3-pin terminal blocks × 2 (for 8 sensors grouped)
- Price: ₹30

**Double-sided Tape / Hot Glue**
- For mounting sensors on test surface
- Price: ₹20

---

## OPTIONAL COMPONENTS (Enhancements)

### Visual Feedback LEDs
- **3mm or 5mm LEDs** - 8 pieces (different colors if possible)
- **10kΩ Resistors** - 8 pieces (current limiting)
- **Purpose:** Show which sensor is currently being read
- **Price:** ₹50 total

### Enclosure
- **Plastic project box** (15×10×5 cm)
- **Purpose:** Professional appearance, protection
- **Price:** ₹150-300

### Sensor Mounting Grid
- **Acrylic sheet** (20×10 cm, 3mm thick)
- **Purpose:** Fixed sensor grid for consistent spacing
- **Price:** ₹100
- **Alternative:** Cardboard (free!)

---

## TEST EQUIPMENT (You Already Have)

### Analog Discovery 3
- **Purpose:** Circuit validation, signal analysis
- **Not needed for this project** (since using digital sensors)
- **Optional use:** Verify 1-Wire protocol timing with logic analyzer

### ADALM 1000
- **Purpose:** Alternative testing tool
- **Not used in this project**

### Multimeter
- **Purpose:** Verify power supply voltages
- **Essential:** Yes (any basic DMM works)

---

## TESTING LOADS / DEMO MATERIALS

### For Heat Source Demo:
- **Soldering iron** (you already have)
- **OR Hair dryer**
- **OR Small heating element** (₹100)

### For Raspberry Pi 5 Demo:
- **Raspberry Pi 5** (you already have!)
- **Thermal paste** (optional, ₹50)
- **Small heatsinks or thermal tape** (₹100)

### For Calibration:
- **Ice cubes** (free - from kitchen)
- **Insulated container** (use any cup/bottle)
- **Thermometer for reference** (₹100-200)
  - Digital thermometer preferred
  - Range: -10°C to +100°C
  - Accuracy: ±1°C acceptable

---

## TOTAL COST BREAKDOWN

| Category | Item | Quantity | Unit Price | Total |
|----------|------|----------|------------|-------|
| Sensors | DS18B20 waterproof probe | 8 | ₹100 | ₹800 |
| Resistors | 4.7kΩ pull-up | 1 | ₹2 | ₹2 |
| Communication | MAX485 module | 1 | ₹60 | ₹60 |
| Communication | USB-to-RS485 adapter | 1 | ₹350 | ₹350 |
| Prototyping | Breadboard | 1 | ₹100 | ₹100 |
| Wiring | Jumper wires, connectors | - | ₹100 | ₹100 |
| **TOTAL** | | | | **₹1,412** |

### Already Owned (No Cost):
- ESP32-S3 DevKitM-1
- MacBook Air M2
- Analog Discovery 3
- Raspberry Pi 5
- Soldering iron
- Basic tools

### Optional Additions:
- Status LEDs: ₹50
- Enclosure: ₹200
- Reference thermometer: ₹150
- Acrylic mounting grid: ₹100
- **Optional Total:** ₹500

**Grand Total with Optional:** ₹1,912

---

## WHERE TO BUY (India)

### Online Stores:

**Robu.in**
- DS18B20 sensors: Available
- MAX485 modules: Available
- Good quality, fast shipping
- Student discounts sometimes available

**Amazon.in**
- All components available
- Fast delivery (Prime)
- May be slightly expensive

**ElectronicComp.com**
- Wholesale prices
- Good for bulk orders
- Shipping takes 3-5 days

**Semikart.com**
- Electronics components specialist
- Competitive prices

**Local Electronics Markets:**
- SP Road, Bangalore (if nearby)
- Lamington Road, Mumbai
- Nehru Place, Delhi
- Usually cheaper but need to verify quality

---

## COMPONENT VERIFICATION CHECKLIST

Before ordering, verify:

DS18B20 Sensors:
- [ ] Waterproof probe version (not bare IC)
- [ ] 1-meter cable length minimum
- [ ] Pre-wired with 3 wires (Red/Black/Yellow)
- [ ] Genuine Maxim/Dallas chip (check reviews for clones)

MAX485 Module:
- [ ] Module (not bare IC) - easier to use
- [ ] 3.3V compatible (check description)
- [ ] Screw terminals for A/B connections

USB-RS485 Adapter:
- [ ] macOS driver support mentioned
- [ ] USB-C connector OR have USB-A to USB-C adapter
- [ ] Status LEDs for TX/RX (helpful for debugging)

---

## NEXT STEPS AFTER RECEIVING COMPONENTS

1. **Unbox and Inventory**
   - Check all 8 DS18B20 sensors present
   - Verify no physical damage
   - Test continuity of sensor cables

2. **Initial Testing**
   - Connect ONE DS18B20 to ESP32
   - Upload test sketch
   - Verify sensor detection and reading

3. **Full System Assembly**
   - Connect all 8 sensors to 1-Wire bus
   - Add 4.7kΩ pull-up resistor
   - Connect MAX485 module
   - Test MODBUS communication

See documentation/WIRING_DIAGRAM.md for connection details!
