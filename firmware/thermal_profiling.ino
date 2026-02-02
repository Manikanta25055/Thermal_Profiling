/*
 * 8-Channel Temperature Profiling System
 *
 * Hardware: ESP32-S3 + 8× DS18B20 Temperature Sensors + MAX485
 * Communication: MODBUS RTU over RS485
 * Student: Manikanta Gonugondla (230906450)
 * Course: ELE 3221 - M&I Lab
 * Date: January 2026
 *
 * Wiring:
 * - GPIO 4: 1-Wire data bus (all 8 DS18B20 sensors) with 4.7k pull-up
 * - GPIO 17 (TX2): MAX485 DI
 * - GPIO 16 (RX2): MAX485 RO
 * - GPIO 5: MAX485 DE/RE (direction control)
 *
 * Required Libraries:
 * - OneWire by Paul Stoffregen
 * - DallasTemperature by Miles Burton
 * - ModbusRTU by Alexander Emelianov
 *
 * Install via Arduino IDE: Tools → Manage Libraries
 */

#include <OneWire.h>
#include <DallasTemperature.h>
#include <ModbusRTU.h>

// ==================== PIN DEFINITIONS ====================
#define ONE_WIRE_BUS 4        // GPIO 4 for DS18B20 data
#define MODBUS_TX 17          // GPIO 17 for RS485 TX
#define MODBUS_RX 16          // GPIO 16 for RS485 RX
#define MODBUS_DE_RE 5        // GPIO 5 for RS485 direction control

// ==================== CONFIGURATION ====================
#define NUM_SENSORS 8         // Total number of DS18B20 sensors
#define MODBUS_SLAVE_ID 1     // MODBUS device ID
#define MODBUS_BAUD 9600      // MODBUS baud rate
#define SAMPLE_INTERVAL 500   // Milliseconds between readings

// ==================== SENSOR GRID LAYOUT ====================
#define GRID_ROWS 2
#define GRID_COLS 4

// ==================== GLOBAL OBJECTS ====================
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
ModbusRTU mb;

// ==================== GLOBAL VARIABLES ====================
float temperatures[NUM_SENSORS];          // Current temperature readings
DeviceAddress sensorAddresses[NUM_SENSORS]; // 64-bit unique addresses
bool sensorsFound[NUM_SENSORS];           // Sensor detection status
int numDetectedSensors = 0;

// Thermal analysis variables
float maxTemp = -127.0;
float minTemp = 127.0;
float avgTemp = 0.0;
int hotspotIndex = -1;
int coldspotIndex = -1;
float tempGradient = 0.0;

// Timing
unsigned long lastSampleTime = 0;

// ==================== MODBUS REGISTER MAP ====================
/*
 * Holding Registers (Function Code 03):
 * 40001-40002: Sensor 0 temp (float, 2 registers)
 * 40003-40004: Sensor 1 temp
 * 40005-40006: Sensor 2 temp
 * 40007-40008: Sensor 3 temp
 * 40009-40010: Sensor 4 temp
 * 40011-40012: Sensor 5 temp
 * 40013-40014: Sensor 6 temp
 * 40015-40016: Sensor 7 temp
 * 40017-40018: Max temp (float)
 * 40019-40020: Min temp (float)
 * 40021-40022: Avg temp (float)
 * 40023: Hotspot sensor index (uint16)
 * 40024: Coldspot sensor index (uint16)
 * 40025-40026: Temperature gradient (float)
 * 40027: Number of sensors detected (uint16)
 */

// ==================== SETUP ====================
void setup() {
  // Initialize serial for debugging
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n========================================");
  Serial.println("  8-CHANNEL THERMAL PROFILING SYSTEM");
  Serial.println("========================================");
  Serial.println("Student: Manikanta Gonugondla");
  Serial.println("Course: ELE 3221 - M&I Lab");
  Serial.println("========================================\n");

  // Initialize 1-Wire bus
  Serial.println("Initializing 1-Wire bus...");
  sensors.begin();

  // Discover sensors on the bus
  discoverSensors();

  // Set temperature resolution (9, 10, 11, or 12 bits)
  // 12-bit gives 0.0625°C resolution with 750ms conversion time
  Serial.println("Setting resolution to 12-bit...");
  sensors.setResolution(12);

  // Initialize MODBUS RTU slave
  Serial.println("Initializing MODBUS RTU...");
  Serial2.begin(MODBUS_BAUD, SERIAL_8E1, MODBUS_RX, MODBUS_TX);
  mb.begin(&Serial2, MODBUS_DE_RE);
  mb.slave(MODBUS_SLAVE_ID);

  // Add MODBUS holding registers (40001-40050)
  for (int i = 0; i < 50; i++) {
    mb.addHreg(40001 + i);
  }

  Serial.println("\n========================================");
  Serial.println("SETUP COMPLETE!");
  Serial.print("Detected sensors: ");
  Serial.println(numDetectedSensors);
  Serial.println("Starting measurements...");
  Serial.println("========================================\n");
}

// ==================== MAIN LOOP ====================
void loop() {
  // Read sensors at fixed interval
  if (millis() - lastSampleTime >= SAMPLE_INTERVAL) {
    lastSampleTime = millis();

    // Request temperature conversion from all sensors
    sensors.requestTemperatures();

    // Read each sensor
    readAllSensors();

    // Perform thermal analysis
    analyzeThermalData();

    // Update MODBUS registers
    updateModbusRegisters();

    // Print to serial monitor
    printTemperatures();
  }

  // Handle MODBUS requests
  mb.task();

  // Small delay to prevent watchdog issues
  delay(10);
}

// ==================== SENSOR DISCOVERY ====================
void discoverSensors() {
  Serial.println("\nSearching for DS18B20 sensors...");

  numDetectedSensors = sensors.getDeviceCount();
  Serial.print("Found ");
  Serial.print(numDetectedSensors);
  Serial.println(" device(s) on 1-Wire bus.");

  if (numDetectedSensors == 0) {
    Serial.println("\n*** ERROR: No sensors found! ***");
    Serial.println("Check wiring:");
    Serial.println("  1. All sensors connected to GPIO 4");
    Serial.println("  2. 4.7k pull-up resistor between GPIO 4 and 3.3V");
    Serial.println("  3. Power (3.3V) and GND connected");
    Serial.println("*** System will not function! ***\n");
    return;
  }

  if (numDetectedSensors > NUM_SENSORS) {
    Serial.print("WARNING: Found ");
    Serial.print(numDetectedSensors);
    Serial.print(" sensors but configured for ");
    Serial.println(NUM_SENSORS);
    Serial.println("Using first 8 sensors only.");
    numDetectedSensors = NUM_SENSORS;
  }

  // Get unique 64-bit address of each sensor
  Serial.println("\nSensor Addresses:");
  for (int i = 0; i < numDetectedSensors; i++) {
    if (sensors.getAddress(sensorAddresses[i], i)) {
      sensorsFound[i] = true;
      Serial.print("  Sensor ");
      Serial.print(i);
      Serial.print(": ");
      printAddress(sensorAddresses[i]);
      Serial.println();
    } else {
      sensorsFound[i] = false;
      Serial.print("  ERROR: Could not get address for sensor ");
      Serial.println(i);
    }
  }
  Serial.println();
}

// ==================== READ ALL SENSORS ====================
void readAllSensors() {
  for (int i = 0; i < numDetectedSensors; i++) {
    if (sensorsFound[i]) {
      float temp = sensors.getTempC(sensorAddresses[i]);

      // Check for valid reading
      if (temp == DEVICE_DISCONNECTED_C || temp < -100.0) {
        Serial.print("ERROR: Sensor ");
        Serial.print(i);
        Serial.println(" disconnected or reading error!");
        temperatures[i] = -127.0; // Error value
      } else {
        temperatures[i] = temp;
      }
    } else {
      temperatures[i] = -127.0; // Sensor not found
    }
  }
}

// ==================== THERMAL ANALYSIS ====================
void analyzeThermalData() {
  // Reset values
  maxTemp = -127.0;
  minTemp = 127.0;
  float tempSum = 0.0;
  int validSensors = 0;

  // Find max, min, and calculate average
  for (int i = 0; i < numDetectedSensors; i++) {
    if (temperatures[i] > -100.0) { // Valid reading
      validSensors++;
      tempSum += temperatures[i];

      if (temperatures[i] > maxTemp) {
        maxTemp = temperatures[i];
        hotspotIndex = i;
      }

      if (temperatures[i] < minTemp) {
        minTemp = temperatures[i];
        coldspotIndex = i;
      }
    }
  }

  // Calculate average and gradient
  if (validSensors > 0) {
    avgTemp = tempSum / validSensors;
    tempGradient = maxTemp - minTemp;
  } else {
    avgTemp = -127.0;
    tempGradient = 0.0;
    hotspotIndex = -1;
    coldspotIndex = -1;
  }
}

// ==================== UPDATE MODBUS REGISTERS ====================
void updateModbusRegisters() {
  // Write sensor temperatures (40001-40016)
  for (int i = 0; i < NUM_SENSORS; i++) {
    int regAddr = 40001 + (i * 2);
    writeFloatToRegisters(regAddr, temperatures[i]);
  }

  // Write thermal analysis results
  writeFloatToRegisters(40017, maxTemp);       // Max temperature
  writeFloatToRegisters(40019, minTemp);       // Min temperature
  writeFloatToRegisters(40021, avgTemp);       // Average temperature
  mb.Hreg(40023, hotspotIndex);               // Hotspot index
  mb.Hreg(40024, coldspotIndex);              // Coldspot index
  writeFloatToRegisters(40025, tempGradient); // Temperature gradient
  mb.Hreg(40027, numDetectedSensors);         // Sensor count
}

// ==================== HELPER: WRITE FLOAT TO MODBUS ====================
void writeFloatToRegisters(uint16_t startAddr, float value) {
  // Convert float to two 16-bit registers
  uint16_t* ptr = (uint16_t*)&value;
  mb.Hreg(startAddr, ptr[0]);
  mb.Hreg(startAddr + 1, ptr[1]);
}

// ==================== DEBUG PRINT ====================
void printTemperatures() {
  Serial.println("╔════════════════════════════════════════════════════╗");
  Serial.println("║        TEMPERATURE GRID (°C)                       ║");
  Serial.println("╠════════════════════════════════════════════════════╣");

  // Print grid
  for (int row = 0; row < GRID_ROWS; row++) {
    Serial.print("║  ");
    for (int col = 0; col < GRID_COLS; col++) {
      int idx = row * GRID_COLS + col;
      if (idx < numDetectedSensors) {
        Serial.print("S");
        Serial.print(idx);
        Serial.print(":");

        if (temperatures[idx] > -100.0) {
          if (temperatures[idx] >= 0) Serial.print(" ");
          Serial.print(temperatures[idx], 2);
        } else {
          Serial.print(" ERR ");
        }
        Serial.print("   ");
      }
    }
    Serial.println("║");
  }

  Serial.println("╠════════════════════════════════════════════════════╣");
  Serial.println("║        THERMAL ANALYSIS                            ║");
  Serial.println("╠════════════════════════════════════════════════════╣");

  // Print analysis
  Serial.print("║  Max: ");
  Serial.print(maxTemp, 2);
  Serial.print(" °C (Sensor ");
  Serial.print(hotspotIndex);
  Serial.println(")                        ║");

  Serial.print("║  Min: ");
  Serial.print(minTemp, 2);
  Serial.print(" °C (Sensor ");
  Serial.print(coldspotIndex);
  Serial.println(")                        ║");

  Serial.print("║  Avg: ");
  Serial.print(avgTemp, 2);
  Serial.println(" °C                                      ║");

  Serial.print("║  Gradient: ");
  Serial.print(tempGradient, 2);
  Serial.println(" °C                                  ║");

  Serial.println("╚════════════════════════════════════════════════════╝");
  Serial.println();
}

// ==================== HELPER: PRINT SENSOR ADDRESS ====================
void printAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++) {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
    if (i < 7) Serial.print(":");
  }
}
