#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include "U8glib.h"

#include <SoftwareSerial.h>

U8GLIB_ST7920_128X64_4X u8g(53);

const int chipSelect = 53;  // Set this to your CS pin

//Serial pin
int rxPin = 10; // Receive pin on Mega
int txPin = 11; // Transmit pin on Mega

float receivedData = 0;
//

Adafruit_MPU6050 mpu;

float gx, gy, gz;
float ax, ay, az;

// Current
float sensorValue = 0.0;
float averageSensorValue = 0.0;

const int inPin =A1;//can change
const float SHUNT_CURRENT =50.00;//A
const float SHUNT_VOLTAGE =75.0;// mV
const float CORRECTION_FACTOR = 2.00;

const int ITERATION = 50; //can change (see video)
const float VOLTAGE_REFERENCE = 1100.00;//1.1V
const int BIT_RESOLUTION =10;//and 12 for Due and MKR
const boolean DEBUG_ONCE = true;

//voltage
const int pinVoltage = A0;  // Analog input pin
float R1 = 30000.0;        // Value of R1 in ohms
float R2 = 2000.0;        // Value of R2 in ohms
float voltageDividerFactor = (R1 + R2) / R2;
float analogVoltage = 0;
float batteryVoltage = 0;

//Wh
float totalWh = 0.0;
 float wh = 0.0;

// Define sensor pin and variables Encoder
const int sensorPin = 2;  // Pin connected to E18-D80NK
volatile int encoderTicks = 0;  // Number of encoder ticks
unsigned long lastTime = 0;
const int interval = 1000;  // Interval in ms to calculate RPM (1 second)
const int ticksPerRevolution = 4;  // Number of ticks for one full revolution

// Assume wheel radius is 0.1 meter (10 cm)
const float wheelRadius = 0.45;  // meters
float wheelCircumference = 2 * 3.1416 * wheelRadius;  // meters

int kecepatan = 0;

void countTicks() {
  encoderTicks++;  // Increment tick count
}

 void draw_splash(void){
    u8g.setRot180();
    u8g.setFont(u8g_font_helvB12);
    u8g.drawStr(22, 13, "WELCOME");
    u8g.setFont(u8g_font_8x13);
    u8g.drawStr(22, 25, "PLEASE WEAR");
    u8g.drawStr(22, 38, "YOUR HELMET");
    u8g.drawStr(19, 50,"Designed By:");
    u8g.drawStr(3, 63,"SMK N 1 SEYEGAN"); 
 }

void draw_main(void){

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  u8g.setRot180();

  u8g.setFont(u8g_font_5x7);
  u8g.drawStr(3, 10,"Watt : ");
  
  u8g.setFont(u8g_font_courB08);
  u8g.setPrintPos(35,10);
  u8g.print(wh,1);

  u8g.setFont(u8g_font_courB08);
  u8g.drawStr(75, 10,"W");
  
  u8g.drawLine(0,14,84,14);
  
  u8g.drawLine(0,41,128,41);
  
 //Speed print
  u8g.setFont(u8g_font_osb21);
  u8g.setPrintPos(30,39);
  u8g.print(kecepatan);
//

  
//Current Print
  u8g.setFont(u8g_font_5x7);
  u8g.drawStr(88, 6,"Current");

  u8g.setFont(u8g_font_courB08);
  //u8g.drawStr(86, 29,"01234");
  u8g.setPrintPos(90,16);
  u8g.print(receivedData);

  u8g.setFont(u8g_font_courB08);
  u8g.drawStr(115, 16," A");
//

  u8g.drawLine(84,0,84,40);

  u8g.drawLine(84,20,128,20);

//Voltage print
  u8g.setFont(u8g_font_5x7);
  u8g.drawStr(90, 28,"Voltage");

  u8g.setFont(u8g_font_courB08);
  u8g.setPrintPos(90,38);
  u8g.print(batteryVoltage );

  u8g.setFont(u8g_font_courB08);
  u8g.drawStr(115, 38," V");
//

//Gyro print
  u8g.setFont(u8g_font_5x7);
  u8g.drawStr(2, 50,"Gx :");

  u8g.setFont(u8g_font_5x7);
  u8g.setPrintPos(21,50);
  u8g.print(g.gyro.x, 1);

  u8g.setFont(u8g_font_5x7);
  u8g.drawStr(45, 50,"Gy :");

  u8g.setFont(u8g_font_5x7);
  u8g.setPrintPos(64,50);
  u8g.print(g.gyro.y, 1);

  u8g.setFont(u8g_font_5x7);
  u8g.drawStr(90, 50,"Gz :");

  u8g.setFont(u8g_font_5x7);
  u8g.setPrintPos(109,50);
  u8g.print(g.gyro.z, 1);

  u8g.setFont(u8g_font_5x7);
  u8g.drawStr(2, 60,"Ax :");

  u8g.setFont(u8g_font_5x7);
  u8g.setPrintPos(21,60);
  u8g.print(a.acceleration.x, 1);

  u8g.setFont(u8g_font_5x7);
  u8g.drawStr(45, 60,"Ay :");

  u8g.setFont(u8g_font_5x7);
  u8g.setPrintPos(64,60);
  u8g.print(a.acceleration.y, 1);

  u8g.setFont(u8g_font_5x7);
  u8g.drawStr(90, 60,"Az :");

  u8g.setFont(u8g_font_5x7);
  u8g.setPrintPos(109,60);
  u8g.print(a.acceleration.z, 1);
  //
}


// Setup function
void setup() {
    // Initialize serial communication (optional, for debugging)
    Serial.begin(9600);
    Serial2.begin(9600);  

    pinMode(pinVoltage, INPUT);
    pinMode(sensorPin,  INPUT);

    // Attach interrupt to sensor pin
    attachInterrupt(digitalPinToInterrupt(sensorPin), countTicks, FALLING);

    // Initialize last time
    lastTime = millis();

     if (!mpu.begin()) {
    //Serial.println("Sensor init failed");
    while (1)
      yield();
  }

//    // Initialize SD card
//  Serial.print("Initializing SD card...");
//  if (!SD.begin(chipSelect)) {
//    Serial.println("Card failed, or not present.");
//    return;
//  }
//  Serial.println("Card initialized.");
}


// Main loop function
void loop() {

//Current data
  if (Serial2.available() > 0) {
    receivedData = Serial2.parseFloat(); // Read float data
    if (receivedData == 1.00){
      receivedData = 0 ;
      Serial.println(receivedData);
    }
    else{
      Serial.print("Received data: ");
      Serial.println(receivedData);
    }
  }


//Voltage
//analogReference(DEFAULT);
  analogVoltage = analogRead(pinVoltage);  // Read analog pin Voltage
  float voltage1 = (analogVoltage * 5.0) / 1023.0;  // Convert to voltage
  batteryVoltage = (voltage1 * voltageDividerFactor)- 11.0;

   unsigned long currentTime = millis();

  if (currentTime - lastTime >= interval) {
    // Disable interrupts temporarily
    noInterrupts();

    // Calculate RPS and RPM
    float rps = (float)encoderTicks / ticksPerRevolution;
    float rpm = rps * 60.0;

    // Re-enable interrupts
    interrupts();

    // Reset tick count and update last time
    encoderTicks = 0;
    lastTime = currentTime;

    // Calculate linear speed in meters per second
    float speed_mps = (rpm * wheelCircumference) / 60.0;

    // Convert speed to kilometers per hour (km/h)
    kecepatan = speed_mps * 3.6;
  }

  //Watt Hours
  // Calculate Wh for this interval
    wh = (batteryVoltage * receivedData);  // Convert ms to hours
    totalWh += wh;  // Accumulate Wh

    Serial.print("Arus: ");
    Serial.println(receivedData);
    Serial.print("Kecepatan (km/h): ");
    Serial.println(kecepatan);
    Serial.print("Tegangan Baterai: ");
    Serial.println(batteryVoltage);

//Data
//  saveDataToSD(receivedData, batteryVoltage, speed, wh, totalWh);
     
    //Print LCD
   if(millis()<6000){ // The time in milli seconds, for you want to show the splash screen 6000 = 6 Seconds
    u8g.firstPage();  
    do {
      draw_splash();
    } while( u8g.nextPage() );
    }else{
    u8g.firstPage();  
      do {
        draw_main();
      } while( u8g.nextPage() );
  }
}

void saveDataToSD(float current, float voltage, float speed, float wh, float totalWh) {
  File dataFile = SD.open("data.csv", FILE_WRITE);

  if (dataFile) {
    // Write data in CSV format
    dataFile.print(current);
    dataFile.print(",");
    dataFile.print(voltage);
    dataFile.print(",");
    dataFile.print(speed);
    dataFile.print(",");
    dataFile.print(wh);
    dataFile.print(",");
    dataFile.println(totalWh);

    // Close the file to save changes
    dataFile.close();
    Serial.println("Data saved to SD card.");
  } else {
    Serial.println("Error opening data.csv for writing.");
  }
}
