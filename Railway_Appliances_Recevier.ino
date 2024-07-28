#include <esp_now.h>
#include <WiFi.h>

// Define GPIO pins
const int ledPin = 2;     // GPIO for LED
const int motorPin1 = 4;  // IN1 pin of L298N
const int motorPin2 = 5;  // IN2 pin of L298N
const int enAPin = 18;    // ENA pin of L298N (PWM)

typedef struct struct_message {
    char code[9]; // Match the sender's data structure
} struct_message;

struct_message received_data;

unsigned long lastCodeReceivedTime = 0;
const unsigned long timeoutPeriod = 10000; // 3 minutes in milliseconds

void onDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&received_data, incomingData, sizeof(received_data));
  Serial.print("Received code: ");
  Serial.println(received_data.code);

  if (strlen(received_data.code) > 0) {  // Check if received code is not empty
    Serial.println("Valid code received. Turning on appliances.");
    digitalWrite(ledPin, HIGH);
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
    ledcWrite(0, 128); // Set motor speed to about 50% duty cycle
    lastCodeReceivedTime = millis(); // Update the last received time
  } 
  else {
    Serial.println("Invalid code received. Turning off appliances.");
    digitalWrite(ledPin, LOW);
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    ledcWrite(0, 0); // Turn motor off
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(enAPin, OUTPUT);

  // Initialize PWM for motor control
  ledcSetup(0, 1000, 8); // Channel 0, 1000 Hz, 8-bit resolution
  ledcAttachPin(enAPin, 0); // Attach enAPin to channel 0

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(onDataRecv);

  // Initialize the last code received time
  lastCodeReceivedTime = millis();
}

void loop() {
  // Check if 3 minutes have passed since the last code was received
  if (millis() - lastCodeReceivedTime > timeoutPeriod) {
    Serial.println("No code received for 3 minutes. Turning off appliances.");
    digitalWrite(ledPin, LOW);
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    ledcWrite(0, 0); // Turn motor off
  }

  delay(1000); // Check every second
}
