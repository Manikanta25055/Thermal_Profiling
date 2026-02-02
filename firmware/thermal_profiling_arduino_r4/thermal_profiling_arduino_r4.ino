/*
 * Thermal Profiling System - Arduino Uno R4 WiFi
 * 4-Channel DS18B20 Temperature Sensor Acquisition
 * Module 1: Sensor Reading & Serial Output
 *
 * Author: Manikanta (230906450)
 * Course: M&I Lab (ELE 3162)
 * Date: Jan 2025
 *
 * Hardware:
 * - Arduino Uno R4 WiFi
 * - 4× DS18B20 Digital Temperature Sensors (1-Wire on D3)
 * - Max485 RS485 Module (D0 RX, D1 TX)
 * - 4.7kΩ pull-up resistor on D3 to 5V
 */

// ============================================================================
// LIBRARY INCLUDES
// ============================================================================
#include <OneWire.h>
#include <DallasTemperature.h>

// ============================================================================
// PIN DEFINITIONS
// ============================================================================
#define ONE_WIRE_BUS 3            // D3 - 1-Wire data line for all 4 sensors
#define RS485_TX_PIN 1            // D1 - Serial1 TX (Max485 DI)
#define RS485_RX_PIN 0            // D0 - Serial1 RX (Max485 RO)
#define RS485_BAUDRATE 9600       // MODBUS RTU standard baud rate
#define SERIAL_MONITOR_BAUDRATE 115200

// ============================================================================
// GLOBAL VARIABLES & OBJECTS
// ============================================================================
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Sensor addresses (will be discovered at startup)
DeviceAddress sensorAddresses[4];
uint8_t sensorCount = 0;

// Temperature storage
float temperatures[4] = {0.0, 0.0, 0.0, 0.0};
unsigned long lastReadTime = 0;
const unsigned long READ_INTERVAL = 500;  // Read sensors every 500ms

// Thermal analysis variables
float maxTemp = 0.0;
float minTemp = 0.0;
float avgTemp = 0.0;
float tempGradient = 0.0;
uint8_t hotspotsensorIdx = 0;

// ============================================================================
// SETUP FUNCTION
// ============================================================================
void setup() {
  // Initialize Serial Monitor (for debugging)
  Serial.begin(SERIAL_MONITOR_BAUDRATE);
  delay(500);

  Serial.println("\n\n========================================");
  Serial.println("Thermal Profiling System - Arduino Uno R4 WiFi");
  Serial.println("Module 1: DS18B20 Sensor Reading");
  Serial.println("========================================\n");

  // Initialize 1-Wire bus
  sensors.begin();

  // Discover connected sensors
  discoverSensors();

  // Initialize Serial1 for RS485 (will be used in Module 2)
  Serial1.begin(RS485_BAUDRATE);

  Serial.println("System initialization complete.\n");
}

// ============================================================================
// MAIN LOOP
// ============================================================================
void loop() {
  // Read sensors at specified interval
  if (millis() - lastReadTime >= READ_INTERVAL) {
    lastReadTime = millis();

    // Request temperature conversion from all sensors
    sensors.requestTemperatures();
    delay(100);  // Wait for conversion

    // Read all sensor values
    readAllSensors();

    // Perform thermal analysis
    analyzeThermalData();

    // Display results
    displayResults();
  }
}

// ============================================================================
// FUNCTION: DISCOVER SENSORS
// ============================================================================
void discoverSensors() {
  Serial.println("Scanning for DS18B20 sensors on D3...\n");

  sensorCount = sensors.getDeviceCount();

  if (sensorCount == 0) {
    Serial.println("ERROR: No sensors found!");
    Serial.println("Check connections and pull-up resistor (4.7kΩ to 5V)");
    while (1);  // Halt
  }

  Serial.print("Found ");
  Serial.print(sensorCount);
  Serial.println(" sensor(s):\n");

  // Get address of each sensor
  for (uint8_t i = 0; i < sensorCount && i < 4; i++) {
    if (sensors.getAddress(sensorAddresses[i], i)) {
      Serial.print("Sensor ");
      Serial.print(i);
      Serial.print(": ");
      printAddress(sensorAddresses[i]);
      Serial.println();

      // Display sensor resolution
      Serial.print("  Resolution: ");
      Serial.print(sensors.getResolution(sensorAddresses[i]));
      Serial.println(" bits");
    } else {
      Serial.print("Sensor ");
      Serial.print(i);
      Serial.println(": Address not found");
    }
  }

  Serial.println("\nSensor discovery complete.\n");
}

// ============================================================================
// FUNCTION: PRINT ADDRESS (Helper)
// ============================================================================
void printAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++) {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

// ============================================================================
// FUNCTION: READ ALL SENSORS
// ============================================================================
void readAllSensors() {
  for (uint8_t i = 0; i < sensorCount && i < 4; i++) {
    temperatures[i] = sensors.getTempC(sensorAddresses[i]);

    // Check for sensor errors
    if (temperatures[i] == DEVICE_DISCONNECTED_C) {
      Serial.print("Sensor ");
      Serial.print(i);
      Serial.println(" ERROR: Not responding!");
      temperatures[i] = 0.0;  // Set to 0 if disconnected
    }
  }
}

// ============================================================================
// FUNCTION: ANALYZE THERMAL DATA
// ============================================================================
void analyzeThermalData() {
  if (sensorCount == 0) return;

  // Find max temperature and hotspot location
  maxTemp = temperatures[0];
  hotspotsensorIdx = 0;

  for (uint8_t i = 1; i < sensorCount && i < 4; i++) {
    if (temperatures[i] > maxTemp) {
      maxTemp = temperatures[i];
      hotspotsensorIdx = i;
    }
  }

  // Find min temperature
  minTemp = temperatures[0];
  for (uint8_t i = 1; i < sensorCount && i < 4; i++) {
    if (temperatures[i] < minTemp) {
      minTemp = temperatures[i];
    }
  }

  // Calculate average temperature
  float sum = 0.0;
  for (uint8_t i = 0; i < sensorCount && i < 4; i++) {
    sum += temperatures[i];
  }
  avgTemp = sum / sensorCount;

  // Calculate thermal gradient (max - min)
  tempGradient = maxTemp - minTemp;
}

// ============================================================================
// FUNCTION: DISPLAY RESULTS
// ============================================================================
void displayResults() {
  // Clear screen equivalent (for readability)
  Serial.println("\n================== THERMAL DATA ==================");
  Serial.print("Timestamp: ");
  Serial.print(millis() / 1000);
  Serial.println("s");

  // Individual sensor readings
  Serial.println("\nIndividual Sensor Readings:");
  for (uint8_t i = 0; i < sensorCount && i < 4; i++) {
    Serial.print("  Sensor ");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(temperatures[i], 2);
    Serial.print("°C");

    if (i == hotspotsensorIdx && maxTemp > minTemp) {
      Serial.print(" [HOTSPOT]");
    }
    Serial.println();
  }

  // Thermal analysis
  Serial.println("\nThermal Analysis:");
  Serial.print("  Max Temperature:  ");
  Serial.print(maxTemp, 2);
  Serial.println("°C");

  Serial.print("  Min Temperature:  ");
  Serial.print(minTemp, 2);
  Serial.println("°C");

  Serial.print("  Avg Temperature:  ");
  Serial.print(avgTemp, 2);
  Serial.println("°C");

  Serial.print("  Thermal Gradient: ");
  Serial.print(tempGradient, 2);
  Serial.println("°C");

  Serial.print("  Hotspot Location:  Sensor ");
  Serial.println(hotspotsensorIdx);

  Serial.println("==================================================");
}

// ============================================================================
// UNUSED (For future MODBUS RTU implementation in Module 2)
// ============================================================================
// void updateModbusRegisters() {
//   // Module 2 will implement MODBUS RTU protocol here
//   // Register mapping:
//   // Registers 0-3: Temperature values (float, 2 registers each)
//   // Registers 8-10: Analysis data (max, min, avg)
// }
