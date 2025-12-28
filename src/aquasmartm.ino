#include<Arduino.h>
#include "AquaWiFiControl.h"
#include "WebServerControl.h"
#include "LightControl.h"
#include "TemperatureControl.h"
#include "EEPROMControl.h"
#include "RTC.h"

void setup() {
  Serial.begin(115200);
  delay(1000);
  initEEPROM();
  initTemperature();
  initWiFi();
  initRTC();
  initLight();  
  initWebServer();
}

void loop() {
  handleWiFi();
  handleLightEffects();
  handleTemperature();
}