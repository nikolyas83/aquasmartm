#include "TemperatureControl.h"
#include <OneWire.h>
#include <DallasTemperature.h>

OneWire oneWire(4);
DallasTemperature sensors(&oneWire);
bool sensorInitialized = false;

void initTemperature() {
  sensors.begin();
  sensors.setWaitForConversion(true);
  sensors.requestTemperatures();
  float temp = sensors.getTempCByIndex(0);
  sensorInitialized = (temp != DEVICE_DISCONNECTED_C && temp >= -50.0 && temp <= 100.0);
  if (!sensorInitialized) {
    Serial.println("Error: DS18B20 not detected");
  } else {
    Serial.println("DS18B20 initialized");
  }
}

float getTemperature() {
  if (!sensorInitialized) return 0.0;
  sensors.requestTemperatures();
  float temp = sensors.getTempCByIndex(0);
  if (temp == DEVICE_DISCONNECTED_C || temp < -50.0 || temp > 100.0) {
    Serial.println("Error: Invalid temperature");
    return 0.0;
  }
  return temp;
}

void handleTemperature() {
  static unsigned long lastCheck = 0;
  if (millis() - lastCheck < 5000) return;
  float temp = getTemperature();
  if (sensorInitialized && temp > 0.0) {
    Serial.println("Temperature: " + String(temp, 1) + " °C");
  }
  lastCheck = millis();
}