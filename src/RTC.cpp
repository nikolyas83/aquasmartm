#include "RTC.h"
#include <Wire.h>
#include <RTClib.h>
#include <WiFi.h>
#include <time.h>



RTC_DS1307 rtc;
bool rtcInitialized = false;

void initRTC() {
  if (!rtc.begin()) {
    Serial.println("Error: DS1307 not found");
    rtcInitialized = false;
    return;
  }
  rtcInitialized = true;
  Serial.println("DS1307 initialized");
  syncTimeWithNTP();
}

void syncTimeWithNTP() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("No WiFi, skipping NTP sync");
    return;
  }
  configTime(0, 0, "pool.ntp.org");
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Error: Failed to get NTP time");
    return;
  }
  if (!rtc.isrunning()) {
    rtc.adjust(DateTime(
      timeinfo.tm_year + 1900,
      timeinfo.tm_mon + 1,
      timeinfo.tm_mday,
      timeinfo.tm_hour,
      timeinfo.tm_min,
      timeinfo.tm_sec
    ));
    Serial.println("DS1307 updated from NTP");
  }
}

String getCurrentDateTime() {
  if (!rtcInitialized || !rtc.isrunning()) {
    return "----/--/-- --:--:--";
  }
  DateTime now = rtc.now();
  char buffer[20];
  snprintf(buffer, sizeof(buffer), "%04d/%02d/%02d %02d:%02d:%02d",
           now.year(), now.month(), now.day(),
           now.hour(), now.minute(), now.second());
  return String(buffer);
}