// crack in track

#include <esp_now.h>
#include <WiFi.h>
#include <TinyGPS++.h>

#define backRead 2
#define ledOnCrack 4
#define RXD2 16
#define TXD2 17

HardwareSerial neogps(2);

TinyGPSPlus gps;

typedef struct gpsCoordinate {
    double lat;
    double lon;
} gpsCoordinate;

gpsCoordinate gpsData;

void gpsLocation() // get location from gps
{
  // Can take up to 60 seconds
  boolean newData = false;
  for (unsigned long start = millis(); millis() - start < 2000;)
  {
    while (neogps.available())
    {
      if (gps.encode(neogps.read()))
      {
        newData = true;
      }
    }
  }
  if (newData)      //If newData is true
  {
    // Serial.print("http://maps.google.com/maps?q=loc:");
    // Serial.print(gps.location.lat(), 6);
    gpsData.lat = gps.location.lat();
    // Serial.print(","); 
    // Serial.println(gps.location.lng(), 6);
    gpsData.lon = gps.location.lng();
    newData = false;
    // delay(300);
  }
}

uint8_t broadcastAddress1[] = {0xd8, 0xbc, 0x38, 0xe5, 0x48, 0x44}; // train 1
uint8_t broadcastAddress2[] = {0xd8, 0xbc, 0x38, 0xe5, 0xba, 0xf8}; // train 2
uint8_t broadcastAddress3[] = {0xd8, 0xbc, 0x38, 0xe4, 0x10, 0xd8}; // platform

String success;

esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  Serial.print("Delivery Status of: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? " Send Successfully" : " Fail to send");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}
 
void setup() {
  pinMode(backRead, INPUT); // to read the voltage passed through inspection van to the tracks
  pinMode(ledOnCrack, OUTPUT); // glow if crack has been detected


  Serial.begin(115200);
  Serial.println("esp32 serial initialize");
  WiFi.mode(WIFI_STA);

  neogps.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("neogps serial initialize");

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
   peerInfo.channel = 0; 
  peerInfo.encrypt = false;
   
  memcpy(peerInfo.peer_addr, broadcastAddress1, 6); // train 1
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  memcpy(peerInfo.peer_addr, broadcastAddress2, 6); // train 2
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  memcpy(peerInfo.peer_addr, broadcastAddress3, 6); // platform
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  delay(1000); // delay before void loop
}
 
void loop() {

  if (!digitalRead(backRead)) // if crack is detected
  {
    digitalWrite(ledOnCrack, HIGH); // led goes on
    gpsLocation(); // get gps coordinates
    esp_err_t result = esp_now_send(0, (uint8_t *) &gpsData, sizeof(gpsCoordinate));

    // check if message is sent or not
    if (result == ESP_OK) {
      Serial.println("Message sent Successfully");
    }
    else {
      Serial.println("Getiing Error while sending the message");
  }  
  }
  else // if track is intact
  {
    digitalWrite(ledOnCrack, LOW); // led goes off
  }

  // delay(3000);
}