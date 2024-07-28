#include <esp_now.h>
#include <WiFi.h>

typedef struct struct_message {
    char code[9]; // 8 characters + null terminator
} struct_message;

struct_message myData;

uint8_t broadcastAddress[] = {0xd8, 0xbc, 0x38, 0xe5, 0x99, 0xd4}; // Replace with receiver's MAC address

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  // Generate a random hexadecimal code
  for (int i = 0; i < 8; i++) {
    int randomValue = random(0, 16);
    if (randomValue < 10) {
      myData.code[i] = '0' + randomValue;
    } else {
      myData.code[i] = 'A' + (randomValue - 10);
    }
  }
  myData.code[8] = '\0'; // Null terminator

  Serial.print("Generated code: ");
  Serial.println(myData.code);

  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.print("Error sending the data: ");
    Serial.println(result);
  }

  delay(10000); // Send data every 10 seconds
}