/*
 * Test Alternative GPIO Pins
 * Tests GPIO41, GPIO40, GPIO39 for 1-Wire functionality
 */

#define TEST_PIN_1 41    // Test GPIO41
#define TEST_PIN_2 40    // Test GPIO40
#define TEST_PIN_3 39    // Test GPIO39

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n========================================");
  Serial.println("GPIO Alternative Pin Test");
  Serial.println("========================================\n");

  pinMode(TEST_PIN_1, INPUT);
  pinMode(TEST_PIN_2, INPUT);
  pinMode(TEST_PIN_3, INPUT);

  Serial.println("Testing alternative GPIO pins...\n");
}

void loop() {
  // Test GPIO41
  int raw41 = analogRead(TEST_PIN_1);
  float volt41 = (raw41 / 4095.0) * 3.3;

  Serial.print("GPIO41 Voltage: ");
  Serial.print(volt41, 3);
  Serial.println("V");

  // Test GPIO40
  int raw40 = analogRead(TEST_PIN_2);
  float volt40 = (raw40 / 4095.0) * 3.3;

  Serial.print("GPIO40 Voltage: ");
  Serial.print(volt40, 3);
  Serial.println("V");

  // Test GPIO39
  int raw39 = analogRead(TEST_PIN_3);
  float volt39 = (raw39 / 4095.0) * 3.3;

  Serial.print("GPIO39 Voltage: ");
  Serial.print(volt39, 3);
  Serial.println("V");

  Serial.println("\nGood pins should read close to 3.3V");
  Serial.println("========================================\n");

  delay(2000);
}
