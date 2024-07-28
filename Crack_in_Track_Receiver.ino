// Crack in track

#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define resetButton 2 // reset button
#define buzzerPin 4 // buzzer pin
#define ledPin 5 // led pin

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// double lat;
// double lon;

String success;

typedef struct gpsCoordinate {
    double lat;
    double lon;
} gpsCoordinate;

gpsCoordinate gpsData;

esp_now_peer_info_t peerInfo;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&gpsData, incomingData, sizeof(gpsData));
  // lat = gpsData.lat;
  // lon = gpsData.lat;

  Serial.print("Lat: ");
  Serial.print(gpsData.lat, 6);
  Serial.print(", Lon: ");
  Serial.print(gpsData.lon, 6);
  Serial.println();

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  // display.println("Hello, world!");
  display.println("Alert! Crack detectedat location:");
  display.println();
  display.print("Lat: ");
  display.println(gpsData.lat, 6);
  display.print("Lon: ");
  display.println(gpsData.lon, 6);
  display.display(); 

  digitalWrite(buzzerPin, HIGH); // buzzer goes on
  digitalWrite(ledPin, HIGH); // led goes on
  delay(2000); // keep buzzer on for 2 seconds
  digitalWrite(buzzerPin, LOW); // buzzer goes off
  digitalWrite(ledPin, LOW); // led goes off
}

void clearWarning() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  // display.println("Hello, world!");
  display.println();
  display.println("Track is clear!");
  display.println();
  display.println("Happy journey!");
  display.display();
}
 
void setup() {
  Serial.begin(115200);

  pinMode(resetButton, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  clearWarning();

  WiFi.mode(WIFI_STA); 
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {
  if (digitalRead(resetButton))
    clearWarning();
}