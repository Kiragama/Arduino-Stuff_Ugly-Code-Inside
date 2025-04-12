#include "Defines.h"
#include "esp_wifi.h"       //to disable wifi

void disable_wifi() {               //CHATGPT CREATED
  esp_err_t err = esp_wifi_stop();  // Stop Wi-Fi
  if (err == ESP_OK) {
    printf("Wi-Fi disabled successfully\n");
  } else {
    printf("Failed to disable Wi-Fi\n");
  }

  err = esp_wifi_deinit();  // Deinitialize Wi-Fi
  if (err == ESP_OK) {
    printf("Wi-Fi deinitialized successfully\n");
  } else {
    printf("Failed to deinitialize Wi-Fi\n");
  }
}