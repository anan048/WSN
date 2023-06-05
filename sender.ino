
#include <Wire.h>
#include <esp_now.h>
#include <WiFi.h>
#include "base64.hpp"

// Pin analog untuk LM35
const int lm35Pin = A0;
esp_now_peer_info_t peerInfo;

// Alamat MAC ESP-NOW penerima
uint8_t receiverMacAddress[] = { 0xc8, 0xf0, 0x9e, 0x4f, 0xe1, 0xb4};
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)  {
  if (status == ESP_NOW_SEND_SUCCESS) {
   
    Serial.println("Delivery Success");
    Serial.println("Masuk ke deep sleep...");
    delay(100);
    // Masuk ke deep sleep
    esp_deep_sleep_start();
  } else {
    Serial.println("Delivery Fail");
  }

}
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  // register peer
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  // register first peer
  memcpy(peerInfo.peer_addr, receiverMacAddress, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
 
esp_sleep_enable_timer_wakeup(10e6);
}



void loop() {
  // Baca nilai analog dari LM35
  int lm35Value = analogRead(lm35Pin);
  // Konversi nilai analog menjadi suhu dalam derajat Celsius
  float temperature = (lm35Value * 3.3 / 4095) * 100;
  Serial.println("");
  Serial.println("Temperature: ");
  Serial.print(temperature);
  Serial.println("°C");
  // Encode data menggunakan base64
  unsigned char encodedData[20];
  int encodedLength = encode_base64((unsigned char *)&temperature, sizeof(temperature), encodedData);

  // Kirim data melalui ESP-NOW
  if (esp_now_send(receiverMacAddress, encodedData, encodedLength) != ESP_OK) {
    Serial.println("Failed to send data");
  } else {
    Serial.print("Sent encoded data: ");
    Serial.println((char *)encodedData);
  }

  delay(1000);
}
