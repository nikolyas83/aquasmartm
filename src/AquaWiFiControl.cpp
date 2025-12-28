#include "AquaWiFiControl.h"
#include "RTC.h"
#include <WiFi.h>

enum WiFiState { TRY_SAVED, TRY_AQUA, AP_MODE, CONNECTED };
WiFiState wifiState = TRY_SAVED;
unsigned long apStartTime = 0;
const unsigned long AP_TIMEOUT = 120000;

void initWiFi() {
  WiFi.persistent(true);
  wifiState = TRY_SAVED;
  tryConnectSaved();
}

void tryConnectSaved() {
  Serial.println("Step 1: Trying saved WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin();
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 10) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    wifiState = CONNECTED;
    Serial.println("\nConnected to saved WiFi, IP: " + WiFi.localIP().toString());
    
  } else {
    Serial.println("\nFailed to connect to saved WiFi");
    wifiState = TRY_AQUA;
    tryConnectAqua();
  }
}

void tryConnectAqua() {
  Serial.println("Step 2: Trying AQUA:12345678");
  WiFi.mode(WIFI_STA);
  WiFi.begin("AQUA", "12345678");
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 10) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    wifiState = CONNECTED;
    Serial.println("\nConnected to AQUA, IP: " + WiFi.localIP().toString());
    syncTimeWithNTP();
  } else {
    Serial.println("\nFailed to connect to AQUA");
    wifiState = AP_MODE;
    startAP();
  }
}

void startAP() {
  Serial.println("Step 3: Starting AP aquaconfig:11111111");
  WiFi.mode(WIFI_AP);
  WiFi.softAP("aquaconfig", "11111111");
  apStartTime = millis();
  Serial.println("AP started, IP: 192.168.4.1");
}

void handleWiFi() {
  if (wifiState == CONNECTED) {
    if (WiFi.status() != WL_CONNECTED) {
      wifiState = TRY_SAVED;
      tryConnectSaved();
    }
    return;
  }

  if (wifiState == AP_MODE) {
    if (WiFi.softAPgetStationNum() > 0) {
      apStartTime = 0;
    } else if (apStartTime > 0 && millis() - apStartTime > AP_TIMEOUT) {
      WiFi.softAPdisconnect(true);
      Serial.println("AP timeout, restarting WiFi");
      wifiState = TRY_SAVED;
      tryConnectSaved();
    }
  }
}

String getAvailableNetworks() {
  String networks = "[";
  int n = WiFi.scanNetworks();
  if (n == 0) {
    networks += "]";
    Serial.println("No WiFi networks found");
  } else {
    for (int i = 0; i < n; i++) {
      networks += "{\"ssid\":\"" + String(WiFi.SSID(i)) + "\",\"rssi\":" + String(WiFi.RSSI(i)) + "}";
      if (i < n - 1) networks += ",";
    }
    networks += "]";
    Serial.println("Found " + String(n) + " networks");
  }
  WiFi.scanDelete();
  return networks;
}