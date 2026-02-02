/*
 * GPIO42 Pin Test
 * Tests if GPIO42 is working and can detect voltage changes
 */

#define ONE_WIRE_BUS 42

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n========================================");
  Serial.println("GPIO42 Pin Test");
  Serial.println("========================================\n");

  pinMode(ONE_WIRE_BUS, INPUT);

  Serial.println("Testing GPIO42 voltage levels...\n");
}

void loop() {
  // Read voltage on GPIO42
  int rawValue = analogRead(ONE_WIRE_BUS);
  float voltage = (rawValue / 4095.0) * 3.3;

  Serial.print("GPIO42 Voltage: ");
  Serial.print(voltage, 3);
  Serial.print("V (Raw: ");
  Serial.print(rawValue);
  Serial.println(")");

  // Interpretation
  if (voltage > 3.0) {
    Serial.println("  → Pin is HIGH (Pull-up working or no pull-down)");
  } else if (voltage < 0.5) {
    Serial.println("  → Pin is LOW (Possible short circuit)");
  } else {
    Serial.println("  → Pin is FLOATING (Weak connection or pull-up issue)");
  }

  // Check if anything is connected
  if (voltage < 2.5) {
    Serial.println("\n⚠ WARNING: Voltage too low!");
    Serial.println("  Check: 4.7kΩ resistor connection to 3.3V");
    Serial.println("  Check: No short circuit on GPIO42");
  }

  Serial.println();
  delay(2000);
}
