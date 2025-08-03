//#include "Defines.h"
#include <WiFi.h>
#include "driver/adc.h"

void disable_wifi() {               //CHATGPT CREATED  
  WiFi.disconnect(true);  // Disconnect from the network
  WiFi.mode(WIFI_OFF);    // Switch WiFi off
  //measured power is 67mA with this off and 172mA with this code removed
}