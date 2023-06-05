#include <esp_now.h>
#include <WiFi.h>
#include "base64.hpp"

// Alamat MAC wildcard untuk menerima siaran broadcast
uint8_t broadcastMacAddress[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
uint8_t senderMacAddress[6];
int c = 0;
unsigned long startTime;

// Waktu yang diinginkan dalam milidetik (misalnya 5 detik)
unsigned long desiredTime = 2000;

unsigned long currentTime;
unsigned long elapsedTime;

void setup() {
  //Initialize Serial Monitor
  Serial.begin(115200);
  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  // Once ESPNow is successfully Init, we will register  for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(onDataReceived);
    esp_sleep_enable_timer_wakeup(10e6);
}

void loop() {
  currentTime = millis();
  elapsedTime = currentTime - startTime;
  if (c == 1 && elapsedTime >= desiredTime) {
    c = 2;
    startTime = currentTime;
  }
  if (c == 2 ) {
    c = 0;
    Serial.println("Masuk ke deep sleep...");
    delay(100);
    // Masuk ke deep sleep
    esp_deep_sleep_start();
  }
}

// Callback yang dipanggil ketika data diterima melalui ESP-NOW
void onDataReceived(const uint8_t *macAddress, const uint8_t *data, int dataLength) {
  
  startTime = millis();

  // Ubah data menjadi tipe unsigned char
  unsigned char encodedData[dataLength];
  memcpy(encodedData, data, dataLength);

  // Mendekode data yang diterima menggunakan base64
  unsigned char decodedData[20];
  int decodedLength = decode_base64(encodedData, decodedData);

  // Mengonversi data dari bentuk byte ke bentuk float
  float temperature;
  memcpy(&temperature, decodedData, sizeof(temperature));
  memcpy(senderMacAddress, macAddress, 6);

  // Menampilkan MAC Address pengirim
  Serial.print("MAC Address Pengirim: ");

  for (int i = 0; i < 6; i++) {
    Serial.print(senderMacAddress[i], HEX);
    if (i < 5) {
      Serial.print(":");
    }
  }
  Serial.println();
  // Memproses data suhu
  Serial.print("Received temperature: ");
  Serial.print(temperature);

  Serial.println("°C");
  c++;

}
