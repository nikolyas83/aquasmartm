#include "WebServerControl.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include "LightControl.h"
#include "TemperatureControl.h"
#include "AquaWiFiControl.h"
#include "EEPROMControl.h"
#include "RTC.h"
#include "index_html.h"
#include "AsyncOTA.h"

AsyncWebServer server(80);

void initWebServer() {
  server.on("/get-status", HTTP_GET, [](AsyncWebServerRequest *request) {
    String status = "{\"temp\":" + String(getTemperature(), 1) +
                    ",\"ssid\":\"" + WiFi.SSID() + "\"" +
                    ",\"rssi\":" + String(WiFi.RSSI()) +
                    ",\"ip\":\"" + WiFi.localIP().toString() + "\"" +
                    ",\"mac\":\"" + WiFi.macAddress() + "\"" +
                    ",\"datetime\":\"" + getCurrentDateTime() + "\"";
    for (int i = 0; i < 5; i++) {
      status += ",\"ch" + String(i) + "\":" + String(getChannelBrightness(i));
    }
    status += "}";
    request->send(200, "application/json", status);
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
    request->send(200, "text/html", index_html);
  });
  syncTimeWithNTP();
  AsyncOTA.begin(&server);
  server.begin();
  Serial.println("Web server started");
}