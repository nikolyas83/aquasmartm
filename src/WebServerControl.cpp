#include "WebServerControl.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "LightControl.h"
#include "TemperatureControl.h"
#include "AquaWiFiControl.h"
#include "EEPROMControl.h"
#include "RTC.h"
#include "index_html.h"
#include "AsyncOTA.h"
#include <SPIFFS.h>

AsyncWebServer server(80);

void initWebServer() {
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS mount failed");
  } else {
    Serial.println("SPIFFS mounted");
  }

  server.on("/get-status", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<512> doc;
    doc["temp"] = getTemperature();
    doc["ssid"] = WiFi.SSID();
    doc["rssi"] = WiFi.RSSI();
    doc["ip"] = WiFi.localIP().toString();
    doc["mac"] = WiFi.macAddress();
    doc["datetime"] = getCurrentDateTime();
    char key[8];
    for (int i = 0; i < 5; i++) {
      snprintf(key, sizeof(key), "ch%d", i);
      doc[key] = getChannelBrightness(i);
    }
    char buf[512];
    serializeJson(doc, buf, sizeof(buf));
    request->send(200, "application/json", buf);
  });

  server.on("/set-brightness", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("channel", true) && request->hasParam("value", true)) {
      int channel = request->getParam("channel", true)->value().toInt();
      int value = request->getParam("value", true)->value().toInt();
      setChannelBrightness(channel, value);
      request->send(200, "text/plain", "OK");
    } else {
      request->send(400, "text/plain", "Invalid parameters");
    }
  });

  server.on("/set-wifi", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("ssid", true) && request->hasParam("password", true)) {
      String ssid = request->getParam("ssid", true)->value();
      String password = request->getParam("password", true)->value();
      Serial.println("Setting WiFi: SSID=" + ssid);
      // Start connection in background and reply immediately to avoid blocking the server
      WiFi.begin(ssid.c_str(), password.c_str());
      request->send(200, "text/plain", "OK");
      WiFi.softAPdisconnect(true);
      // syncTimeWithNTP will check WiFi status internally
      syncTimeWithNTP();
    } else {
      request->send(400, "text/plain", "Invalid WiFi parameters");
    }
  });

  server.on("/get-wifi-networks", HTTP_GET, [](AsyncWebServerRequest *request) {
    String networks = getAvailableNetworks();
    request->send(200, "application/json", networks);
  });

  server.on("/set-effect", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("effect", true)) {
      String effect = request->getParam("effect", true)->value();
      if (effect == "sunrise") startSunrise(10);
      else if (effect == "sunset") startSunset(10);
      else if (effect == "night") setNightMode();
      else request->send(400, "text/plain", "Invalid effect");
      request->send(200, "text/plain", "OK");
    } else {
      request->send(400, "text/plain", "Missing effect parameter");
    }
  });

  server.on("/restart", HTTP_POST, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Restarting");
    
    delay(1000);
    ESP.restart();
  });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (SPIFFS.exists("/index.html")) {
      request->send(SPIFFS, "/index.html", "text/html");
    } else {
      request->send(200, "text/html", index_html);
    }
  });
  syncTimeWithNTP();
  AsyncOTA.begin(&server);
  server.begin();
  Serial.println("Web server started");
}