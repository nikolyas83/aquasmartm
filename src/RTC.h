#include <Arduino.h>

#ifndef RTC_H
#define RTC_H

void initRTC();
String getCurrentDateTime();
void syncTimeWithNTP();

#endif