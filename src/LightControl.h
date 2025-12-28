#ifndef LIGHT_CONTROL_H
#define LIGHT_CONTROL_H

void initLight();
void handleLightEffects();
void setChannelBrightness(int channel, int brightness);
int getChannelBrightness(int channel);
void startSunrise(int durationMinutes);
void startSunset(int durationMinutes);
void setNightMode();

#endif