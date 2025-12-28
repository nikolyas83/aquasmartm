#include "LightControl.h"
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
int savedBrightness[5] = {0, 0, 0, 0, 0};
bool lightInitialized = false;
String currentMode = "manual";
unsigned long effectStartTime = 0;
int effectDuration = 0;
int effectChannels[5] = {0, 0, 0, 0, 0};

// Гамма-коррекція (γ=2.8), 256 значень
const uint16_t gammaTable[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 4, 4, 5, 5, 6, 7,
    8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 20, 22, 24, 26, 28, 30,
    32, 34, 37, 39, 42, 44, 47, 50, 53, 56, 59, 62, 66, 69, 73, 77,
    81, 85, 89, 93, 98, 102, 107, 112, 117, 122, 128, 134, 139, 145, 151, 158,
    164, 171, 178, 185, 192, 199, 207, 215, 223, 231, 239, 248, 257, 266, 275, 284,
    294, 304, 314, 324, 335, 346, 357, 369, 380, 392, 404, 417, 430, 443, 456, 470,
    484, 498, 513, 528, 543, 559, 575, 591, 608, 625, 642, 660, 678, 696, 715, 734,
    753, 773, 793, 813, 834, 855, 877, 899, 921, 944, 967, 990, 1014, 1038, 1063, 1088,
    1113, 1139, 1165, 1192, 1219, 1247, 1275, 1303, 1332, 1361, 1391, 1421, 1452, 1483, 1515, 1547,
    1580, 1613, 1647, 1681, 1716, 1751, 1787, 1823, 1860, 1897, 1935, 1973, 2012, 2052, 2092, 2132,
    2173, 2215, 2257, 2300, 2343, 2387, 2432, 2477, 2523, 2569, 2616, 2664, 2712, 2761, 2810, 2860,
    2911, 2962, 3014, 3067, 3120, 3174, 3228, 3283, 3339, 3395, 3452, 3510, 3568, 3627, 3687, 3747,
    3808, 3870, 3932, 3995, 4059, 4123, 4188
};

void initLight() {
  Wire.beginTransmission(0x40);
  if (Wire.endTransmission() != 0) {
    Serial.println("Error: PCA9685 not found");
    lightInitialized = false;
    return;
  }
  pwm.begin();
  pwm.setPWMFreq(1000);
  lightInitialized = true;
  Serial.println("PCA9685 initialized");
  for (int i = 0; i < 5; i++) {
    setChannelBrightness(i, savedBrightness[i]);
  }
}

void handleLightEffects() {
  if (!lightInitialized) return;
  if (currentMode == "manual") return;
  unsigned long now = millis();
  if (now >= effectStartTime + effectDuration) {
    for (int i = 0; i < 5; i++) {
      savedBrightness[i] = effectChannels[i];
    }
    currentMode = "manual";
    Serial.println("Effect completed");
    return;
  }
  float progress = (now - effectStartTime) / (float)effectDuration;
  for (int i = 0; i < 5; i++) {
    int startBright = (currentMode == "sunrise") ? 0 : savedBrightness[i];
    int targetBright = effectChannels[i];
    int currentBright = startBright + (targetBright - startBright) * progress;
    setChannelBrightness(i, currentBright);
  }
}

void setChannelBrightness(int channel, int brightness) {
  if (!lightInitialized) return;
  if (channel < 0 || channel >= 5) return;
  brightness = constrain(brightness, 0, 255);
  int pwmValue = gammaTable[brightness];
  if (pwmValue > 4095) pwmValue = 4095; // Clamp to PCA9685 12-bit maximum
  pwm.setPWM(channel, 0, pwmValue);
  if (currentMode == "manual") {
    savedBrightness[channel] = brightness;
  }
}

int getChannelBrightness(int channel) {
  if (channel < 0 || channel >= 5) return 0;
  return savedBrightness[channel];
}

void startSunrise(int durationMinutes) {
  if (!lightInitialized) return;
  currentMode = "sunrise";
  effectStartTime = millis();
  effectDuration = durationMinutes * 60000;
  for (int i = 0; i < 5; i++) {
    effectChannels[i] = 204; // ~80%
  }
  Serial.println("Sunrise started");
}

void startSunset(int durationMinutes) {
  if (!lightInitialized) return;
  currentMode = "sunset";
  effectStartTime = millis();
  effectDuration = durationMinutes * 60000;
  for (int i = 0; i < 5; i++) {
    effectChannels[i] = 25; // ~10%
  }
  Serial.println("Sunset started");
}

void setNightMode() {
  if (!lightInitialized) return;
  currentMode = "night";
  for (int i = 0; i < 5; i++) {
    setChannelBrightness(i, 10); // ~4%
    savedBrightness[i] = 10;
  }
  Serial.println("Night mode");
}