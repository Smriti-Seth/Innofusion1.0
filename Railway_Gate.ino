#include <ESP32Servo.h>  // Use the ESP32Servo library

// Define pin numbers for ultrasonic sensors
const int triggerPin1 = 4;  // GPIO pin for ultrasonic sensor 1 trigger
const int echoPin1 = 16;    // GPIO pin for ultrasonic sensor 1 echo
const int triggerPin2 = 15; // GPIO pin for ultrasonic sensor 2 trigger
const int echoPin2 = 2;     // GPIO pin for ultrasonic sensor 2 echo

// Define pin numbers for other components
const int servoPin = 18;    // GPIO pin for servo motor
const int greenLedPin = 25; // GPIO pin for green LED
const int redLedPin = 26;   // GPIO pin for red LED

Servo gateServo;
bool gateClosed = false;

void setup() {
  pinMode(triggerPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(triggerPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  
  gateServo.attach(servoPin);
  gateServo.write(90);  // Start with the gate open

  digitalWrite(greenLedPin, HIGH); // Green LED on (gate open)
  digitalWrite(redLedPin, LOW);    // Red LED off (gate open)

  Serial.begin(115200); // For debugging
}

long getDistance(int triggerPin, int echoPin) {
  long duration;
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  return (duration / 2) / 29.1; // Convert to cm
}

void loop() {
  long distance1 = getDistance(triggerPin1, echoPin1);
  long distance2 = getDistance(triggerPin2, echoPin2);

  Serial.print("Distance 1: ");
  Serial.print(distance1);
  Serial.print(" cm, Distance 2: ");
  Serial.print(distance2);
  Serial.println(" cm");

  if (distance1 < 10 && !gateClosed) {
    // Close the gate if an object is detected by the first sensor and the gate is not already closed
    gateServo.write(0); // Close the gate (0 degrees)
    gateClosed = true;
    digitalWrite(greenLedPin, LOW); // Green LED off (gate closed)
    digitalWrite(redLedPin, HIGH);  // Red LED on (gate closed)
    Serial.println("Gate closed");
  }

  if (distance2 < 10 && gateClosed) {
    // Open the gate if an object is detected by the second sensor and the gate is closed
    gateServo.write(90); // Open the gate (90 degrees)
    gateClosed = false;
    digitalWrite(greenLedPin, HIGH); // Green LED on (gate open)
    digitalWrite(redLedPin, LOW);    // Red LED off (gate open)
    Serial.println("Gate opened");
  }

  delay(1000); // Short delay to avoid rapid triggering
}