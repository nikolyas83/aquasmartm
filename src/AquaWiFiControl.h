#include <Arduino.h>

#ifndef AQUA_WIFI_CONTROL_H
#define AQUA_WIFI_CONTROL_H

void initWiFi();
void handleWiFi();
String getAvailableNetworks();
void tryConnectSaved();
void tryConnectAqua();
void startAP();

#endif