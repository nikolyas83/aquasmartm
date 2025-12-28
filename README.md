# AquaSmart (PlatformIO project)

[![CI](https://github.com/nikolyas83/aquasmartm/actions/workflows/ci.yml/badge.svg)](https://github.com/nikolyas83/aquasmartm/actions/workflows/ci.yml)

Quick notes to build and run this project:

1. Install PlatformIO (VS Code extension) or PlatformIO Core CLI:
   - VS Code: Install "PlatformIO IDE" extension
   - CLI: `pip install -U platformio`

2. From project root run:
   - `pio run` (build)
   - `pio run -t upload` (upload to device)
   - `pio device monitor -b 115200` (open serial monitor)

3. Dependencies:
   - The `platformio.ini` has `lib_deps` entries for required libraries (Async WebServer, AsyncTCP, AsyncOta, OneWire, DallasTemperature, RTClib, ArduinoJson, PCA9685 driver).
   - If a lib fails to be found, verify the exact package name in PlatformIO library registry or use GitHub repo path.

4. Notes and suggestions:
   - `/set-wifi` handler is non-blocking (it starts a connection and replies immediately).
   - PWM values are clamped to the PCA9685 12-bit maximum (4095) to prevent overflow.
   - Consider minimizing dynamic `String` usage in server handlers to avoid heap fragmentation on constrained MCUs.

If you'd like, I can try a build next (if `pio` CLI is available on your system) and address any compile errors.

---

## Опис функціоналу (українською) 🔧

### Основні можливості
- Управління підсвіткою (5 каналів) через PCA9685 (PWM, гамма-корекція).
- Зчитування температури з датчика DS18B20 і вивід у веб‑інтерфейсі.
- Підключення до Wi‑Fi з кількома стратегіями (збережені мережі, пробна мережа `AQUA`, режим AP для конфігурації).
- RTC (DS1307) з синхронізацією часу через NTP при наявності Wi‑Fi.
- Веб‑інтерфейс з вертикальними слайдерами, кнопками ефектів та OTA оновленням (браузерне OTA через AsyncOTA).
- REST‑ендпоінти для отримання статусу та керування пристроєм.

### Endpoints (HTTP)
- GET  /get-status — повертає JSON зі станом: `temp`, `ssid`, `rssi`, `ip`, `mac`, `datetime`, `ch0..ch4`.
- POST /set-brightness — body (form): `channel`, `value` (0..255) — встановлює яскравість каналу.
- POST /set-wifi — body (form): `ssid`, `password` — ініціює підключення до вказаної мережі.
- GET  /get-wifi-networks — повертає JSON масив доступних мереж `{ssid,rssi}`.
- POST /set-effect — body (form): `effect` = `sunrise|sunset|night` — запускає ефект.
- POST /restart — перезавантажує пристрій.
- /ota or /update (AsyncOTA) — веб‑інтерфейс для оновлення прошивки через браузер.

### Можливі проблеми та рекомендації ⚠️
- Пам'ять: активне використання `String` у старих гілках могло викликати фрагментацію heap; я замінив ключові хендлери на `ArduinoJson` для стабільності.
- Блокуючі виклики: уникати довгих `delay()` у веб‑хендлерах — це блокує обробку запитів. Там, де потрібно, краще використовувати state-machine або асинхронні патерни.
- PCA9685: значення у `gammaTable` повинні бути в межах 0..4095 (додано клампування); перевірте таблицю гамми, щоб уникнути переповнення.
- RTC/NTP: синхронізація часу пропускається без Wi‑Fi; переконайтесь, що пристрій підключився до мережі перед викликом `syncTimeWithNTP()`.
- OneWire/DS18B20: перевіряйте підключення та землю; датчик інколи повертає `DEVICE_DISCONNECTED_C` — обробка помилок додана.
- AP timeout: у режимі AP пристрій виходить з AP після таймауту; можна налаштувати час і логіку повторних спроб.

---

> Якщо хочете, можу додати автоматичну перевірку форматів JSON у CI (curl + jq або невеликий Python/Node скрипт) та прості інтеграційні тести.
