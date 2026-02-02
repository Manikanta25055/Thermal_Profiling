/*
 * Thermal Profiling System - Arduino Uno R4 WiFi
 * 4-Channel DS18B20 Temperature Sensor Acquisition + MODBUS RTU
 * Module 2: MODBUS RTU Protocol Implementation
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
 *
 * Communication:
 * - MODBUS RTU over RS485 at 9600 baud, 8E1
 * - Slave ID: 1
 * - Holding Registers: 0-15 (temperature and analysis data)
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
#define MODBUS_SLAVE_ID 1         // Modbus slave address

// ============================================================================
// MODBUS DEFINITIONS
// ============================================================================
#define MODBUS_FUNCTION_READ_HOLDING 3
#define MODBUS_CRC_HIGH_FIRST
#define MAX_BUFFER 128

// ============================================================================
// GLOBAL VARIABLES & OBJECTS
// ============================================================================
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Sensor addresses
DeviceAddress sensorAddresses[4];
uint8_t sensorCount = 0;

// Temperature storage
float temperatures[4] = {0.0, 0.0, 0.0, 0.0};
unsigned long lastReadTime = 0;
const unsigned long READ_INTERVAL = 500;

// Thermal analysis variables
float maxTemp = 0.0;
float minTemp = 0.0;
float avgTemp = 0.0;
float tempGradient = 0.0;
uint8_t hotspotsensorIdx = 0;

// MODBUS holding registers (16-bit each)
uint16_t holdingRegisters[16];

// MODBUS communication
uint8_t modbusRxBuffer[MAX_BUFFER];
uint8_t modbusTxBuffer[MAX_BUFFER];
uint8_t rxIndex = 0;
unsigned long lastRxTime = 0;

// ============================================================================
// SETUP FUNCTION
// ============================================================================
void setup() {
  // Initialize Serial Monitor (for debugging)
  Serial.begin(SERIAL_MONITOR_BAUDRATE);
  delay(500);

  Serial.println("\n\n========================================");
  Serial.println("Thermal Profiling System - Arduino Uno R4 WiFi");
  Serial.println("Module 2: MODBUS RTU Implementation");
  Serial.println("========================================\n");

  // Initialize 1-Wire bus
  sensors.begin();

  // Discover connected sensors
  discoverSensors();

  // Initialize Serial1 for RS485
  Serial1.begin(RS485_BAUDRATE);

  Serial.println("System initialization complete.");
  Serial.println("Waiting for MODBUS requests on RS485...\n");
}

// ============================================================================
// MAIN LOOP
// ============================================================================
void loop() {
  // Read sensors at specified interval
  if (millis() - lastReadTime >= READ_INTERVAL) {
    lastReadTime = millis();

    // Request temperature conversion
    sensors.requestTemperatures();
    delay(100);

    // Read all sensor values
    readAllSensors();

    // Perform thermal analysis
    analyzeThermalData();

    // Update MODBUS holding registers
    updateModbusRegisters();

    // Display results (debug)
    displayResults();
  }

  // Check for MODBUS requests
  checkModbusRequest();
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
    while (1);
  }

  Serial.print("Found ");
  Serial.print(sensorCount);
  Serial.println(" sensor(s):\n");

  for (uint8_t i = 0; i < sensorCount && i < 4; i++) {
    if (sensors.getAddress(sensorAddresses[i], i)) {
      Serial.print("Sensor ");
      Serial.print(i);
      Serial.print(": ");
      printAddress(sensorAddresses[i]);
      Serial.println();

      Serial.print("  Resolution: ");
      Serial.print(sensors.getResolution(sensorAddresses[i]));
      Serial.println(" bits");
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

    if (temperatures[i] == DEVICE_DISCONNECTED_C) {
      Serial.print("Sensor ");
      Serial.print(i);
      Serial.println(" ERROR: Not responding!");
      temperatures[i] = 0.0;
    }
  }
}

// ============================================================================
// FUNCTION: ANALYZE THERMAL DATA
// ============================================================================
void analyzeThermalData() {
  if (sensorCount == 0) return;

  maxTemp = temperatures[0];
  hotspotsensorIdx = 0;

  for (uint8_t i = 1; i < sensorCount && i < 4; i++) {
    if (temperatures[i] > maxTemp) {
      maxTemp = temperatures[i];
      hotspotsensorIdx = i;
    }
  }

  minTemp = temperatures[0];
  for (uint8_t i = 1; i < sensorCount && i < 4; i++) {
    if (temperatures[i] < minTemp) {
      minTemp = temperatures[i];
    }
  }

  float sum = 0.0;
  for (uint8_t i = 0; i < sensorCount && i < 4; i++) {
    sum += temperatures[i];
  }
  avgTemp = sum / sensorCount;

  tempGradient = maxTemp - minTemp;
}

// ============================================================================
// FUNCTION: UPDATE MODBUS REGISTERS
// ============================================================================
void updateModbusRegisters() {
  // Convert float temperatures to MODBUS format (2 registers per float)
  // Registers 0-1: Sensor 0 (float)
  // Registers 2-3: Sensor 1 (float)
  // Registers 4-5: Sensor 2 (float)
  // Registers 6-7: Sensor 3 (float)
  // Register 8: Max Temperature (int16 * 100)
  // Register 9: Min Temperature (int16 * 100)
  // Register 10: Avg Temperature (int16 * 100)
  // Register 11: Thermal Gradient (int16 * 100)
  // Register 12: Hotspot Sensor Index
  // Register 13-15: Reserved

  // Store temperatures as float (split into 2 uint16 registers)
  for (uint8_t i = 0; i < 4; i++) {
    uint32_t tempBits = floatToUint32(temperatures[i]);
    holdingRegisters[i * 2] = (tempBits >> 16) & 0xFFFF;
    holdingRegisters[i * 2 + 1] = tempBits & 0xFFFF;
  }

  // Store analysis data (scaled by 100 for int16)
  holdingRegisters[8] = (int16_t)(maxTemp * 100);
  holdingRegisters[9] = (int16_t)(minTemp * 100);
  holdingRegisters[10] = (int16_t)(avgTemp * 100);
  holdingRegisters[11] = (int16_t)(tempGradient * 100);
  holdingRegisters[12] = hotspotsensorIdx;
}

// ============================================================================
// FUNCTION: CONVERT FLOAT TO UINT32
// ============================================================================
uint32_t floatToUint32(float value) {
  uint32_t result;
  memcpy(&result, &value, sizeof(float));
  return result;
}

// ============================================================================
// FUNCTION: CONVERT UINT32 TO FLOAT
// ============================================================================
float uint32ToFloat(uint32_t value) {
  float result;
  memcpy(&result, &value, sizeof(float));
  return result;
}

// ============================================================================
// FUNCTION: CALCULATE MODBUS CRC16
// ============================================================================
uint16_t calculateCRC16(uint8_t *data, uint8_t length) {
  uint16_t crc = 0xFFFF;

  for (uint8_t i = 0; i < length; i++) {
    crc ^= data[i];

    for (uint8_t j = 0; j < 8; j++) {
      if (crc & 0x0001) {
        crc = (crc >> 1) ^ 0xA001;
      } else {
        crc = crc >> 1;
      }
    }
  }

  return crc;
}

// ============================================================================
// FUNCTION: CHECK FOR MODBUS REQUEST
// ============================================================================
void checkModbusRequest() {
  // Check if data available on Serial1
  while (Serial1.available()) {
    uint8_t byte = Serial1.read();
    modbusRxBuffer[rxIndex++] = byte;
    lastRxTime = millis();

    if (rxIndex >= MAX_BUFFER) {
      rxIndex = MAX_BUFFER - 1;
    }
  }

  // Process frame if we have a complete request (3.5 character timeout)
  if (rxIndex > 0 && (millis() - lastRxTime > 50)) {
    processModbusRequest();
    rxIndex = 0;
  }
}

// ============================================================================
// FUNCTION: PROCESS MODBUS REQUEST
// ============================================================================
void processModbusRequest() {
  // Check minimum length (8 bytes for MODBUS RTU)
  if (rxIndex < 8) {
    return;
  }

  uint8_t slaveID = modbusRxBuffer[0];
  uint8_t function = modbusRxBuffer[1];
  uint16_t startReg = (modbusRxBuffer[2] << 8) | modbusRxBuffer[3];
  uint16_t numRegs = (modbusRxBuffer[4] << 8) | modbusRxBuffer[5];
  uint16_t rxCRC = (modbusRxBuffer[rxIndex - 1] << 8) | modbusRxBuffer[rxIndex - 2];

  // Verify CRC
  uint16_t calcCRC = calculateCRC16(modbusRxBuffer, rxIndex - 2);
  if (rxCRC != calcCRC) {
    Serial.println("MODBUS: CRC Error");
    return;
  }

  // Check if request is for this slave
  if (slaveID != MODBUS_SLAVE_ID) {
    return;
  }

  // Only support function 3 (Read Holding Registers)
  if (function != MODBUS_FUNCTION_READ_HOLDING) {
    sendModbusException(slaveID, function, 1);  // Illegal function
    return;
  }

  // Check register range
  if ((startReg + numRegs) > 16) {
    sendModbusException(slaveID, function, 2);  // Illegal address
    return;
  }

  // Build response
  uint8_t txIndex = 0;
  modbusTxBuffer[txIndex++] = slaveID;
  modbusTxBuffer[txIndex++] = function;
  modbusTxBuffer[txIndex++] = numRegs * 2;  // Byte count

  // Add register values
  for (uint16_t i = 0; i < numRegs; i++) {
    uint16_t regValue = holdingRegisters[startReg + i];
    modbusTxBuffer[txIndex++] = (regValue >> 8) & 0xFF;
    modbusTxBuffer[txIndex++] = regValue & 0xFF;
  }

  // Calculate and add CRC
  uint16_t txCRC = calculateCRC16(modbusTxBuffer, txIndex);
  modbusTxBuffer[txIndex++] = txCRC & 0xFF;
  modbusTxBuffer[txIndex++] = (txCRC >> 8) & 0xFF;

  // Send response
  Serial1.write(modbusTxBuffer, txIndex);

  // Debug output
  Serial.print("MODBUS: Read registers ");
  Serial.print(startReg);
  Serial.print(" to ");
  Serial.println(startReg + numRegs - 1);
}

// ============================================================================
// FUNCTION: SEND MODBUS EXCEPTION
// ============================================================================
void sendModbusException(uint8_t slaveID, uint8_t function, uint8_t exceptionCode) {
  uint8_t txIndex = 0;
  modbusTxBuffer[txIndex++] = slaveID;
  modbusTxBuffer[txIndex++] = function | 0x80;  // Set exception bit
  modbusTxBuffer[txIndex++] = exceptionCode;

  uint16_t txCRC = calculateCRC16(modbusTxBuffer, txIndex);
  modbusTxBuffer[txIndex++] = txCRC & 0xFF;
  modbusTxBuffer[txIndex++] = (txCRC >> 8) & 0xFF;

  Serial1.write(modbusTxBuffer, txIndex);

  Serial.print("MODBUS Exception: ");
  Serial.println(exceptionCode);
}

// ============================================================================
// FUNCTION: DISPLAY RESULTS
// ============================================================================
void displayResults() {
  Serial.println("\n================== THERMAL DATA ==================");
  Serial.print("Timestamp: ");
  Serial.print(millis() / 1000);
  Serial.println("s");

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

  Serial.println("\nMODBUS Holding Registers:");
  Serial.print("  Reg 0-1: Sensor 0 = ");
  Serial.println(temperatures[0], 2);

  Serial.print("  Reg 2-3: Sensor 1 = ");
  Serial.println(temperatures[1], 2);

  Serial.print("  Reg 4-5: Sensor 2 = ");
  Serial.println(temperatures[2], 2);

  Serial.print("  Reg 6-7: Sensor 3 = ");
  Serial.println(temperatures[3], 2);

  Serial.print("  Reg 8: Max Temp = ");
  Serial.println(holdingRegisters[8]);

  Serial.print("  Reg 9: Min Temp = ");
  Serial.println(holdingRegisters[9]);

  Serial.print("  Reg 10: Avg Temp = ");
  Serial.println(holdingRegisters[10]);

  Serial.print("  Reg 11: Gradient = ");
  Serial.println(holdingRegisters[11]);

  Serial.print("  Reg 12: Hotspot = ");
  Serial.println(holdingRegisters[12]);

  Serial.println("==================================================");
}
