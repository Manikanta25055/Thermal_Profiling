/*
 * 1-Wire Diagnostic Test
 * Tests GPIO42 connection for DS18B20 sensors
 * Helps identify wiring issues
 */

#include <OneWire.h>

#define ONE_WIRE_BUS 42

OneWire ds(ONE_WIRE_BUS);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n========================================");
  Serial.println("1-Wire Diagnostic Test on GPIO42");
  Serial.println("========================================\n");

  Serial.println("Scanning 1-Wire bus...\n");
}

void loop() {
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];
  int sensorCount = 0;

  if (!ds.search(addr)) {
    Serial.println("\nNo devices found on 1-Wire bus.");
    Serial.println("\nPossible causes:");
    Serial.println("1. No sensors connected to GPIO42");
    Serial.println("2. Missing 4.7kΩ pull-up resistor between GPIO42 and 3.3V");
    Serial.println("3. Weak breadboard contacts - try reseating wires");
    Serial.println("4. Sensor power issue - check 3.3V and GND connections");
    Serial.println("5. GPIO42 not connected to sensor data line");
    Serial.println("\nRetrying in 5 seconds...\n");

    ds.reset_search();
    delay(5000);
    return;
  }

  Serial.println("========== 1-Wire Device Found! ==========");

  // Count devices
  do {
    sensorCount++;
    Serial.print("Device ");
    Serial.print(sensorCount);
    Serial.print(" Address: ");

    for (i = 0; i < 8; i++) {
      if (addr[i] < 16) Serial.print("0");
      Serial.print(addr[i], HEX);
      if (i < 7) Serial.print("-");
    }

    Serial.print(" (Family: 0x");
    Serial.print(addr[0], HEX);
    Serial.println(")");

    // Check if it's a DS18B20 (family code 0x28)
    if (addr[0] == 0x28) {
      Serial.println("  → DS18B20 Detected ✓");
    } else if (addr[0] == 0x10) {
      Serial.println("  → DS18S20 Detected");
    } else {
      Serial.println("  → Unknown Device");
    }

    // Try to read temperature
    ds.reset();
    ds.select(addr);
    ds.write(0x44, 1);  // Start temperature conversion
    delay(750);

    ds.reset();
    ds.select(addr);
    ds.write(0xBE);  // Read scratchpad

    for (i = 0; i < 9; i++) {
      data[i] = ds.read();
    }

    int16_t raw = (data[1] << 8) | data[0];
    float celsius = (float)raw / 16.0;

    Serial.print("  → Temperature: ");
    Serial.print(celsius, 2);
    Serial.println("°C");
    Serial.println();

  } while (ds.search(addr));

  Serial.print("Total devices found: ");
  Serial.println(sensorCount);
  Serial.println("=========================================\n");

  if (sensorCount == 0) {
    Serial.println("ERROR: No devices responding!");
    ds.reset_search();
  }

  delay(3000);
}
