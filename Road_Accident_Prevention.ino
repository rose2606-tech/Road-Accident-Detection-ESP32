// Include Blynk library and WiFi library
#define BLYNK_TEMPLATE_ID "TMPL3ZatEVMp6"
#define BLYNK_TEMPLATE_NAME "Road Accident Prevention System"
#define BLYNK_AUTH_TOKEN "ozcRSPltlKlDCsPbg3x-F1RCea1l_Efm"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// WiFi credentials
char ssid[] = "Galaxy A54 5G 4BD8";
char pass[] = "Abhra143@";

// Pin Configuration
#define IRSensor 32
#define PIRSensor 27
#define Buzzer 16
#define LED 13

// Blynk Virtual Pins
#define VPIN_STATUS_LED V0      // LED indicator on Blynk app
#define VPIN_MESSAGE V1         // Message display on Blynk app
#define VPIN_IR_STATUS V2       // IR sensor status
#define VPIN_PIR_STATUS V3      // PIR sensor status

// Variables for tracking
unsigned long lastNotificationTime = 0;
const unsigned long NOTIFICATION_INTERVAL = 30000; // 30 seconds between notifications

BlynkTimer timer;

void setup() {
  // Initialize pins
  pinMode(IRSensor, INPUT);
  pinMode(PIRSensor, INPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(LED, OUTPUT);
  
  Serial.begin(9600);
  
  // Initialize Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  
  // Setup timer for regular updates
  timer.setInterval(1000L, sendSensorData);
  
  // Send initial message to Blynk
  Blynk.virtualWrite(VPIN_MESSAGE, "System Started - Monitoring...");
  Blynk.virtualWrite(VPIN_STATUS_LED, 0); // Turn off status LED initially
  
  Serial.println("Accident Detection System with Blynk Started!");
}

void loop() {
  Blynk.run();
  timer.run();
  
  int irValue = digitalRead(IRSensor);
  int pirValue = digitalRead(PIRSensor);
  
  bool obstacleDetected, vehicleDetected;
  if (irValue == LOW)
    obstacleDetected = true;
  else
    obstacleDetected = false;
    
  if (pirValue == HIGH)
    vehicleDetected = true;
  else
    vehicleDetected = false;
  
  // Case 1: Both sensors triggered - CRITICAL
  if (obstacleDetected && vehicleDetected) {
    Serial.println("CRITICAL: Accident + Motion Detected!");
    
    // Send critical alert to Blynk
    Blynk.virtualWrite(VPIN_STATUS_LED, 255); // Turn on status LED (red)
    Blynk.virtualWrite(VPIN_MESSAGE, "🚨 CRITICAL ALERT: Accident + Motion Detected!");
    
    // Send push notification (with rate limiting)
    if (millis() - lastNotificationTime > NOTIFICATION_INTERVAL) {
      Blynk.logEvent("critical_accident", "CRITICAL: Accident with motion detected at location!");
      lastNotificationTime = millis();
    }
    
    // Local alert pattern
    for (int i = 0; i < 3; i++) {
      digitalWrite(Buzzer, HIGH);
      digitalWrite(LED, HIGH);
      delay(200);
      digitalWrite(Buzzer, LOW);
      digitalWrite(LED, LOW);
      delay(200);
    }
  }
  // Case 2: Only accident detected
  else if (obstacleDetected) {
    Serial.println("Accident Detected!");
    
    // Send accident alert to Blynk
    Blynk.virtualWrite(VPIN_STATUS_LED, 180); // Orange/yellow status
    Blynk.virtualWrite(VPIN_MESSAGE, "⚠ ACCIDENT DETECTED: Immediate attention required!");
    
    // Send push notification (with rate limiting)
    if (millis() - lastNotificationTime > NOTIFICATION_INTERVAL) {
      Blynk.logEvent("accident_detected", "Accident detected! Please check the location.");
      lastNotificationTime = millis();
    }
    
    digitalWrite(Buzzer, HIGH);
    digitalWrite(LED, HIGH);
    delay(800);
    digitalWrite(Buzzer, LOW);
    digitalWrite(LED, LOW);
    delay(500);
  }
  // Case 3: Only motion detected
  else if (vehicleDetected) {
    Serial.println("Motion Detected!");
    
    // Send motion alert to Blynk
    Blynk.virtualWrite(VPIN_STATUS_LED, 100); // Dim blue status
    Blynk.virtualWrite(VPIN_MESSAGE, "👤 Motion Detected: Activity in monitored area");
    
    digitalWrite(Buzzer, HIGH);
    digitalWrite(LED, HIGH);
    delay(800);
    digitalWrite(Buzzer, LOW);
    digitalWrite(LED, LOW);
    delay(500);
  }
  // Case 4: No detection
  else {
    Serial.println("No Accident / No Motion");
    
    // Update Blynk with normal status
    Blynk.virtualWrite(VPIN_STATUS_LED, 0); // Turn off status LED
    Blynk.virtualWrite(VPIN_MESSAGE, "✅ All Clear: System monitoring normally");
    
    digitalWrite(Buzzer, LOW);
    digitalWrite(LED, LOW);
  }
  
  delay(500);
}

// Function to send sensor data to Blynk regularly
void sendSensorData() {
  int irValue = digitalRead(IRSensor);
  int pirValue = digitalRead(PIRSensor);
  
  // Send individual sensor statuses
  Blynk.virtualWrite(VPIN_IR_STATUS, (irValue == LOW) ? 1 : 0);
  Blynk.virtualWrite(VPIN_PIR_STATUS, (pirValue == HIGH) ? 1 : 0);
}

// Connection status monitoring
BLYNK_CONNECTED() {
  Serial.println("Connected to Blynk server");
  Blynk.virtualWrite(VPIN_MESSAGE, "🌐 Connected: System online and monitoring");
}

BLYNK_DISCONNECTED() {
  Serial.println("Disconnected from Blynk server");
}
