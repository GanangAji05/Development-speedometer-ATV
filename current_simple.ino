void setup() {
  // put your setup code here, to run once:

}

void loop() {
    // ******** Current Calculation with INTERNAL1V1 Reference ********
  analogReference(INTERNAL1V1);            // Switch to internal 1.1V reference for precise current measurement
  delay(5);                                // Allow reference to stabilize
  sensorValue = analogRead(A1);            // Read the analog value from pin A1
  
//   Validate the sensor value and compute current
  if (sensorValue != 0 && sensorValue < 100) {
    float voltage = (sensorValue + 0.5) * (VOLTAGE_REFERENCE / (pow(2, BIT_RESOLUTION) - 1));  // Calculate voltage
    float current = voltage * (SHUNT_CURRENT / SHUNT_VOLTAGE);  // Calculate current using the shunt resistor specs
    averageSensorValue = current + CORRECTION_FACTOR;  // Apply correction factor
    delay(100);
  } else {
    averageSensorValue = -1;  // Invalid reading
  }

  // Optional: Print the results
  Serial.print("Battery Voltage: ");
  Serial.println(batteryVoltage);
  Serial.print("Ananlog : ");
  Serial.println(averageSensorValue);

}
